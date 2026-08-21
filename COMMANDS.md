# WaveFEM Solver Commands

These commands assume the course Docker/HPC environment with deal.II modules.

## Build

```bash
module load gcc-glibc dealii
rm -rf build
cmake -S . -B build
cmake --build build -j$(nproc)
```

## Run Default Validation

```bash
./build/wave_fem
```

This uses:

- `n_subdivisions = 24`;
- `degree = 1`;
- `final_time = 1.0`;
- `time_step = 0.005`;
- `output_frequency = 10`.

## Run With Custom Parameters

```bash
./build/wave_fem 32 1 1.0 0.0025 10 output
```

Argument order:

```text
wave_fem [n_subdivisions] [degree] [final_time] [time_step] [output_frequency] [output_directory]
```

## Run A Small Validation Suite

```bash
bash scripts/run_validation_suite.sh
```

This creates separate folders under `validation-output/` for coarse/fine mesh
and large/small timestep experiments.

## Plot Diagnostics

```bash
python scripts/plot_diagnostics.py output/diagnostics.csv
```

If matplotlib is missing:

```bash
python -m pip install --user matplotlib
```

## Files To Open In ParaView

Open:

```text
output/wave_solution.pvd
```

Useful fields:

- `u`: displacement, the main wave solution;
- `velocity`: time derivative;
- `acceleration`: second time derivative.

Suggested ParaView checks:

- color by `u`;
- use `Warp By Scalar` with scalar `u` to see the membrane move;
- play the time animation;
- check that the boundary remains fixed at zero;
- compare coarse/fine runs by changing `n_subdivisions`.

## Expected Generated Files

```text
output/mesh.vtk
output/wave_solution.pvd
output/wave_solution_0000.vtu
output/wave_solution_0010.vtu
...
output/diagnostics.csv
output/errors.png
output/energy.png
```

The generated `output/` folder is not meant to be committed.
