# WaveFEM Solver Commands

These commands assume the course Docker/HPC environment with deal.II modules.

Inside the `mery-polimi` Docker image, initialize the module system with:

```bash
source /u/sw/etc/profile
module load gcc-glibc/11.2.0 dealii/9.5.1
```

Do not use `module load all`: that module does not exist.

## One-Command Docker Workflow

From inside the repository folder in Docker:

```bash
bash scripts/docker_build_run.sh
```

This script:

- initializes the module system;
- loads `gcc-glibc/11.2.0` and `dealii/9.5.1`;
- configures CMake;
- builds `wave_fem`;
- runs the default validation;
- tries to create diagnostic plots if matplotlib is available.

If the project is in the shared Docker folder, a typical path is:

```bash
cd /home/jellyfish/shared-folder/WaveFEM-Solver
bash scripts/docker_build_run.sh
```

The default parameters are Mariano's current validation setup:

- `n_subdivisions = 24`;
- `degree = 1`;
- `final_time = 1.0`;
- `time_step = 0.005`;
- `output_frequency = 10`;
- `output_directory = output`.

Every teammate can override them directly:

```bash
bash scripts/docker_build_run.sh 32 1 1.0 0.0025 10 output_fine
```

or with environment variables:

```bash
N_SUBDIVISIONS=48 TIME_STEP=0.0025 OUTPUT_DIRECTORY=output_48 \
  bash scripts/docker_build_run.sh
```

To see the script options:

```bash
bash scripts/docker_build_run.sh --help
```

## Build

```bash
source /u/sw/etc/profile
module load gcc-glibc/11.2.0 dealii/9.5.1
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
source /u/sw/etc/profile
module load gcc-glibc/11.2.0 dealii/9.5.1
bash scripts/run_validation_suite.sh
```

This creates separate folders under `validation-output/` for coarse/fine mesh
and large/small timestep experiments.

The validation suite is configurable through environment variables. For example:

```bash
SPACE_NS="16 32 64" TIME_DTS="0.01 0.005 0.0025" \
  bash scripts/run_validation_suite.sh
```

Useful variables:

- `VALIDATION_OUTPUT_DIR`, default `validation-output`;
- `SPACE_NS`, default `"12 24 48"`;
- `TIME_DTS`, default `"0.02 0.01 0.005"`;
- `MAKE_PLOTS`, default `yes`.

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

Suggested screenshot folder for the report:

```text
resources/paraview-screenshots/
```

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
