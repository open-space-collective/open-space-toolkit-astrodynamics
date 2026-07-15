#
# Open Space Toolkit ▸ Astrodynamics — Sphinx documentation configuration.
#
#
# The API reference is generated automatically:
#   - Python : discovered by introspecting the compiled ``ostk.astrodynamics``
#              module, so submodules never need to be listed by hand.
#   - C++    : generated from the Doxygen XML by Exhale, so classes never need
#              to be listed by hand.
#

import importlib
import os
import pkgutil
import types

import ostk.astrodynamics
from ostk.astrodynamics import OpenSpaceToolkitAstrodynamicsPy

# -- Paths ----------------------------------------------------------------

HERE = os.path.abspath(os.path.dirname(__file__))

# -- General configuration ------------------------------------------------

needs_sphinx = "7.2"

extensions = [
    "sphinx.ext.autodoc",
    "sphinx.ext.intersphinx",
    "sphinx.ext.autosummary",
    "sphinx.ext.napoleon",
    "sphinx.ext.viewcode",
    "sphinx_math_dollar",
    "sphinx_design",
    "sphinx_copybutton",
    "sphinxcontrib.mermaid",
    "breathe",
    "exhale",
    "myst_nb",
]

# Paths that contain templates, relative to this directory.
templates_path = ["templates"]

# The suffix(es) of source filenames. ``myst_nb`` registers the parser for
# ``.md`` and ``.ipynb``; ``.rst`` is handled by the built-in parser.
source_suffix = [".rst", ".md", ".ipynb"]

add_module_names = False

# The master toctree document.
master_doc = "index"

# General information about the project.
project = "Open Space Toolkit Astrodynamics"
copyright = "2024, Lucas Bremond"
author = "Lucas Bremond"

# The version info for the project.
version = OpenSpaceToolkitAstrodynamicsPy.__version__
release = OpenSpaceToolkitAstrodynamicsPy.__version__

language = "en"

# Patterns to ignore when looking for source files.
exclude_patterns = [
    "_build",
    "Thumbs.db",
    ".DS_Store",
    ".ipynb_checkpoints",
]

todo_include_todos = False

python_maximum_signature_line_length = 88

# -- Autodoc / autosummary configuration ----------------------------------

autosummary_generate = True  # Turn on sphinx.ext.autosummary
autodoc_member_order = "bysource"  # Order members by source order
html_show_sourcelink = False  # Remove 'view source code' from top of page
autodoc_inherit_docstrings = True  # If no docstring, inherit from base class
set_type_checking_flag = True  # Enable 'expensive' imports for type hints
autoclass_content = "both"  # Include both class docstring and __init__

# Members that should never appear in the generated Python reference.
_SKIP_MEMBERS = {
    "OpenSpaceToolkitAstrodynamicsPy",
    "OpenSpaceToolkitCorePy",
    "OpenSpaceToolkitIOPy",
    "OpenSpaceToolkitMathematicsPy",
    "OpenSpaceToolkitPhysicsPy",
    "pytrajectory",
    "test",
    "URL",
    "ip",
}


def skip_member(app, what, name, obj, skip, options):
    if name in _SKIP_MEMBERS:
        return True
    return skip


def autodoc_process_docstring(app, what, name, obj, options, lines):
    for i in range(len(lines)):
        lines[i] = lines[i].replace("np.", "numpy.")
        lines[i] = lines[i].replace("List[", "~typing.List[")


# -- Automatic Python API discovery ---------------------------------------
#
# ``ostk.astrodynamics`` mixes two kinds of submodule:
#
#   - compiled pybind11 submodules (e.g. ``ostk.astrodynamics.trajectory``)
#     are attributes on the module object rather than files on disk, so
#     ``autosummary``'s ``:recursive:`` mode (which relies on ``pkgutil``
#     walking a package's ``__path__``) cannot discover them;
#   - pure-Python submodules (e.g. ``ostk.astrodynamics.dataframe``) live as
#     files next to ``__init__.py`` and are not necessarily imported by it.
#
# We therefore combine both discovery strategies and enumerate every reachable
# submodule ourselves. The list is written into a generated reference page so
# that adding, removing or renaming a submodule never requires a manual edit to
# a ``.rst`` file.

_ROOT_PACKAGE = "ostk.astrodynamics"


def _discover_submodules(module, root=_ROOT_PACKAGE, seen=None):
    """Recursively collect the fully-qualified names of every submodule
    reachable from ``module`` (compiled pybind11 and pure-Python alike)."""

    if seen is None:
        seen = set()

    discovered = []

    # Import pure-Python submodules that live on disk but may not be imported
    # by ``__init__.py`` (so they become attributes and are found below).
    for info in pkgutil.iter_modules(getattr(module, "__path__", [])):
        if info.name.startswith("_") or info.name in _SKIP_MEMBERS:
            continue
        try:
            importlib.import_module(f"{module.__name__}.{info.name}")
        except Exception:
            continue

    for name in sorted(dir(module)):
        if name.startswith("_") or name in _SKIP_MEMBERS:
            continue

        try:
            obj = getattr(module, name)
        except Exception:
            continue

        if not isinstance(obj, types.ModuleType):
            continue

        modname = getattr(obj, "__name__", "")

        # ``ostk.astrodynamics`` re-exports the lower toolkit layers via
        # ``from ostk.physics import *``; the ``startswith(root)`` guard keeps
        # those out of the Astrodynamics reference.
        if not modname.startswith(root) or modname in seen:
            continue

        seen.add(modname)
        discovered.append(modname)
        discovered.extend(_discover_submodules(obj, root, seen))

    return discovered


