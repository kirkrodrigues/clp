#!/usr/bin/env bash

# Exit on error
set -e

# Treat unset variables as errors
set -u

# Builds the specified library.
#
# @param {string} $1 Source directory.
# @param {string} $2 Library name.
# @param {string} $3 If non-empty, the location where the library will eventually be installed.
# @return 0 on success, non-zero otherwise.
build_boost_lib () {
    local source_dir="$1"
    local lib_name="$2"
    local install_prefix="$3"

    if [[ -n "$install_prefix" ]] ; then
        extra_build_args=("--prefix=$install_prefix")
    fi
    cd "$source_dir" \
            && ./bootstrap.sh --with-libraries="$lib_name" "${extra_build_args[@]}" \
            && ./b2 -j"$(nproc)"
}

# Builds a Debian package archive (.deb) and installs it.
#
# @param {string} $1 Source directory.
# @param {string} $2 Name of the Debian package.
# @param {string} $3 Version of the Debian package.
# @param {string} $4 Name of the Boost library.
# @param {string} $5 Directory that contains the files for the Debian package.
# @param {string} $6 Directory in which to store the Debian package archive.
# @param {string} $7 If non-empty, the command that can be used to escalate privileges.
# @return 0 on success, 1 on failure.
build_deb_pkg_and_install () {
    local source_dir="$1"
    local pkg_name="$2"
    local pkg_version="$3"
    local lib_name="$4"
    local deb_pkg_contents_dir="$5"
    local deb_pkg_output_dir="$6"
    local privilege_escalation_cmd="$7"

    if ! write_dpkg_metadata_dir "$deb_pkg_contents_dir/DEBIAN" "$pkg_name" "$pkg_version"; then
        return 1
    fi

    local deb_pkg_path=$"${deb_pkg_output_dir}/${pkg_name}.deb"
    if ! dpkg-deb --root-owner-group --build "$deb_pkg_contents_dir" "$deb_pkg_path"; then
        return 1
    fi

    if ! install_lib "$source_dir" "" ""; then
        return 1
    fi

    install_cmd=()
    if [[ -n "$privilege_escalation_cmd" ]]; then
        install_cmd+=("$privilege_escalation_cmd")
    fi
    install_cmd+=(
        dpkg
        -i "$deb_pkg_path"
    )
    if ! "${install_cmd[@]}"; then
        return 1
    fi

    return 0
}

# Installs the library.
#
# @param {string} $1 Source directory.
# @param {string} $2 If non-empty, the command that can be used to escalate privileges.
# @param {string} $3 If non-empty, the location where the library should be installed.
# @return 0 on success, non-zero on failure.
install_lib () {
    local source_dir="$1"
    local privilege_escalation_cmd="$2"
    local install_prefix="$3"

    install_cmd=()
    if [[ -n "$privilege_escalation_cmd" ]] ; then
        install_cmd+=("$privilege_escalation_cmd")
    fi
    install_cmd+=(
        ./b2
        install
    )
    if [[ -n "$install_prefix" ]] ; then
        install_cmd+=("--prefix=$install_prefix")
    fi
    cd "$source_dir" && "${install_cmd[@]}"
}

# Writes the metadata for a Debian package.
#
# @param {string} $1 Directory in which to store the metadata.
# @param {string} $2 Name of the Debian package.
# @param {string} $3 Version of the Debian package.
# @return 0 on success, non-zero otherwise.
write_dpkg_metadata_dir () {
    local metadata_dir="$1"
    local pkg_name="$2"
    local pkg_version="$3"

    mkdir --parents "$metadata_dir" \
            && cat <<EOF > "${metadata_dir}/control"
Package: $pkg_name
Architecture: $(dpkg --print-architecture)
Version: $pkg_version
Maintainer: YScope Inc. <dev@yscope.com>
Description: Development files for Boost's ${lib_name} library.
Section: libdevel
Priority: optional
EOF
}

