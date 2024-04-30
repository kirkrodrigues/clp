# Compression

You can compress your logs using a script in the package. Depending on the format of your logs,
choose one of the options below.

:::{caution}
If you're using the `clp-json` release, you can only compress JSON logs. If you're using the
`clp-text` release, you should only compress text logs (you can compress JSON logs as text, but
searches will be haphazard). This limitation will be addressed in a future version of CLP.
:::

::::{grid} 1 1 2 2
:gutter: 2

:::{grid-item-card}
:link: clp-json
Compressing JSON logs
:::

:::{grid-item-card}
:link: clp-text
Compressing text logs
:::
::::

:::{toctree}
:hidden:
:caption: Compression

clp-json
clp-text
:::
