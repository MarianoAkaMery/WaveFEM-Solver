# Project Source

This folder contains the WaveFEM solver implementation.

- `main.cpp`: parses basic run parameters and launches the simulation.
- `WaveEquation.hpp`: declares the solver class, parameters, finite element objects and vectors.
- `WaveEquation.cpp`: implements setup, matrix assembly, Newmark time stepping, diagnostics and ParaView output.
