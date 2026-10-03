# Reference

```{toctree}
:hidden:

Python API <python>
C++ API <cpp/library_root>
```

Reference material is **information-oriented**: a complete, accurate description
of the toolkit's public interface. It is generated automatically and always
reflects the installed version.

::::{grid} 1 1 2 2
:gutter: 3

:::{grid-item-card} {octicon}`terminal;1.5em;sd-mr-1` Python API
:link: python
:link-type: doc

Every module, class and function of the `ostk.astrodynamics` package, discovered
by introspecting the installed module.
:::

:::{grid-item-card} {octicon}`file-code;1.5em;sd-mr-1` C++ API
:link: cpp/library_root
:link-type: doc

Every namespace, class and function of the C++ library, generated from the
Doxygen documentation in the headers.
:::

::::

```{note}
Neither page is maintained by hand. The Python reference is regenerated from the
compiled `ostk.astrodynamics` module and the C++ reference is regenerated from the
header comments on every documentation build.
```
