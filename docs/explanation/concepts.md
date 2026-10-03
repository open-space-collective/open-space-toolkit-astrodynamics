# Concepts

This page explains the main ideas behind Open Space Toolkit Astrodynamics and
how they fit together. It is not a step-by-step guide — for that, see the
[tutorials](../tutorials/index) and [how-to guides](../how-to/index) — but a map
of the domain model, so the rest of the documentation makes sense.

## Trajectories and states

The central abstraction is the
{py:class}`Trajectory <ostk.astrodynamics.Trajectory>`: something that, given an
`Instant`, can tell you the
{py:class}`State <ostk.astrodynamics.trajectory.State>` of an object.

A **state** bundles together everything known about the object at that instant —
at minimum its position and velocity, but optionally attitude, angular velocity,
mass, drag coefficient and any other quantity. Which quantities a state carries
is described by its set of **coordinate subsets**, and every numeric coordinate
is expressed in an explicit reference `Frame` and set of units. This is what
lets states from different sources be compared and transformed unambiguously.

## Orbits and orbit models

An {py:class}`Orbit <ostk.astrodynamics.trajectory.Orbit>` is a trajectory
specialised for a body in orbit around a central body. Its behaviour comes from
an **orbit model**, and the toolkit offers several, trading fidelity for cost:

- **{py:class}`Kepler <ostk.astrodynamics.trajectory.orbit.model.Kepler>`** —
  analytical two-body motion, optionally with a J2/J4 secular perturbation.
- **{py:class}`SGP4 <ostk.astrodynamics.trajectory.orbit.model.SGP4>`** —
  propagation from a Two-Line Element set, matching how satellite catalogues
  distribute orbits.
- **{py:class}`Propagated <ostk.astrodynamics.trajectory.orbit.model.Propagated>`** —
  numerical propagation under an arbitrary set of dynamics (see below), for the
  highest fidelity.
- **{py:class}`Tabulated <ostk.astrodynamics.trajectory.orbit.model.Tabulated>`** —
  interpolation of a precomputed table of states.
- **Mean-element models** such as
  {py:class}`BrouwerLyddaneMean <ostk.astrodynamics.trajectory.orbit.model.BrouwerLyddaneMean>`
  and {py:class}`ModifiedEquinoctial <ostk.astrodynamics.trajectory.orbit.model.ModifiedEquinoctial>`,
  useful for station-keeping and long-term analysis.

Because every model presents the same trajectory interface, code that consumes
an orbit does not care which model produced it. An orbit can also report
derived events such as its {py:class}`Pass <ostk.astrodynamics.trajectory.orbit.Pass>`
structure (ascending/descending crossings and revolution numbering).

## Dynamics and propagation

For high-fidelity work you describe the forces acting on the spacecraft as a set
of independent {py:mod}`dynamics <ostk.astrodynamics.dynamics>` contributions:

- {py:class}`CentralBodyGravity <ostk.astrodynamics.dynamics.CentralBodyGravity>`
- {py:class}`ThirdBodyGravity <ostk.astrodynamics.dynamics.ThirdBodyGravity>`
- {py:class}`AtmosphericDrag <ostk.astrodynamics.dynamics.AtmosphericDrag>`
- {py:class}`Thruster <ostk.astrodynamics.dynamics.Thruster>` (driven by a guidance law)

A {py:class}`Propagator <ostk.astrodynamics.trajectory.Propagator>` collects these
contributions and integrates the equations of motion with a numerical solver.
You include only the forces your analysis needs, and adding a new effect is a
matter of adding another dynamics term rather than rewriting the integrator.

## Guidance laws

A {py:mod}`guidance law <ostk.astrodynamics.guidance_law>` decides the direction
and magnitude of thrust over time. The toolkit ships
{py:class}`ConstantThrust <ostk.astrodynamics.guidance_law.ConstantThrust>` for
simple burns and {py:class}`QLaw <ostk.astrodynamics.guidance_law.QLaw>`, a
Lyapunov feedback law for low-thrust orbit transfers and station-keeping. A
guidance law feeds a `Thruster`, which contributes to the dynamics like any
other force.

## Event conditions, segments and sequences

Real missions are not flown for a fixed number of seconds — they run *until
something happens*. An {py:mod}`event condition <ostk.astrodynamics.event_condition>`
expresses that "something": reaching an instant
({py:class}`InstantCondition <ostk.astrodynamics.event_condition.InstantCondition>`),
crossing an orbital-element threshold
({py:class}`COECondition <ostk.astrodynamics.event_condition.COECondition>`),
an angular event
({py:class}`AngularCondition <ostk.astrodynamics.event_condition.AngularCondition>`),
or a logical combination of these.

A {py:class}`Segment <ostk.astrodynamics.trajectory.Segment>` propagates a given
set of dynamics until its event condition triggers — a coast or a thrust arc. A
{py:class}`Sequence <ostk.astrodynamics.trajectory.Sequence>` chains segments
together, so a manoeuvre plan reads much like the mission it represents. This
event-driven structure is what the station-keeping and QLaw how-to guides use.

## Accesses

An {py:class}`access <ostk.astrodynamics.Access>` is an interval during
which one object can "see" another subject to a
{py:class}`VisibilityCriterion <ostk.astrodynamics.access.VisibilityCriterion>` —
for example a ground station observing a satellite above a minimum elevation.
The {py:class}`Generator <ostk.astrodynamics.access.Generator>` computes accesses
between trajectories over an interval, which underpins contact planning,
coverage and interference analysis.

## Flight profiles

Beyond where a spacecraft is, mission analysis often needs to know how it is
oriented. A {py:class}`Profile <ostk.astrodynamics.flight.Profile>` describes
attitude over time — nadir pointing, target tracking, with or without yaw
compensation — while
{py:class}`System <ostk.astrodynamics.flight.System>` and
{py:class}`Maneuver <ostk.astrodynamics.flight.Maneuver>` capture the physical
satellite and its manoeuvres. These are what the pointing and tracking how-to
guides build on.

## Why mirror C++ and Python

The library is implemented once in C++ and exposed to Python through pybind11.
Keeping the model identical across both languages means documentation, examples
and mental models transfer directly, the [API reference](../reference/index) can
be generated from a single source of truth on each side, and
performance-sensitive code can drop to C++ without changing how the problem is
modelled.

```{seealso}
See [About Open Space Toolkit Astrodynamics](about) for where the library sits
in the wider toolkit and the principles behind it.
```
