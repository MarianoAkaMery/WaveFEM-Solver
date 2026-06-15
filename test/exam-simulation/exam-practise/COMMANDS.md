# Commands

Run these commands from the exam root folder, the folder that contains `Exam.pdf` and `exam-practise/`.

If your environment uses modules, load deal.II before CMake, for example:

```bash
module load dealii
```

## Check Meshes

The `.msh` files are already in `exam-practise/mesh/`.

```bash
cd exam-practise
ls -lh mesh/*.msh
```

Expected files:

```text
mesh/mesh-problem-0.msh
mesh/mesh-problem-1.msh
mesh/mesh-pipe.msh
```

If you need to re-download them:

```bash
curl -L -o mesh/mesh-problem-0.msh https://raw.githubusercontent.com/michelebucelli/nmpde-labs-aa-25-26/main/lab-08/mesh/mesh-problem-0.msh
curl -L -o mesh/mesh-problem-1.msh https://raw.githubusercontent.com/michelebucelli/nmpde-labs-aa-25-26/main/lab-08/mesh/mesh-problem-1.msh
curl -L -o mesh/mesh-pipe.msh https://raw.githubusercontent.com/michelebucelli/nmpde-labs-aa-24-25/main/examples/gmsh/mesh-pipe.msh
```

## Build

```bash
rm -rf build
cmake -S . -B build
cmake --build build -j$(nproc)
```

Equivalent classic build style:

```bash
mkdir -p build
cd build
cmake ..
make -j$(nproc)
cd ..
```

Do not run `cmake ..` from inside `exam-practise/`; it is valid only from `exam-practise/build/`.

## Run

```bash
cd build
./exercise1_domain_decomposition
./exercise2_unsteady_stokes
cd ..
```

If the Stokes executable must be launched through MPI in your environment:

```bash
cd build
mpirun -np 1 ./exercise2_unsteady_stokes
cd ..
```

## Create PNG Plots From CSV Files

This plotting script uses `matplotlib`. If the environment has an old `numpy`,
upgrade the user Python packages first:

```bash
python -m pip install --user --upgrade "numpy>=1.23,<2" matplotlib
```

```bash
python scripts/plot_csv_profiles.py output
```

## Inspect Generated Files

```bash
find output -maxdepth 2 -type f
```

## Prepare Deliverables Folder

Optional but recommended: keep only the files needed for plots/submission.

```bash
rm -rf deliverables
mkdir -p deliverables

cp output/exercise1_lambda_1p00_subdomain_0_iter_15.vtk deliverables/
cp output/exercise1_lambda_1p00_subdomain_1_iter_15.vtk deliverables/
cp output/exercise1_lambda_0p25_subdomain_0_iter_15.vtk deliverables/
cp output/exercise1_lambda_0p25_subdomain_1_iter_15.vtk deliverables/

cp output/exercise2_stokes_50.pvtu deliverables/
cp output/exercise2_stokes_50.0.vtu deliverables/

cp output/exercise1_line_y_0p5.png deliverables/
cp output/exercise2_velocity_magnitude_line_y_eq_x_final.png deliverables/
cp output/exercise2_velocity_magnitude_probe_time.png deliverables/

ls -lh deliverables
```

## ParaView Guide

Exercise 1, final solution for `lambda = 1.0`:

- Open both files together:
  - `deliverables/exercise1_lambda_1p00_subdomain_0_iter_15.vtk`
  - `deliverables/exercise1_lambda_1p00_subdomain_1_iter_15.vtk`
- In ParaView, select both objects and color by `solution`.
- Export screenshot as `exercise1_solution_lambda_1p00_final.png`.

Exercise 1, final solution for `lambda = 0.25`:

- Open both files together:
  - `deliverables/exercise1_lambda_0p25_subdomain_0_iter_15.vtk`
  - `deliverables/exercise1_lambda_0p25_subdomain_1_iter_15.vtk`
- In ParaView, select both objects and color by `solution`.
- Export screenshot as `exercise1_solution_lambda_0p25_final.png`.

Exercise 2, final solution at `T = 1`:

- Open:
  - `deliverables/exercise2_stokes_50.pvtu`
- Keep `deliverables/exercise2_stokes_50.0.vtu` in the same folder, because the `.pvtu` references it.
- Color by velocity magnitude if available, or use the Calculator filter with `sqrt(velocity_X^2 + velocity_Y^2)`.
- Export screenshot as `exercise2_solution_final.png`.

Already generated with Python/matplotlib, no ParaView needed:

```text
deliverables/exercise1_line_y_0p5.png
deliverables/exercise2_velocity_magnitude_line_y_eq_x_final.png
deliverables/exercise2_velocity_magnitude_probe_time.png
```
