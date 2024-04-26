# JSON log search syntax

To search JSON logs, CLP's currently supports a variant of the [Kibana Query Language (KQL)][1]. For
instance, to search for error-level log events about jobs, you might enter the query:

```
level: "ERROR" AND message: "*job*"
```

[1]: https://www.elastic.co/guide/en/kibana/current/kuery-query.html
