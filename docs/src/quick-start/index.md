# Quick start

To use CLP, follow the guides below to set up a cluster, compress your logs, and search your logs.

* [Cluster setup](cluster-setup.md)
* [Compression](compression.md)
* [Search](search.md)

## Housekeeping

As you read the guides, keep these points in mind:

* We refer to a CLP release as the CLP package.
* Most relative paths (paths which don’t start with `/`) are relative to the root of the package.
  * The root of any other relative paths will be clear from context.
* When we refer to configuration keys within a config file, we use `.` to indicate the hierarchy
  (nesting level) of a key.
  * E.g., the config key `webui.port` refers to the config key `port` that’s nested in the config 
    `webui`.

```{toctree}
:hidden:

cluster-setup.md
compression.md
search.md
```
