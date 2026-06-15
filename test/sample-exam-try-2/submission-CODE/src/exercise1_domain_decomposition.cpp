#include "Exercise1PoissonDD.hpp"

#include <deal.II/base/mpi.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

using namespace dealii;

namespace
{
  std::string
  lambda_label(const double lambda)
  {
    if (std::abs(lambda - 1.0) < 1e-12)
      return "1p00";
    if (std::abs(lambda - 0.25) < 1e-12)
      return "0p25";

    std::ostringstream label;
    label << std::fixed << std::setprecision(2) << lambda;
    std::string result = label.str();
    std::replace(result.begin(), result.end(), '.', 'p');
    return result;
  }

  void
  write_line_profile(const Exercise1PoissonDD &omega_1,
                     const Exercise1PoissonDD &omega_2,
                     const std::string        &label)
  {
    std::filesystem::create_directories("output");

    const std::string file_name =
      "output/exercise1_line_y_0p5_lambda_" + label + ".csv";
    std::ofstream file(file_name);
    file << "x,y,solution\n";

    const unsigned int n_samples = 300;
    for (unsigned int i = 0; i <= n_samples; ++i)
      {
        const double x = 3.0 * static_cast<double>(i) / n_samples;
        const Point<2> point(x, 0.5);

        double value = std::numeric_limits<double>::quiet_NaN();
        try
          {
            value = (x <= 1.0 ? omega_1.value_at(point) :
                                omega_2.value_at(point));
          }
        catch (...)
          {}

        file << std::setprecision(16) << x << ",0.5," << value << "\n";
      }
  }

  void
  run_for_lambda(const double lambda)
  {
    const std::string label = lambda_label(lambda);

    Exercise1PoissonDD omega_1(0, "../mesh/mesh-problem-0.msh");
    Exercise1PoissonDD omega_2(1, "../mesh/mesh-problem-1.msh");

    omega_1.setup();
    omega_2.setup();

    const unsigned int max_iterations = 15;

    for (unsigned int iteration = 1; iteration <= max_iterations; ++iteration)
      {
        Vector<double> old_solution_2 = omega_2.get_solution();

        omega_1.assemble();
        omega_1.apply_interface_dirichlet(omega_2);
        omega_1.solve();

        omega_2.assemble();
        omega_2.apply_interface_neumann(omega_1);
        omega_2.solve();
        omega_2.apply_relaxation(old_solution_2, lambda);

        old_solution_2 -= omega_2.get_solution();
        std::cout << "lambda = " << lambda << ", iteration " << iteration
                  << ", increment = " << old_solution_2.l2_norm()
                  << std::endl;
      }

    omega_1.output(label, max_iterations);
    omega_2.output(label, max_iterations);
    write_line_profile(omega_1, omega_2, label);
  }
}

int
main(int argc, char *argv[])
{
  Utilities::MPI::MPI_InitFinalize mpi_init(argc, argv);

  std::filesystem::create_directories("output");

  run_for_lambda(1.0);
  run_for_lambda(0.25);

  return 0;
}
