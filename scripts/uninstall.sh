#!/usr/bin/env bash
set -euo pipefail

BIN_NAME="sysalias"
PREFIX_USER="$HOME/.local/bin"
PREFIX_SYSTEM="/usr/local/bin"

TARGET="$PREFIX_USER"
if [ "${1:-}" = "--system" ]; then
  TARGET="$PREFIX_SYSTEM"
fi

echo "[*] Removing $BIN_NAME from $TARGET"
rm -f "$TARGET/$BIN_NAME"

echo "[OK] Uninstalled $BIN_NAME from $TARGET"
