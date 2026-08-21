#!/usr/bin/env bash
set -euo pipefail

# Full Docker workflow for the mery-polimi/course image.
#
# Use this from the repository root:
#
#   cd /home/jellyfish/shared-folder/WaveFEM-Solver
#   bash scripts/docker_build_run.sh
#
# It intentionally keeps all generated files under build/ and output/, both of
# which are ignored by Git.

if [[ -f /u/sw/etc/profile ]]; then
  # The Docker image used for the course does not expose `module` until this
  # profile is sourced.
  export LOGNAME="${LOGNAME:-$(whoami)}"
  source /u/sw/etc/profile
fi

module load gcc-glibc/11.2.0 dealii/9.5.1

echo "== Cleaning old build/output =="
rm -rf build output

echo "== Configuring =="
cmake -S . -B build

echo "== Building =="
cmake --build build -j"$(nproc)"

echo "== Running default validation =="
./build/wave_fem

echo "== Generated files =="
find output -maxdepth 1 -type f | sort

if command -v python >/dev/null 2>&1; then
  echo "== Plotting diagnostics if matplotlib is available =="
  if python -c "import matplotlib" >/dev/null 2>&1; then
    python scripts/plot_diagnostics.py output/diagnostics.csv
  else
    echo "matplotlib not available; skipping PNG plots."
    echo "Install if needed with: python -m pip install --user matplotlib"
  fi
fi

echo "== Done =="
echo "Open output/wave_solution.pvd in ParaView."
