project = "QEPM"
copyright = "Qualcomm Technologies, Inc. and/or its subsidiaries"
author = "Qualcomm Technologies, Inc."

extensions = [
    "myst_parser",
    "sphinx.ext.autosectionlabel",
    "sphinx_click.ext",
    "sphinx_markdown_tables",
    "pallets_sphinx_themes",
]

source_suffix = {
    ".rst": "restructuredtext",
    ".md": "markdown",
}

master_doc = "index"
exclude_patterns = ["tools", "Thumbs.db", ".DS_Store"]

html_theme = "flask"

man_pages = [
    (master_doc, "qepm", "QEPM Documentation", [author], 1),
]
