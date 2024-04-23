FROM ghcr.io/y-scope/clp/clp-core-dependencies-x86-ubuntu-focal:main AS BASE

ARG CONTAINER_GID=1000
ARG CONTAINER_UID=1000
ARG CONTAINER_USER=clp-user

# NOTE: gcc and python3-dev are necessary for installing Python packages that need to build C
# extensions
RUN apt-get update \
    && apt-get install -y \
    gcc \
    python3-dev \
    python3-venv \
    rsync

# Install Task
RUN sh -c "$(curl --location https://taskfile.dev/install.sh)" -- -d -b /usr/local/bin

# Remove cached files
RUN apt-get clean \
    && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# Add user and group
RUN groupadd -g "${CONTAINER_GID}" "${CONTAINER_USER}"
RUN useradd \
    --uid "${CONTAINER_UID}" \
    --gid "${CONTAINER_GID}" \
    --no-log-init \
    --shell /bin/bash \
    --create-home \
     "${CONTAINER_USER}"

USER "${CONTAINER_USER}"

WORKDIR "/home/${CONTAINER_USER}"

# Install & setup nvm
RUN curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.7/install.sh | bash
ENV NVM_DIR="/home/${CONTAINER_USER}/.nvm"

# Install Node.js
RUN . "${NVM_DIR}/nvm.sh" \
    && nvm install 14

# Install meteor
# NOTE: We use `--ignore-meteor-setup-exec-path` to stop Meteor from inserting its PATH into
# `~/.bash_profile` causing the default Ubuntu shell setup to ignore `~/.bashrc`. This means we need
# to manually insert Meteor's path into `~/.bashrc`.
RUN . "${NVM_DIR}/nvm.sh" \
    && npm install -g meteor --ignore-meteor-setup-exec-path
RUN echo "export PATH=\${HOME}/.meteor:\${PATH}" > ~/new.bashrc
RUN cat ~/.bashrc >> ~/new.bashrc
RUN mv ~/new.bashrc ~/.bashrc

# Flatten the image
FROM scratch
COPY --from=BASE / /

# We need to repeat the arg since we flattened the image above
ARG CONTAINER_USER=clp-user

WORKDIR "/home/${CONTAINER_USER}"

CMD bash
