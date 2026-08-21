#!/usr/bin/env bash
set -euo pipefail

# Small reproducibility helper for the report.
#
# It assumes that the project has already been built and that ./build/wave_fem
# exists. Each run writes to a different output folder so results are not
# overwritten.

executable="./build/wave_fem"

if [[ ! -x "${executable}" ]]; then
  echo "Missing ${executable}. Build first with:"
  echo "  cmake -S . -B build"
  echo "  cmake --build build -j\$(nproc)"
  exit 1
fi

rm -rf validation-output
mkdir -p validation-output

echo "Running space refinement checks"
for n in 12 24 48; do
  "${executable}" "${n}" 1 1.0 0.0025 100 "validation-output/space_N${n}"
done

echo "Running time-step refinement checks"
for dt in 0.02 0.01 0.005; do
  safe_dt="${dt/./p}"
  "${executable}" 32 1 1.0 "${dt}" 100 "validation-output/time_dt${safe_dt}"
done

echo "Validation suite completed. Inspect:"
find validation-output -name diagnostics.csv -print
