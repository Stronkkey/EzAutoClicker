#!/usr/bin/env bash
set -euo pipefail

export ORIGINAL_PWD="$PWD"

[ -f "../meson.build" ] && cd ..

mkdir -p build-release

meson setup build-release \
  --wipe \
  --buildtype=release \
  -Db_lto=true \
  -Db_ndebug=true \
  -Dprefix=/usr
meson compile -C build-release
strip build-release/EzAutoClicker
meson install -C build-release --strip --destdir ezautoclicker
cd "$ORIGINAL_PWD"
