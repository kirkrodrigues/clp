# Cluster setup

To setup a CLP cluster, you’ll need to:

* Download a release.
* Choose between a single or multi-node deployment.
* Ensure you meet the requirements for running the release.
* Configure the release if necessary.
* Start CLP.

## Downloading a release

Download the flavour of release that’s appropriate for your logs:

* **clp-json** for compressing and searching **JSON** logs.
* **clp-text** for compressing and searching **free-text** logs.

:::{note}
Both flavours contain the same binaries but are configured with different values for the
`package.storage_engine` key.
:::

Once downloaded, extract the release into a directory.

## Single-node deployment

A single-node deployment allows you to run CLP on a single host.

### Requirements

* [Docker][1]
  * If you’re not running as root, ensure `docker` can be run [without superuser privileges][2].
* Python 3.8 or higher

### Starting CLP

```bash
sbin/start-clp.sh
```

:::{note}
If CLP fails to start (e.g., due to a port conflict), try adjusting the config in
`etc/clp-config.yml` and then running the start command again.
:::

### Stopping CLP

If you need to stop the cluster, run:

```bash
sbin/stop-clp.sh
```

## Multi-node Deployment

A multi-node deployment allows you to run CLP across a distributed set of hosts.

### Requirements

* [Docker][1]
  * If you’re not running as root, ensure docker can be run [without superuser privileges][2].
* Python 3.8 or higher
* One or more hosts networked together
* A distributed filesystem (e.g. [SeaweedFS][3]) accessible by all worker hosts through a filesystem
  mount
  * See [below](#setting-up-seaweedfs) for how to set up a simple SeaweedFS cluster.

### Cluster overview

The CLP package is composed of several components—controller components and worker components. In a
cluster, there should be a single instance of each controller component and one or more instances of
worker components. The tables below list the components and their functions.

**Controller components**

| Component             | Description                                                      |
|-----------------------|------------------------------------------------------------------|
| compression_scheduler | Scheduler for compression jobs                                   |
| search_scheduler      | Scheduler for search jobs                                        |
| webui                 | Web server for web UI                                            |
| database              | Database for archive metadata, compression jobs, and search jobs |
| queue                 | Task queue for schedulers                                        |
| redis                 | Task result storage for workers                                  |
| results_cache         | Storage for the workers to return search results to the UI       |

**Worker components**

| Component          | Description                            |
|--------------------|----------------------------------------|
| compression_worker | Worker processes for compression tasks |
| search_worker      | Worker processes for search task       |

Running additional workers increases the parallelism of compression and search jobs.

### Configuring CLP

1. Copy `etc/credentials.template.yml` to `etc/credentials.yml`.
2. Edit `etc/credentials.yml`:

    {style=lower-alpha}
    1. Uncomment the file.
    2. Set the username and password to something appropriate.
       * Note that these are *new* credentials that will be used by the components.
3. Choose which nodes you would like to use for the controller components.
   * You can use a single node for all controller components.
4. Edit `etc/clp-config.yml`:

    {style=lower-alpha}
    1. Uncomment the file.
    2. Set the host of each controller component to the nodes you choose in step 3.
    3. Change any of the controller components' ports that will conflict with services you already
       have running.
    4. Set `archive_output.directory` to a directory on the distributed filesystem.
5. Download and extract the package on all nodes.
6. Take the versions of `credentials.yml` and `clp-config.yml` that you created above and copy them
   into `etc/` on all the nodes where you extracted the package.

### Starting CLP

For each component, on the host where you want to run the component, run:

```
sbin/start-clp.sh <component>
```

### Stopping CLP

If you need to stop the cluster, run:

```bash
sbin/stop-clp.sh
```

(setting-up-seaweedfs)=
### Setting up SeaweedFS

The instructions below are for running a simple SeaweedFS cluster on a set of hosts. For other use
cases, see the [SeaweedFS docs][4].

1. Install [SeaweedFS][5].
2. Start the master and a filer on one of the nodes:

    ```bash
    weed master -port 9333
    weed filer -port 8888 -master "localhost:9333"
    ```
    
3. Start one or more volume servers on one or more nodes.
   1. Create a directory where you want SeaweedFS to store data.
   2. Start the volume server:
        
      ```bash
      weed volume -mserver "<master-node>:9333" -dir <storage-dir> -max 0
      ```
        
      * `<master-node>` is the hostname/IP of the master node.
      * `<storage-dir>` is the directory where you want SeaweedFS to store data.
4. Start a FUSE mount on every node that you want to run a CLP worker:

   ```
   weed mount -filer "<master-node>:8888" -dir <mount-path>
   ```

   * `<master-node>` is the hostname/IP of the master node.
   * `<mount-path>` is the path where you want the mount to be.

## Opening the UI

CLP includes a web interface available at http://localhost:4000 by default (if you changed
`webui.host` or `webui.port` in `etc/clp-config.yml`, use the new values).

[1]: https://docs.docker.com/engine/install/
[2]: https://docs.docker.com/engine/install/linux-postinstall/#manage-docker-as-a-non-root-user
[3]: https://github.com/seaweedfs/seaweedfs
[4]: https://github.com/seaweedfs/seaweedfs/blob/master/README.md
[5]: https://github.com/seaweedfs/seaweedfs?tab=readme-ov-file#quick-start-with-single-binary
