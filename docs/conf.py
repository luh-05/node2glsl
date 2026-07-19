import os
import sys

# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'mollusk'
copyright = '2026, Vincent Antoni, Laurin Von Bergmann, Helena Hummel, Tom Schäfer, Lasse Hüffler'
author = 'Vincent Antoni, Laurin Von Bergmann, Helena Hummel, Tom Schäfer, Lasse Hüffler'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    "breathe",
]

breathe_projects = {
    "mollusk": "../build/docs/doxygen/xml/"
}
breathe_default_project = "mollusk"

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']



# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
