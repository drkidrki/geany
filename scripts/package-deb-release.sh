#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

PREFIX="$ROOT/deploy/release"
PKGROOT="$ROOT/package/release"
DEBIAN_DIR="$PKGROOT/DEBIAN"

APP_NAME="genie"
VERSION="2.1-drki"
ARCH="amd64"

echo "== Building RELEASE .deb package =="

# Sanity check
if [ ! -d "$PREFIX" ]; then
    echo "ERROR: Install directory not found: $PREFIX"
    exit 1
fi

# Clean previous package
rm -rf "$PKGROOT"

# copy all installed files
mkdir -p "$PKGROOT/"
cp -r "$PREFIX/usr/" "$PKGROOT/usr"

# Create directory structure
mkdir -p "$DEBIAN_DIR"
mkdir -p "$PKGROOT/usr/share/applications"

# delete include files
rm -rf "$PKGROOT/usr/include"

# Create control file with dependencies
cat > "$DEBIAN_DIR/control" <<EOF
Package: $APP_NAME
Version: $VERSION
Section: editors
Priority: optional
Architecture: $ARCH
Maintainer: Custom Build <you@example.com>
Depends: libc6 (>= 2.31), libgtk-3-0 (>= 3.24), libglib2.0-0 (>= 2.64)
Description: Custom build of Geany 2.1
 A lightweight IDE with basic features (custom build).
EOF

# Permissions
chmod 755 "$PKGROOT"
chmod 755 "$DEBIAN_DIR"
chmod 755 "$PKGROOT/usr/bin/geany"

# Build package
OUTPUT_DEB="$ROOT/package/${APP_NAME}_${VERSION}_${ARCH}.deb"

echo "Building .deb..."
dpkg-deb --build "$PKGROOT" "$OUTPUT_DEB"

echo "Done!"
echo "Package:"
echo "$OUTPUT_DEB"