# Documentation build tools

This directory contains the tooling used to build the QEPM documentation
(everything under `docs/bootcamp` and `docs/getting-started`) into a static
HTML site with [Sphinx](https://www.sphinx-doc.org/).

The Sphinx configuration lives at `docs/conf.py` and the top-level toctree at
`docs/index.rst`. `generate-docs.py` in this directory drives the build.

## Usage

From this directory:

```
pip install -r requirements.txt
python generate-docs.py
```

The built site is written to `__Builds/docs/` by default. At install time it
is placed at `docs/` alongside the installed application binaries — this is
the path the shipped Qt applications' offline Help menu expects. Pass
`--output` to write elsewhere, and `--version` to stamp a version string into
the docs (used by CI to pass the release tag):

```
python generate-docs.py --output /path/to/output --version v6.4.0
```
