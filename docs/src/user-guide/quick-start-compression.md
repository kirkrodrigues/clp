# Compression

You can compress your logs using a script in the package. Depending on the format of your logs,
refer to the appropriate subsection below.

:::{caution}
If you're using a CLP release for JSON logs, you can only compress JSON logs. Similarly, if you're
using a CLP release for text logs, you should only compress text logs. This limitation will be
addressed in a future version of CLP.
:::

## Compressing JSON logs

To compress JSON logs, from inside the package directory, run:

```bash
sbin/compress.sh --timestamp-key '<timestamp-key>' <path1> [<path2> ...]
```

* `<timestamp-key>` is the field path of the kv-pair containing each log event's timestamp.
    * E.g., if your log events look like
      `{"timestamp": {"iso8601": "2024-01-01 00:01:02.345", ...}}`, you should enter
      `timestamp.iso8601` as the timestamp key.
* `<path...>` are paths to JSON log files or directories containing JSON log files.
    * Each JSON log file should contain newline-delimited JSON (ndjson) log events (i.e., each line
      should contain a single JSON log event).

## Compressing text logs

To compress text logs, from inside the package directory, run:

```bash
sbin/compress.sh <path1> [<path2> ...]
```

`<path...>` are paths to text log files or directories containing text log files.

## Examining compression statistics

The compression script used above will output the compression ratio of each dataset you compress, or
you can use CLP's web interface to view overall statistics.

## Sample logs

For some sample logs, check out the open-source [datasets](resources-datasets.md).
