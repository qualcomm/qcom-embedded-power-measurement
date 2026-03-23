#!/bin/sh

# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

rm -rf EPMDev.egg-info
rm -rf TACDev.egg-info
rm -rf UDASDev.egg-info
rm -rf build
rm -rf dist

pip3 uninstall -y EPMDev
pip3 uninstall -y TACDev
pip3 uninstall -y UDASDev
python3 setup.py install
