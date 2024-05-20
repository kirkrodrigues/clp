#ifndef CLP_S_PASSTHROUGHCOMPRESSOR_HPP
#define CLP_S_PASSTHROUGHCOMPRESSOR_HPP

#include "Compressor.hpp"
#include "FileWriter.hpp"

namespace clp_s {
class PassthroughCompressor : public Compressor {
public:
    // Types
    class OperationFailed : public TraceableException {
    public:
        // Constructors
        OperationFailed(ErrorCode error_code, char const* const filename, int line_number)
                : TraceableException(error_code, filename, line_number) {}
    };

    // Constants
    static constexpr int cDefaultCompressionLevel = 3;

    // Constructors
    PassthroughCompressor() : Compressor(CompressorType::Passthrough) {};

    PassthroughCompressor(PassthroughCompressor const&) = delete;
    PassthroughCompressor(PassthroughCompressor const&&) = delete;
    PassthroughCompressor& operator=(PassthroughCompressor const&) = delete;
    PassthroughCompressor& operator=(PassthroughCompressor&&) = delete;

    // Destructor
    ~PassthroughCompressor() override = default;

    // Methods implementing the WriterInterface
    /**
     * Writes the given data to the compressor
     * @param data
     * @param data_length
     */
    void write(char const* data, size_t data_length);

    /**
     * Writes the given numeric value to the compressor
     * @param val
     * @tparam ValueType
     */
    template <typename ValueType>
    void write_numeric_value(ValueType val) {
        write(reinterpret_cast<char*>(&val), sizeof(val));
    }

    /**
     * Writes the given string to the compressor
     * @param str
     */
    void write_string(std::string const& str) { write(str.c_str(), str.length()); }

    /**
     * Writes any internally buffered data to file and ends the current frame
     */
    void flush();

    // Methods implementing the Compressor interface
    /**
     * Closes the compressor
     */
    void close() override;

    // Methods
    /**
     * Initialize streaming compressor
     * @param file_writer
     * @param compression_level
     */
    void open(FileWriter& file_writer, int compression_level = cDefaultCompressionLevel);

private:
    FileWriter* m_compressed_stream_file_writer{};
};
}  // namespace clp_s

#endif  // CLP_S_PASSTHROUGHCOMPRESSOR_HPP
