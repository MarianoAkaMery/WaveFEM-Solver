#include "Exercise1PoissonDD.hpp"

#include <fstream>
#include <filesystem>
#include <iostream>

Exercise1PoissonDD::Exercise1PoissonDD(const unsigned int subdomain_id_,
                                       const std::string &mesh_file_name_)
  : subdomain_id(subdomain_id_)
  , mesh_file_name(mesh_file_name_)
{}

void
Exercise1PoissonDD::setup()
{
  GridIn<dim> grid_in;
  grid_in.attach_triangulation(mesh);

  std::ifstream grid_in_file(mesh_file_name);
  AssertThrow(grid_in_file,
              ExcMessage("Could not open mesh file: " + mesh_file_name));
  grid_in.read_msh(grid_in_file);

  fe         = std::make_unique<FE_SimplexP<dim>>(1);
  quadrature = std::make_unique<QGaussSimplex<dim>>(2);

  dof_handler.reinit(mesh);
  dof_handler.distribute_dofs(*fe);

  const FE_SimplexP<dim> fe_linear(1);
  const MappingFE<dim>   mapping(fe_linear);
  support_points = DoFTools::map_dofs_to_support_points(mapping, dof_handler);

  DynamicSparsityPattern dsp(dof_handler.n_dofs());
  DoFTools::make_sparsity_pattern(dof_handler, dsp);
  sparsity_pattern.copy_from(dsp);

  system_matrix.reinit(sparsity_pattern);
  system_rhs.reinit(dof_handler.n_dofs());
  solution.reinit(dof_handler.n_dofs());
}

std::set<types::boundary_id>
Exercise1PoissonDD::external_boundary_ids() const
{
  if (subdomain_id == 0)
    return {0, 2, 3}; // x=0, y=0, y=1.

  return {1, 2, 3}; // x=3, y=0, y=1.
}

void
Exercise1PoissonDD::assemble()
{
  const unsigned int dofs_per_cell = fe->dofs_per_cell;
  const unsigned int n_q           = quadrature->size();

  FEValues<dim> fe_values(*fe,
                          *quadrature,
                          update_values | update_gradients |
                            update_JxW_values);

  FullMatrix<double> cell_matrix(dofs_per_cell, dofs_per_cell);
  Vector<double>     cell_rhs(dofs_per_cell);
  std::vector<types::global_dof_index> dof_indices(dofs_per_cell);

  system_matrix = 0.0;
  system_rhs    = 0.0;

  for (const auto &cell : dof_handler.active_cell_iterators())
    {
      fe_values.reinit(cell);
      cell_matrix = 0.0;
      cell_rhs    = 0.0;

      for (unsigned int q = 0; q < n_q; ++q)
        {
          for (unsigned int i = 0; i < dofs_per_cell; ++i)
            {
              for (unsigned int j = 0; j < dofs_per_cell; ++j)
                {
                  cell_matrix(i, j) +=
                    alpha *
                      scalar_product(fe_values.shape_grad(i, q),
                                     fe_values.shape_grad(j, q)) *
                      fe_values.JxW(q) +
                    gamma * fe_values.shape_value(i, q) *
                      fe_values.shape_value(j, q) * fe_values.JxW(q);
                }

              cell_rhs(i) += rhs_value * fe_values.shape_value(i, q) *
                             fe_values.JxW(q);
            }
        }

      cell->get_dof_indices(dof_indices);
      system_matrix.add(dof_indices, cell_matrix);
      system_rhs.add(dof_indices, cell_rhs);
    }

  std::map<types::global_dof_index, double> boundary_values;
  Functions::ZeroFunction<dim>              zero_function;

  for (const auto boundary_id : external_boundary_ids())
    VectorTools::interpolate_boundary_values(dof_handler,
                                             boundary_id,
                                             zero_function,
                                             boundary_values);

  MatrixTools::apply_boundary_values(
    boundary_values, system_matrix, solution, system_rhs, false);
}

void
Exercise1PoissonDD::solve()
{
  SolverControl solver_control(1000, 1e-12 * system_rhs.l2_norm());
  SolverCG<Vector<double>> solver(solver_control);
  solver.solve(system_matrix, solution, system_rhs, PreconditionIdentity());
}

void
Exercise1PoissonDD::output(const std::string &lambda_label,
                           const unsigned int iteration) const
{
  std::filesystem::create_directories("../output");

  DataOut<dim> data_out;
  data_out.add_data_vector(dof_handler, solution, "solution");
  data_out.build_patches();

  const std::string output_file_name =
    "../output/exercise1_lambda_" + lambda_label + "_subdomain_" +
    std::to_string(subdomain_id) + "_iter_" + std::to_string(iteration) +
    ".vtk";

  std::ofstream output_file(output_file_name);
  data_out.write_vtk(output_file);
}

void
Exercise1PoissonDD::apply_interface_dirichlet(const Exercise1PoissonDD &other)
{
  const auto interface_map = compute_interface_map(other);

  std::map<types::global_dof_index, double> boundary_values;
  for (const auto &dof_pair : interface_map)
    boundary_values[dof_pair.first] = other.solution[dof_pair.second];

  MatrixTools::apply_boundary_values(
    boundary_values, system_matrix, solution, system_rhs, false);
}

void
Exercise1PoissonDD::apply_interface_neumann(Exercise1PoissonDD &other)
{
  const auto interface_map = compute_interface_map(other);

  Vector<double> interface_residual;
  other.assemble();
  interface_residual = other.system_rhs;
  interface_residual *= -1.0;
  other.system_matrix.vmult_add(interface_residual, other.solution);

  for (const auto &dof_pair : interface_map)
    system_rhs[dof_pair.first] -= interface_residual[dof_pair.second];
}

std::map<types::global_dof_index, types::global_dof_index>
Exercise1PoissonDD::compute_interface_map(const Exercise1PoissonDD &other) const
{
  IndexSet current_interface_dofs;
  IndexSet other_interface_dofs;

  if (subdomain_id == 0)
    {
      current_interface_dofs =
        DoFTools::extract_boundary_dofs(dof_handler, ComponentMask(), {1});
      other_interface_dofs = DoFTools::extract_boundary_dofs(other.dof_handler,
                                                             ComponentMask(),
                                                             {0});
    }
  else
    {
      current_interface_dofs =
        DoFTools::extract_boundary_dofs(dof_handler, ComponentMask(), {0});
      other_interface_dofs = DoFTools::extract_boundary_dofs(other.dof_handler,
                                                             ComponentMask(),
                                                             {1});
    }

  std::map<types::global_dof_index, types::global_dof_index> interface_map;
  for (const auto &dof_current : current_interface_dofs)
    {
      const Point<dim> &point_current = support_points.at(dof_current);

      types::global_dof_index nearest = *other_interface_dofs.begin();
      for (const auto &dof_other : other_interface_dofs)
        if (point_current.distance_square(other.support_points.at(dof_other)) <
            point_current.distance_square(other.support_points.at(nearest)))
          nearest = dof_other;

      interface_map[dof_current] = nearest;
    }

  return interface_map;
}

void
Exercise1PoissonDD::apply_relaxation(const Vector<double> &old_solution,
                                     const double          lambda)
{
  solution *= lambda;
  solution.add(1.0 - lambda, old_solution);
}

double
Exercise1PoissonDD::value_at(const Point<dim> &point) const
{
  return VectorTools::point_value(dof_handler, solution, point);
}

const Vector<double> &
Exercise1PoissonDD::get_solution() const
{
  return solution;
}
