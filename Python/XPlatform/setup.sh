#!/bin/sh

rm -rf EPMDev.egg-info
rm -rf TACDev.egg-info
rm -rf UDASDev.egg-info
rm -rf build
rm -rf dist

pip3 uninstall -y EPMDev
pip3 uninstall -y TACDev
pip3 uninstall -y UDASDev
python3 setup.py install
