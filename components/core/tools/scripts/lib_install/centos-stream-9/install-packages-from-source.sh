#!/usr/bin/env bash

# Exit on any error
set -e

# Error on undefined variable
set -u

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
lib_install_scripts_dir=$script_dir/..

# NOTE: The remaining installation scripts depend on and boost, so we install them beforehand.
"$lib_install_scripts_dir"/install-boost.sh 1.76.0
"$lib_install_scripts_dir"/fmtlib.sh 8.0.1
"$lib_install_scripts_dir"/spdlog.sh 1.9.2
# TODO Why does shared linking make these unlinkable?
#"$lib_install_scripts_dir"/mongoc.sh 1.24.4
"$lib_install_scripts_dir"/mongocxx.sh 3.10.2
"$lib_install_scripts_dir"/msgpack.sh 6.0.0
