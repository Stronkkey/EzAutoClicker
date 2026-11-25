#!/usr/bin/env bash

docker run --rm --userns=keep-id \
  -v "$PWD":/src \
  -u "$(id -u):$(id -g)" \
  ez-auto-clicker-builder \
  bash -c '
    source packaging/build_release.sh'
