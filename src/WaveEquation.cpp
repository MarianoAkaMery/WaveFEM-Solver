#include "WaveEquation.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>

namespace
{
constexpr double pi = numbers::PI;

std::string
make_output_name(const unsigned int timestep_number)
{
  std::ostringstream name;
  name << "wave_solution_" << std::setw(4) << std::setfill('0')
       << timestep_number << ".vtu";
  return name.str();
}
} // namespace

double
WaveEquation::ExactSolution::value(const Point<dim> &p,
                                   const unsigned int /*component*/) const
{
  const double omega = std::sqrt(2.0) * pi;

  return std::sin(pi * p[0]) * std::sin(pi * p[1]) *
         std::cos(omega * this->get_time());
}

Tensor<1, WaveEquation::dim>
WaveEquation::ExactSolution::gradient(const Point<dim> &p,
                                      const unsigned int /*component*/) const
{
  const double omega = std::sqrt(2.0) * pi;

  Tensor<1, dim> grad;
  grad[0] = pi * std::cos(pi * p[0]) * std::sin(pi * p[1]) *
            std::cos(omega * this->get_time());
  grad[1] = pi * std::sin(pi * p[0]) * std::cos(pi * p[1]) *
            std::cos(omega * this->get_time());

  return grad;
}

double
WaveEquation::InitialVelocity::value(const Point<dim> & /*p*/,
                                     const unsigned int /*component*/) const
{
  return 0.0;
}

WaveEquation::WaveEquation(const unsigned int &n_subdivisions_,
                           const unsigned int &fe_degree_,
                           const double       &final_time_,
                           const double       &time_step_,
                           const unsigned int &output_frequency_,
                           const std::string  &output_directory_)
  : n_subdivisions(n_subdivisions_)
  , fe_degree(fe_degree_)
  , final_time(final_time_)
  , time_step(time_step_)
  , output_frequency(output_frequency_)
  , output_directory(output_directory_)
  , dof_handler(mesh)
{}

void
WaveEquation::setup()
{
  std::cout << "===============================================" << std::endl;

  // Mesh.
  //
  // We start with the unit square because the manufactured solution is defined
  // there and no external .msh file is needed. This is the safest first target
  // before adding more geometries.
  {
    std::cout << "Initializing the mesh" << std::endl;
    GridGenerator::subdivided_hyper_cube(mesh,
                                         n_subdivisions,
                                         0.0,
                                         1.0,
                                         /*colorize = */ false);

    std::cout << "  Number of elements = " << mesh.n_active_cells()
              << std::endl;

    std::filesystem::create_directories(output_directory);

    GridOut       grid_out;
    std::ofstream mesh_file(output_directory + "/mesh.vtk");
    grid_out.write_vtk(mesh, mesh_file);
    std::cout << "  Mesh written to " << output_directory + "/mesh.vtk"
              << std::endl;
  }

  std::cout << "-----------------------------------------------" << std::endl;

  // Finite element space.
  //
  // FE_Q is the standard Lagrange element on quadrilateral meshes. The labs use
  // FE_SimplexP when reading triangular meshes; the assembly pattern is the
  // same, only the element and quadrature classes change.
  {
    std::cout << "Initializing the finite element space" << std::endl;

    fe         = std::make_unique<FE_Q<dim>>(fe_degree);
    quadrature = std::make_unique<QGauss<dim>>(fe_degree + 1);

    std::cout << "  Degree                     = " << fe->degree << std::endl;
    std::cout << "  DoFs per cell              = " << fe->dofs_per_cell
              << std::endl;
    std::cout << "  Quadrature points per cell = " << quadrature->size()
              << std::endl;
  }

  std::cout << "-----------------------------------------------" << std::endl;

  // DoF handler.
  {
    std::cout << "Initializing the DoF handler" << std::endl;

    dof_handler.reinit(mesh);
    dof_handler.distribute_dofs(*fe);

    std::cout << "  Number of DoFs = " << dof_handler.n_dofs() << std::endl;
  }

  std::cout << "-----------------------------------------------" << std::endl;

  // Linear algebra structures.
  {
    std::cout << "Initializing matrices and vectors" << std::endl;

    DynamicSparsityPattern dsp(dof_handler.n_dofs());
    DoFTools::make_sparsity_pattern(dof_handler, dsp);
    sparsity_pattern.copy_from(dsp);

    mass_matrix.reinit(sparsity_pattern);
    stiffness_matrix.reinit(sparsity_pattern);
    newmark_matrix.reinit(sparsity_pattern);
    mass_matrix_constrained.reinit(sparsity_pattern);

    displacement.reinit(dof_handler.n_dofs());
    velocity.reinit(dof_handler.n_dofs());
    acceleration.reinit(dof_handler.n_dofs());
    system_rhs.reinit(dof_handler.n_dofs());
  }

  // Homogeneous Dirichlet boundary values.
  //
  // All boundary faces have boundary id 0 because the mesh was generated with
  // colorize=false. This map is reused whenever we need to constrain a system.
  {
    Functions::ZeroFunction<dim> zero_boundary;
    VectorTools::interpolate_boundary_values(dof_handler,
                                             /*boundary_id = */ 0,
                                             zero_boundary,
                                             boundary_values);
  }
}

