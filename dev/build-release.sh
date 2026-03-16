#!/usr/bin/env bash
set -e

ROOT=$(cd "$(dirname "$0")/.." && pwd)

BUILD=$ROOT/build/release
PREFIX=$ROOT/deploy/release

mkdir -p "$BUILD"
mkdir -p "$PREFIX"

cd "$BUILD"

"$ROOT/configure" \
    --prefix="$PREFIX" \
    CFLAGS="-O2" \
    CXXFLAGS="-O2"

make -j$(nproc)