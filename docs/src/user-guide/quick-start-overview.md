# Overview

Follow the guides below to set up a cluster, compress your logs, and search your logs.

* [Cluster setup](quick-start-cluster-setup)
* [Compression](quick-start-compression)
* [Search](quick-start-search)

## Housekeeping

As you read the guides, keep these points in mind:

* When we refer to keys within a config file, we use `.` to indicate the hierarchy (nesting level)
  of a key.
  * E.g., the config key `webui.port` refers to the config key `port` that's nested in the config 
    `webui`.
* We refer to a CLP release as a package.
* Relative paths (paths which don't start with `/`) are relative to the root of the package unless
  context indicates otherwise.
