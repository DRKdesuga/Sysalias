#!/usr/bin/env bash
set -euo pipefail

BIN_NAME="sysalias"
PREFIX_USER="$HOME/.local/bin"
PREFIX_SYSTEM="/usr/local/bin"

echo "[*] Building $BIN_NAME..."
make

TARGET="$PREFIX_USER"
if [ "${1:-}" = "--system" ]; then
  TARGET="$PREFIX_SYSTEM"
fi

echo "[*] Installing to $TARGET"
mkdir -p "$TARGET"
cp "$BIN_NAME" "$TARGET/$BIN_NAME"

echo "[OK] Installed $BIN_NAME into $TARGET"
echo "Add $PREFIX_USER to your PATH if not already."
