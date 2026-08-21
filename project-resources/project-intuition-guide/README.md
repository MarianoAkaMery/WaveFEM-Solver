# Project 2 Intuition Guide

This document explains what we are going to build before writing the full codebase.
It is written for computer science students with weak PDE background.

Read this as a practical mental model first, not as a theorem list. The goal is to understand
what the code is trying to do before asking GPT/Codex to generate the full solver.

## 1. What The Project Is About

We must implement a finite element solver for the 2D wave equation:

```math
\frac{\partial^2 u}{\partial t^2} - \Delta u = f \quad \text{in } \Omega
```

with boundary and initial data:

```math
u = g \quad \text{on } \partial\Omega,
\qquad
u(0) = u_0,
\qquad
\partial_t u(0) = u_1.
```

Intuitively, `u(x,y,t)` is a displacement. Think of a vibrating membrane, like a drum surface.
At every point `(x,y)` and time `t`, `u` tells how high or low the membrane is.

The project is not only "make it run". We must also explain:

- how we discretize space with finite elements;
- how we discretize time;
- whether the method is stable;
- whether it introduces numerical dissipation or numerical dispersion;
- how the algorithm is organized and what it costs computationally.

## 2. Meaning Of The PDE

The equation is:

```math
u_{tt} - \Delta u = f.
```

The term `u_tt` is the acceleration in time. If the membrane is moving fast and changing velocity, this term measures that.

The term `- Delta u` describes spatial tension. It compares the value of `u` at one point with nearby points. If the membrane is curved, the Laplacian creates a restoring force.

The term `f` is an external force. If `f = 0`, the wave moves only because of its initial displacement or initial velocity.

The boundary condition `u = g` fixes the membrane on the boundary. The easiest first case is `g = 0`, meaning the boundary is fixed at height zero.

The initial condition `u_0` gives the initial shape. The initial velocity `u_1` gives how fast the membrane starts moving.

## 3. What We Need To Produce

The final code should:

1. create or read a 2D mesh;
2. build a finite element space;
3. assemble the mass matrix `M`;
4. assemble the stiffness matrix `K`;
5. initialize the vectors from `u_0` and `u_1`;
6. advance the solution in time;
7. write ParaView output;
8. run tests showing accuracy, stability, dissipation, dispersion, and computational cost.

The report/presentation should explain the same points in words.

## 4. Weak Formulation, In Simple Terms

The computer does not solve the PDE point by point. FEM solves an integrated version.

We multiply the PDE by a test function `v` and integrate on the domain:

```math
\int_\Omega u_{tt} v \, dx - \int_\Omega \Delta u \, v \, dx
=
\int_\Omega f v \, dx.
```

Then we integrate by parts the Laplacian term:

```math
\int_\Omega u_{tt} v \, dx
+
\int_\Omega \nabla u \cdot \nabla v \, dx
=
\int_\Omega f v \, dx.
```

For homogeneous Dirichlet boundary conditions, the boundary term disappears because the test functions are zero on the boundary.

Final weak form:

```math
(u_{tt}, v)_\Omega + (\nabla u, \nabla v)_\Omega = (f,v)_\Omega
\qquad \forall v \in V.
```

Minimal meaning:

- `(u_tt, v)` becomes the mass matrix part;
- `(grad u, grad v)` becomes the stiffness matrix part;
- `(f, v)` becomes the right-hand side vector.

## 5. From Weak Form To Matrices

In FEM, we approximate:

```math
u_h(x,y,t) = \sum_j U_j(t) \phi_j(x,y).
```

The functions `phi_j` are basis functions. The unknowns are the coefficients `U_j(t)`.

After inserting this approximation into the weak form, we get:

```math
M \ddot U(t) + K U(t) = F(t).
```

Where:

```math
M_{ij} = \int_\Omega \phi_j \phi_i \, dx,
\qquad
K_{ij} = \int_\Omega \nabla \phi_j \cdot \nabla \phi_i \, dx,
\qquad
F_i = \int_\Omega f \phi_i \, dx.
```

