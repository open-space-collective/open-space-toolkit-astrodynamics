---
sd_hide_title: true
---

# Open Space Toolkit ▸ Astrodynamics

```{toctree}
:hidden:

Home <self>
```

```{toctree}
:hidden:
:caption: Documentation

tutorials/index
how-to/index
reference/index
explanation/index
```

<div align="center">

# Open Space Toolkit ▸ Astrodynamics

**Orbits, trajectories, accesses and flight dynamics for space mission analysis.**

</div>

Open Space Toolkit Astrodynamics builds on the lower layers of the Open Space
Toolkit to model the motion of spacecraft: {py:mod}`orbits
<ostk.astrodynamics.trajectory.orbit>` and {py:mod}`trajectories
<ostk.astrodynamics.trajectory>`, numerical {py:mod}`propagation
<ostk.astrodynamics.dynamics>`, {py:mod}`accesses <ostk.astrodynamics.access>`
between objects, {py:mod}`flight profiles <ostk.astrodynamics.flight>`,
{py:mod}`guidance laws <ostk.astrodynamics.guidance_law>` and
{py:mod}`event-driven sequences <ostk.astrodynamics.trajectory>`. It is written
in C++ and exposed to Python through pybind11 bindings.

## How the documentation is organised

It is split into four sections, each serving a different need. If you are not sure
where to start, the tutorials are the best entry point.

::::{grid} 1 1 2 2
:gutter: 3

:::{grid-item-card} {octicon}`mortar-board;1.5em;sd-mr-1` Tutorials
:link: tutorials/index
:link-type: doc

Learning-oriented lessons that take you by the hand through a series of steps
to build something with the toolkit. **Start here if you are new.**
:::

:::{grid-item-card} {octicon}`tools;1.5em;sd-mr-1` How-to guides
:link: how-to/index
:link-type: doc

Task-oriented, practical recipes that show how to solve a specific,
real-world problem — access computation, propagation with thrust,
station-keeping, sensor modelling and more.
:::

:::{grid-item-card} {octicon}`book;1.5em;sd-mr-1` Reference
:link: reference/index
:link-type: doc

Information-oriented, automatically generated descriptions of the Python and
C++ APIs.
:::

:::{grid-item-card} {octicon}`light-bulb;1.5em;sd-mr-1` Explanation
:link: explanation/index
:link-type: doc

Understanding-oriented discussion that clarifies and illuminates the design
and concepts behind the toolkit.
:::

::::

## Installation

Install the Python package from [PyPI](https://pypi.org/project/open-space-toolkit-astrodynamics/):

```bash
pip install open-space-toolkit-astrodynamics
```

C++ binary packages are published on
[GitHub Releases](https://github.com/open-space-collective/open-space-toolkit-astrodynamics/releases).
See the [how-to guides](how-to/index) for details.
