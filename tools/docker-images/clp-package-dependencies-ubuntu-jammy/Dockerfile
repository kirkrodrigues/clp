#FROM ghcr.io/y-scope/clp/clp-core-dependencies-x86-ubuntu-jammy:main AS base
FROM clp-core-dependencies-x86-ubuntu-jammy:dev AS base

ARG CONTAINER_USER_NAME=dev

ARG CONTAINER_USER_GID=1000
ARG CONTAINER_USER_UID=1000

WORKDIR /root

RUN mkdir -p ./tools/docker-images/clp-package-dependencies-ubuntu-jammy
ADD ./tools/docker-images/clp-package-dependencies-ubuntu-jammy/setup-scripts \
    ./tools/docker-images/clp-package-dependencies-ubuntu-jammy/setup-scripts

RUN ./tools/docker-images/clp-package-dependencies-ubuntu-jammy\
/setup-scripts/install-prebuilt-packages.sh

# Remove cached files
RUN apt-get clean \
    && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

RUN groupadd --gid "$CONTAINER_USER_GID" "$CONTAINER_USER_NAME" \
    && useradd \
    --uid "$CONTAINER_USER_UID" \
    --gid "$CONTAINER_USER_GID" \
    --create-home \
    --shell /bin/bash \
    "$CONTAINER_USER_NAME"

COPY ./tools/docker-images/clp-package-dependencies-ubuntu-jammy/files /
RUN chmod a+rx /entrypoint.sh

# Flatten the image
FROM scratch
COPY --from=base / /

ENTRYPOINT ["/entrypoint.sh"]
