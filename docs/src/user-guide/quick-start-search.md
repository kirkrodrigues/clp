# Search

You can search your logs from the UI or from the command line.

## Searching from the UI

* Open the UI and navigate to the search page by clicking the 🔍 Search button in the sidebar on the
  left.
* You can enter a query in the text box at the top of the search page.
    * The format of a query depends on the format of your logs—refer to the appropriate subsection
      below.
* To narrow your search to a specific time range or perform case-sensitive searches, click the
  hamburger icon to the left of the text box and use the appropriate controls.

:::{note}
By default, the UI will only return 1000 of the latest search results. To perform searches which
return more results, you can search from the command line.
::: 

## Searching from the command line

```
sbin/search.sh '<query>'
```

The format of `<query>` depends on the format your logs—refer to the appropriate subsection below.

To narrow your search to a specific time range:

* Add `--begin-time <epoch-timestamp-millis>` to filter for log events after a certain time.
    * `<epoch-timestamp-millis>` is the timestamp as milliseconds since the UNIX epoch.
* Add `--end-time <epoch-timestamp-millis>` to filter for log events after a certain time.

To perform case-insensitive searches, add the `--ignore-case` flag.

:::{caution}
To match the convention of other tools, by default, searches are case-**insensitive** in the UI and
searches are case-**sensitive** on the command line.
::: 

## Searching JSON logs

To search JSON logs, CLP's currently supports a variant of the [Kibana Query Language (KQL)][1]. For
instance, to search for error-level log events about jobs, you might enter the query:

```
level: "ERROR" AND message: "*job*"
```

## Searching text logs

To search text logs, CLP currently supports wildcard queries. A wildcard query is a query where:

* `*` matches zero or more characters
* `?` matches any single character

For instance, to search for log events containing the words “container” and “failed”`, in that
order, you could enter the query:

```
container * failed
```

:::{note}
By default, CLP treats queries as substring searches (alike `grep`). So the user query
`container * failed` is interpreted as `*container * failed*`.
:::

[1]: https://www.elastic.co/guide/en/kibana/current/kuery-query.html
