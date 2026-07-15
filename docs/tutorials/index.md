# Tutorials

```{toctree}
:hidden:

getting-started
Orbit computation </notebooks/Flight Dynamics/Orbit Computation>
```

Tutorials are **learning-oriented**: they walk you, step by step, through
building something real with Open Space Toolkit Astrodynamics. Their goal is not
to solve a specific problem you have today, but to give you a feel for the
toolkit so that everything else in the documentation makes sense.

Work through them in order — you only need a working Python installation of the
package (`pip install open-space-toolkit-astrodynamics`).

::::{grid} 1 1 2 2
:gutter: 3

:::{grid-item-card} {octicon}`rocket;1.5em;sd-mr-1` Getting started
:link: getting-started
:link-type: doc

Set up your environment, build a Keplerian orbit and propagate it to obtain a
spacecraft state. A gentle first contact with the library.
:::

::::

Then continue with the {doc}`Orbit computation </notebooks/Flight Dynamics/Orbit Computation>`
notebook — a runnable lesson that builds an orbit from a Two-Line Element set,
propagates it with SGP4, and inspects the resulting states.

```{seealso}
Once you are comfortable, head to the [how-to guides](../how-to/index) for
recipes that solve specific problems, or the [reference](../reference/index)
for the complete API.
```
