#ifndef EXERCISE2_UNSTEADY_STOKES_HPP
#define EXERCISE2_UNSTEADY_STOKES_HPP

#include <deal.II/base/conditional_ostream.h>
#include <deal.II/base/function.h>
#include <deal.II/base/mpi.h>
#include <deal.II/base/quadrature_lib.h>

#include <deal.II/distributed/fully_distributed_tria.h>

#include <deal.II/dofs/dof_handler.h>
#include <deal.II/dofs/dof_renumbering.h>
#include <deal.II/dofs/dof_tools.h>

#include <deal.II/fe/fe_simplex_p.h>
#include <deal.II/fe/fe_system.h>
#include <deal.II/fe/fe_values.h>
#include <deal.II/fe/fe_values_extractors.h>

#include <deal.II/grid/grid_in.h>
#include <deal.II/grid/grid_tools.h>
#include <deal.II/grid/tria_description.h>

#include <deal.II/lac/solver_cg.h>
#include <deal.II/lac/solver_gmres.h>
#include <deal.II/lac/trilinos_block_sparse_matrix.h>
#include <deal.II/lac/trilinos_parallel_block_vector.h>
#include <deal.II/lac/trilinos_precondition.h>
#include <deal.II/lac/trilinos_sparse_matrix.h>

#include <deal.II/numerics/data_out.h>
#include <deal.II/numerics/matrix_tools.h>
#include <deal.II/numerics/vector_tools.h>

#include <fstream>
#include <memory>
#include <string>
#include <vector>

using namespace dealii;

class Exercise2UnsteadyStokes
{
public:
  static constexpr unsigned int dim = 2;

  class PreconditionBlockTriangular
  {
  public:
    void
    initialize(const TrilinosWrappers::SparseMatrix &velocity_block_,
               const TrilinosWrappers::SparseMatrix &pressure_mass_,
               const TrilinosWrappers::SparseMatrix &B_);

    void
    vmult(TrilinosWrappers::MPI::BlockVector       &dst,
          const TrilinosWrappers::MPI::BlockVector &src) const;

  private:
    const TrilinosWrappers::SparseMatrix *velocity_block = nullptr;
    const TrilinosWrappers::SparseMatrix *pressure_mass  = nullptr;
    const TrilinosWrappers::SparseMatrix *B              = nullptr;

    TrilinosWrappers::PreconditionILU preconditioner_velocity;
    TrilinosWrappers::PreconditionILU preconditioner_pressure;

    mutable TrilinosWrappers::MPI::Vector tmp;
  };

  Exercise2UnsteadyStokes(const std::string  &mesh_file_name_,
                          const unsigned int degree_velocity_,
                          const unsigned int degree_pressure_,
                          const double       final_time_,
                          const double       delta_t_);

  void
  run();

private:
  void
  setup();

  void
  assemble();

  void
  solve_time_step();

  void
  output() const;

  void
  append_probe_value() const;

  void
  write_final_line_profile() const;

  bool
  velocity_magnitude_at(const Point<dim> &point, double &magnitude) const;

  double
  pressure_data_on_boundary(const types::boundary_id boundary_id) const;

  const std::string  mesh_file_name;
  const unsigned int degree_velocity;
  const unsigned int degree_pressure;
  const double       final_time;
  const double       delta_t;

  const double mu = 1.0;

  double       time = 0.0;
  unsigned int timestep_number = 0;

  const unsigned int mpi_size;
  const unsigned int mpi_rank;
  ConditionalOStream pcout;

  parallel::fullydistributed::Triangulation<dim> mesh;

  std::unique_ptr<FiniteElement<dim>> fe;
  std::unique_ptr<Quadrature<dim>>    quadrature;
  std::unique_ptr<Quadrature<dim - 1>> quadrature_face;

  DoFHandler<dim> dof_handler;

  IndexSet locally_owned_dofs;
  IndexSet locally_relevant_dofs;
  std::vector<IndexSet> block_owned_dofs;
  std::vector<IndexSet> block_relevant_dofs;

  TrilinosWrappers::BlockSparseMatrix system_matrix;
  TrilinosWrappers::BlockSparseMatrix pressure_mass;

  TrilinosWrappers::MPI::BlockVector system_rhs;
  TrilinosWrappers::MPI::BlockVector solution_owned;
  TrilinosWrappers::MPI::BlockVector solution;
};

#endif
