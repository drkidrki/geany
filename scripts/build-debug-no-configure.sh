#!/usr/bin/env bash
set -e

ROOT=$(cd "$(dirname "$0")/.." && pwd)

BUILD=$ROOT/build/debug
PREFIX=$ROOT/deploy/debug

mkdir -p "$BUILD"
mkdir -p "$PREFIX"

cd "$BUILD"

make -j$(nproc)