#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

BUILD="$ROOT/build/release"
PREFIX="$ROOT/deploy/release"

echo "== Install RELEASE =="
echo "BUILD:  $BUILD"
echo "PREFIX: $PREFIX"

# Sanity checks
if [ ! -d "$BUILD" ]; then
    echo "ERROR: Build directory does not exist: $BUILD"
    exit 1
fi

if [ ! -f "$BUILD/Makefile" ]; then
    echo "ERROR: Makefile not found. Did you run configure?"
    exit 1
fi

# Clean previous install
if [ -d "$PREFIX" ]; then
    echo "Removing old install..."
    rm -rf "$PREFIX"
fi

mkdir -p "$PREFIX"

cd "$BUILD"

echo "Installing..."
make DESTDIR="$PREFIX" install

echo "Done."
echo "Binary:"
echo "$PREFIX/bin/geany"