void
WaveEquation::assemble_matrices()
{
  std::cout << "===============================================" << std::endl;
  std::cout << "Assembling mass, stiffness and Newmark matrices"
            << std::endl;

  const unsigned int dofs_per_cell = fe->dofs_per_cell;
  const unsigned int n_q           = quadrature->size();

  FEValues<dim> fe_values(*fe,
                          *quadrature,
                          update_values | update_gradients |
                            update_JxW_values);

  FullMatrix<double> cell_mass(dofs_per_cell, dofs_per_cell);
  FullMatrix<double> cell_stiffness(dofs_per_cell, dofs_per_cell);

  std::vector<types::global_dof_index> dof_indices(dofs_per_cell);

  mass_matrix      = 0.0;
  stiffness_matrix = 0.0;

  for (const auto &cell : dof_handler.active_cell_iterators())
    {
      fe_values.reinit(cell);

      cell_mass      = 0.0;
      cell_stiffness = 0.0;

      for (unsigned int q = 0; q < n_q; ++q)
        {
          for (unsigned int i = 0; i < dofs_per_cell; ++i)
            {
              for (unsigned int j = 0; j < dofs_per_cell; ++j)
                {
                  // Mass matrix:
                  //   M_ij = int phi_j phi_i dx.
                  // This comes from the u_tt term in the weak form.
                  cell_mass(i, j) += fe_values.shape_value(i, q) *
                                     fe_values.shape_value(j, q) *
                                     fe_values.JxW(q);

                  // Stiffness matrix:
                  //   K_ij = int grad(phi_j) dot grad(phi_i) dx.
                  // This comes from integrating -Delta u by parts.
                  cell_stiffness(i, j) +=
                    scalar_product(fe_values.shape_grad(i, q),
                                   fe_values.shape_grad(j, q)) *
                    fe_values.JxW(q);
                }
            }
        }

      cell->get_dof_indices(dof_indices);
      mass_matrix.add(dof_indices, cell_mass);
      stiffness_matrix.add(dof_indices, cell_stiffness);
    }

  // Matrix for Newmark acceleration solve:
  //
  //   (M + beta dt^2 K) a^{n+1} = rhs.
  //
  // The matrix is constant because M and K are constant and dt is fixed, so we
  // assemble it once.
  newmark_matrix.copy_from(mass_matrix);
  newmark_matrix.add(beta * time_step * time_step, stiffness_matrix);

  // Matrix for the initial acceleration solve:
  //
  //   M a(0) = F(0) - K u(0).
  mass_matrix_constrained.copy_from(mass_matrix);

  // Apply homogeneous Dirichlet rows to the two matrices that are directly
  // solved. The original M and K are left untouched for energy and rhs products.
  Vector<double> dummy_solution(dof_handler.n_dofs());
  Vector<double> dummy_rhs(dof_handler.n_dofs());
  MatrixTools::apply_boundary_values(boundary_values,
                                     newmark_matrix,
                                     dummy_solution,
                                     dummy_rhs,
                                     true);
  MatrixTools::apply_boundary_values(boundary_values,
                                     mass_matrix_constrained,
                                     dummy_solution,
                                     dummy_rhs,
                                     true);
}

