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

# Create directory structure
#mkdir -p "$PKGROOT/usr/bin"
#mkdir -p "$PKGROOT/usr/lib/genie"
#mkdir -p "$PKGROOT/usr/share/geany"
#mkdir -p "$PKGROOT/usr/share/icons/hicolor/48x48/apps"

cp -r "$PREFIX/usr" "$PKGROOT/usr"

# create additional directories
mkdir -p "$DEBIAN_DIR"
mkdir -p "$PKGROOT/usr/share/applications"

# Copy program files
#echo "Copying files..."
#cp -r "$PREFIX/share/geany"/* "$PKGROOT/usr/share/geany/" 2>/dev/null || true

# Copy binary
#if [ -f "$PREFIX/bin/geany" ]; then
#    cp "$PREFIX/bin/geany" "$PKGROOT/usr/bin/genie"
#else
#    echo "ERROR: geany binary not found in $PREFIX/bin"
#    exit 1
#fi

# Copy libraries
#cp "$PREFIX/lib/"libgeany.so* "$PKGROOT/usr/lib/genie/"

# Fix RPATH on libraries
#patchelf --set-rpath '$ORIGIN/../lib/genie' "$PKGROOT/usr/bin/genie"

# Copy icon (try to find one)
#ICON_SRC="$PREFIX/share/icons/hicolor/48x48/apps/geany.png"
#if [ -f "$ICON_SRC" ]; then
#    cp "$ICON_SRC" "$PKGROOT/usr/share/icons/hicolor/48x48/apps/genie.png"
#else
#    echo "Warning: icon not found, skipping"
#fi

# Create launcher (.desktop)
cat > "$PKGROOT/usr/share/applications/genie.desktop" <<EOF
[Desktop Entry]
Name=Genie
Comment=Lightweight IDE (custom build)
Exec=/usr/bin/geany
Icon=geany
Terminal=false
Type=Application
Categories=Development;IDE;
EOF

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