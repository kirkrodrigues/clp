#ifndef CLP_IR_LOGEVENTDESERIALIZER_HPP
#define CLP_IR_LOGEVENTDESERIALIZER_HPP

#include <type_traits>

#include <outcome/single-header/outcome.hpp>

#include "../ReaderInterface.hpp"
#include "../time_types.hpp"
#include "../TimestampPattern.hpp"
#include "../type_utils.hpp"
#include "LogEvent.hpp"
#include "types.hpp"

namespace clp::ir {
/**
 * Class for deserializing IR log events from an IR stream.
 *
 * TODO: We're currently returning std::errc error codes, but we should replace these with our own
 * custom error codes (derived from std::error_code), ideally replacing IRErrorCode.
 * @tparam encoded_variable_t Type of encoded variables in the stream
 */
template <typename encoded_variable_t>
class LogEventDeserializer {
public:
    // Factory functions
    /**
     * Creates a log event deserializer for the given stream
     * @param reader A reader for the IR stream
     * @return A result containing the serializer or an error code indicating the failure:
     * - std::errc::result_out_of_range if the IR stream is truncated
     * - std::errc::protocol_error if the IR stream is corrupted
     * - std::errc::protocol_not_supported if the IR stream contains an unsupported metadata format
     *   or uses an unsupported version
     */
    static auto create(ReaderInterface& reader
    ) -> OUTCOME_V2_NAMESPACE::std_result<LogEventDeserializer<encoded_variable_t>>;

    // Delete copy constructor and assignment operator
    LogEventDeserializer(LogEventDeserializer const&) = delete;
    auto operator=(LogEventDeserializer const&) -> LogEventDeserializer& = delete;

    // Define default move constructor
    LogEventDeserializer(LogEventDeserializer&&) = default;
    // Delete move assignment operator since it's not possible to have one when you have a member
    // that's a reference.
    auto operator=(LogEventDeserializer&&) -> LogEventDeserializer& = delete;

    ~LogEventDeserializer() = default;

    // Methods
    [[nodiscard]] auto get_timestamp_pattern() const -> TimestampPattern const& {
        return m_timestamp_pattern;
    }

    [[nodiscard]] auto get_current_utc_offset() const -> UtcOffset { return m_utc_offset; }

    /**
     * Deserializes a log event from the stream
     * @return A result containing the log event or an error code indicating the failure:
     * - std::errc::no_message_available on reaching the end of the IR stream
     * - std::errc::result_out_of_range if the IR stream is truncated
     * - std::errc::protocol_error if the IR stream is corrupted
     */
    [[nodiscard]] auto deserialize_log_event(
    ) -> OUTCOME_V2_NAMESPACE::std_result<LogEvent<encoded_variable_t>>;

private:
    // Constructors
    explicit LogEventDeserializer(ReaderInterface& reader) : m_reader{reader} {}

    LogEventDeserializer(ReaderInterface& reader, epoch_time_ms_t ref_timestamp)
            : m_reader{reader},
              m_prev_msg_timestamp{ref_timestamp} {}

    // Variables
    ReaderInterface& m_reader;
    TimestampPattern m_timestamp_pattern{0, "%Y-%m-%dT%H:%M:%S.%3"};
    UtcOffset m_utc_offset{0};
    [[no_unique_address]] std::conditional_t<
            std::is_same_v<encoded_variable_t, four_byte_encoded_variable_t>,
            epoch_time_ms_t,
            EmptyType> m_prev_msg_timestamp{};
};
}  // namespace clp::ir

#endif  // CLP_IR_LOGEVENTDESERIALIZER_HPP
