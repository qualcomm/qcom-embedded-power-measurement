#!/bin/bash

# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

set -e

if [ -z "$QTBIN" ]; then
    echo ""
    echo "ERROR: QTBIN is not set."
    echo "       QTBIN must point to the Qt bin directory, e.g.:"
    echo "         export QTBIN=/path/to/Qt/<version>/gcc_64/bin"
    echo "       Then re-run this script."
    exit 1
fi

if [ ! -d "$QTBIN" ]; then
    echo ""
    echo "ERROR: QTBIN directory does not exist: $QTBIN"
    echo "       Install Qt 6.9+ via the Qt Online Installer (https://www.qt.io/download-qt-installer-oss)"
    echo "       and include the GCC 64-bit component, then update QTBIN."
    exit 1
fi

if ! echo "$QTBIN" | grep -q "gcc_64"; then
    echo ""
    echo "ERROR: QTBIN does not point to a GCC 64-bit Qt installation."
    echo "       QTBIN is currently: $QTBIN"
    echo "       A Linux build requires the Qt GCC 64-bit component. QTBIN must contain 'gcc_64', e.g.:"
    echo "         export QTBIN=/path/to/Qt/<version>/gcc_64/bin"
    exit 1
fi

if ! command -v ninja &>/dev/null; then
    echo ""
    echo "ERROR: ninja not found in PATH."
    echo "       Install ninja via your package manager, e.g.:"
    echo "         sudo apt install ninja-build"
    echo "       Or via the Qt installer (Tools > Ninja)."
    exit 1
fi

export PATH="$QTBIN:$PATH"

###############################################################################
# Clean start
###############################################################################

rm -rf build __Builds

###############################################################################
# Debug Build
###############################################################################

cmake -S . -B build/Debug \
    -DCMAKE_PREFIX_PATH="$(dirname "$QTBIN")" \
    -DCMAKE_COLOR_DIAGNOSTICS=ON \
    -DCMAKE_GENERATOR=Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS_INIT=-DQT_QML_DEBUG
cmake --build build/Debug

###############################################################################
# Release Build
###############################################################################

cmake -S . -B build/Release \
    -DCMAKE_PREFIX_PATH="$(dirname "$QTBIN")" \
    -DCMAKE_COLOR_DIAGNOSTICS=ON \
    -DCMAKE_GENERATOR=Ninja \
    -DCMAKE_BUILD_TYPE=Release
cmake --build build/Release

###############################################################################
# Qt Runtime Deployment
###############################################################################

echo ""
echo "=========================================================="
echo "Deploying Qt Runtime"
echo "=========================================================="

QT_ROOT="$(dirname "$QTBIN")"

DEPLOY_BIN_DIR="__Builds/Linux/Release/bin"
DEPLOY_LIB_DIR="__Builds/Linux/Release/lib"
DEPLOY_PLUGIN_DIR="__Builds/Linux/Release/plugins"

mkdir -p "$DEPLOY_LIB_DIR"
mkdir -p "$DEPLOY_PLUGIN_DIR"

###############################################################################
# Copy Qt Runtime Libraries
###############################################################################

echo ""
echo "Copying Qt runtime libraries..."

find "$QT_ROOT/lib" \
    -maxdepth 1 \
    -name "libQt6*.so*" \
    -exec cp -a {} "$DEPLOY_LIB_DIR/" \;

echo "Qt runtime libraries copied."

###############################################################################
# Copy ICU Libraries
###############################################################################

echo ""
echo "Copying ICU libraries..."

for icu_lib in libicui18n.so libicuuc.so libicudata.so; do
    find "$QT_ROOT/lib" -name "${icu_lib}*" -exec cp -a {} "$DEPLOY_LIB_DIR/" \; 2>/dev/null || true
done

###############################################################################
# Deploy Qt Plugins
###############################################################################

echo ""
echo "Deploying Qt plugins..."

for plugin_dir in \
    iconengines \
    imageformats \
    platforminputcontexts \
    platforms \
    platformthemes \
    xcbglintegrations
do
    if [ -d "$QT_ROOT/plugins/$plugin_dir" ]; then
        mkdir -p "$DEPLOY_PLUGIN_DIR/$plugin_dir"
        echo "  $plugin_dir"
        cp -a "$QT_ROOT/plugins/$plugin_dir/." "$DEPLOY_PLUGIN_DIR/$plugin_dir/"
    fi
done

###############################################################################
# Fix RUNPATH for deployed binaries
###############################################################################

echo ""
echo "Fixing RUNPATH..."

find "$DEPLOY_BIN_DIR" -type f -executable | while read -r exe; do
    patchelf --set-rpath '$ORIGIN/../lib' "$exe" 2>/dev/null || true
done

###############################################################################
# Validate deployment
###############################################################################

echo ""
echo "Checking for broken library symlinks..."

BROKEN_SYMLINKS=$(find "$DEPLOY_LIB_DIR" -xtype l 2>/dev/null || true)

if [ -n "$BROKEN_SYMLINKS" ]; then
    echo ""
    echo "ERROR: Broken library symlinks detected:"
    echo "$BROKEN_SYMLINKS"
    exit 1
fi

echo "No broken library symlinks detected."

###############################################################################
# Summary
###############################################################################

echo ""
echo "=========================================================="
echo "Qt Deployment Complete"
echo "=========================================================="

echo ""
echo "Release Output:"
echo "  __Builds/Linux/Release"

echo ""
echo "Libraries:"
echo "  $DEPLOY_LIB_DIR"

echo ""
echo "Plugins:"
echo "  $DEPLOY_PLUGIN_DIR"

echo ""
echo "Library Count:"
find "$DEPLOY_LIB_DIR" -type f | wc -l

echo ""
echo "Plugin Count:"
find "$DEPLOY_PLUGIN_DIR" -type f | wc -l

echo ""
echo "Check __Builds directory"