# Checks if pkg_version `$2` of package name `$1` is installed and that a static
# library called `$3` exists.
#
# @param $1 Name of the package
# @param $2 Version of the package
# @param $3 Name of the library
# @return 0 on success, 1 on failure.
check_if_installed () {
    local pkg_name="$1"
    local pkg_version="$2"
    local lib_name="$3"

    set +e

    # Check if the package is installed
    local installed_version
    installed_version=$(dpkg -s "$pkg_name" | awk '/^Version:/ {print $2}')
    local pkg_found=0
    if [[ "$installed_version" != "$pkg_version" ]]; then
        pkg_found=1
    fi

    # Check if the static library is installed
    local static_lib_found=0
    if [ $pkg_found -eq 0 ]; then
        local lib_filename="libboost_${lib_name}.a"
        find /usr/lib/ /usr/local/lib/ -name "$lib_filename" | grep -q "."
        static_lib_found=$?
        if [ $static_lib_found -ne 0 ]; then
            echo "$lib_filename not found."
        fi
    fi

    installed=$((pkg_found | static_lib_found))

    set -e

    if [ $installed -eq 0 ]; then
        echo "Found ${pkg_name}=${pkg_version}."
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

    if [ -e "$output_extraction_dir" ]; then
        return 0
    fi

    if [ ! -e "$output_archive_path" ]; then
        if ! curl \
                --fail \
                --show-error \
                --location "$source_url" \
                --output "$output_archive_path"; then
            return 1
        fi
    fi

    if ! cd "$(dirname "$output_extraction_dir")"; then
        return 1
    fi
    extraction_cmd=()
    if [[ "$output_archive_path" == *.tar.gz ]] ; then
        extraction_cmd+=("tar" "xf")
    elif [[ "$output_archive_path" == *.zip ]] ; then
        extraction_cmd+=("unzip")
    fi
    extraction_cmd+=("$output_archive_path")
    if ! "${extraction_cmd[@]}"; then
        return 1
    fi

    return 0
}

cUsage="Usage: ${BASH_SOURCE[0]} <version> <libraries> [ <.deb output directory>]"
if [ "$#" -lt 2 ] ; then
    echo "$cUsage"
    exit
fi
pkg_version=$1
libs_concatenated="$2"

libs_to_install=()
IFS="," read -r -a libs <<< "$libs_concatenated"
for lib_name in "${libs[@]}"; do
    pkg_name="libboost-${lib_name}-dev"
    if ! check_if_installed "$pkg_name" "$pkg_version" "$lib_name"; then
        libs_to_install+=("$lib_name")
    fi
done
if [ ${#libs_to_install[@]} -eq 0 ]; then
    # Nothing to do
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
mkdir --parents "$temp_dir"

version_with_underscores="${pkg_version//./_}"
tar_filename="boost_${version_with_underscores}.tar.gz"
source_url="https://boostorg.jfrog.io/artifactory/main/release/${pkg_version}/source/${tar_filename}"
output_tar_path="${temp_dir}/${tar_filename}"
output_extraction_dir="${temp_dir}/boost_${version_with_underscores}"
if ! download_and_extract_source "$source_url" "$output_tar_path" "$output_extraction_dir" ; then
    exit 1
fi

for lib_name in "${libs_to_install[@]}"; do
    pkg_name="libboost-${lib_name//_/-}-dev"

    if command -v dpkg-deb; then
        deb_pkg_name="${pkg_name}-${pkg_version}"

        deb_pkg_contents_dir="${temp_dir}/${deb_pkg_name}"
        install_prefix="${deb_pkg_contents_dir}/usr"
        mkdir --parents "$install_prefix"

        if ! build_boost_lib "$output_extraction_dir" "$lib_name" "$install_prefix" ; then
            exit 1
        fi

        if ! build_deb_pkg_and_install "$output_extraction_dir" "$pkg_name" "$pkg_version" \
                "$lib_name" "$deb_pkg_contents_dir" "$temp_dir" "${install_cmd_prefix[@]}"; then
            exit 1
        fi
    else
        if ! build_boost_lib "$output_extraction_dir" "$lib_name" "" ; then
            exit 1
        fi

        if ! install_lib "$output_extraction_dir" "${install_cmd_prefix[@]}" ""; then
            exit 1
        fi
    fi
done
