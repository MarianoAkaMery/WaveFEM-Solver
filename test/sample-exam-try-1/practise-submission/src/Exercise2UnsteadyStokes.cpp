#include "Exercise2UnsteadyStokes.hpp"

#include <cmath>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <limits>

void
Exercise2UnsteadyStokes::PreconditionBlockTriangular::initialize(
  const TrilinosWrappers::SparseMatrix &velocity_block_,
  const TrilinosWrappers::SparseMatrix &pressure_mass_,
  const TrilinosWrappers::SparseMatrix &B_)
{
  velocity_block = &velocity_block_;
  pressure_mass  = &pressure_mass_;
  B              = &B_;

  preconditioner_velocity.initialize(velocity_block_);
  preconditioner_pressure.initialize(pressure_mass_);
}

void
Exercise2UnsteadyStokes::PreconditionBlockTriangular::vmult(
  TrilinosWrappers::MPI::BlockVector       &dst,
  const TrilinosWrappers::MPI::BlockVector &src) const
{
  SolverControl solver_control_velocity(1000, 1e-2 * src.block(0).l2_norm());
  SolverCG<TrilinosWrappers::MPI::Vector> solver_cg_velocity(
    solver_control_velocity);
  solver_cg_velocity.solve(*velocity_block,
                           dst.block(0),
                           src.block(0),
                           preconditioner_velocity);

  tmp.reinit(src.block(1));
  B->vmult(tmp, dst.block(0));
  tmp.sadd(-1.0, src.block(1));

  SolverControl solver_control_pressure(1000, 1e-2 * src.block(1).l2_norm());
  SolverCG<TrilinosWrappers::MPI::Vector> solver_cg_pressure(
    solver_control_pressure);
  solver_cg_pressure.solve(*pressure_mass,
                           dst.block(1),
                           tmp,
                           preconditioner_pressure);
}

Exercise2UnsteadyStokes::Exercise2UnsteadyStokes(
  const std::string  &mesh_file_name_,
  const unsigned int degree_velocity_,
  const unsigned int degree_pressure_,
  const double       final_time_,
  const double       delta_t_)
  : mesh_file_name(mesh_file_name_)
  , degree_velocity(degree_velocity_)
  , degree_pressure(degree_pressure_)
  , final_time(final_time_)
  , delta_t(delta_t_)
  , mpi_size(Utilities::MPI::n_mpi_processes(MPI_COMM_WORLD))
  , mpi_rank(Utilities::MPI::this_mpi_process(MPI_COMM_WORLD))
  , pcout(std::cout, mpi_rank == 0)
  , mesh(MPI_COMM_WORLD)
{}

