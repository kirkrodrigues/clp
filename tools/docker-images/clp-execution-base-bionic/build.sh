#!/bin/bash

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
repo_root=${script_dir}/../../../

docker build -t clp-execution-x86-ubuntu-bionic:dev ${repo_root} --file ${script_dir}/Dockerfile
