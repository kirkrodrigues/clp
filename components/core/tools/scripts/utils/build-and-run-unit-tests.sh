#!/usr/bin/env bash

# Exit on any error
set -e
# Error on undefined variable
set -u

# Builds the project.
#
# @param src_dir
# @param build_dir
# @param num_jobs Max number of jobs to run when building.
# @param use_static_libs Whether to use static libraries when building.
build () {
    src_dir="$1"
    build_dir="$2"
    use_static_libs="$3"
    num_jobs="$4"

    cmake_flags=()
    if [ "$use_static_libs" -eq 0 ]; then
        cmake_flags+=(-DCLP_USE_STATIC_LIBS=OFF)
    else
        cmake_flags+=(-DCLP_USE_STATIC_LIBS=OFF)
    fi

    rm -rf "$build_dir"
    cmake "${cmake_flags[@]}" -S "$src_dir" -B "$build_dir"
    cmake --build "$build_dir" --parallel "$num_jobs"
}

cUsage="Usage: ${BASH_SOURCE[0]} <source-dir> <build-dir>[ <unit-tests-filter>]"
if [ "$#" -lt 2 ]; then
    echo "$cUsage"
    exit 1
fi
src_dir="$1"
build_dir="$2"
if [ "$#" -gt 2 ]; then
    unit_tests_filter="$3"
fi

num_cores="$(getconf _NPROCESSORS_ONLN)"

build "$src_dir" "$build_dir" 0 "$num_cores"
build "$src_dir" "$build_dir" 1 "$num_cores"

cd "$build_dir"
if [ -z "${unit_tests_filter+x}" ]; then
    ./unitTest
else
    ./unitTest "$unit_tests_filter"
fi
