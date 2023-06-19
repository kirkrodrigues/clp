#!/usr/bin/env bash

# Exit on error
set -e

brew update
brew install \
  boost \
  cmake \
  fmt \
  gcc \
  libarchive \
  lz4 \
  mariadb-connector-c \
  msgpack-cxx \
  spdlog \
  pkg-config \
  zstd
