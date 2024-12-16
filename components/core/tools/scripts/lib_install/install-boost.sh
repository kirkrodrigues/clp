#!/usr/bin/env bash

# Exit on error
set -e

# Treat unset variables as errors
set -u

# Checks if version `version` of `package_name` is installed.
#
# @param $1 Name of the package
# @param $2 Concatenated list of libraries to check for
check_if_installed () {
    local package_name="$1"
    local libs_concatenated="$2"
    local version="$3"

    set +e
    pkg-config --exact-version="$version" "$package_name"
    pkg_found=$?
    static_lib_found=0
    if [ $pkg_found -eq 0 ]; then
        IFS="," read -r -a libs <<< "$libs_concatenated"
        for lib in "${libs[@]}"; do
            lib_filename="libboost_${lib}.a"
            find /usr/lib/ /usr/local/lib/ -name "$lib_filename" | grep -q "."
            static_lib_found=$?
            if [ $static_lib_found -ne 0 ]; then
                echo "$lib_filename not found."
                break
            fi
        done
    fi
    installed=$((pkg_found | static_lib_found))
    set -e
    if [ $installed -eq 0 ]; then
        echo "Found ${package_name}=${version}."
        return 0
    fi

    return 1
}

download_and_extract_source () {
    local archive_file_path="$1"
    local extracted_dir="$2"
    local url="$3"

    if [ -e "$extracted_dir" ] ; then
        return 0
    fi

    if [ ! -e "$archive_file_path" ] ; then
        curl \
                --fail \
                --silent \
                --show-error \
                --location "$url" \
                --output "$archive_file_path"
    fi

    if [[ "$archive_file_path" == *.tar.gz ]] ; then
        tar xf "$archive_file_path"
    elif [[ "$archive_file_path" == *.zip ]] ; then
        unzip "$archive_file_path"
    fi
}

build () {
    local extracted_dir="$1"
    local install_prefix="$2"

    cd "$extracted_dir"
    if [[ -n "$install_prefix" ]] ; then
        extra_build_args=("--prefix=$install_prefix")
    fi
    ./bootstrap.sh --with-libraries="$libs_concatenated" "${extra_build_args[@]}"
    ./b2 -j"$(nproc)"
}

build_deb_pkg_and_install () {
    local install_dir="$1"
    local privilege_escalation_cmd="$2"
    local deb_output_dir="$3"

    local metadata_dir="${install_dir}/DEBIAN"
    mkdir -p "$metadata_dir"

    cat <<EOF > "${metadata_dir}/control"
Package: $package_name
Architecture: $(dpkg --print-architecture)
Version: $version
Maintainer: YScope Inc. <dev@yscope.com>
Description: Boost C++ Libraries development file
Section: libdevel
Priority: optional
EOF

    local deb_pkg_path=$"${deb_output_dir}/${deb_pkg_name}.deb"
    dpkg-deb --root-owner-group --build "$install_dir" "$deb_pkg_path"

    install_cmd=(
        "$privilege_escalation_cmd"
        dpkg
        -i "$deb_pkg_path"
    )
    "${install_cmd[@]}"
}

install () {
    local extracted_dir="$1"
    local install_prefix="$2"
    local privilege_escalation_cmd="$3"

    cd "$extracted_dir"
    if [[ -n "$install_prefix" ]] ; then
        extra_build_args=("--prefix=$install_prefix")
    fi
    install_cmd=(
        "$privilege_escalation_cmd"
        ./b2
        install
        --prefix="$install_prefix"
    )
    "${install_cmd[@]}"
}

cUsage="Usage: ${BASH_SOURCE[0]} <version> <libraries> [ <.deb output directory>]"
if [ "$#" -lt 1 ] ; then
    echo "$cUsage"
    exit
fi
version=$1
libs_concatenated="$2"

temp_dir=/tmp/${package_name}-installation

deb_output_dir="$temp_dir"
if [[ "$#" -gt 2 ]] ; then
    deb_output_dir="$(readlink -f "$3")"
    if [ ! -d "$deb_output_dir" ] ; then
        echo "$deb_output_dir does not exist or is not a directory"
        exit
    fi
fi

package_name=libboost-all-dev

if check_if_installed "$package_name" "$libs_concatenated" "$version" ; then
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

version_with_underscores="${version//./_}"
tar_filename="boost_${version_with_underscores}.tar.gz"
tar_path="${temp_dir}/${tar_filename}"
extracted_dir="${temp_dir}/boost_${version_with_underscores}"
url="https://boostorg.jfrog.io/artifactory/main/release/${version}/source/${tar_filename}"
if ! download_and_extract_source "$tar_path" "$extracted_dir" "$url" ; then
    exit 1
fi

install_prefix=""
if command -v dpkg-deb ; then
    deb_pkg_name="${package_name}-${version}"
    install_dir="${temp_dir}/${deb_pkg_name}"
    install_prefix="${install_dir}/usr"
    mkdir -p "$install_prefix"
fi

if ! build "$extracted_dir" "$install_prefix" ; then
    exit 1
fi

if command -v dpkg-deb ; then
    build_deb_pkg_and_install "$install_dir" "${install_cmd_prefix[*]}" "$deb_output_dir"
else
    install "$extracted_dir" "$install_prefix" "${install_cmd_prefix[*]}"
fi

# Clean up
rm -rf "$temp_dir"
