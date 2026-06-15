#ifndef EXERCISE1_POISSON_DD_HPP
#define EXERCISE1_POISSON_DD_HPP

#include <deal.II/base/function.h>
#include <deal.II/base/quadrature_lib.h>

#include <deal.II/dofs/dof_handler.h>
#include <deal.II/dofs/dof_tools.h>

#include <deal.II/fe/fe_simplex_p.h>
#include <deal.II/fe/fe_values.h>
#include <deal.II/fe/mapping_fe.h>

#include <deal.II/grid/grid_in.h>
#include <deal.II/grid/tria.h>

#include <deal.II/lac/dynamic_sparsity_pattern.h>
#include <deal.II/lac/full_matrix.h>
#include <deal.II/lac/precondition.h>
#include <deal.II/lac/solver_cg.h>
#include <deal.II/lac/sparse_matrix.h>
#include <deal.II/lac/vector.h>

#include <deal.II/numerics/data_out.h>
#include <deal.II/numerics/matrix_tools.h>
#include <deal.II/numerics/vector_tools.h>

#include <fstream>
#include <map>
#include <memory>
#include <set>
#include <string>

using namespace dealii;

class Exercise1PoissonDD
{
public:
  static constexpr unsigned int dim = 2;

  Exercise1PoissonDD(const unsigned int subdomain_id_,
                     const std::string &mesh_file_name_);

  void
  setup();

  void
  assemble();

  void
  solve();

  void
  output(const std::string &lambda_label,
         const unsigned int iteration) const;

  void
  apply_interface_dirichlet(const Exercise1PoissonDD &other);

  void
  apply_interface_neumann(Exercise1PoissonDD &other);

  void
  apply_relaxation(const Vector<double> &old_solution, const double lambda);

  double
  value_at(const Point<dim> &point) const;

  const Vector<double> &
  get_solution() const;

private:
  std::map<types::global_dof_index, types::global_dof_index>
  compute_interface_map(const Exercise1PoissonDD &other) const;

  std::set<types::boundary_id>
  external_boundary_ids() const;

  const unsigned int subdomain_id;
  const std::string  mesh_file_name;

  const double alpha = 1.0;
  const double gamma = 1.0;
  const double rhs_value = 1.0;

  Triangulation<dim> mesh;
  std::map<types::global_dof_index, Point<dim>> support_points;

  std::unique_ptr<FiniteElement<dim>> fe;
  std::unique_ptr<Quadrature<dim>>    quadrature;

  DoFHandler<dim> dof_handler;

  SparsityPattern      sparsity_pattern;
  SparseMatrix<double> system_matrix;
  Vector<double>       system_rhs;
  Vector<double>       solution;
};

#endif
