#!/usr/bin/env bash

# Exit on error
set -e

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

# Enable gcc 7
source /opt/rh/devtoolset-7/enable

# NOTE: cmake and boost must be installed first since the remaining packages depend on them
"$script_dir"/../install-cmake.sh 3.21.2
"$script_dir"/../install-boost.sh 1.76.0

"$script_dir"/../fmtlib.sh 8.0.1
"$script_dir"/../libarchive.sh 3.5.1
"$script_dir"/../lz4.sh 1.8.2
"$script_dir"/../mariadb-connector-c.sh 3.2.3
"$script_dir"/../msgpack.sh 6.0.0
"$script_dir"/../spdlog.sh 1.9.2
"$script_dir"/../zstandard.sh 1.5.5
