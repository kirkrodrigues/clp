#include "Decompressor.hpp"

// C++ Standard Libraries
#include <algorithm>

// Boost libraries
#include <boost/filesystem.hpp>

// spdlog
#include <spdlog/spdlog.h>

// Project headers
#include "../../Defs.h"

namespace streaming_compression { namespace gzip {
    Decompressor::Decompressor () : ::streaming_compression::Decompressor(CompressorType::GZIP), m_input_type(InputType::NotInitialized),
            m_decompression_stream(nullptr), m_file_reader(nullptr), m_file_reader_initial_pos(0), m_file_read_buffer_length(0), m_file_read_buffer_capacity(0),
            m_decompressed_stream_pos(0), m_unused_decompressed_stream_block_size(0)
    {
        // Create block to hold unused decompressed data
        m_unused_decompressed_stream_block_buffer = std::make_unique<char[]>(m_unused_decompressed_stream_block_size);

        m_decompression_stream = new z_stream;
    }

    Decompressor::~Decompressor () {
        auto return_value = inflateEnd(m_decompression_stream);
        if (Z_OK != return_value) {
            SPDLOG_ERROR("streaming_compression::gzip::Decompressor: inflateEnd() failed - {}", m_decompression_stream->msg);
        }
    }

    ErrorCode Decompressor::try_read (char* buf, size_t num_bytes_to_read, size_t& num_bytes_read) {
        if (InputType::NotInitialized == m_input_type) {
            return ErrorCode_NotInit;
        }
        if (nullptr == buf) {
            return ErrorCode_BadParam;
        }

        num_bytes_read = 0;

        m_decompression_stream->next_out = reinterpret_cast<Bytef*>(buf);
        m_decompression_stream->avail_out = num_bytes_to_read;
        while (true) {
            // Check if there's data that can be decompressed
            if (0 == m_decompression_stream->avail_in) {
                if (InputType::File != m_input_type) {

                } else {
                    auto error_code = m_file_reader->try_read(m_file_read_buffer.get(), m_file_read_buffer_capacity,
                                                              m_file_read_buffer_length);
                    if (ErrorCode_Success != error_code) {
                        if (ErrorCode_EndOfFile == error_code) {

                        }
                    }
                }
            }

            auto return_value = inflate(m_decompression_stream, Z_SYNC_FLUSH);
            switch (return_value) {
                case Z_OK:
                case Z_BUF_ERROR:
                    if (0 == m_decompression_stream->avail_out) {
                        m_decompression_stream->next_out = nullptr;
                        return ErrorCode_Success;
                    }
                    break;
                case Z_STREAM_END:
                    // TODO

                    break;
                case Z_NEED_DICT:
                    SPDLOG_ERROR("streaming_compression::gzip::Decompressor does not support compression with dictionaries.");
                    throw OperationFailed(ErrorCode_Unsupported, __FILENAME__, __LINE__);
                case Z_STREAM_ERROR:
                    SPDLOG_ERROR("streaming_compression::gzip::Decompressor inflate() failed due to a logic error.");
                    throw OperationFailed(ErrorCode_Unsupported, __FILENAME__, __LINE__);
                case Z_DATA_ERROR:
                    SPDLOG_ERROR("streaming_compression::gzip::Decompressor inflate() failed - {}", m_decompression_stream->msg);
                    throw OperationFailed(ErrorCode_Failure, __FILENAME__, __LINE__);
                case Z_MEM_ERROR:
                    SPDLOG_ERROR("streaming_compression::gzip::Decompressor inflate() ran out of memory");
                    throw OperationFailed(ErrorCode_Failure, __FILENAME__, __LINE__);
                default:
                    SPDLOG_ERROR("inflate() returned an unexpected value - {}.", return_value);
                    throw OperationFailed(ErrorCode_Failure, __FILENAME__, __LINE__);
            }
        }

        while (decompressed_stream_block.pos < num_bytes_to_read) {
            // Check if there's data that can be decompressed
            if (m_compressed_stream_block.pos == m_compressed_stream_block.size) {
                if (InputType::File != m_input_type) {
                    num_bytes_read = decompressed_stream_block.pos;
                    if (0 == decompressed_stream_block.pos) {
                        return ErrorCode_EndOfFile;
                    } else {
                        return ErrorCode_Success;
                    }
                } else {
                    auto error_code = m_file_reader->try_read(reinterpret_cast<char*>(m_file_read_buffer.get()), m_file_read_buffer_capacity,
                                                              m_file_read_buffer_length);
                    if (ErrorCode_Success != error_code) {
                        if (ErrorCode_EndOfFile == error_code) {
                            num_bytes_read = decompressed_stream_block.pos;
                            if (0 == decompressed_stream_block.pos) {
                                return ErrorCode_EndOfFile;
                            } else {
                                return ErrorCode_Success;
                            }
                        } else {
                            return error_code;
                        }
                    }

                    m_compressed_stream_block.pos = 0;
                    m_compressed_stream_block.size = m_file_read_buffer_length;
                }
            }

            // Decompress
            size_t error = ZSTD_decompressStream(m_decompression_stream, &decompressed_stream_block, &m_compressed_stream_block);
            if (ZSTD_isError(error)) {
                SPDLOG_ERROR("streaming_compression::zstd::Decompressor: ZSTD_decompressStream() error: {}", ZSTD_getErrorName(error));
                return ErrorCode_Failure;
            }
        }

        // Update decompression stream position
        m_decompressed_stream_pos += decompressed_stream_block.pos;

        num_bytes_read = decompressed_stream_block.pos;
        return ErrorCode_Success;
    }