def generate_python_reference(app=None):
    """Write ``reference/python.rst`` with an autosummary block covering the
    whole ``ostk.astrodynamics`` module tree, discovered at build time."""

    modules = [_ROOT_PACKAGE]

    try:
        modules.extend(_discover_submodules(ostk.astrodynamics))
    except Exception as error:  # pragma: no cover - defensive
        print(f"[ostk-docs] Python API discovery failed: {error}")

    # De-duplicate while preserving order.
    seen = set()
    modules = [m for m in modules if not (m in seen or seen.add(m))]

    module_lines = "\n".join(f"   {module}" for module in modules)

    content = f""".. This file is generated by ``generate_python_reference`` in ``conf.py``.
   Do not edit it by hand — edit the generator instead.

Python API
==========

The Python API mirrors the C++ library through pybind11 bindings. The pages
below are generated automatically by introspecting the installed
``ostk.astrodynamics`` module, so every submodule and class is always covered.

.. autosummary::
   :toctree: _autosummary
   :template: custom-module-template.rst
   :nosignatures:

{module_lines}
"""

    output_path = os.path.join(HERE, "reference", "python.rst")
    os.makedirs(os.path.dirname(output_path), exist_ok=True)

    with open(output_path, "w", encoding="utf-8") as handle:
        handle.write(content)

    print(f"[ostk-docs] Generated Python reference for {len(modules)} module(s).")


# Generate the Python reference immediately so the file exists on disk before
# Sphinx reads the source tree and before ``autosummary`` runs.
generate_python_reference()


def setup(app):
    app.connect("autodoc-skip-member", skip_member)
    app.connect("autodoc-process-docstring", autodoc_process_docstring)
    app.connect("builder-inited", generate_python_reference)


# -- HTML output (Furo theme) ---------------------------------------------

html_theme = "furo"
html_title = "Open Space Toolkit ▸ Astrodynamics"

# Custom stylesheet (loaded after the theme's CSS).
html_static_path = ["_static"]
html_css_files = ["custom.css"]

html_theme_options = {
    "source_repository": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics/",
    "source_branch": "main",
    "source_directory": "docs/",
    "footer_icons": [
        {
            "name": "GitHub",
            "url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
            "html": (
                '<svg stroke="currentColor" fill="currentColor" '
                'stroke-width="0" viewBox="0 0 16 16"><path fill-rule="evenodd" '
                'd="M8 0C3.58 0 0 3.58 0 8c0 3.54 2.29 6.53 5.47 7.59.4.07.55-.17.55-.38 '
                "0-.19-.01-.82-.01-1.49-2.01.37-2.53-.49-2.69-.94-.09-.23-.48-.94-.82-1.13-.28-.15-.68-.52-.01-.53.63-.01"
                "1.08.58 1.23.82.72 1.21 1.87.87 2.33.66.07-.52.28-.87.51-1.07-1.78-.2-3.64-.89-3.64-3.95"
                "0-.87.31-1.59.82-2.15-.08-.2-.36-1.02.08-2.12 0 0 .67-.21 2.2.82.64-.18 1.32-.27 2-.27.68"
                "0 1.36.09 2 .27 1.53-1.04 2.2-.82 2.2-.82.44 1.1.16 1.92.08 2.12.51.56.82 1.27.82 2.15"
                "0 3.07-1.87 3.75-3.65 3.95.29.25.54.73.54 1.48 0 1.07-.01 1.93-.01 2.2 0 .21.15.46.55.38"
                'A8.013 8.013 0 0 0 16 8c0-4.42-3.58-8-8-8z"></path></svg>'
            ),
            "class": "",
        },
    ],
}

htmlhelp_basename = "ostk_astrodynamics_doc"

# -- Napoleon configuration -----------------------------------------------

napoleon_google_docstring = True
napoleon_numpy_docstring = True
napoleon_include_init_with_doc = True

# -- MyST / MyST-NB configuration -----------------------------------------

myst_enable_extensions = [
    "colon_fence",  # ::: fences for sphinx-design directives
    "deflist",
    "substitution",
]
myst_heading_anchors = 3
myst_footnote_transition = False

nb_execution_mode = "off"  # Do not execute notebooks at build time
nb_execution_timeout = 60

# -- Breathe / Exhale configuration (C++ API) ------------------------------
#
# Exhale drives Doxygen and generates the entire C++ API reference tree from
# the XML output, so individual classes never need to be listed by hand.

breathe_projects = {"open-space-toolkit-astrodynamics": "./_doxygen/xml"}
breathe_default_project = "open-space-toolkit-astrodynamics"
breathe_default_members = ("members", "undoc-members")

exhale_args = {
    # Where the generated reStructuredText tree is written (relative to this
    # configuration directory, matching ``breathe_projects`` above).
    "containmentFolder": "./reference/cpp",
    "rootFileName": "library_root.rst",
    "rootFileTitle": "C++ API",
    "doxygenStripFromPath": "..",
    "createTreeView": True,
    # Let Exhale run Doxygen itself so the build is self-contained and does
    # not depend on an external ``doxygen`` / ``breathe-apidoc`` step.
    "exhaleExecutesDoxygen": True,
    "exhaleDoxygenStdin": "\n".join(
        [
            "INPUT = ../include",
            "EXTRACT_ALL = YES",
            "RECURSIVE = YES",
            "GENERATE_HTML = NO",
            "GENERATE_LATEX = NO",
            "XML_PROGRAMLISTING = YES",
            "JAVADOC_AUTOBRIEF = YES",
            "FILE_PATTERNS = *.hpp *.tpp *.h",
        ]
    ),
}

# -- Intersphinx configuration --------------------------------------------

intersphinx_mapping = {
    "python": ("https://docs.python.org/3", None),
    "numpy": ("https://numpy.org/doc/stable/", None),
}