void
Exercise2UnsteadyStokes::setup()
{
  pcout << "Initializing Exercise 2 mesh" << std::endl;

  Triangulation<dim> mesh_serial;
  GridIn<dim>        grid_in;
  grid_in.attach_triangulation(mesh_serial);

  std::ifstream grid_in_file(mesh_file_name);
  grid_in.read_msh(grid_in_file);

  GridTools::partition_triangulation(mpi_size, mesh_serial);
  const auto construction_data = TriangulationDescription::Utilities::
    create_description_from_triangulation(mesh_serial, MPI_COMM_WORLD);
  mesh.create_triangulation(construction_data);

  const FE_SimplexP<dim> fe_scalar_velocity(degree_velocity);
  const FE_SimplexP<dim> fe_scalar_pressure(degree_pressure);
  fe = std::make_unique<FESystem<dim>>(fe_scalar_velocity,
                                       dim,
                                       fe_scalar_pressure,
                                       1);

  quadrature      = std::make_unique<QGaussSimplex<dim>>(fe->degree + 1);
  quadrature_face = std::make_unique<QGaussSimplex<dim - 1>>(fe->degree + 1);

  dof_handler.reinit(mesh);
  dof_handler.distribute_dofs(*fe);

  std::vector<unsigned int> block_component(dim + 1, 0);
  block_component[dim] = 1;
  DoFRenumbering::component_wise(dof_handler, block_component);

  locally_owned_dofs    = dof_handler.locally_owned_dofs();
  locally_relevant_dofs = DoFTools::extract_locally_relevant_dofs(dof_handler);

  std::vector<types::global_dof_index> dofs_per_block =
    DoFTools::count_dofs_per_fe_block(dof_handler, block_component);
  const unsigned int n_u = dofs_per_block[0];
  const unsigned int n_p = dofs_per_block[1];

  block_owned_dofs.resize(2);
  block_relevant_dofs.resize(2);
  block_owned_dofs[0]    = locally_owned_dofs.get_view(0, n_u);
  block_owned_dofs[1]    = locally_owned_dofs.get_view(n_u, n_u + n_p);
  block_relevant_dofs[0] = locally_relevant_dofs.get_view(0, n_u);
  block_relevant_dofs[1] = locally_relevant_dofs.get_view(n_u, n_u + n_p);

  Table<2, DoFTools::Coupling> coupling(dim + 1, dim + 1);
  for (unsigned int c = 0; c < dim + 1; ++c)
    for (unsigned int d = 0; d < dim + 1; ++d)
      coupling[c][d] = (c == dim && d == dim) ? DoFTools::none :
                                                 DoFTools::always;

  TrilinosWrappers::BlockSparsityPattern sparsity(block_owned_dofs,
                                                  MPI_COMM_WORLD);
  DoFTools::make_sparsity_pattern(dof_handler, coupling, sparsity);
  sparsity.compress();

  for (unsigned int c = 0; c < dim + 1; ++c)
    for (unsigned int d = 0; d < dim + 1; ++d)
      coupling[c][d] = (c == dim && d == dim) ? DoFTools::always :
                                                 DoFTools::none;

  TrilinosWrappers::BlockSparsityPattern sparsity_pressure_mass(
    block_owned_dofs, MPI_COMM_WORLD);
  DoFTools::make_sparsity_pattern(dof_handler,
                                  coupling,
                                  sparsity_pressure_mass);
  sparsity_pressure_mass.compress();

  system_matrix.reinit(sparsity);
  pressure_mass.reinit(sparsity_pressure_mass);

  system_rhs.reinit(block_owned_dofs, MPI_COMM_WORLD);
  solution_owned.reinit(block_owned_dofs, MPI_COMM_WORLD);
  solution.reinit(block_owned_dofs, block_relevant_dofs, MPI_COMM_WORLD);

  solution_owned = 0.0;
  solution       = solution_owned;

  pcout << "  Elements = " << mesh.n_global_active_cells() << std::endl;
  pcout << "  Velocity DoFs = " << n_u << std::endl;
  pcout << "  Pressure DoFs = " << n_p << std::endl;
}

double
Exercise2UnsteadyStokes::pressure_data_on_boundary(
  const types::boundary_id boundary_id) const
{
  if (boundary_id == 0)
    return 1.0;
  if (boundary_id == 1)
    return 2.0;

  return 0.0;
}