    ErrorCode Decompressor::try_seek_from_begin (size_t pos) {
        if (InputType::NotInitialized == m_input_type) {
            throw OperationFailed(ErrorCode_NotInit, __FILENAME__, __LINE__);
        }

        // Check if we've already decompressed passed the desired position
        if (m_decompressed_stream_pos > pos) {
            // ZStd has no way for us to seek back to the desired position, so just reset the stream to the beginning
            reset_stream();
        }

        // We need to fast-forward the decompression stream to decompressed_stream_pos
        ErrorCode error;
        while (m_decompressed_stream_pos < pos) {
            size_t num_bytes_to_decompress = std::min(m_unused_decompressed_stream_block_size, pos - m_decompressed_stream_pos);
            error = try_read_exact_length(m_unused_decompressed_stream_block_buffer.get(), num_bytes_to_decompress);
            if (ErrorCode_Success != error) {
                return error;
            }
        }

        return ErrorCode_Success;
    }

    ErrorCode Decompressor::try_get_pos (size_t& pos) {
        if (InputType::NotInitialized == m_input_type) {
            return ErrorCode_NotInit;
        }

        pos = m_decompressed_stream_pos;
        return ErrorCode_Success;
    }

    void Decompressor::close () {
        if (InputType::MemoryMappedCompressedFile == m_input_type) {
            if (m_memory_mapped_compressed_file.is_open()) {
                // An existing file is memory mapped by the decompressor
                m_memory_mapped_compressed_file.close();
            }
        } else if (InputType::File == m_input_type) {
            m_file_read_buffer.reset();
            m_file_read_buffer_capacity = 0;
            m_file_read_buffer_length = 0;
            m_file_reader = nullptr;
        }
        m_input_type = InputType::NotInitialized;
    }

    void Decompressor::open (const char* compressed_data_buf, size_t compressed_data_buf_size) {
        if (InputType::NotInitialized != m_input_type) {
            throw OperationFailed(ErrorCode_NotReady, __FILENAME__, __LINE__);
        }
        m_input_type = InputType::CompressedDataBuf;

        // Configure input stream
        m_decompression_stream = new z_stream;
        m_decompression_stream->next_in = reinterpret_cast<Bytef*>(const_cast<char*>(compressed_data_buf));
        m_decompression_stream->avail_in = compressed_data_buf_size;
        m_decompression_stream->next_out = nullptr;
        m_decompression_stream->avail_out = 0;
        m_decompression_stream->data_type = Z_BINARY;
        m_decompression_stream->zalloc = Z_NULL;
        m_decompression_stream->zfree = Z_NULL;
        m_decompression_stream->opaque = Z_NULL;

        reset_stream();
    }

