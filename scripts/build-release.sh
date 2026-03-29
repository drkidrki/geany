#!/usr/bin/env bash
set -e

ROOT=$(cd "$(dirname "$0")/.." && pwd)

BUILD=$ROOT/build/release

mkdir -p "$BUILD"

cd "$BUILD"

"$ROOT/configure" \
    --prefix="/usr" \
    CFLAGS="-O2" \
    CXXFLAGS="-O2"

make -j$(nproc)