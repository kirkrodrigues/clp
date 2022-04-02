#include "Decompressor.hpp"

// C++ standard libraries
#include <cstring>

namespace streaming_compression { namespace passthrough {
    ErrorCode Decompressor::try_read (char* buf, size_t num_bytes_to_read, size_t& num_bytes_read) {
        if (nullptr == buf) {
            return ErrorCode_BadParam;
        }
        if (InputType::CompressedDataBuf == m_input_type) {
            if (nullptr == m_compressed_data_buf) {
                return ErrorCode_NotInit;
            }

            if (m_compressed_data_buf_len == m_decompressed_stream_pos) {
                return ErrorCode_EndOfFile;
            }

            num_bytes_read = std::min(num_bytes_to_read, m_compressed_data_buf_len - m_decompressed_stream_pos);
            memcpy(buf, &m_compressed_data_buf[m_decompressed_stream_pos], num_bytes_read);
        } else if (InputType::File == m_input_type) {
            if (nullptr == m_file_reader) {
                return ErrorCode_NotInit;
            }

            auto error_code = m_file_reader->try_read(buf, num_bytes_to_read, num_bytes_read);
            if (ErrorCode_Success != error_code) {
                return error_code;
            }
        }

        m_decompressed_stream_pos += num_bytes_read;

        return ErrorCode_Success;
    }

    ErrorCode Decompressor::try_seek_from_begin (size_t pos) {
        if (InputType::CompressedDataBuf == m_input_type) {
            if (nullptr == m_compressed_data_buf) {
                return ErrorCode_NotInit;
            }

            if (pos > m_compressed_data_buf_len) {
                return ErrorCode_Truncated;
            }

        } else if (InputType::File == m_input_type) {
            if (nullptr == m_file_reader) {
                return ErrorCode_NotInit;
            }

            auto error_code = m_file_reader->try_seek_from_begin(pos);
            if (ErrorCode_Success != error_code) {
                return error_code;
            }
        }

        m_decompressed_stream_pos = pos;

        return ErrorCode_Success;
    }

    ErrorCode Decompressor::try_get_pos (size_t& pos) {
        if (InputType::CompressedDataBuf == m_input_type) {
            if (nullptr == m_compressed_data_buf) {
                return ErrorCode_NotInit;
            }
        } else if (InputType::File == m_input_type) {
            if (nullptr == m_file_reader) {
                return ErrorCode_NotInit;
            }
        }

        pos = m_decompressed_stream_pos;

        return ErrorCode_Success;
    }

    void Decompressor::close () {
        m_file_reader = nullptr;
        m_compressed_data_buf = nullptr;
        m_compressed_data_buf_len = 0;
    }

    ErrorCode Decompressor::get_decompressed_stream_region (size_t decompressed_stream_pos, char* extraction_buf, size_t extraction_len) {
        auto error_code = try_seek_from_begin(decompressed_stream_pos);
        if (ErrorCode_Success != error_code) {
            return error_code;
        }

        error_code = try_read_exact_length(extraction_buf, extraction_len);
        return error_code;
    }

    void Decompressor::open (const char* compressed_data_buf, size_t compressed_data_buf_size) {
        if (nullptr == compressed_data_buf) {
            throw OperationFailed(ErrorCode_BadParam, __FILENAME__, __LINE__);
        }

        m_compressed_data_buf = compressed_data_buf;
        m_compressed_data_buf_len = compressed_data_buf_size;
        m_decompressed_stream_pos = 0;
        m_input_type = InputType::CompressedDataBuf;
    }

    void Decompressor::open (FileReader& file_reader) {
        m_file_reader = &file_reader;
        m_decompressed_stream_pos = 0;
        m_input_type = InputType::File;
    }
} }
