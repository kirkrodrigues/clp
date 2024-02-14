FROM ghcr.io/y-scope/clp/clp-core-dependencies-x86-ubuntu-focal:main

ARG CONTAINER_GID=1000
ARG CONTAINER_UID=1000
ARG CONTAINER_USER=clp-user

RUN apt-get update \
    && apt-get install -y \
    python3-venv

# Install Task
RUN sh -c "$(curl --location https://taskfile.dev/install.sh)" -- -d -b /usr/local/bin

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
RUN . "${NVM_DIR}/nvm.sh" \
    && nvm install 14

# Install meteor
# NOTE: We use `--ignore-meteor-setup-exec-path` to stop Meteor from inserting its PATH into
# `~/.bash_profile` causing the default Ubuntu shell setup to ignore `~/.bashrc`. This means we need
# to manually insert Meteor's path into `~/.bashrc`.
RUN . "${NVM_DIR}/nvm.sh" \
    && npm install -g meteor --ignore-meteor-setup-exec-path
RUN echo "export PATH=/home/\${USER}/.meteor:\${PATH}" > ~/new.bashrc
RUN cat ~/.bashrc >> ~/new.bashrc
RUN mv ~/new.bashrc ~/.bashrc
