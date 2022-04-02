#include "Compressor.hpp"

// spdlog
#include <spdlog/spdlog.h>

// Project headers
#include "../../Defs.h"

// File-scope constants
static constexpr size_t cCompressedStreamBlockBufferSize = 4096; // 4KiB

namespace streaming_compression { namespace gzip {
    Compressor::Compressor () : ::streaming_compression::Compressor(CompressorType::GZIP), m_compression_stream_contains_data(false),
            m_compressed_stream_file_writer(nullptr), m_compression_stream(nullptr)
    {
        m_compressed_stream_block_buffer = std::make_unique<Bytef[]>(cCompressedStreamBlockBufferSize);
    }

    Compressor::~Compressor () {
        if (nullptr != m_compression_stream) {
            int return_value = deflateEnd(m_compression_stream);
            if (Z_OK != return_value) {
//                SPDLOG_ERROR("streaming_compression::gzip::Compressor: deflateEnd() error - {}", m_compression_stream->msg);
            }
            delete m_compression_stream;
        }
    }

    void Compressor::open (FileWriter& file_writer, const int compression_level) {
        if (nullptr != m_compressed_stream_file_writer) {
            throw OperationFailed(ErrorCode_NotReady, __FILENAME__, __LINE__);
        }

        if (compression_level < 0 || 9 < compression_level) {
            throw OperationFailed(ErrorCode_BadParam, __FILENAME__, __LINE__);
        }

        // Setup compressed stream parameters
        m_compression_stream = new z_stream;
        m_compression_stream->next_in = nullptr;
        m_compression_stream->avail_in = 0;
        m_compression_stream->next_out = m_compressed_stream_block_buffer.get();
        m_compression_stream->avail_out = cCompressedStreamBlockBufferSize;
        m_compression_stream->data_type = Z_BINARY;
        m_compression_stream->zalloc = nullptr;
        m_compression_stream->zfree = nullptr;

        // Setup compression stream
        int return_value = deflateInit(m_compression_stream, compression_level);
        if (Z_OK != return_value) {
//            SPDLOG_ERROR("streaming_compression::gzip::Compressor: deflateInit() error: {}", m_compression_stream->msg);
            throw OperationFailed(ErrorCode_Failure, __FILENAME__, __LINE__);
        }

        m_compressed_stream_file_writer = &file_writer;

        m_uncompressed_stream_pos = 0;
    }

    void Compressor::close () {
        if (nullptr == m_compressed_stream_file_writer) {
            throw OperationFailed(ErrorCode_NotInit, __FILENAME__, __LINE__);
        }

        flush_and_close_compression_stream();
        delete m_compression_stream;
        m_compression_stream = nullptr;
        m_compressed_stream_file_writer = nullptr;
    }

    void Compressor::write (const char* data, size_t data_length) {
        if (nullptr == m_compressed_stream_file_writer) {
            throw OperationFailed(ErrorCode_NotInit, __FILENAME__, __LINE__);
        }

        if (0 == data_length) {
            // Nothing needs to be done because we do not need to compress anything
            return;
        }
        if (nullptr == data) {
            throw OperationFailed(ErrorCode_BadParam, __FILENAME__, __LINE__);
        }

        m_compression_stream->next_in = reinterpret_cast<Bytef*>(const_cast<char*>(data));
        m_compression_stream->avail_in = data_length;

        // Compress all data
        while (true) {
            auto return_value = deflate(m_compression_stream, Z_NO_FLUSH);
            switch (return_value) {
                case Z_OK:
                case Z_BUF_ERROR:
                    break;
                case Z_STREAM_ERROR:
//                    SPDLOG_ERROR("deflate() failed due to a logic error.");
                    throw OperationFailed(ErrorCode_Failure, __FILENAME__, __LINE__);
                default:
//                    SPDLOG_ERROR("deflate() returned an unexpected value - {}.", return_value);
                    throw OperationFailed(ErrorCode_Failure, __FILENAME__, __LINE__);
            }

            if (0 == m_compression_stream->avail_in) {
                // No more data to compress
                break;
            }

            // Write output buffer to file if it's full
            if (0 == m_compression_stream->avail_out) {
                m_compressed_stream_file_writer->write(reinterpret_cast<char*>(m_compressed_stream_block_buffer.get()), cCompressedStreamBlockBufferSize);
                m_compression_stream->next_out = m_compressed_stream_block_buffer.get();
                m_compression_stream->avail_out = cCompressedStreamBlockBufferSize;
            }
        }

        // Write any compressed data
        if (m_compression_stream->avail_out < cCompressedStreamBlockBufferSize) {
            m_compressed_stream_file_writer->write(reinterpret_cast<char*>(m_compressed_stream_block_buffer.get()),
                                                   cCompressedStreamBlockBufferSize - m_compression_stream->avail_out);
            m_compression_stream->next_out = m_compressed_stream_block_buffer.get();
            m_compression_stream->avail_out = cCompressedStreamBlockBufferSize;
        }

        m_compression_stream->next_in = nullptr;

        m_compression_stream_contains_data = true;
        m_uncompressed_stream_pos += data_length;
    }

