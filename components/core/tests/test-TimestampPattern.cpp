#include <Catch2/single_include/catch2/catch.hpp>

#include "../src/clp/time_types.hpp"
#include "../src/clp/TimestampPattern.hpp"

using clp::epochtime_t;
using clp::TimestampPattern;
using clp::UtcOffset;
using std::string;

namespace {
/**
 * Parses a timestamp with the given pattern from the given line and validates the result.
 * @param line Line to parse timestamp from
 * @param pattern Pattern of timestamp to parse
 * @param expected_timestamp
 * @param expected_utc_offset
 * @param expected_timestamp_begin_pos
 * @param expected_timestamp_end_pos
 * @param expected_restored_line Expected line after restoring timestamp
 */
void parse_and_validate_timestamp_pattern(
        string const& line,
        TimestampPattern const& pattern,
        epochtime_t expected_timestamp,
        UtcOffset expected_utc_offset,
        size_t expected_timestamp_begin_pos,
        size_t expected_timestamp_end_pos,
        string const& expected_restored_line
);

/**
 * Validates that the given line is parsed with the expected timestamp pattern.
 * @param line
 * @param expected_timestamp_pattern
 * @param expected_timestamp
 * @param expected_utc_offset
 * @param expected_timestamp_begin_pos
 * @param expected_timestamp_end_pos
 */
void search_and_validate_timestamp_pattern(
        string const& line,
        TimestampPattern const& expected_timestamp_pattern,
        epochtime_t expected_timestamp,
        UtcOffset expected_utc_offset,
        size_t expected_timestamp_begin_pos,
        size_t expected_timestamp_end_pos
);

/**
 * Validates formatting and inserting a timestamp into a line.
 * @param line Original line
 * @param timestamp_begin_pos
 * @param timestamp_end_pos
 * @param timestamp
 * @param utc_offset
 * @param pattern
 * @param expected_restored_line
 */
void validate_inserting_formatted_timestamp(
        string const& line,
        size_t timestamp_begin_pos,
        size_t timestamp_end_pos,
        epochtime_t timestamp,
        UtcOffset utc_offset,
        TimestampPattern const& pattern,
        string const& expected_restored_line
);

/**
 * Validates the result of parsing a timestamp.
 * @param expected_timestamp
 * @param expected_utc_offset
 * @param expected_timestamp_begin_pos
 * @param expected_timestamp_end_pos
 * @param timestamp
 * @param utc_offset
 * @param timestamp_begin_pos
 * @param timestamp_end_pos
 */
void validate_timestamp_parsing_result(
        epochtime_t expected_timestamp,
        UtcOffset expected_utc_offset,
        size_t expected_timestamp_begin_pos,
        size_t expected_timestamp_end_pos,
        epochtime_t timestamp,
        UtcOffset utc_offset,
        size_t timestamp_begin_pos,
        size_t timestamp_end_pos
);

void parse_and_validate_timestamp_pattern(
        string const& line,
        TimestampPattern const& pattern,
        epochtime_t expected_timestamp,
        UtcOffset expected_utc_offset,
        size_t expected_timestamp_begin_pos,
        size_t expected_timestamp_end_pos,
        string const& expected_restored_line
) {
    epochtime_t timestamp{0};
    UtcOffset utc_offset{0};
    size_t timestamp_begin_pos{0};
    size_t timestamp_end_pos{0};
    pattern.parse_timestamp(line, timestamp, utc_offset, timestamp_begin_pos, timestamp_end_pos);

    validate_timestamp_parsing_result(
            expected_timestamp,
            expected_utc_offset,
            expected_timestamp_begin_pos,
            expected_timestamp_end_pos,
            timestamp,
            utc_offset,
            timestamp_begin_pos,
            timestamp_end_pos
    );
    validate_inserting_formatted_timestamp(
            line,
            timestamp_begin_pos,
            timestamp_end_pos,
            timestamp,
            utc_offset,
            pattern,
            expected_restored_line
    );
}

void search_and_validate_timestamp_pattern(
        string const& line,
        TimestampPattern const& expected_timestamp_pattern,
        epochtime_t expected_timestamp,
        UtcOffset expected_utc_offset,
        size_t expected_timestamp_begin_pos,
        size_t expected_timestamp_end_pos
) {
    epochtime_t timestamp{0};
    UtcOffset utc_offset{0};
    size_t timestamp_begin_pos{0};
    size_t timestamp_end_pos{0};
    auto* pattern = TimestampPattern::search_known_ts_patterns(
            line,
            timestamp,
            utc_offset,
            timestamp_begin_pos,
            timestamp_end_pos
    );
    REQUIRE(nullptr != pattern);
    REQUIRE(pattern->get_num_spaces_before_ts()
            == expected_timestamp_pattern.get_num_spaces_before_ts());
    REQUIRE(pattern->get_format() == expected_timestamp_pattern.get_format());

    validate_timestamp_parsing_result(
            expected_timestamp,
            expected_utc_offset,
            expected_timestamp_begin_pos,
            expected_timestamp_end_pos,
            timestamp,
            utc_offset,
            timestamp_begin_pos,
            timestamp_end_pos
    );
    validate_inserting_formatted_timestamp(
            line,
            timestamp_begin_pos,
            timestamp_end_pos,
            timestamp,
            utc_offset,
            *pattern,
            line
    );
}

void validate_inserting_formatted_timestamp(
        string const& line,
        size_t timestamp_begin_pos,
        size_t timestamp_end_pos,
        epochtime_t timestamp,
        UtcOffset utc_offset,
        TimestampPattern const& pattern,
        string const& expected_restored_line
) {
    // Generate the line without the timestamp
    string restored_line;
    restored_line.assign(line, 0, timestamp_begin_pos);
    restored_line.append(line, timestamp_end_pos, line.length() - timestamp_end_pos);

    pattern.insert_formatted_timestamp(timestamp, utc_offset, restored_line);

    REQUIRE(expected_restored_line == restored_line);
}

void validate_timestamp_parsing_result(
        epochtime_t expected_timestamp,
        UtcOffset expected_utc_offset,
        size_t expected_timestamp_begin_pos,
        size_t expected_timestamp_end_pos,
        epochtime_t timestamp,
        UtcOffset utc_offset,
        size_t timestamp_begin_pos,
        size_t timestamp_end_pos
) {
    REQUIRE(expected_timestamp == timestamp);
    REQUIRE(expected_utc_offset == utc_offset);
    REQUIRE(expected_timestamp_begin_pos == timestamp_begin_pos);
    REQUIRE(expected_timestamp_end_pos == timestamp_end_pos);
}
}  // namespace