This is the key translation from mathematics to code.

As computer scientists, we can read it like this:

- `M` stores how degrees of freedom interact through time acceleration;
- `K` stores how degrees of freedom interact through spatial gradients;
- `U` is the vector of current unknown values;
- `F` is the vector of external forces.

## 6. Time Discretization

The wave equation is second order in time. That means we need both:

- displacement `U`;
- velocity or previous displacement information.

A simple option is a centered finite difference / leapfrog-like method:

```math
M \frac{U^{n+1} - 2U^n + U^{n-1}}{\Delta t^2}
+ K U^n
= F^n.
```

Then:

```math
M U^{n+1}
=
\Delta t^2 F^n
- \Delta t^2 K U^n
+ 2 M U^n
- M U^{n-1}.
```

This is intuitive and second order in time, but conditionally stable: the time step must be small enough compared to the mesh size.

Another option is an implicit Newmark method. It is usually more stable for larger time steps, but each time step is more expensive because we solve a linear system involving both `M` and `K`.

For the first implementation, we should choose one robust method and explain the tradeoff clearly.

## 7. Stability, Dissipation, Dispersion

These words will appear in the project discussion.

Stability means the numerical solution does not blow up for no physical reason.

Numerical dissipation means the method artificially damps the wave. The wave loses energy even if the true equation would conserve it.

Numerical dispersion means different numerical wave frequencies travel at wrong speeds. Visually, the wave shape may become shifted or distorted.

For the report, we should not overcomplicate this. We can test and discuss:

- whether the energy stays roughly constant when `f = 0` and `g = 0`;
- whether smaller `dt` and smaller `h` improve the solution;
- whether the wave amplitude decays too much;
- whether the wave arrives too early or too late compared to an exact solution.

## 8. First Validation Test

Use a manufactured solution on the unit square:

```math
\Omega = (0,1)^2.
```

A simple exact solution is:

```math
u(x,y,t) = \sin(\pi x)\sin(\pi y)\cos(\sqrt{2}\pi t).
```

It satisfies homogeneous Dirichlet boundary conditions because the sine is zero on the square boundary.

For this solution:

```math
f = 0,
\qquad
u_0(x,y) = \sin(\pi x)\sin(\pi y),
\qquad
u_1(x,y) = 0.
```

This is a good first test because we know what the solution should look like.

## 9. Implementation Plan

The code should follow the standard deal.II lab style.

Suggested structure:

- `WaveEquation.hpp`: class declaration;
- `WaveEquation.cpp`: implementation;
- `main.cpp`: parameters and run call;
- `CMakeLists.txt`: build configuration;
- `README.md`: build/run instructions;
- `mesh/`: optional mesh scripts;
- `output/`: generated results, ignored by Git if needed.

The main class should have methods like:

- `setup()`: create mesh, finite element space, DoFs, matrices, vectors;
- `assemble_matrices()`: assemble `M` and `K`;
- `interpolate_initial_conditions()`: fill initial displacement and velocity;
- `solve_time_step()`: compute the next solution;
- `output_results()`: write `.vtu`/`.pvd` files for ParaView;
- `compute_errors()`: compare with exact solution when available;
- `run()`: orchestrate the whole simulation.

## 10. Algorithm In Pseudocode

```text
read parameters
create mesh
create finite element space
distribute degrees of freedom
assemble mass matrix M
assemble stiffness matrix K
interpolate u0 and u1
compute U^0
compute U^1 or initialize velocity

for each time step:
    assemble or evaluate F(t)
    compute U^{n+1}
    apply Dirichlet boundary conditions
    compute optional error/energy
    write output every few steps

write summary CSV files
```

## 11. What To Look At In ParaView

The most important visualization is the displacement field `u`.

Useful outputs:

- final displacement at time `T`;
- animation over time;
- comparison between coarse and fine mesh;
- comparison between large and small `dt`.

In ParaView, we should check:

