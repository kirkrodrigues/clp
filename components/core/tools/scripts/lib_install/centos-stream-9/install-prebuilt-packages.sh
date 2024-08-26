#!/usr/bin/env bash

# Exit on any error
set -e

# Error on undefined variable
set -u

dnf install -y \
    cmake \
    make \
    gcc-c++ \
    git \
    java-11-openjdk \
    libarchive-devel \
    libcurl-devel \
    openssl-devel \
    mariadb-connector-c-devel \
    libzstd-devel