    void Compressor::flush () {
        if (false == m_compression_stream_contains_data) {
            return;
        }

        bool flush_complete = false;
        while (true) {
            // NOTE: We use Z_PARTIAL_FLUSH instead of Z_SYNC_FLUSH since it should result in lower overhead
            auto return_value = deflate(m_compression_stream, Z_PARTIAL_FLUSH);
            switch (return_value) {
                case Z_OK:
                    break;
                case Z_BUF_ERROR:
                    flush_complete = true;
                    break;
                case Z_STREAM_ERROR:
//                    SPDLOG_ERROR("deflate() failed due to a logic error.");
                    throw OperationFailed(ErrorCode_Failure, __FILENAME__, __LINE__);
                default:
//                    SPDLOG_ERROR("deflate() returned an unexpected value - {}.", return_value);
                    throw OperationFailed(ErrorCode_Failure, __FILENAME__, __LINE__);
            }
            if (flush_complete) {
                break;
            }

            // Write output buffer to file if it's full
            if (0 == m_compression_stream->avail_out) {
                m_compressed_stream_file_writer->write(reinterpret_cast<char*>(m_compressed_stream_block_buffer.get()), cCompressedStreamBlockBufferSize);
                m_compression_stream->next_out = m_compressed_stream_block_buffer.get();
                m_compression_stream->avail_out = cCompressedStreamBlockBufferSize;
            }
        }

        // Write any compressed data
        if (m_compression_stream->avail_out < cCompressedStreamBlockBufferSize) {
            m_compressed_stream_file_writer->write(reinterpret_cast<char*>(m_compressed_stream_block_buffer.get()),
                                                   cCompressedStreamBlockBufferSize - m_compression_stream->avail_out);
            m_compression_stream->next_out = m_compressed_stream_block_buffer.get();
            m_compression_stream->avail_out = cCompressedStreamBlockBufferSize;
        }

        m_compression_stream_contains_data = false;
    }

    ErrorCode Compressor::try_get_pos (size_t& pos) const {
        if (nullptr == m_compressed_stream_file_writer) {
            return ErrorCode_NotInit;
        }

        pos = m_uncompressed_stream_pos;
        return ErrorCode_Success;
    }

    void Compressor::flush_and_close_compression_stream () {
        if (nullptr == m_compressed_stream_file_writer) {
            throw OperationFailed(ErrorCode_NotInit, __FILENAME__, __LINE__);
        }

        bool flush_complete = false;
        while (true) {
            auto return_value = deflate(m_compression_stream, Z_FINISH);
            switch (return_value) {
                case Z_OK:
                case Z_BUF_ERROR:
                    break;
                case Z_STREAM_ERROR:
//                    SPDLOG_ERROR("deflate() failed due to a logic error.");
                    throw OperationFailed(ErrorCode_Failure, __FILENAME__, __LINE__);
                case Z_STREAM_END:
                    flush_complete = true;
                    break;
                default:
//                    SPDLOG_ERROR("deflate() returned an unexpected value - {}.", return_value);
                    throw OperationFailed(ErrorCode_Failure, __FILENAME__, __LINE__);
            }
            if (flush_complete) {
                break;
            }

            // Write output buffer to file if it's full
            if (0 == m_compression_stream->avail_out) {
                m_compressed_stream_file_writer->write(reinterpret_cast<char*>(m_compressed_stream_block_buffer.get()), cCompressedStreamBlockBufferSize);
                m_compression_stream->next_out = m_compressed_stream_block_buffer.get();
                m_compression_stream->avail_out = cCompressedStreamBlockBufferSize;
            }
        }

        // Write any compressed data
        if (m_compression_stream->avail_out < cCompressedStreamBlockBufferSize) {
            m_compressed_stream_file_writer->write(reinterpret_cast<char*>(m_compressed_stream_block_buffer.get()),
                                                   cCompressedStreamBlockBufferSize - m_compression_stream->avail_out);
            m_compression_stream->next_out = m_compressed_stream_block_buffer.get();
            m_compression_stream->avail_out = cCompressedStreamBlockBufferSize;
        }

        m_compression_stream_contains_data = false;

        // TODO Should we call this in the destructor
        auto return_value = deflateEnd(m_compression_stream);
        if (Z_OK != return_value) {
            SPDLOG_ERROR("deflateEnd() failed - {}.", m_compression_stream->msg);
        }
    }
} }