    ErrorCode Decompressor::open (const std::string& compressed_file_path) {
        if (InputType::NotInitialized != m_input_type) {
            throw OperationFailed(ErrorCode_NotReady, __FILENAME__, __LINE__);
        }
        m_input_type = InputType::MemoryMappedCompressedFile;

        // Create memory mapping for compressed_file_path, use boost read only memory mapped file
        boost::system::error_code boost_error_code;
        size_t compressed_file_size = boost::filesystem::file_size(compressed_file_path, boost_error_code);
        if (boost_error_code) {
            SPDLOG_ERROR("streaming_compression::zstd::Decompressor: Unable to obtain file size for '{}' - {}.", compressed_file_path.c_str(),
                         boost_error_code.message().c_str());
            return ErrorCode_Failure;
        }

        boost::iostreams::mapped_file_params memory_map_params;
        memory_map_params.path = compressed_file_path;
        memory_map_params.flags = boost::iostreams::mapped_file::readonly;
        memory_map_params.length = compressed_file_size;
        memory_map_params.hint = m_memory_mapped_compressed_file.data();  // Try to map it to the same memory location as previous memory mapped file
        m_memory_mapped_compressed_file.open(memory_map_params);
        if (!m_memory_mapped_compressed_file.is_open()) {
            SPDLOG_ERROR("streaming_compression::gzip::Decompressor: Unable to memory map the compressed file with path: {}", compressed_file_path.c_str());
            return ErrorCode_Failure;
        }

        // Configure input stream
        m_decompression_stream = new z_stream;
        m_decompression_stream->next_in = reinterpret_cast<Bytef*>(const_cast<char*>(m_memory_mapped_compressed_file.data()));
        m_decompression_stream->avail_in = compressed_file_size;
        m_decompression_stream->next_out = nullptr;
        m_decompression_stream->avail_out = 0;
        m_decompression_stream->zalloc = Z_NULL;
        m_decompression_stream->zfree = Z_NULL;
        m_decompression_stream->opaque = Z_NULL;

        reset_stream();

        return ErrorCode_Success;
    }

    void Decompressor::open (FileReader& file_reader, size_t file_read_buffer_capacity) {
        if (InputType::NotInitialized != m_input_type) {
            throw OperationFailed(ErrorCode_NotReady, __FILENAME__, __LINE__);
        }
        m_input_type = InputType::File;

        m_file_reader = &file_reader;
        m_file_reader_initial_pos = m_file_reader->get_pos();

        m_file_read_buffer_capacity = file_read_buffer_capacity;
        m_file_read_buffer = std::make_unique<char[]>(m_file_read_buffer_capacity);
        m_file_read_buffer_length = 0;

        // Configure input stream
        m_decompression_stream = new z_stream;
        m_decompression_stream->next_in = reinterpret_cast<Bytef*>(m_file_read_buffer.get());
        m_decompression_stream->avail_in = m_file_read_buffer_length;
        m_decompression_stream->next_out = nullptr;
        m_decompression_stream->avail_out = 0;
        m_decompression_stream->data_type = Z_BINARY;
        m_decompression_stream->zalloc = Z_NULL;
        m_decompression_stream->zfree = Z_NULL;
        m_decompression_stream->opaque = Z_NULL;

        reset_stream();
    }

    ErrorCode Decompressor::get_decompressed_stream_region (size_t decompressed_stream_pos, char* extraction_buf, size_t extraction_len) {
        auto error_code = try_seek_from_begin(decompressed_stream_pos);
        if (ErrorCode_Success != error_code) {
            return error_code;
        }

        error_code = try_read_exact_length(extraction_buf, extraction_len);
        return error_code;
    }

    void Decompressor::reset_stream () {
        if (InputType::File == m_input_type) {
            m_file_reader->seek_from_begin(m_file_reader_initial_pos);
            m_file_read_buffer_length = 0;
        }

        // TODO Need to call inflateEnd if this is a second call?
        auto return_value = inflateInit(m_decompression_stream);
        if (Z_OK != return_value) {
            // TODO Error handling
            SPDLOG_ERROR("streaming_compression::gzip::Decompressor: inflateInit() failed - {}", m_decompression_stream->msg);
        }
        m_decompressed_stream_pos = 0;
    }
} }
