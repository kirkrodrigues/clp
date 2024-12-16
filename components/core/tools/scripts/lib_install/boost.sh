#!/usr/bin/env bash

# Exit on error
set -e

# Treat unset variables as errors
set -u

# Checks if version `$2` of package name `$1` is installed and that a static
# library called `$3` exists.
#
# @param $1 Name of the package
# @param $2 Version of the package
# @param $3 Name of the library
# @return 0 on success, 1 on failure.
check_if_installed () {
    local package_name="$1"
    local version="$2"
    local lib_name="$3"

    set +e

    # Check if the package is installed
    local installed_version
    installed_version=$(dpkg -s "$package_name" | awk '/^Version:/ {print $2}')
    local pkg_found=0
    if [[ "$installed_version" != "$version" ]]; then
        pkg_found=1
    fi

    # Check if the static library is installed
    local static_lib_found=0
    if [ $pkg_found -eq 0 ]; then
        local lib_filename="lib${lib_name}.a"
        find /usr/lib/ /usr/local/lib/ -name "$lib_filename" | grep -q "."
        static_lib_found=$?
        if [ $static_lib_found -ne 0 ]; then
            echo "$lib_filename not found."
        fi
    fi

    installed=$((pkg_found | static_lib_found))

    set -e

    if [ $installed -eq 0 ]; then
        echo "Found ${package_name}=${version}."
        return 0
    fi

    return 1
}

# Downloads source code from a URL and extracts it.
#
# @param $1 URL of the source code.
# @param $2 Where to store the downloaded source archive.
# @param $3 Where to extract the source archive.
# @return 0 on success, 1 on failure.
download_and_extract_source () {
    local source_url="$1"
    local output_archive_path="$2"
    local output_extraction_dir="$3"

    if [ -e "$output_extraction_dir" ] ; then
        return 0
    fi

    if [ ! -e "$output_archive_path" ] ; then
        curl \
                --fail \
                --show-error \
                --location "$source_url" \
                --output "$output_archive_path"
    fi

    cd "$(dirname "$output_extraction_dir")"
    if [[ "$output_archive_path" == *.tar.gz ]] ; then
        tar xf "$output_archive_path"
    elif [[ "$output_archive_path" == *.zip ]] ; then
        unzip "$output_archive_path"
    fi
}

cUsage="Usage: ${BASH_SOURCE[0]} <version> <libraries> [ <.deb output directory>]"
if [ "$#" -lt 2 ] ; then
    echo "$cUsage"
    exit
fi
version=$1
libs_concatenated="$2"

all_libs_installed=0
IFS="," read -r -a libs <<< "$libs_concatenated"
for lib in "${libs[@]}"; do
    package_name="libboost-${lib}-dev"
    if ! check_if_installed "$package_name" "$version" "$lib"; then
        all_libs_installed=$((all_libs_installed | 1))
    fi
done
if [ $all_libs_installed -eq 0 ]; then
    # Nothing to do
    echo "DEBUG: Already installed"
    exit
fi

echo "Checking for elevated privileges..."
install_cmd_prefix=()
if [ ${EUID:-$(id -u)} -ne 0 ]; then
    sudo echo "Script can elevate privileges."
    install_cmd_prefix+=("sudo")
else
    echo "Script running as root."
fi

temp_dir="/tmp/boost-installation"
mkdir -p "$temp_dir"

version_with_underscores="${version//./_}"
tar_filename="boost_${version_with_underscores}.tar.gz"
source_url="https://boostorg.jfrog.io/artifactory/main/release/${version}/source/${tar_filename}"
output_tar_path="${temp_dir}/${tar_filename}"
output_extraction_dir="${temp_dir}/boost_${version_with_underscores}"
if ! download_and_extract_source "$source_url" "$output_tar_path" "$output_extraction_dir" ; then
    exit 1
fi

for lib in "${libs[@]}"; do
    package_name="libboost-${lib}-dev"

    # Build library with/without deb package as necessary
    # install library
done
