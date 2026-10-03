# About Open Space Toolkit Astrodynamics

## What it is

Open Space Toolkit Astrodynamics is the flight-dynamics layer of the
[Open Space Toolkit](https://github.com/open-space-collective/open-space-toolkit),
a collection of open-source libraries for space engineering and mission
analysis. It models how spacecraft move: orbits and trajectories, numerical
propagation under realistic forces, accesses between objects, attitude and
pointing profiles, guidance laws, and event-driven manoeuvre sequences.

Where the lower layers provide the vocabulary — types and containers from
[Core](https://github.com/open-space-collective/open-space-toolkit-core),
geometry and solvers from
[Mathematics](https://github.com/open-space-collective/open-space-toolkit-mathematics),
and time, frames, units and environment models from
[Physics](https://github.com/open-space-collective/open-space-toolkit-physics) —
Astrodynamics is where those come together to answer questions about a mission.

## Where it sits

```{mermaid}
graph TD
    Core["Core<br/><i>types, containers, filesystem</i>"]
    Math["Mathematics<br/><i>geometry, solvers</i>"]
    Physics["Physics<br/><i>time, frames, units, environment</i>"]
    Astro["Open Space Toolkit ▸ Astrodynamics<br/><i>orbits, trajectories, access, flight</i>"]

    Core --> Math
    Math --> Physics
    Physics --> Astro
```

Astrodynamics sits at the top of the toolkit and depends, directly or
transitively, on every layer below it. The Python bindings re-export those
layers, so `from ostk.physics import ...` works alongside
`from ostk.astrodynamics import ...` in the same session.

## Design principles

**One implementation, two languages.** The library is written in C++ for
performance and exposed to Python through
[pybind11](https://pybind11.readthedocs.io/) bindings. The Python API mirrors
the C++ API closely, so knowledge transfers between the two. This is also why
the [reference](../reference/index) is generated automatically from both the
C++ headers and the compiled Python module — the two never drift apart.

**Composable dynamics.** Propagation is assembled from independent
{py:mod}`dynamics <ostk.astrodynamics.dynamics>` contributions — central-body
gravity, third-body gravity, atmospheric drag, thrust — integrated by a shared
numerical solver. You add only the forces your analysis needs.

**Event-driven mission modelling.** Rather than integrating for a fixed time,
you can propagate until a physical
{py:mod}`condition <ostk.astrodynamics.event_condition>` is met — an altitude,
an anomaly, a duration — and chain such segments into a
{py:mod}`sequence <ostk.astrodynamics.trajectory>` that mirrors how a real
mission is flown.

**Validated against reference tools.** Correctness is checked against
established flight-dynamics software. The
[cross-platform validation](../how-to/index) notebooks reproduce mission
scenarios in Open Space Toolkit and compare the results against
[GMAT](https://gmat.gsfc.nasa.gov/) and [Orekit](https://www.orekit.org/),
so the numbers can be trusted.

```{seealso}
For the concrete building blocks and how they fit together, see
[Concepts](concepts).
```
