#!/usr/bin/env bash

# Exit on error
set -e

# Treat unset variables as errors
set -u

new_uid=${CONTAINER_UID:-1000}
new_gid=${CONTAINER_GID:-1000}
CONTAINER_USERNAME="dev"

user_updated=0

# Update GID if necessary
if [[ $(id --group $CONTAINER_USERNAME) -ne $new_gid ]]; then
    groupmod -g $new_gid $CONTAINER_USERNAME
    user_updated=1
fi

# Update UID if necessary
if [[ $(id --user $CONTAINER_USERNAME) -ne $new_uid ]]; then
    usermod -u $new_uid $CONTAINER_USERNAME
    user_updated=1
fi

if [[ $user_updated -eq 1 ]]; then
    # Update home directory ownership
    chown -R $CONTAINER_USERNAME:$CONTAINER_USERNAME /home/$CONTAINER_USERNAME
fi

exec "$@"