TEST_CASE("Test known timestamp patterns", "[KnownTimestampPatterns]") {
    TimestampPattern::init();

    search_and_validate_timestamp_pattern(
            "2015-02-01T01:02:03.004 content after",
            {0, "%Y-%m-%dT%H:%M:%S.%3"},
            1'422'752'523'004,
            UtcOffset{0},
            0,
            23
    );

    search_and_validate_timestamp_pattern(
            "2015-02-01T01:02:03,004 content after",
            {0, "%Y-%m-%dT%H:%M:%S,%3"},
            1'422'752'523'004,
            UtcOffset{0},
            0,
            23
    );

    search_and_validate_timestamp_pattern(
            "2015-02-01 01:02:03.004 content after",
            {0, "%Y-%m-%d %H:%M:%S.%3"},
            1'422'752'523'004,
            UtcOffset{0},
            0,
            23
    );

    search_and_validate_timestamp_pattern(
            "2015-02-01 01:02:03,004 content after",
            {0, "%Y-%m-%d %H:%M:%S,%3"},
            1'422'752'523'004,
            UtcOffset{0},
            0,
            23
    );

    search_and_validate_timestamp_pattern(
            "2015/01/31T15:50:45.123 content after",
            {0, "%Y/%m/%dT%H:%M:%S.%3"},
            1'422'719'445'123,
            UtcOffset{0},
            0,
            23
    );

    search_and_validate_timestamp_pattern(
            "2015/01/31T15:50:45,123 content after",
            {0, "%Y/%m/%dT%H:%M:%S,%3"},
            1'422'719'445'123,
            UtcOffset{0},
            0,
            23
    );

    search_and_validate_timestamp_pattern(
            "2015/01/31 15:50:45.123 content after",
            {0, "%Y/%m/%d %H:%M:%S.%3"},
            1'422'719'445'123,
            UtcOffset{0},
            0,
            23
    );

    search_and_validate_timestamp_pattern(
            "2015/01/31 15:50:45,123 content after",
            {0, "%Y/%m/%d %H:%M:%S,%3"},
            1'422'719'445'123,
            UtcOffset{0},
            0,
            23
    );

    search_and_validate_timestamp_pattern(
            "[2015-02-01 01:02:03,004] content after",
            {0, "[%Y-%m-%d %H:%M:%S,%3]"},
            1'422'752'523'004,
            UtcOffset{0},
            0,
            25
    );

    search_and_validate_timestamp_pattern(
            "INFO [main] 2015-02-01 01:02:03,004 content after",
            {2, "%Y-%m-%d %H:%M:%S,%3"},
            1'422'752'523'004,
            UtcOffset{0},
            12,
            35
    );

    search_and_validate_timestamp_pattern(
            "<<<2015-02-01 01:02:03:004 content after",
            {0, "<<<%Y-%m-%d %H:%M:%S:%3"},
            1'422'752'523'004,
            UtcOffset{0},
            0,
            26
    );

    search_and_validate_timestamp_pattern(
            "01 Feb 2015 01:02:03,004 content after",
            {0, "%d %b %Y %H:%M:%S,%3"},
            1'422'752'523'004,
            UtcOffset{0},
            0,
            24
    );

    search_and_validate_timestamp_pattern(
            "2015-01-31T15:50:45 content after",
            {0, "%Y-%m-%dT%H:%M:%S"},
            1'422'719'445'000,
            UtcOffset{0},
            0,
            19
    );

    search_and_validate_timestamp_pattern(
            "2015-02-01 01:02:03 content after",
            {0, "%Y-%m-%d %H:%M:%S"},
            1'422'752'523'000,
            UtcOffset{0},
            0,
            19
    );

    search_and_validate_timestamp_pattern(
            "2015/01/31T15:50:45 content after",
            {0, "%Y/%m/%dT%H:%M:%S"},
            1'422'719'445'000,
            UtcOffset{0},
            0,
            19
    );

    search_and_validate_timestamp_pattern(
            "2015/02/01 01:02:03 content after",
            {0, "%Y/%m/%d %H:%M:%S"},
            1'422'752'523'000,
            UtcOffset{0},
            0,
            19
    );

    search_and_validate_timestamp_pattern(
            "[2015-02-01T01:02:03 content after",
            {0, "[%Y-%m-%dT%H:%M:%S"},
            1'422'752'523'000,
            UtcOffset{0},
            0,
            20
    );

    search_and_validate_timestamp_pattern(
            "[20150201-01:02:03] content after",
            {0, "[%Y%m%d-%H:%M:%S]"},
            1'422'752'523'000,
            UtcOffset{0},
            0,
            19
    );

    search_and_validate_timestamp_pattern(
            "15/02/01 01:02:03 content after",
            {0, "%y/%m/%d %H:%M:%S"},
            1'422'752'523'000,
            UtcOffset{0},
            0,
            17
    );

    search_and_validate_timestamp_pattern(
            "150201  1:02:03 content after",
            {0, "%y%m%d %k:%M:%S"},
            1'422'752'523'000,
            UtcOffset{0},
            0,
            15
    );

    search_and_validate_timestamp_pattern(
            "Feb 01, 2015  1:02:03 AM content after",
            {0, "%b %d, %Y %l:%M:%S %p"},
            1'422'752'523'000,
            UtcOffset{0},
            0,
            24
    );

    search_and_validate_timestamp_pattern(
            "February 01, 2015 01:02 content after",
            {0, "%B %d, %Y %H:%M"},
            1'422'752'520'000,
            UtcOffset{0},
            0,
            23
    );

    search_and_validate_timestamp_pattern(
            "E [01/Feb/2015:01:02:03 content after",
            {1, "[%d/%b/%Y:%H:%M:%S"},
            1'422'752'523'000,
            UtcOffset{0},
            2,
            23
    );

    search_and_validate_timestamp_pattern(
            "localhost - - [01/Feb/2015:01:02:03 content after",
            {3, "[%d/%b/%Y:%H:%M:%S"},
            1'422'752'523'000,
            UtcOffset{0},
            14,
            35
    );

    search_and_validate_timestamp_pattern(
            "localhost - - [01/02/2015:01:02:03 content after",
            {3, "[%d/%m/%Y:%H:%M:%S"},
            1'422'752'523'000,
            UtcOffset{0},
            14,
            34
    );

    search_and_validate_timestamp_pattern(
            "Started POST \"/api/v3/internal/allowed\" for 127.0.0.1 at 2015-02-01 01:02:03 "
            "content after",
            {6, "%Y-%m-%d %H:%M:%S"},
            1'422'752'523'000,
            UtcOffset{0},
            57,
            76
    );

    search_and_validate_timestamp_pattern(
            "update-alternatives 2015-02-01 01:02:03 content after",
            {1, "%Y-%m-%d %H:%M:%S"},
            1'422'752'523'000,
            UtcOffset{0},
            20,
            39
    );

    search_and_validate_timestamp_pattern(
            "ERROR: apport (pid 4557) Sun Feb  1 01:02:03 2015 content after",
            {4, "%a %b %e %H:%M:%S %Y"},
            1'422'752'523'000,
            UtcOffset{0},
            25,
            49
    );

    search_and_validate_timestamp_pattern(
            "Sun Feb  1 01:02:03 2015 content after",
            {0, "%a %b %e %H:%M:%S %Y"},
            1'422'752'523'000,
            UtcOffset{0},
            0,
            24
    );

    search_and_validate_timestamp_pattern(
            "Jan 21 11:56:42",
            {0, "%b %d %H:%M:%S"},
            1'771'002'000,
            UtcOffset{0},
            0,
            15
    );

    search_and_validate_timestamp_pattern(
            "01-21 11:56:42.392",
            {0, "%m-%d %H:%M:%S.%3"},
            1'771'002'392,
            UtcOffset{0},
            0,
            18
    );

    search_and_validate_timestamp_pattern(
            "626515123 content after",
            {0, "%#3"},
            626'515'123,
            UtcOffset{0},
            0,
            9
    );

    // Inputs for the patterns below get recognized as other timestamp patterns, so we can only test
    // the patterns by specifying them manually.
    // NOTE: Since CLP currently stores timestamps with millisecond resolution, microsecond and
    // nanosecond-precision timestamps get truncated.
    parse_and_validate_timestamp_pattern(
            "626515123 content after",
            {0, "%#6"},
            626'515,
            UtcOffset{0},
            0,
            9,
            "626515000 content after"
    );

    parse_and_validate_timestamp_pattern(
            "626515123 content after",
            {0, "%#9"},
            626,
            UtcOffset{0},
            0,
            9,
            "626000000 content after"
    );
}