- the wave moves smoothly;
- the boundary condition is respected;
- the solution is not exploding;
- the shape is plausible;
- the amplitude is not disappearing too quickly unless the chosen method is dissipative.

## 12. Experiments To Include

Minimum useful experiments:

1. exact solution test on `(0,1)^2`;
2. error vs mesh size `h`;
3. error vs time step `dt`;
4. energy over time for the homogeneous case;
5. visual comparison in ParaView.

Optional experiments:

- compare `P1` and `P2` elements;
- compare explicit and implicit time methods;
- test a nonzero forcing term `f`;
- test a more interesting initial displacement.

## 13. How To Split Work In A Group Of 5

One possible split:

- Person 1: weak formulation and report explanation;
- Person 2: deal.II mesh, DoFs, matrices;
- Person 3: time discretization and solver;
- Person 4: validation, errors, energy plots;
- Person 5: ParaView output, README, final presentation.

Everyone should still understand the core equation:

```math
M \ddot U + K U = F.
```

That formula is the bridge between PDE theory and code.

## 14. What We Should Be Able To Say Orally

Short explanation:

> We solve the 2D wave equation with the finite element method in space. After multiplying by test functions and integrating by parts, we obtain a weak form. Using Lagrange basis functions, this becomes a second-order ODE system in time, `M U'' + K U = F`. We then choose a time discretization, apply Dirichlet boundary conditions, and advance the solution step by step. We validate the solver with an exact sinusoidal solution and study stability, dissipation, dispersion, and computational cost.

Even shorter:

> FEM turns the PDE into `M U'' + K U = F`. Then the time method turns that ODE system into linear algebra at each time step.

## 15. Risk Checklist Before Coding

Check these carefully:

- sign of the Laplacian term: the weak form should have `+ grad u dot grad v`;
- boundary conditions: homogeneous Dirichlet is the easiest first case;
- initial velocity: the wave equation needs both `u_0` and `u_1`;
- time step: explicit methods can blow up if `dt` is too large;
- output frequency: do not write too many huge files;
- exact solution: verify that `f`, `u_0`, `u_1`, and `g` match the chosen formula;
- convergence: decreasing `h` and `dt` should improve accuracy;
- energy: for the homogeneous case, energy should be approximately conserved unless the method is dissipative.

## 16. Final Mental Model

The project is a pipeline:

```text
PDE problem
  -> weak formulation
  -> FEM matrices M and K
  -> time stepping
  -> numerical solution files
  -> validation plots and report
```

If we understand this pipeline, the implementation becomes a structured programming task instead of a mysterious PDE problem.

## 17. Super Basic Picture

Imagine the domain as a flat square surface:

```text
(0,1) x (0,1)

top boundary fixed
  -----------------
  |               |
  |   membrane    |
  |               |
  -----------------
bottom boundary fixed
```

At each point, the solution `u(x,y,t)` is the height of the surface.

Examples:

- `u = 0`: the surface is flat.
- `u > 0`: the surface is above the flat position.
- `u < 0`: the surface is below the flat position.
- `u_0`: the starting shape.
- `u_1`: the starting velocity.
- `f`: an external push.

The solver answers this question:

> If I know the starting shape and starting speed, what does the surface look like later?

## 18. Very Small Fake Mesh Example

FEM starts by cutting the domain into small pieces.

Very rough picture:

```text
4 square cells:

o----o----o
|    |    |
o----o----o
|    |    |
o----o----o
```

Each `o` is a node. In a simple `P1` method, the unknowns are the values of `u` at the nodes.

If the boundary is fixed with `u = 0`, then all boundary nodes are known:

```text
0----0----0
|    |    |
0----?----0
|    |    |
0----0----0
```

Only the center node is really unknown in this tiny example.

In a real mesh there are many nodes, so instead of one `?`, we have a vector:

```text
U = [u_1, u_2, u_3, ..., u_N]
```

This is why the PDE becomes linear algebra.

## 19. What The Matrices Mean With A Toy Example

The final semi-discrete equation is:

