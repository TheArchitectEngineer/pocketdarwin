#!/bin/sh

# autogen.sh - Bootstrap script for FreeBoard autotools

set -e

echo "FreeBoard Autotools Bootstrap"
echo "============================="

# Check for required tools
for tool in aclocal autoconf autoheader automake; do
    if ! command -v $tool >/dev/null 2>&1; then
        echo "Error: $tool is required but not found in PATH"
        exit 1
    fi
done

# Check for libtoolize (glibtoolize on macOS)
if command -v glibtoolize >/dev/null 2>&1; then
    LIBTOOLIZE="glibtoolize"
elif command -v libtoolize >/dev/null 2>&1; then
    LIBTOOLIZE="libtoolize"
else
    echo "Error: libtoolize or glibtoolize is required but not found in PATH"
    exit 1
fi

echo "Running libtoolize..."
$LIBTOOLIZE --force --copy

echo "Running aclocal..."
aclocal -I m4

echo "Running autoheader..."
autoheader

echo "Running autoconf..."
autoconf

echo "Running automake..."
automake --add-missing --copy

echo "Bootstrap complete!"
echo ""
echo "Now run: ./configure"
echo "Then: make"
