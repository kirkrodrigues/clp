#ifndef CLP_FFI_IR_STREAM_DECODING_METHODS_HPP
#define CLP_FFI_IR_STREAM_DECODING_METHODS_HPP

#include <string>
#include <vector>

#include "../../ir/types.hpp"
#include "../../ReaderInterface.hpp"
#include "../../time_types.hpp"
#include "../encoding_methods.hpp"

namespace clp::ffi::ir_stream {
using encoded_tag_t = int8_t;

typedef enum {
    IRErrorCode_Success,
    IRErrorCode_Decode_Error,
    IRErrorCode_Eof,
    IRErrorCode_Corrupted_IR,
    IRErrorCode_Incomplete_IR,
} IRErrorCode;

typedef enum {
    IRProtocolErrorCode_Supported,
    IRProtocolErrorCode_Too_Old,
    IRProtocolErrorCode_Too_New,
    IRProtocolErrorCode_Invalid,
} IRProtocolErrorCode;

class DecodingException : public TraceableException {
public:
    // Constructors
    DecodingException(
            ErrorCode error_code,
            char const* const filename,
            int line_number,
            std::string message
    )
            : TraceableException(error_code, filename, line_number),
              m_message(std::move(message)) {}

    // Methods
    [[nodiscard]] char const* what() const noexcept override { return m_message.c_str(); }

private:
    std::string m_message;
};

/**
 * Deserializes the IR stream's encoding type
 * @param reader
 * @param is_four_bytes_encoding Returns the encoding type
 * @return ErrorCode_Success on success
 * @return ErrorCode_Corrupted_IR if reader contains invalid IR
 * @return ErrorCode_Incomplete_IR if reader doesn't contain enough data to decode
 */
IRErrorCode get_encoding_type(ReaderInterface& reader, bool& is_four_bytes_encoding);

/**
 * Deserializes the tag for the next packet.
 * @param reader
 * @param tag Returns the tag of the next packet.
 * @return IRErrorCode_Success on success
 * @return IRErrorCode_Incomplete_IR if reader doesn't contain enough data to deserialize
 */
[[nodiscard]] IRErrorCode deserialize_tag(ReaderInterface& reader, encoded_tag_t& tag);

/**
 * Deserializes a log event from the given stream
 * @tparam encoded_variable_t
 * @param reader
 * @param encoded_tag Tag of the next packet to read
 * @param logtype Returns the logtype
 * @param encoded_vars Returns the encoded variables
 * @param dict_vars Returns the dictionary variables
 * @param timestamp_or_timestamp_delta Returns the timestamp (in the eight-byte encoding case) or
 * the timestamp delta (in the four-byte encoding case)
 * @return IRErrorCode_Success on success
 * @return IRErrorCode_Corrupted_IR if reader contains invalid IR
 * @return IRErrorCode_Incomplete_IR if reader doesn't contain enough data
 */
template <typename encoded_variable_t>
auto deserialize_log_event(
        ReaderInterface& reader,
        encoded_tag_t encoded_tag,
        std::string& logtype,
        std::vector<encoded_variable_t>& encoded_vars,
        std::vector<std::string>& dict_vars,
        ir::epoch_time_ms_t& timestamp_or_timestamp_delta
) -> IRErrorCode;

/**
 * Decodes the IR message calls the given methods to handle each component of the message
 * @tparam unescape_logtype Whether to remove the escape characters from the logtype before calling
 * \p ConstantHandler
 * @tparam encoded_variable_t Type of the encoded variable
 * @tparam ConstantHandler Method to handle constants in the logtype.
 * Signature: (const std::string&, size_t, size_t) -> void
 * @tparam EncodedIntHandler Method to handle encoded integers.
 * Signature: (encoded_variable_t) -> void
 * @tparam EncodedFloatHandler Method to handle encoded floats.
 * Signature: (encoded_variable_t) -> void
 * @tparam DictVarHandler Method to handle dictionary variables.
 * Signature: (const std::string&) -> void
 * @param logtype
 * @param encoded_vars
 * @param dict_vars
 * @param constant_handler
 * @param encoded_int_handler
 * @param encoded_float_handler
 * @param dict_var_handler
 * @throw DecodingException if the message can not be decoded properly
 */
template <
        bool unescape_logtype,
        typename encoded_variable_t,
        typename ConstantHandler,
        typename EncodedIntHandler,
        typename EncodedFloatHandler,
        typename DictVarHandler>
void generic_decode_message(
        std::string const& logtype,
        std::vector<encoded_variable_t> const& encoded_vars,
        std::vector<std::string> const& dict_vars,
        ConstantHandler constant_handler,
        EncodedIntHandler encoded_int_handler,
        EncodedFloatHandler encoded_float_handler,
        DictVarHandler dict_var_handler
);

/**
 * Deserializes the preamble for an IR stream.
 * @param reader
 * @param metadata_type Returns the type of the metadata deserialized from the IR
 * @param metadata_pos Returns the starting position of the metadata in reader
 * @param metadata_size Returns the size of the metadata deserialized from the IR
 * @return IRErrorCode_Success on success
 * @return IRErrorCode_Corrupted_IR if reader contains invalid IR
 * @return IRErrorCode_Incomplete_IR if reader doesn't contain enough data to deserialize
 */
IRErrorCode deserialize_preamble(
        ReaderInterface& reader,
        encoded_tag_t& metadata_type,
        size_t& metadata_pos,
        uint16_t& metadata_size
);

/**
 * Deserializes the preamble for an IR stream.
 * @param reader
 * @param metadata_type Returns the type of the metadata deserialized from the IR
 * @param metadata Returns the metadata in the given vector
 * @return IRErrorCode_Success on success
 * @return IRErrorCode_Corrupted_IR if reader contains invalid IR
 * @return IRErrorCode_Incomplete_IR if reader doesn't contain enough data to deserialize
 */
IRErrorCode deserialize_preamble(
        ReaderInterface& reader,
        encoded_tag_t& metadata_type,
        std::vector<int8_t>& metadata
);

/**
 * Deserializes a UTC offset change packet.
 * @param reader
 * @param utc_offset The deserialized UTC offset.
 * @return IRErrorCode_Success on success
 * @return IRErrorCode_Incomplete_IR if reader doesn't contain enough data to deserialize
 */
IRErrorCode deserialize_utc_offset_change(ReaderInterface& reader, UtcOffset& utc_offset);

/**
 * Validates whether the given protocol version can be supported by the current build.
 * @param protocol_version
 * @return IRProtocolErrorCode_Supported if the protocol version is supported.
 * @return IRProtocolErrorCode_Too_Old if the protocol version is no longer supported by this
 * build's protocol version.
 * @return IRProtocolErrorCode_Too_New if the protocol version is newer than this build's protocol
 * version.
 * @return IRProtocolErrorCode_Invalid if the protocol version does not follow the SemVer
 * specification.
 */
IRProtocolErrorCode validate_protocol_version(std::string_view protocol_version);

namespace eight_byte_encoding {
/**
 * Deserializes the next log event from an eight-byte encoding IR stream and decodes the message.
 * @param reader
 * @param encoded_tag
 * @param message Returns the deserialized message
 * @param timestamp Returns the deserialized timestamp
 * @return ErrorCode_Success on success
 * @return ErrorCode_Corrupted_IR if reader contains invalid IR
 * @return ErrorCode_Decode_Error if the log event cannot be properly deserialized
 * @return ErrorCode_Incomplete_IR if reader doesn't contain enough data to deserialize
 */
IRErrorCode deserialize_log_event(
        ReaderInterface& reader,
        encoded_tag_t encoded_tag,
        std::string& message,
        ir::epoch_time_ms_t& timestamp
);
}  // namespace eight_byte_encoding

namespace four_byte_encoding {
/**
 * Deserializes the next log event from a four-byte encoding IR stream and decodes the message.
 * @param reader
 * @param encoded_tag
 * @param message Returns the deserialized message
 * @param timestamp_delta Returns the deserialized timestamp delta
 * @return ErrorCode_Success on success
 * @return ErrorCode_Corrupted_IR if reader contains invalid IR
 * @return ErrorCode_Decode_Error if the log event cannot be properly deserialized
 * @return ErrorCode_Incomplete_IR if reader doesn't contain enough data to deserialize
 */
IRErrorCode deserialize_log_event(
        ReaderInterface& reader,
        encoded_tag_t encoded_tag,
        std::string& message,
        ir::epoch_time_ms_t& timestamp_delta
);
}  // namespace four_byte_encoding

template <
        bool unescape_logtype,
        typename encoded_variable_t,
        typename ConstantHandler,
        typename EncodedIntHandler,
        typename EncodedFloatHandler,
        typename DictVarHandler>
void generic_decode_message(
        std::string const& logtype,
        std::vector<encoded_variable_t> const& encoded_vars,
        std::vector<std::string> const& dict_vars,
        ConstantHandler constant_handler,
        EncodedIntHandler encoded_int_handler,
        EncodedFloatHandler encoded_float_handler,
        DictVarHandler dict_var_handler
) {
    auto const logtype_length = logtype.length();
    auto const encoded_vars_length = encoded_vars.size();
    auto const dict_vars_length = dict_vars.size();
    size_t next_static_text_begin_pos = 0;

    size_t dictionary_vars_ix = 0;
    size_t encoded_vars_ix = 0;
    for (size_t cur_pos = 0; cur_pos < logtype_length; ++cur_pos) {
        auto c = logtype[cur_pos];
        switch (c) {
            case enum_to_underlying_type(ir::VariablePlaceholder::Float): {
                constant_handler(
                        logtype,
                        next_static_text_begin_pos,
                        cur_pos - next_static_text_begin_pos
                );
                next_static_text_begin_pos = cur_pos + 1;
                if (encoded_vars_ix >= encoded_vars_length) {
                    throw DecodingException(
                            ErrorCode_Corrupt,
                            __FILENAME__,
                            __LINE__,
                            cTooFewEncodedVarsErrorMessage
                    );
                }
                encoded_float_handler(encoded_vars[encoded_vars_ix]);
                ++encoded_vars_ix;

                break;
            }

            case enum_to_underlying_type(ir::VariablePlaceholder::Integer): {
                constant_handler(
                        logtype,
                        next_static_text_begin_pos,
                        cur_pos - next_static_text_begin_pos
                );
                next_static_text_begin_pos = cur_pos + 1;
                if (encoded_vars_ix >= encoded_vars_length) {
                    throw DecodingException(
                            ErrorCode_Corrupt,
                            __FILENAME__,
                            __LINE__,
                            cTooFewEncodedVarsErrorMessage
                    );
                }
                encoded_int_handler(encoded_vars[encoded_vars_ix]);
                ++encoded_vars_ix;

                break;
            }

            case enum_to_underlying_type(ir::VariablePlaceholder::Dictionary): {
                constant_handler(
                        logtype,
                        next_static_text_begin_pos,
                        cur_pos - next_static_text_begin_pos
                );
                next_static_text_begin_pos = cur_pos + 1;
                if (dictionary_vars_ix >= dict_vars_length) {
                    throw DecodingException(
                            ErrorCode_Corrupt,
                            __FILENAME__,
                            __LINE__,
                            cTooFewDictionaryVarsErrorMessage
                    );
                }
                dict_var_handler(dict_vars[dictionary_vars_ix]);
                ++dictionary_vars_ix;

                break;
            }

            case enum_to_underlying_type(ir::VariablePlaceholder::Escape): {
                // Ensure the escape character is followed by a character that's being escaped
                if (cur_pos == logtype_length - 1) {
                    throw DecodingException(
                            ErrorCode_Corrupt,
                            __FILENAME__,
                            __LINE__,
                            cUnexpectedEscapeCharacterMessage
                    );
                }

                if constexpr (unescape_logtype) {
                    constant_handler(
                            logtype,
                            next_static_text_begin_pos,
                            cur_pos - next_static_text_begin_pos
                    );

                    // Skip the escape character
                    next_static_text_begin_pos = cur_pos + 1;
                }
                // The character after the escape character is static text (regardless of whether it
                // is a variable placeholder), so increment cur_pos by 1 to ensure we don't process
                // the next character in any of the other cases (instead it will be added to the
                // message).
                ++cur_pos;

                break;
            }
        }
    }
    // Add remainder
    if (next_static_text_begin_pos < logtype_length) {
        constant_handler(
                logtype,
                next_static_text_begin_pos,
                logtype_length - next_static_text_begin_pos
        );
    }
}
}  // namespace clp::ffi::ir_stream

#endif  // CLP_FFI_IR_STREAM_DECODING_METHODS_HPP
