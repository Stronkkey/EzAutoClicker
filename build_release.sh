#!/usr/bin/env bash
set -euo pipefail

mkdir -p build-release
docker build -t ez-auto-clicker-builder .

docker run --rm --userns=keep-id \
  -v "$PWD":/src \
  -u "$(id -u):$(id -g)" \
  ez-auto-clicker-builder \
  bash -c '
    meson setup build-release --wipe --buildtype=release \
      -Db_lto=true -Db_ndebug=true \
      -Dcpp_link_args="-static-libstdc++ -static-libgcc" &&
    meson compile -C build-release &&
    strip build-release/EzAutoClicker
    '
