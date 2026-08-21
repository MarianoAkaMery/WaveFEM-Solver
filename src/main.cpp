#include "WaveEquation.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

namespace
{
void
print_usage(const char *program_name)
{
  std::cout
    << "Usage:\n"
    << "  " << program_name
    << " [n_subdivisions] [degree] [final_time] [time_step] "
       "[output_frequency] [output_directory]\n\n"
    << "Defaults:\n"
    << "  n_subdivisions   = 24\n"
    << "  degree           = 1\n"
    << "  final_time       = 1.0\n"
    << "  time_step        = 0.005\n"
    << "  output_frequency = 10\n"
    << "  output_directory = output\n";
}
} // namespace

int
main(int argc, char *argv[])
{
  // Default parameters. They are intentionally modest so the first run is quick
  // in the course Docker environment.
  unsigned int n_subdivisions   = 24;
  unsigned int degree           = 1;
  double       final_time       = 1.0;
  double       time_step        = 0.005;
  unsigned int output_frequency = 10;
  std::string  output_directory = "output";

  if (argc == 2 && std::string(argv[1]) == "--help")
    {
      print_usage(argv[0]);
      return 0;
    }

  if (argc > 1)
    n_subdivisions = static_cast<unsigned int>(std::stoul(argv[1]));
  if (argc > 2)
    degree = static_cast<unsigned int>(std::stoul(argv[2]));
  if (argc > 3)
    final_time = std::stod(argv[3]);
  if (argc > 4)
    time_step = std::stod(argv[4]);
  if (argc > 5)
    output_frequency = static_cast<unsigned int>(std::stoul(argv[5]));
  if (argc > 6)
    output_directory = argv[6];

  if (argc > 7)
    {
      print_usage(argv[0]);
      return 1;
    }

  WaveEquation problem(n_subdivisions,
                       degree,
                       final_time,
                       time_step,
                       output_frequency,
                       output_directory);

  problem.run();

  return 0;
}
