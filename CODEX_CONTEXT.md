# Codex Handoff Context

This file exists so a new Codex session opened inside the Docker/shared folder can
continue from the current project state without needing the old chat history.

## Current Goal

Project 2 for Numerical Methods for PDE:

> Implement and explain a finite element solver for the 2D wave equation.

The group is made of computer science students with weak PDE background, so the
codebase should stay clean, heavily understandable, and close to the course lab
style.

## Repository State

Main repository:

```text
https://github.com/MarianoAkaMery/WaveFEM-Solver.git
```

Docker/shared-folder clone:

```bash
cd /home/jellyfish/shared-folder/WaveFEM-Solver
```

Windows/local working folder:

```text
D:\Dekstop\PDE\PROJECT - 058168 - Numerical Methods for PDE - Quarteroni - 2025-26
```

The Docker container used so far is named:

```text
mery-polimi
```

Inside Docker, the shared folder is:

```text
/home/jellyfish/shared-folder
```

On Windows, the same shared folder is:

```text
C:\Users\maria\Documents\sharedDocker
```

## What Has Been Implemented

Initial working codebase:

- `CMakeLists.txt`
- `src/main.cpp`
- `src/WaveEquation.hpp`
- `src/WaveEquation.cpp`
- `scripts/docker_build_run.sh`
- `scripts/run_validation_suite.sh`
- `scripts/plot_diagnostics.py`
- `COMMANDS.md`
- `project-intuition-guide/`

The solver currently does:

- 2D wave equation on the unit square `(0,1)^2`;
- homogeneous Dirichlet boundary conditions;
- finite element space with `FE_Q`;
- Newmark average acceleration time method:
  - `beta = 1/4`;
  - `gamma = 1/2`;
- manufactured exact solution:

```math
u(x,y,t) = \sin(\pi x)\sin(\pi y)\cos(\sqrt{2}\pi t)
```

- force `f = 0`;
- initial displacement from exact solution at `t=0`;
- initial velocity `u_t(0)=0`;
- output for ParaView:
  - `output/wave_solution.pvd`;
  - `output/wave_solution_XXXX.vtu`;
- diagnostics:
  - `output/diagnostics.csv`;
  - `output/errors.png`;
  - `output/energy.png`.

## Verified Docker Commands

The correct module setup inside `mery-polimi` is:

```bash
source /u/sw/etc/profile
module load gcc-glibc/11.2.0 dealii/9.5.1
```

Do not use:

```bash
module load all
```

because that module does not exist.

The easy command to build and run everything is:

```bash
cd /home/jellyfish/shared-folder/WaveFEM-Solver
bash scripts/docker_build_run.sh
```

This was tested successfully in Docker. It configured CMake, built `wave_fem`,
ran the default validation, generated ParaView files, and generated diagnostic
plots.

Manual build/run equivalent:

```bash
cd /home/jellyfish/shared-folder/WaveFEM-Solver
source /u/sw/etc/profile
module load gcc-glibc/11.2.0 dealii/9.5.1
rm -rf build
cmake -S . -B build
cmake --build build -j$(nproc)
./build/wave_fem
python scripts/plot_diagnostics.py output/diagnostics.csv
```

Open in ParaView:

```text
output/wave_solution.pvd
```

## Important Design Choices

The current code intentionally starts simple:

- generated unit-square mesh instead of external `.msh`;
- serial deal.II code instead of MPI parallel code;
- one manufactured solution for validation;
- Newmark implicit method for robustness;
- comments explain each major code section.

This is a good base implementation, but it is not the final project yet.

## What To Do Next

Recommended next steps:

1. Run and visually inspect `output/wave_solution.pvd` in ParaView.
2. Run `bash scripts/run_validation_suite.sh` inside Docker.
3. Check that errors decrease when mesh/time step are refined.
4. Add a cleaner summary table generator for validation results.
5. Decide whether to add:
   - comparison with a second time method;
   - `P2` finite elements;
   - nonzero forcing;
   - non-homogeneous boundary conditions;
   - more interesting initial data.
6. Start drafting the report sections:
   - PDE and weak form;
   - FEM discretization;
   - Newmark time discretization;
   - stability/dissipation/dispersion;
   - validation results;
   - computational aspects.

## Coding Style To Preserve

Use the course lab style:

- class-based solver;
- methods like `setup`, `assemble_matrices`, `solve_time_step`, `output_results`;
- deal.II objects and naming conventions similar to lab code;
- comments before meaningful blocks, not noisy comments on every line;
- keep generated files out of Git.

## Prompt For A New Codex Session

Paste this in the new Codex session opened from the Docker repo folder:

```text
We are working on Project 2 of Numerical Methods for PDE: a FEM solver for the
2D wave equation. Read CODEX_CONTEXT.md, README.md, COMMANDS.md, and the files
in src/. Continue from the current codebase.

Important:
- Use the course lab style from Laboratory Lectures.
- Build and run inside Docker with:
  bash scripts/docker_build_run.sh
- The current solver already builds and runs in mery-polimi.
- Keep the code readable for CS students with weak PDE background.
- Preserve explanatory comments for each major section.
- Do not add unrelated refactors.

Next task: inspect the current solver, run validation, and improve the project
towards a complete report-ready WaveFEM implementation.
```
