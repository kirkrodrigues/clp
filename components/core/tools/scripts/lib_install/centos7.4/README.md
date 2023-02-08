These instructions are used to install the dependencies to build CLP. The same 
steps are used by our Docker containers.

# Installing dependencies

Before you run any commands below, you should review the scripts to ensure they
will not install any dependencies you don't expect.

* Install all dependencies:

  ```bash
  install-all.sh
  ```

# Setup dependencies

* Enable gcc 8

  ```bash
  ln -s /opt/rh/devtoolset-8/enable /etc/profile.d/devtoolset.sh
  ```

* Set PKG_CONFIG_PATH since CentOS doesn't look in `/usr/local` by default.
  You should add this to your shell's profile/startup file (e.g., `.bashrc).

  ```bash
  export PKG_CONFIG_PATH=/usr/local/lib64/pkgconfig:/usr/local/lib/pkgconfig
  ```

* Enable git 2.27. You should add this to your shell's profile/startup file 
  (e.g., `.bashrc).

  ```bash
  source /opt/rh/rh-git227/enable
  ```

# Building CLP

* See the instructions in the [main README](../../../../README.md#build)
