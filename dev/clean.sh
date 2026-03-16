#!/usr/bin/env bash
set -e

ROOT=$(cd "$(dirname "$0")/.." && pwd)

rm -rf "$ROOT/build"
rm -rf "$ROOT/deploy"