#!/usr/bin/env bash

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

pushd "$script_dir"

./install-prebuilt-packages.sh
./install-packages-from-source.sh

popd
