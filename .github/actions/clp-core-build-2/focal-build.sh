#!/usr/bin/env bash

set -e
set -u

pushd components/core || exit

./tools/scripts/deps-download/download-all.sh
cmake -B ./focal-build -DCMAKE_BUILD_TYPE=Release
cmake --build ./focal-build --config Release
