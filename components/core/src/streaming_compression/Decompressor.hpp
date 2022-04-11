#ifndef STREAMING_COMPRESSION_DECOMPRESSOR_HPP
#define STREAMING_COMPRESSION_DECOMPRESSOR_HPP

// C++ libraries
#include <string>

// Project headers
#include "../FileReader.hpp"
#include "../ReaderInterface.hpp"
#include "../TraceableException.hpp"
#include "Constants.hpp"

namespace streaming_compression {
    class Decompressor : public ReaderInterface {
    public:
        // Types
        class OperationFailed : public TraceableException {
        public:
            // Constructors
            OperationFailed (ErrorCode error_code, const char* const filename, int line_number) : TraceableException (error_code, filename, line_number) {}

            // Methods
            const char* what () const noexcept override {
                return "streaming_compression::Decompressor operation failed";
            }
        };

        // Constructor
        explicit Decompressor (CompressorType type) : m_compression_type(type) {}

        // Destructor
        ~Decompressor () = default;

        // Explicitly disable copy and move constructor/assignment
        Decompressor (const Decompressor&) = delete;
        Decompressor& operator = (const Decompressor&) = delete;

        // Methods
        /**
         * Initialize streaming decompressor to decompress from the specified compressed data buffer
         * @param compressed_data_buf
         * @param compressed_data_buf_size
         */
        virtual void open (const char* compressed_data_buf, size_t compressed_data_buf_size) = 0;
        /**
         * Initializes the decompressor to decompress from an open file
         * @param file_reader
         * @param file_read_buffer_capacity The maximum amount of data to read from a file at a time
         */
        virtual void open (FileReader& file_reader, size_t file_read_buffer_capacity) = 0;
        /**
         * Closes decompression stream
         */
        virtual void close () = 0;

        virtual ErrorCode get_decompressed_stream_region (size_t decompressed_stream_pos, char* extraction_buf, size_t extraction_len) = 0;

    protected:
        // Variables
        CompressorType m_compression_type;
    };
}

#endif //STREAMING_COMPRESSION_DECOMPRESSOR_HPP
