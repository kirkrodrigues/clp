#!/usr/bin/env bash

set -e
set -u

pushd components/core || exit

cmake -B ./focal-build -DCMAKE_BUILD_TYPE=Release
cmake --build ./focal-build --config Release
