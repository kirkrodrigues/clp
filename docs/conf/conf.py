# -- Project information -------------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = "CLP"

# -- General configuration -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    "myst_parser",
    "sphinxcontrib.mermaid",
    "sphinx_design",
    "sphinx.ext.viewcode",
    "sphinx.ext.autodoc",
    "sphinx_copybutton",
]

templates_path = ["_templates"]

# -- HTML output options -------------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_favicon = "https://docs.yscope.com/_static/favicon.ico"
html_title = "CLP"
html_show_copyright = False
html_static_path = ["../src/_static"]

html_theme = "furo"

# -- Furo theme options --------------------------------------------------------
# https://furo.readthedocs.io/customisation/index.html
html_theme_options = {
    "source_repository": "https://github.com/y-scope/clp/",
    "source_branch": "main",
    "source_directory": "docs/src",
}

# -- MyST extensions -----------------------------------------------------------
# https://myst-parser.readthedocs.io/en/stable/syntax/optional.html
myst_enable_extensions = [
    "attrs_block",
    "colon_fence",
]

# -- Sphinx autodoc options ----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/extensions/autodoc.html#configuration

autoclass_content = "class"
autodoc_class_signature = "separated"
autodoc_typehints = "description"
