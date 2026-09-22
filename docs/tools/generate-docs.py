# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.

"""Build the QEPM documentation with Sphinx."""
import argparse
import shutil
import subprocess
import sys
from pathlib import Path

DOCS_DIR = Path(__file__).resolve().parent.parent
DEFAULT_OUTPUT_DIR = DOCS_DIR.parent / "__Builds" / "docs"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", default=str(DEFAULT_OUTPUT_DIR), help="Output directory (default: %(default)s)")
    parser.add_argument("--version", default="", help="Version string, typically the release pipeline's git tag")
    args = parser.parse_args()

    output_dir = Path(args.output)
    if output_dir.exists():
        shutil.rmtree(output_dir)

    cmd = [sys.executable, "-m", "sphinx", "-b", "html", str(DOCS_DIR), str(output_dir)]
    if args.version:
        cmd += ["-D", f"version={args.version}", "-D", f"release={args.version}"]

    sys.exit(subprocess.run(cmd).returncode)


if __name__ == "__main__":
    main()