void
Exercise2UnsteadyStokes::assemble()
{
  const unsigned int dofs_per_cell = fe->dofs_per_cell;
  const unsigned int n_q           = quadrature->size();
  const unsigned int n_q_face      = quadrature_face->size();

  FEValues<dim> fe_values(*fe,
                          *quadrature,
                          update_values | update_gradients |
                            update_quadrature_points | update_JxW_values);

  FEFaceValues<dim> fe_face_values(*fe,
                                   *quadrature_face,
                                   update_values | update_normal_vectors |
                                     update_JxW_values);

  FullMatrix<double> cell_matrix(dofs_per_cell, dofs_per_cell);
  FullMatrix<double> cell_pressure_mass(dofs_per_cell, dofs_per_cell);
  Vector<double>     cell_rhs(dofs_per_cell);

  std::vector<types::global_dof_index> dof_indices(dofs_per_cell);

  system_matrix = 0.0;
  pressure_mass = 0.0;
  system_rhs    = 0.0;

  const FEValuesExtractors::Vector velocity(0);
  const FEValuesExtractors::Scalar pressure(dim);

  std::vector<Tensor<1, dim>> old_velocity_values(n_q);

  for (const auto &cell : dof_handler.active_cell_iterators())
    {
      if (!cell->is_locally_owned())
        continue;

      fe_values.reinit(cell);
      fe_values[velocity].get_function_values(solution, old_velocity_values);

      cell_matrix        = 0.0;
      cell_pressure_mass = 0.0;
      cell_rhs           = 0.0;

      for (unsigned int q = 0; q < n_q; ++q)
        {
          for (unsigned int i = 0; i < dofs_per_cell; ++i)
            {
              for (unsigned int j = 0; j < dofs_per_cell; ++j)
                {
                  cell_matrix(i, j) +=
                    (1.0 / delta_t) *
                    scalar_product(fe_values[velocity].value(i, q),
                                   fe_values[velocity].value(j, q)) *
                    fe_values.JxW(q);

                  cell_matrix(i, j) +=
                    mu *
                    scalar_product(fe_values[velocity].gradient(i, q),
                                   fe_values[velocity].gradient(j, q)) *
                    fe_values.JxW(q);

                  cell_matrix(i, j) -= fe_values[velocity].divergence(i, q) *
                                       fe_values[pressure].value(j, q) *
                                       fe_values.JxW(q);

                  cell_matrix(i, j) -= fe_values[velocity].divergence(j, q) *
                                       fe_values[pressure].value(i, q) *
                                       fe_values.JxW(q);

                  cell_pressure_mass(i, j) +=
                    fe_values[pressure].value(i, q) *
                    fe_values[pressure].value(j, q) / mu * fe_values.JxW(q);
                }

              cell_rhs(i) +=
                (1.0 / delta_t) *
                scalar_product(old_velocity_values[q],
                               fe_values[velocity].value(i, q)) *
                fe_values.JxW(q);
            }
        }

      if (cell->at_boundary())
        for (unsigned int face = 0; face < cell->n_faces(); ++face)
          if (cell->face(face)->at_boundary() &&
              (cell->face(face)->boundary_id() == 0 ||
               cell->face(face)->boundary_id() == 1))
            {
              fe_face_values.reinit(cell, face);
              const double p0 =
                pressure_data_on_boundary(cell->face(face)->boundary_id());

              for (unsigned int q = 0; q < n_q_face; ++q)
                for (unsigned int i = 0; i < dofs_per_cell; ++i)
                  cell_rhs(i) +=
                    -p0 *
                    scalar_product(fe_face_values.normal_vector(q),
                                   fe_face_values[velocity].value(i, q)) *
                    fe_face_values.JxW(q);
            }

      cell->get_dof_indices(dof_indices);
      system_matrix.add(dof_indices, cell_matrix);
      pressure_mass.add(dof_indices, cell_pressure_mass);
      system_rhs.add(dof_indices, cell_rhs);
    }

  system_matrix.compress(VectorOperation::add);
  pressure_mass.compress(VectorOperation::add);
  system_rhs.compress(VectorOperation::add);

  std::map<types::global_dof_index, double> boundary_values;
  Functions::ZeroFunction<dim>              zero_function(dim + 1);

  ComponentMask velocity_mask(dim + 1, true);
  velocity_mask.set(dim, false);

  VectorTools::interpolate_boundary_values(dof_handler,
                                           2,
                                           zero_function,
                                           boundary_values,
                                           velocity_mask);
  VectorTools::interpolate_boundary_values(dof_handler,
                                           3,
                                           zero_function,
                                           boundary_values,
                                           velocity_mask);

  MatrixTools::apply_boundary_values(
    boundary_values, system_matrix, solution_owned, system_rhs, false);
}

