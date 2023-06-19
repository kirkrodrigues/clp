#!/usr/bin/env bash

# Exit on error
set -e

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
lib_install_dir="$script_dir"/..

"$lib_install_dir"/fmtlib.sh 8.0.1
"$lib_install_dir"/libarchive.sh 3.5.1
"$lib_install_dir"/lz4.sh 1.8.2
"$lib_install_dir"/mariadb-connector-c.sh 3.2.3
"$lib_install_dir"/msgpack.sh 6.0.0
"$lib_install_dir"/spdlog.sh 1.9.2
"$lib_install_dir"/zstandard.sh 1.4.9