void
WaveEquation::initialize_solution()
{
  std::cout << "===============================================" << std::endl;
  std::cout << "Interpolating initial displacement and velocity"
            << std::endl;

  ExactSolution exact_displacement;
  exact_displacement.set_time(0.0);
  VectorTools::interpolate(dof_handler, exact_displacement, displacement);

  InitialVelocity initial_velocity;
  VectorTools::interpolate(dof_handler, initial_velocity, velocity);

  // Boundary values are zero for both displacement and velocity.
  for (const auto &[dof_index, value] : boundary_values)
    {
      displacement[dof_index] = value;
      velocity[dof_index]     = 0.0;
    }

  solve_initial_acceleration();
}

void
WaveEquation::solve_initial_acceleration()
{
  // Initial acceleration comes from the PDE at t=0:
  //
  //   M a(0) + K u(0) = F(0).
  //
  // Therefore we solve M a(0) = F(0) - K u(0).
  Vector<double> force(dof_handler.n_dofs());
  assemble_force(0.0, force);

  Vector<double> stiffness_times_displacement(dof_handler.n_dofs());
  stiffness_matrix.vmult(stiffness_times_displacement, displacement);

  system_rhs = force;
  system_rhs -= stiffness_times_displacement;

  for (const auto &[dof_index, value] : boundary_values)
    {
      acceleration[dof_index] = value;
      system_rhs[dof_index]   = value;
    }

  solve_linear_system(mass_matrix_constrained,
                      acceleration,
                      system_rhs,
                      "initial acceleration");
}

void
WaveEquation::solve_time_step()
{
  // Newmark predictor:
  //
  //   u_predict = u^n + dt v^n + dt^2 (1/2 - beta) a^n.
  //
  // Then solve:
  //
  //   (M + beta dt^2 K) a^{n+1} = F^{n+1} - K u_predict.
  Vector<double> force(dof_handler.n_dofs());
  assemble_force(time + time_step, force);

  Vector<double> displacement_predictor(displacement);
  displacement_predictor.add(time_step, velocity);
  displacement_predictor.add(time_step * time_step * (0.5 - beta),
                             acceleration);

  Vector<double> stiffness_times_predictor(dof_handler.n_dofs());
  stiffness_matrix.vmult(stiffness_times_predictor, displacement_predictor);

  system_rhs = force;
  system_rhs -= stiffness_times_predictor;

  Vector<double> new_acceleration(dof_handler.n_dofs());

  for (const auto &[dof_index, value] : boundary_values)
    {
      new_acceleration[dof_index] = value;
      system_rhs[dof_index]       = value;
    }

  solve_linear_system(newmark_matrix,
                      new_acceleration,
                      system_rhs,
                      "Newmark acceleration");

  // Newmark corrector:
  //
  //   u^{n+1} = u_predict + beta dt^2 a^{n+1}
  //   v^{n+1} = v^n + dt ((1-gamma) a^n + gamma a^{n+1})
  displacement = displacement_predictor;
  displacement.add(beta * time_step * time_step, new_acceleration);

  velocity.add(time_step * (1.0 - gamma), acceleration);
  velocity.add(time_step * gamma, new_acceleration);

  acceleration = new_acceleration;

  for (const auto &[dof_index, value] : boundary_values)
    {
      displacement[dof_index] = value;
      velocity[dof_index]     = 0.0;
      acceleration[dof_index] = 0.0;
    }
}

void
WaveEquation::solve_linear_system(const SparseMatrix<double> &matrix,
                                  Vector<double>             &unknown,
                                  const Vector<double>       &rhs,
                                  const std::string          &label) const
{
  ReductionControl solver_control(/* maxiter = */ 5000,
                                  /* tolerance = */ 1.0e-14,
                                  /* reduce = */ 1.0e-10);

  SolverCG<Vector<double>> solver(solver_control);

  PreconditionSSOR<SparseMatrix<double>> preconditioner;
  preconditioner.initialize(
    matrix, PreconditionSSOR<SparseMatrix<double>>::AdditionalData(1.0));

  solver.solve(matrix, unknown, rhs, preconditioner);

  std::cout << "  Solved " << label << " in " << solver_control.last_step()
            << " CG iterations" << std::endl;
}

void
WaveEquation::output_results()
{
  DataOut<dim> data_out;

  data_out.add_data_vector(dof_handler, displacement, "u");
  data_out.add_data_vector(dof_handler, velocity, "velocity");
  data_out.add_data_vector(dof_handler, acceleration, "acceleration");

  data_out.build_patches(fe->degree);

  const std::string file_name = make_output_name(timestep_number);
  const std::string full_path = output_directory + "/" + file_name;

  std::ofstream output_file(full_path);
  data_out.write_vtu(output_file);

  pvd_entries.emplace_back(time, file_name);
  write_pvd_record();

  std::cout << "  Output written to " << full_path << std::endl;
}