void
Exercise2UnsteadyStokes::solve_time_step()
{
  SolverControl solver_control(2000, 1e-8 * system_rhs.l2_norm());
  SolverGMRES<TrilinosWrappers::MPI::BlockVector> solver(solver_control);

  PreconditionBlockTriangular preconditioner;
  preconditioner.initialize(system_matrix.block(0, 0),
                            pressure_mass.block(1, 1),
                            system_matrix.block(1, 0));

  solver.solve(system_matrix, solution_owned, system_rhs, preconditioner);
  pcout << "  GMRES iterations = " << solver_control.last_step() << std::endl;

  solution = solution_owned;
}

void
Exercise2UnsteadyStokes::output() const
{
  std::filesystem::create_directories("output");

  DataOut<dim> data_out;

  std::vector<DataComponentInterpretation::DataComponentInterpretation>
    interpretation(dim,
                   DataComponentInterpretation::component_is_part_of_vector);
  interpretation.push_back(DataComponentInterpretation::component_is_scalar);

  std::vector<std::string> names(dim, "velocity");
  names.push_back("pressure");

  data_out.add_data_vector(dof_handler, solution, names, interpretation);

  std::vector<unsigned int> partition_int(mesh.n_active_cells());
  GridTools::get_subdomain_association(mesh, partition_int);
  const Vector<double> partitioning(partition_int.begin(), partition_int.end());
  data_out.add_data_vector(partitioning, "partitioning");

  data_out.build_patches();
  data_out.write_vtu_with_pvtu_record("output",
                                      "exercise2_stokes",
                                      timestep_number,
                                      MPI_COMM_WORLD);
}

bool
Exercise2UnsteadyStokes::velocity_magnitude_at(const Point<dim> &point,
                                               double &magnitude) const
{
  try
    {
      Vector<double> value(dim + 1);
      VectorTools::point_value(dof_handler, solution, point, value);

      magnitude = 0.0;
      for (unsigned int d = 0; d < dim; ++d)
        magnitude += value[d] * value[d];
      magnitude = std::sqrt(magnitude);

      return true;
    }
  catch (...)
    {
      magnitude = std::numeric_limits<double>::quiet_NaN();
      return false;
    }
}

void
Exercise2UnsteadyStokes::append_probe_value() const
{
  if (mpi_rank != 0)
    return;

  std::filesystem::create_directories("output");

  const bool write_header = (timestep_number == 0);
  std::ofstream file("output/exercise2_velocity_magnitude_probe_time.csv",
                     write_header ? std::ios::out : std::ios::app);

  if (write_header)
    file << "time,x,y,velocity_magnitude\n";

  double magnitude = std::numeric_limits<double>::quiet_NaN();
  velocity_magnitude_at(Point<dim>(1.25, -0.4), magnitude);

  file << std::setprecision(16) << time << ",1.25,-0.4," << magnitude << "\n";
}

void
Exercise2UnsteadyStokes::write_final_line_profile() const
{
  if (mpi_rank != 0)
    return;

  std::filesystem::create_directories("output");

  std::ofstream file(
    "output/exercise2_velocity_magnitude_line_y_eq_x_final.csv");
  file << "x,y,velocity_magnitude\n";

  const unsigned int n_samples = 500;
  for (unsigned int i = 0; i <= n_samples; ++i)
    {
      const double s = -3.0 + 6.0 * static_cast<double>(i) / n_samples;
      double       magnitude = std::numeric_limits<double>::quiet_NaN();
      const bool inside = velocity_magnitude_at(Point<dim>(s, s), magnitude);

      if (inside)
        file << std::setprecision(16) << s << "," << s << "," << magnitude
             << "\n";
    }
}

void
Exercise2UnsteadyStokes::run()
{
  setup();

  time            = 0.0;
  timestep_number = 0;
  output();
  append_probe_value();

  while (time < final_time - 0.5 * delta_t)
    {
      time += delta_t;
      ++timestep_number;

      pcout << "Timestep " << timestep_number << ", time = " << time
            << std::endl;

      assemble();
      solve_time_step();
      output();
      append_probe_value();
    }

  write_final_line_profile();
}
