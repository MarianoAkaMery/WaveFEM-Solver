#include "Exercise2UnsteadyStokes.hpp"

#include <deal.II/base/mpi.h>

using namespace dealii;

int
main(int argc, char *argv[])
{
  Utilities::MPI::MPI_InitFinalize mpi_init(argc, argv);

  Exercise2UnsteadyStokes problem("../mesh/mesh-pipe.msh",
                                  /*degree_velocity=*/2,
                                  /*degree_pressure=*/1,
                                  /*final_time=*/1.0,
                                  /*delta_t=*/0.02);

  problem.run();

  return 0;
}
