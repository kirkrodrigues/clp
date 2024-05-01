# Cluster setup

To set up a cluster, you'll need to:

* Download a release.
* Choose between a single or multi-node deployment.
* Ensure you meet the requirements for running the release.
* Configure the release (if necessary).
* Start CLP.

## Downloading a release

Download the flavour of [release][clp-releases] that's appropriate for your logs:

* **clp-json** for compressing and searching **JSON** logs.
* **clp-text** for compressing and searching **free-text** logs.

:::{note}
Both flavours contain the same binaries but are configured with different values for the
`package.storage_engine` key.
:::

Once downloaded, extract the release into a directory.

## Deployment options

Choose one of the deployment options below:

::::{grid} 1 1 2 2
:gutter: 2

:::{grid-item-card}
:link: single-node
Single-node deployment
:::

:::{grid-item-card}
:link: multi-node
Multi-node deployment
:::
::::

## Opening the UI

CLP includes a web interface available at [http://localhost:4000](http://localhost:4000) by default
(if you changed `webui.host` or `webui.port` in `etc/clp-config.yml`, use the new values).

:::{toctree}
:hidden:
:caption: Cluster setup

single-node
multi-node
:::

[clp-releases]: https://github.com/y-scope/clp/releases
