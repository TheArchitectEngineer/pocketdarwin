#!/bin/bash
# build.sh - Build script for Interlude Boot System with Autotools

set -e

echo "=== Interlude Boot System Build Script ==="
echo ""

# Check if we're in the right directory
if [ ! -f "configure.ac" ] || [ ! -f "Makefile.am" ]; then
    echo "Error: Must be run from interlude source directory"
    echo "Expected files: configure.ac, Makefile.am"
    exit 1
fi

# Clean previous build
echo "Cleaning previous build..."
if [ -f "Makefile" ]; then
    make clean || true
fi
rm -f config.status config.log

# Generate configure script
echo "Generating configure script..."
autoreconf -fiv

# Configure with common options
echo "Configuring build..."
./configure \
    --prefix=/usr/local \
    --sbindir=/usr/local/sbin \
    --enable-silent-rules

# Build
echo "Building..."
make

# Show build results
echo ""
echo "=== Build Complete ==="
echo "Binaries created:"
ls -la darwin_interlude vmapple 2>/dev/null || echo "  Main binaries not found"

if [ -f "l2d_dump" ]; then
    echo "  l2d_dump (with libfdt support)"
fi

echo ""
echo "Installation commands:"
echo "  sudo make install    # Install to /usr/local/sbin"
echo "  make install        # Install to configured prefix"
echo ""
echo "Test commands:"
echo "  make test           # Run basic tests"
echo "  ./darwin_interlude --help"
echo "  ./vmapple --help"
if [ -f "l2d_dump" ]; then
    echo "  ./l2d_dump --help"
fi
