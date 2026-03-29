#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

PREFIX="$ROOT/deploy/debug"
PKGROOT="$ROOT/package/debug"
DEBIAN_DIR="$PKGROOT/DEBIAN"

APP_NAME="geany-custom-debug"
VERSION="2.1-custom"
ARCH="amd64"

echo "== Building DEBUG .deb package =="
echo "PREFIX:  $PREFIX"

# Sanity check
if [ ! -d "$PREFIX" ]; then
    echo "ERROR: Install directory not found: $PREFIX"
    exit 1
fi

# Clean previous package
rm -rf "$PKGROOT"
mkdir -p "$DEBIAN_DIR"

# Copy installed files
echo "Copying files..."
cp -r "$PREFIX"/* "$PKGROOT"/

# Create control file
cat > "$DEBIAN_DIR/control" <<EOF
Package: $APP_NAME
Version: $VERSION
Section: editors
Priority: optional
Architecture: $ARCH
Maintainer: Custom Build <you@example.com>
Description: Debug build of Geany 2.1
EOF

# Fix permissions
chmod 755 "$PKGROOT"
chmod 755 "$DEBIAN_DIR"

# Build package
OUTPUT_DEB="$ROOT/package/${APP_NAME}_${VERSION}_${ARCH}.deb"

echo "Building .deb..."
dpkg-deb --build "$PKGROOT" "$OUTPUT_DEB"

echo "Done!"
echo "$OUTPUT_DEB"