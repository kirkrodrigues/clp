#!/usr/bin/env bash

# Exit on any error
set -e

# Error on undefined variable
set -u

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
repo_root=${script_dir}/../../../

docker build -t clp-dependencies-x86-ubuntu-focal:dev "${repo_root}" \
    --file "${script_dir}/Dockerfile"
