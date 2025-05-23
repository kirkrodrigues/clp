# Auto-generated KV pairs

Structured log events typically contain two categories of KV pairs---those generated by the user
(a developer) and those generated by the logging library. To illustrate, consider the structured
[Zap] log printing statement (LPS) in [Figure 1](#figure-1) and its output in [Figure 2](#figure-2).
Comparing the user-defined KV pairs in [Figure 1](#figure-1) with KV pairs in [Figure 2](#figure-2),
we can see that the logging library adds the `timestamp` and `level` KV pairs automatically. We
define the two categories of KV pairs as follows:

* *Auto-generated* KV pairs: those inserted automatically by a logging library.
* *User-generated* KV pairs: those inserted explicitly by the user.

For instance, in [Figure 2](#figure-2), we categorize the `timestamp` and `level` KV pairs as
auto-generated. We categorize `level` as such since even though the user's LPS choice determines the
value of the level, the user doesn't explicitly insert it into the log event.

(figure-1)=
:::{card}

```go
logger.Info("Task completed successfully.",
  zap.String("task", task_id),
  zap.Int("pending_tasks", 2),
)
```

+++
**Figure 1**: An example structured LPS.
:::

(figure-2)=
:::{card}

```json
{
  "ts": 1708161000.123456,
  "level": "info",
  "message": "Task completed successfully.",
  "task": "task_1",
  "pending_tasks": 2
}
```

+++
**Figure 2**: The JSON output of the LPS in [Figure 1](#figure-1).
:::

One of the key differences between KV-IR streams and [JSON lines][json-lines] (JSONL) files is that
KV-IR streams support storing auto-generated KV pairs separately from user-generated KV pairs. Since
the JSONL format has no such support, the two categories are stored in the same key namespace, which
introduces the following constraints:

* Users must avoid selecting keys that might conflict with those used by the logging library. This
  requires the user to keep track of all library-reserved keys, and can be exacerbated if the user
  needs to use multiple logging libraries across multiple languages. Some libraries like [Zap] allow
  users to configure the keys that are used for the library-generated KV pairs, but this still
  requires the user to avoid those keys.
* Logging libraries must avoid using keys that are likely to conflict with user-defined keys, *and*
  implement a policy to resolve any conflicts that do occur. Libraries could store their generated
  KV pairs under a special key, but this only minimizes the problem rather than solving it entirely.

As we'll see below, storing auto-generated and user-generated KV pairs separately requires a more
advanced query syntax, but also has additional benefits.

## Querying auto-generated KV pairs

If a log event has both an auto-generated KV pair and user-generated KV pair with the same key,
e.g., `timestamp`, we need a mechanism to let the user query one or the other unambiguously.
Naively, we could use two different queries, one to query each namespace, but this wouldn't allow
users to join filters from both namespaces, and it would double the query evaluation workload.
Instead, we use a special query syntax to indicate which namespace to query. Specifically, we use
the key-prefix `@` to denote a filter on an auto-generated KV pair---e.g., `@timestamp: 0` denotes a
filter for an auto-generated KV pair with the key `timestamp` and value `0`. Accordingly, filters on
user-generated KV pairs whose keys start with the character `@` will need to escape the `@` symbol.

## Additional benefits

One benefit of supporting auto-generated KV pairs is that our CLP logging libraries/plugins can use
a consistent set of keys for these KV pairs, making it easier for users to filter for them. For
instance, if a user is logging in both Java and Python, and they use CLP logging plugins to generate
KV-IR streams, the generated streams can both use the same key for log levels---e.g., `level`. In
turn, this would allow users to query log levels across both their Java and Python logs using the
query syntax `@level: <value>`. In addition, it would allow users to use the same key to filter
their logs by level when viewing them in the [log viewer][log-viewer].

Another benefit of supporting auto-generated KV pairs is that we can potentially leverage
the query syntax for more advanced queries on unstructured text KV pairs. For instance, we could
support a syntax like `@message` to query the KV pair `message`, and the syntax `@message.#vars[0]`
to query the first variable parsed from the KV pair. We'll explore this further once the feature has
been implemented.

:::{note}
The reason we categorize unstructured text KV pairs as auto-generated is because we see them as a
set of key-less auto-generated and user-generated values, formatted into a single string.
:::

[json-lines]: https://jsonlines.org/
[log-viewer]: https://github.com/y-scope/yscope-log-viewer
[Zap]: https://github.com/uber-go/zap
