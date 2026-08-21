#!/usr/bin/env bash
set -euo pipefail

# Full Docker workflow for the mery-polimi/course image.
#
# Use this from the repository root:
#
#   cd /home/jellyfish/shared-folder/WaveFEM-Solver
#   bash scripts/docker_build_run.sh
#
# Optional arguments:
#
#   bash scripts/docker_build_run.sh [n_subdivisions] [degree] [final_time] \
#     [time_step] [output_frequency] [output_directory]
#
# The defaults below are Mariano's current validation parameters.
#
# It intentionally keeps all generated files under build/ and output/, both of
# which are ignored by Git.

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd "${script_dir}/.." && pwd)"

source "${script_dir}/common.sh"

usage() {
  cat <<EOF
Usage:
  bash scripts/docker_build_run.sh [n_subdivisions] [degree] [final_time] [time_step] [output_frequency] [output_directory]

Defaults:
  n_subdivisions   = ${N_SUBDIVISIONS:-24}
  degree           = ${FE_DEGREE:-1}
  final_time       = ${FINAL_TIME:-1.0}
  time_step        = ${TIME_STEP:-0.005}
  output_frequency = ${OUTPUT_FREQUENCY:-10}
  output_directory = ${OUTPUT_DIRECTORY:-output}

The same values can also be set with environment variables:
  N_SUBDIVISIONS FE_DEGREE FINAL_TIME TIME_STEP OUTPUT_FREQUENCY OUTPUT_DIRECTORY

Examples:
  bash scripts/docker_build_run.sh
  bash scripts/docker_build_run.sh 32 1 1.0 0.0025 10 output_fine
  N_SUBDIVISIONS=48 TIME_STEP=0.0025 bash scripts/docker_build_run.sh
EOF
}

if [[ "${1:-}" == "--help" || "${1:-}" == "-h" ]]; then
  usage
  exit 0
fi

if [[ "$#" -gt 6 ]]; then
  usage
  exit 1
fi

cd "${repo_root}"

n_subdivisions="${1:-${N_SUBDIVISIONS:-24}}"
fe_degree="${2:-${FE_DEGREE:-1}}"
final_time="${3:-${FINAL_TIME:-1.0}}"
time_step="${4:-${TIME_STEP:-0.005}}"
output_frequency="${5:-${OUTPUT_FREQUENCY:-10}}"
output_directory="${6:-${OUTPUT_DIRECTORY:-output}}"

load_course_modules

echo "== Parameters =="
echo "n_subdivisions   = ${n_subdivisions}"
echo "degree           = ${fe_degree}"
echo "final_time       = ${final_time}"
echo "time_step        = ${time_step}"
echo "output_frequency = ${output_frequency}"
echo "output_directory = ${output_directory}"

echo "== Cleaning old build and selected output directory =="
rm -rf build "${output_directory}"

echo "== Configuring =="
cmake -S . -B build

echo "== Building =="
cmake --build build -j"$(nproc)"

echo "== Running validation =="
./build/wave_fem \
  "${n_subdivisions}" \
  "${fe_degree}" \
  "${final_time}" \
  "${time_step}" \
  "${output_frequency}" \
  "${output_directory}"

echo "== Generated files =="
find "${output_directory}" -maxdepth 1 -type f | sort

echo "== Plotting diagnostics if matplotlib is available =="
plot_diagnostics_if_possible "${output_directory}/diagnostics.csv"

echo "== Done =="
echo "Open ${output_directory}/wave_solution.pvd in ParaView."
