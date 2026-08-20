# WaveFEM Solver

Finite element solver for the two-dimensional wave equation, developed for the course **058168 - Numerical Methods for Partial Differential Equations**.

## Team

- Salvatore Mariano Librici - `salvatoremariano.librici@mail.polimi.it`
- Rong Huang - `rong.huang@mail.polimi.it`
- KHirdesh Kumar - `hirdesh.kumar@mail.polimi.it`
- Mehdi Ghiasipour - `mehdi.ghiasipour@mail.polimi.it`

## Project Goal

The goal is to implement a finite element solver for the 2D wave equation:

```math
\frac{\partial^2 u}{\partial t^2} - \Delta u = f \quad \text{in } \Omega,
```

with Dirichlet boundary conditions:

```math
u = g \quad \text{on } \partial\Omega,
```

and initial conditions:

```math
u(0) = u_0, \qquad \partial_t u(0) = u_1.
```

The project must include both the implementation and a discussion of the numerical choices:

- finite element space discretization;
- time discretization;
- stability, numerical dissipation and numerical dispersion;
- computational and algorithmic aspects of the solver.

For a beginner-friendly overview before coding, see:

- [`project-intuition-guide/README.md`](project-intuition-guide/README.md)
- [`project-intuition-guide/PROJECT_INTUITION_GUIDE.pdf`](project-intuition-guide/PROJECT_INTUITION_GUIDE.pdf)

## Official Project Setup Notes

The official project template is available at:

```text
https://github.com/michelebucelli/nmpde-projects
```

Relevant setup rules from the template:

- all project source files should be placed in the `src` folder;
- binary files and executables must not be uploaded to the repository;
- generated mesh files should not be uploaded;
- if meshes are needed, the repository should contain `gmsh` scripts and instructions to generate them;
- if mesh generation scripts are not practical, meshes should be provided through an external download link;
- the project should be built with CMake and deal.II.

Expected build workflow:

```bash
module load gcc-glibc dealii
mkdir build
cd build
cmake ..
make
```

The executable will be generated inside `build/`.

## Available Reference Material

The workspace contains the official project PDF, the laboratory PDFs, and the official laboratory source code copied from:

```text
https://github.com/michelebucelli/nmpde-labs-aa-25-26
```

Relevant references in the local material:

- `projects.pdf`: official Project 2 description and general delivery instructions;
- `lab-00-introduction`: development environment, deal.II, ParaView and laboratory repository;
- `lab-01-poisson-1d`: basic structure of a stationary FEM solver (`setup`, `assemble`, `solve`, `output`);
- `lab-02-poisson-2d`: 2D FEM implementation, mesh handling and boundary conditions;
- `lab-04-heat`: time-dependent FEM problem, mass and stiffness matrices, time loop and ParaView output.

The laboratory source code has been copied next to the corresponding PDFs, for example:

- `Laboratory lectures-20260602/lab-01-poisson-1d/src`;
- `Laboratory lectures-20260602/lab-02-poisson-2d/src`;
- `Laboratory lectures-20260602/lab-04-heat/src`;
- `Laboratory lectures-20260602/common`;
- `Laboratory lectures-20260602/examples`.

The laboratory folders are reference material. The actual project implementation should be developed in `src/`, following the official project template.

## Expected FEM Formulation

For homogeneous Dirichlet conditions, or after applying a lifting for non-homogeneous data, the natural space is:

```math
V = H^1_0(\Omega).
```

The weak formulation is:

```math
(\partial_{tt} u, v)_\Omega + (\nabla u, \nabla v)_\Omega = (f, v)_\Omega
\qquad \forall v \in V.
```

Using a finite element basis `phi_i`, the semi-discrete system is:

```math
M \ddot U(t) + K U(t) = F(t),
```

where:

- `M_ij = int_Omega phi_j phi_i dx` is the mass matrix;
- `K_ij = int_Omega grad(phi_j) . grad(phi_i) dx` is the stiffness matrix;
- `U(t)` contains the solution degrees of freedom;
- `F(t)` contains the forcing term and possible boundary/lifting contributions.

## Numerical Choices to Discuss

Initial space discretization:

- continuous Lagrange finite elements, such as `P1` or `P2`;
- simple unit-square mesh for the first validation tests;
- possible extension to more complex geometries or mesh-generation scripts.

Time discretization options:

- explicit centered finite differences / leapfrog: second order, conditionally stable, suitable with mass lumping, low numerical dissipation but affected by numerical dispersion;
- Newmark methods: standard family for second-order evolution problems, with implicit stable variants and controllable numerical dissipation;
- comparison between explicit and implicit strategies in terms of cost, stability, accuracy, dissipation and dispersion.

## Roadmap

1. Create the project source layout in `src/`, following the official C++/deal.II template.
2. Assemble the mass matrix `M` and stiffness matrix `K` once.
3. Interpolate `u_0` and `u_1` on the finite element degrees of freedom.
4. Implement a first time stepper for `M U'' + K U = F`.
5. Handle homogeneous Dirichlet boundary conditions first, then non-homogeneous data.
6. Export `.vtu`/`.pvd` output for ParaView visualization.
7. Validate the solver with a manufactured solution on `(0, 1)^2`, for example:

```math
u(x,y,t) = \sin(\pi x)\sin(\pi y)\cos(\sqrt{2}\pi t),
```

which satisfies the homogeneous problem with `f = 0`, `g = 0`, `u_0 = sin(pi x) sin(pi y)` and `u_1 = 0`.

8. Measure error, energy conservation, dependence on `h` and `dt`, and computational cost.
9. Prepare the final report with the mathematical formulation, implementation choices and numerical results.

## Deliverables

- FEM solver source code.
- Numerical tests and reproducibility instructions.
- ParaView-compatible output.
- Final report including:
  - weak and discrete formulation;
  - description of the space and time discretization;
  - stability, numerical dissipation and numerical dispersion analysis;
  - numerical results and computational observations.

## References

- A. Quarteroni, *Numerical Models for Differential Problems*, Springer, 2017.
- S. Salsa, G. Verzini, *Partial Differential Equations in Action: From Modelling to Theory*, Springer, 2022.
