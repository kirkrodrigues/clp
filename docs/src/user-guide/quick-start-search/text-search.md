# Text log search syntax

To search text logs, CLP currently supports wildcard queries. A wildcard query is a query where:

* `*` matches zero or more characters
* `?` matches any single character

For instance, to search for log events containing the words “container” and “failed”, in that
order, you could enter the query:

```
container * failed
```

:::{note}
By default, CLP treats queries as substring searches (alike `grep`). So the user query
`container * failed` is interpreted as `*container * failed*`.
:::
