#!/usr/bin/env bash
set -e

ROOT=$(cd "$(dirname "$0")/.." && pwd)

BUILD=$ROOT/build/debug
PREFIX=$ROOT/deploy/debug

mkdir -p "$BUILD"
mkdir -p "$PREFIX"

cd "$BUILD"

"$ROOT/configure" \
    --prefix="$PREFIX" \
    CFLAGS="-O0 -g3" \
    CXXFLAGS="-O0 -g3"

make -j$(nproc)