```math
M \ddot U + K U = F.
```

Think of it like a physical system:

```text
mass * acceleration + stiffness * displacement = force
```

This is the same idea as:

```math
m x'' + k x = force
```

but instead of one object moving up/down, we have many mesh nodes moving together.

Toy version with 3 unknown nodes:

```text
U = [height at node 1,
     height at node 2,
     height at node 3]
```

Then:

```text
M tells how the node masses/inertia are distributed.
K tells how each node is connected to nearby nodes by spatial stiffness.
F tells which nodes are being pushed by the source term.
```

If `K` is large, the solution reacts strongly to curvature. If `M` is large, acceleration changes more slowly.

## 20. One Time Step In Plain Words

At time `t_n`, assume we know:

```text
U^{n-1}: previous shape
U^n:     current shape
```

We want:

```text
U^{n+1}: next shape
```

For a centered explicit-like method:

```math
M \frac{U^{n+1} - 2U^n + U^{n-1}}{\Delta t^2}
+ K U^n = F^n.
```

Plain translation:

```text
next shape =
  current motion effect
  - stiffness effect
  + external forcing effect
```

In code, this usually means:

```text
rhs = dt^2 * F
rhs -= dt^2 * K * current_solution
rhs += 2 * M * current_solution
rhs -= M * old_solution
solve M * next_solution = rhs
```

So each time step is not magic. It is just building a right-hand side vector and solving a matrix system.

## 21. Visual Examples We Should Expect

### Example A: Flat Start, No Force

```text
u_0 = 0
u_1 = 0
f = 0
```

Expected result:

```text
the solution remains zero forever
```

This is a basic sanity test. If the code creates waves here, something is wrong.

### Example B: Initial Bump, No Force

```text
u_0 = sin(pi x) sin(pi y)
u_1 = 0
f = 0
```

Expected result:

```text
the membrane starts as one smooth bump,
then moves down,
then up again,
oscillating over time.
```

In ParaView, this should look like a smooth vibrating surface.

### Example C: Zero Shape, Initial Velocity

```text
u_0 = 0
u_1 = sin(pi x) sin(pi y)
f = 0
```

Expected result:

```text
the membrane starts flat,
but immediately moves because it has initial velocity.
```

This reminds us why the wave equation needs both `u_0` and `u_1`.

### Example D: External Push

```text
u_0 = 0
u_1 = 0
f != 0
```

Expected result:

```text
the membrane moves because the source term pushes it.
```

This is like applying a force from outside.

## 22. How To Think About ParaView Output

When ParaView opens a `.vtu` or `.pvd` file, we are not looking at formulas anymore.
We are looking at the vector `U` drawn on the mesh.

Good signs:

- boundary stays fixed if `u = 0` on the boundary;
- wave is smooth for smooth initial data;
- smaller mesh size gives a cleaner shape;
- smaller time step gives a more reliable animation;
- solution does not explode.

Bad signs:

- boundary values move even though they should be fixed;
- solution becomes huge very quickly;
- strong random oscillations appear;
- wave disappears too fast without reason;
- output is empty or all one color when it should not be.

## 23. How To Reason Before Coding

Before writing a function, ask:

```text
Which mathematical object am I implementing?
```

Examples:

- mesh creation implements the domain `Omega`;
- finite element space implements `V_h`;
- mass assembly implements `M`;
- stiffness assembly implements `K`;
- initial interpolation implements `u_0` and `u_1`;
- time loop implements the time discretization;
- output implements visualization and validation.

This makes the code easier to explain.

## 24. Minimal Story For The Group

Use this story when studying together:

```text
We have a vibrating 2D surface.
We split the surface into a mesh.
The unknown is the height at mesh degrees of freedom.
The weak form gives us two main matrices: M and K.
M handles acceleration in time.
K handles spatial bending/curvature.
The PDE becomes M U'' + K U = F.
A time method tells us how to compute the next U.
Then we export U over time and check if the wave behaves correctly.
```

If everyone understands this story, the project is already much less abstract.
