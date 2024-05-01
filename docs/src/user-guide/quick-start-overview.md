# Overview

CLP operates as a distributed system, so you'll need to set up a cluster before compressing and
searching your logs. Follow the guides below for each step:

1. [Cluster setup](quick-start-cluster-setup/index)
2. [Compression](quick-start-compression/index)
3. [Search](quick-start-search/index)

## Housekeeping

As you read the guides, keep these points in mind:

* When we refer to keys within a config file, we use `.` to indicate the hierarchy (nesting level)
  of a key.
  * E.g., the config key `webui.port` refers to the config key `port` that's nested in the config 
    `webui`.
* We refer to a CLP release as a package.
* Relative paths (paths which don't start with `/`) are relative to the root of the package unless
  context indicates otherwise.
