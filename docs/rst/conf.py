# -*- coding: utf-8 -*-

# ----------------------------
# Import required modules
# ----------------------------
import os
import sys
import inspect
import importlib

# ----------------------------
# Add sys paths for modules
# ----------------------------
paths = [
    "/usr/local/lib/python3.11/site-packages/ostk/",
]

for path in paths:
    sys.path.insert(0, os.path.abspath(path))

# ----------------------------
# Project Information
# ----------------------------
project = "Open Space Toolkit Astrodynamics"
copyright = "2023, Lucas Bremond"
author = "Lucas Bremond"

# ----------------------------
# General Configuration
# ----------------------------
extensions = [
    "sphinx.ext.autodoc",
    "sphinx.ext.intersphinx",
    "sphinx.ext.napoleon",
    "sphinx_immaterial",
    "sphinx_math_dollar",
    "sphinx_design",
    "sphinx_immaterial.apidoc.python.apigen",
    "myst_parser",
    "breathe",
]

source_suffix = ".rst"
autosummary_generate = True
pygments_style = "sphinx"

intersphinx_mapping = {
    "python": ("https://docs.python.org/3", None),
    "numpy": ("https://numpy.org/doc/stable/", None),
}

templates_path = ["../_templates"]
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store", ".ipynb_checkpoints"]

autodoc_default_options = {
    "undoc-members": True,
}

# Breathe
breathe_projects = {
    "ostk-astro": "../xml/",
}

# Myst
myst_enable_extensions = [
    "colon_fence",
]

# ----------------------------
# Python Apigen Configuration
# ----------------------------


def get_modules_for_package(package, module_list):
    """Recursively fetch module names from a package and its subpackages."""
    for _, obj in inspect.getmembers(package, inspect.ismodule):
        if len(inspect.getmembers(obj, inspect.isclass)) > 0:
            module_list.append(obj.__name__)

        if len(inspect.getmembers(obj, inspect.ismodule)) > 0:
            get_modules_for_package(obj, module_list)


module_list = ["ostk.astrodynamics"]
get_modules_for_package(
    package=importlib.import_module("ostk.astrodynamics.OpenSpaceToolkitAstrodynamicsPy"),
    module_list=module_list,
)

python_apigen_modules = {
    module: f"api/{module.replace('.','/')}/" for module in module_list
}
python_apigen_modules[
    "ostk.astrodynamics.trajectory.state"
] = "api/ostk/astrodynamics/pytrajectory/"

python_apigen_default_groups = [
    (r"method:.*", "Public methods"),
    (r"method:.*\.__[A-Za-z,_]*__", "Special methods"),
    (r"method:.*\.__(init|new)__", "Constructors"),
    (r"method:.*\.get_.*", "Getters"),
    (r"method:.*\.access_.*", "Getters"),
    (r"method:.*\.undefined.*", "Static methods"),
    (r"method:.*\.default.*", "Static methods"),
    (r"class:.*\.[A-Z]*", "Enums"),
    ("property:.*", "Properties"),
]
python_apigen_default_order = [
    ("class:.*", 10),
    (r"method:.*", 40),
    (r"method:.*\.__[A-Za-z,_]*__", 20),
    (r"method:.*\.__(init|new)__", 10),
    (r"method:.*\.get_.*", 30),
    ("property:.*", 40),
]

python_apigen_order_tiebreaker = "alphabetical"
python_apigen_case_insensitive_filesystem = False
python_apigen_show_base_classes = True

# General API configuration
object_description_options = [
    ("py:.*", dict(include_rubrics_in_toc=True)),
]

python_module_names_to_strip_from_xrefs = ["ostk.astrodynamics", "ostk"]

# ----------------------------
# HTML Output Configuration
# ----------------------------
html_theme = "sphinx_immaterial"
html_static_path = ["../_static"]
# html_css_files = ["extra.css"]
html_title = "Open Space Toolkit Astrodynamics"
html_favicon = "../assets/ostk.png"
html_logo = "../assets/ostk.png"

# Additional configurations
html_theme_options = {...}  # as per your existing setup


# ----------------------------
# Custom Sphinx Behavior
# ----------------------------
def remove_unwanted_bases(app, name, obj, options, bases):
    bases[:] = [
        base for base in bases if "pybind11_builtins.pybind11_object" not in str(base)
    ]


def add_order_for_group(app, what, name, obj, options, lines):
    for ignored_name in ["ostk.astrodynamics.URL", "ostk.astrodynamics.Environment"]:
        if name.startswith(ignored_name):
            return

    if what == "class":
        if "group" not in "".join(lines):
            lines[:] = lines + [":group: Enum", ":order: 50", ""]

    if what == "method":
        # instance method
        if (
            hasattr(obj, "__self__")
            and hasattr(obj, "__func__")
            and not isinstance(obj.__self__, type)
        ):
            return

        # class method
        if hasattr(obj, "__self__") and isinstance(obj.__self__, type):
            return

        # must be a static method
        if ":group: Static methods" not in lines:
            lines[:] = lines + [":group: Static methods", ""]

        lines[:] = lines + [":order: 60", ""]


def setup(app):
    app.connect("autodoc-process-bases", remove_unwanted_bases)
    app.connect("autodoc-process-docstring", add_order_for_group)


# ----------------------------
# Sphinx Immaterial Options
# ----------------------------

html_theme_options = {
    "icon": {
        "repo": "fontawesome/brands/github",
    },
    # "site_url": "https://galois.readthedocs.io/",
    "repo_url": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
    "repo_name": "open-space-toolkit-astrodynamics",
    "social": [
        {
            "icon": "fontawesome/brands/github",
            "link": "https://github.com/open-space-collective/open-space-toolkit-astrodynamics",
        },
        {
            "icon": "fontawesome/brands/python",
            "link": "https://pypi.org/project/open-space-toolkit-astrodynamics/",
        },
    ],
    "edit_uri": "",
    "globaltoc_collapse": True,
    "features": [
        # "navigation.expand",
        "navigation.tabs",
        # "toc.integrate",
        # "navigation.instant",
        # "header.autohide",
        "navigation.top",
        "navigation.tracking",
        "toc.follow",
        "toc.sticky",
        "content.tabs.link",
        "announce.dismiss",
    ],
    "palette": [
        {
            "media": "(prefers-color-scheme: light)",
            "scheme": "default",
            "toggle": {
                "icon": "material/weather-night",
                "name": "Switch to dark mode",
            },
        },
        {
            "media": "(prefers-color-scheme: dark)",
            "scheme": "slate",
            "toggle": {
                "icon": "material/weather-sunny",
                "name": "Switch to light mode",
            },
        },
    ],
}

# html_last_updated_fmt: Literal[''] = ""
html_use_index = True
html_domain_indices = True

sphinx_immaterial_custom_admonitions = [
    {
        "name": "seealso",
        "title": "See also",
        "classes": ["collapsible"],
        "icon": "fontawesome/regular/eye",
        "override": True,
    },
    {
        "name": "star",
        "icon": "octicons/star-16",
        "color": (255, 233, 3),  # Gold
    },
    {
        "name": "fast-performance",
        "title": "Faster performance",
        "icon": "material/speedometer",
        "color": (47, 177, 112),  # Green: --md-code-hl-string-color
    },
    {
        "name": "slow-performance",
        "title": "Slower performance",
        "icon": "material/speedometer-slow",
        "color": (230, 105, 91),  # Red: --md-code-hl-number-color
    },
]
