#ifndef WAVE_EQUATION_HPP
#define WAVE_EQUATION_HPP

#include <deal.II/base/function.h>
#include <deal.II/base/function_lib.h>
#include <deal.II/base/numbers.h>
#include <deal.II/base/quadrature_lib.h>

#include <deal.II/dofs/dof_handler.h>
#include <deal.II/dofs/dof_tools.h>

#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_values.h>

#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/grid_out.h>
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

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using namespace dealii;

/**
 * Finite element solver for the 2D wave equation
 *
 *     u_tt - Delta u = f      in Omega = (0,1)^2,
 *     u = 0                  on boundary,
 *     u(0) = u0, u_t(0) = u1.
 *
 * The class follows the style used in the course labs: setup(), assemble(),
 * solve/output style methods, deal.II vectors and sparse matrices, and explicit
 * logging of each major phase.
 */
class WaveEquation
{
public:
  // Physical dimension of the project problem.
  static constexpr unsigned int dim = 2;

  /**
   * Exact manufactured solution used for validation:
   *
   *   u(x,y,t) = sin(pi x) sin(pi y) cos(sqrt(2) pi t).
   *
   * For this function, u_tt - Delta u = 0, and u is zero on the boundary of
   * the unit square. This gives a clean first test with known answer.
   */
  class ExactSolution : public Function<dim>
  {
  public:
    ExactSolution() = default;

    virtual double
    value(const Point<dim> &p,
          const unsigned int component = 0) const override;

    virtual Tensor<1, dim>
    gradient(const Point<dim> &p,
             const unsigned int component = 0) const override;
  };

  /**
   * Initial velocity for the default validation test.
   *
   * The exact solution above has u_t(x,y,0) = 0, so this function is just zero.
   * We still keep it explicit because the wave equation always needs both an
   * initial displacement and an initial velocity.
   */
  class InitialVelocity : public Function<dim>
  {
  public:
    InitialVelocity() = default;

    virtual double
    value(const Point<dim> &p,
          const unsigned int component = 0) const override;
  };

  // Constructor collecting the numerical parameters of one simulation.
  WaveEquation(const unsigned int &n_subdivisions_,
               const unsigned int &fe_degree_,
               const double       &final_time_,
               const double       &time_step_,
               const unsigned int &output_frequency_,
               const std::string  &output_directory_);

  // Run the full simulation.
  void
  run();

protected:
  // Create mesh, finite element space, DoFs, sparsity pattern and vectors.
  void
  setup();

  // Assemble the time-independent FEM matrices M, K and Newmark matrix.
  void
  assemble_matrices();

  // Interpolate u0 and u1 and compute the initial acceleration.
  void
  initialize_solution();

  // Solve M a(0) = F(0) - K u(0) to initialize the second-order problem.
  void
  solve_initial_acceleration();

  // Compute one Newmark time step.
  void
  solve_time_step();

  // Solve a symmetric positive definite sparse linear system with CG.
  void
  solve_linear_system(const SparseMatrix<double> &matrix,
                      Vector<double>             &unknown,
                      const Vector<double>       &rhs,
                      const std::string          &label) const;

  // Write the current displacement/velocity/acceleration fields for ParaView.
  void
  output_results();

  // Rewrite the ParaView time-series collection file.
  void
  write_pvd_record() const;

  // Compute L2 and H1-seminorm error against the manufactured solution.
  std::pair<double, double>
  compute_errors() const;

  // Compute discrete mechanical energy: 1/2 v^T M v + 1/2 u^T K u.
  double
  compute_energy() const;

  // Append one row to output/diagnostics.csv.
  void
  append_diagnostics();

  // Build the forcing vector F(t). The default manufactured test has F = 0.
  void
  assemble_force(const double &time, Vector<double> &force) const;

  // Numerical parameters.
  const unsigned int n_subdivisions;
  const unsigned int fe_degree;
  const double       final_time;
  const double       time_step;
  const unsigned int output_frequency;
  const std::string  output_directory;

  // Newmark average acceleration parameters. beta=1/4, gamma=1/2 is a standard
  // second-order, unconditionally stable choice for linear undamped waves.
  const double beta  = 0.25;
  const double gamma = 0.5;

  // Current time and step number.
  double       time            = 0.0;
  unsigned int timestep_number = 0;

  // Mesh and finite element objects.
  Triangulation<dim>            mesh;
  std::unique_ptr<FiniteElement<dim>> fe;
  std::unique_ptr<Quadrature<dim>>    quadrature;
  DoFHandler<dim>                    dof_handler;

  // Matrix graph and FEM matrices.
  SparsityPattern     sparsity_pattern;
  SparseMatrix<double> mass_matrix;
  SparseMatrix<double> stiffness_matrix;
  SparseMatrix<double> newmark_matrix;
  SparseMatrix<double> mass_matrix_constrained;

  // Unknown vectors for the second-order-in-time problem.
  Vector<double> displacement;
  Vector<double> velocity;
  Vector<double> acceleration;
  Vector<double> system_rhs;

  // Boundary DoF values for homogeneous Dirichlet conditions.
  std::map<types::global_dof_index, double> boundary_values;

  // ParaView collection entries: (time, file name).
  std::vector<std::pair<double, std::string>> pvd_entries;
};

#endif
