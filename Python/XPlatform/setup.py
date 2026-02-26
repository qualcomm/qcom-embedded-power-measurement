# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

from setuptools import find_packages, setup

setup(
    name="TACDev",
    version="1.0.1",
    description="X Platform TACDev",
    author="Michael Simpson",
    author_email="msimpson@qti.qualcomm.com",
    keywords="TACDev TAC",
    packages=find_packages(exclude=["EPMDev", "UDASDev"]),
    license="Qualcomm Inc. Proprietary",
)

setup(
    name="EPMDev",
    version="1.0.1",
    description="X Platform EPMDev",
    author="Michael Simpson",
    author_email="msimpson@qti.qualcomm.com",
    keywords="EPMDev EPM",
    packages=find_packages(exclude=["TACDev", "UDASDev"]),
    license="Qualcomm Inc. Proprietary",
)

setup(
    name="UDASDev",
    version="1.0.1",
    description="X Platform UDASDev",
    author="Biswajit Roy",
    author_email="biswroy@qti.qualcomm.com",
    keywords="UDASDev EPM",
    packages=find_packages(exclude=["TACDev", "EPMDev"]),
    license="Qualcomm Inc. Proprietary",
)