void
WaveEquation::write_pvd_record() const
{
  std::ofstream pvd_file(output_directory + "/wave_solution.pvd");

  pvd_file << "<?xml version=\"1.0\"?>\n";
  pvd_file << "<VTKFile type=\"Collection\" version=\"0.1\" "
              "byte_order=\"LittleEndian\">\n";
  pvd_file << "  <Collection>\n";

  for (const auto &[entry_time, file_name] : pvd_entries)
    {
      pvd_file << "    <DataSet timestep=\"" << entry_time
               << "\" group=\"\" part=\"0\" file=\"" << file_name
               << "\"/>\n";
    }

  pvd_file << "  </Collection>\n";
  pvd_file << "</VTKFile>\n";
}

std::pair<double, double>
WaveEquation::compute_errors() const
{
  ExactSolution exact_solution;
  exact_solution.set_time(time);

  const QGauss<dim> quadrature_error(fe_degree + 2);

  Vector<double> error_per_cell(mesh.n_active_cells());

  VectorTools::integrate_difference(dof_handler,
                                    displacement,
                                    exact_solution,
                                    error_per_cell,
                                    quadrature_error,
                                    VectorTools::L2_norm);
  const double l2_error = VectorTools::compute_global_error(mesh,
                                                           error_per_cell,
                                                           VectorTools::L2_norm);

  VectorTools::integrate_difference(dof_handler,
                                    displacement,
                                    exact_solution,
                                    error_per_cell,
                                    quadrature_error,
                                    VectorTools::H1_seminorm);
  const double h1_error =
    VectorTools::compute_global_error(mesh,
                                      error_per_cell,
                                      VectorTools::H1_seminorm);

  return {l2_error, h1_error};
}

double
WaveEquation::compute_energy() const
{
  Vector<double> matrix_times_vector(dof_handler.n_dofs());

  mass_matrix.vmult(matrix_times_vector, velocity);
  const double kinetic_energy = 0.5 * (velocity * matrix_times_vector);

  stiffness_matrix.vmult(matrix_times_vector, displacement);
  const double potential_energy = 0.5 * (displacement * matrix_times_vector);

  return kinetic_energy + potential_energy;
}

void
WaveEquation::append_diagnostics()
{
  const auto [l2_error, h1_error] = compute_errors();
  const double energy             = compute_energy();

  const bool write_header =
    !std::filesystem::exists(output_directory + "/diagnostics.csv");

  std::ofstream diagnostics(output_directory + "/diagnostics.csv",
                            std::ios::app);

  if (write_header)
    diagnostics << "step,time,l2_error,h1_seminorm,energy\n";

  diagnostics << timestep_number << "," << std::setprecision(16) << time
              << "," << l2_error << "," << h1_error << "," << energy
              << "\n";

  std::cout << "  L2 error = " << l2_error
            << ", H1 seminorm error = " << h1_error
            << ", energy = " << energy << std::endl;
}

void
WaveEquation::assemble_force(const double & /*time*/,
                             Vector<double> &force) const
{
  // The manufactured validation case has f = 0. We keep this method separate
  // because changing to a nonzero source should only require editing this block.
  force = 0.0;
}

void
WaveEquation::run()
{
  setup();
  assemble_matrices();
  initialize_solution();

  time            = 0.0;
  timestep_number = 0;

  std::filesystem::remove(output_directory + "/diagnostics.csv");

  std::cout << "===============================================" << std::endl;
  std::cout << "Starting time loop" << std::endl;

  output_results();
  append_diagnostics();

  while (time < final_time - 0.5 * time_step)
    {
      std::cout << "-----------------------------------------------" << std::endl;

      solve_time_step();

      time += time_step;
      ++timestep_number;

      std::cout << "Timestep " << timestep_number << ", time = " << time
                << std::endl;

      append_diagnostics();

      if (timestep_number % output_frequency == 0 ||
          time >= final_time - 0.5 * time_step)
        output_results();
    }

  std::cout << "===============================================" << std::endl;
  std::cout << "Simulation completed" << std::endl;
}
