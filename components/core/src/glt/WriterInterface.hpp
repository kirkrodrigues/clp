#ifndef GLT_WRITERINTERFACE_HPP
#define GLT_WRITERINTERFACE_HPP

#include <sys/types.h>

#include <cstddef>
#include <string>

#include "ErrorCode.hpp"
#include "TraceableException.hpp"

namespace glt {
class WriterInterface {
public:
    // Types
    class OperationFailed : public TraceableException {
    public:
        // Constructors
        OperationFailed(ErrorCode error_code, char const* const filename, int line_number)
                : TraceableException(error_code, filename, line_number) {}

        // Methods
        [[nodiscard]] auto what() const noexcept -> char const* override {
            return "WriterInterface operation failed";
        }
    };

    // Constructors
    WriterInterface() = default;

    // Default copy & move constructors and assignment operators
    WriterInterface(WriterInterface const&) = default;
    WriterInterface(WriterInterface&&) = default;
    auto operator=(WriterInterface const&) -> WriterInterface& = default;
    auto operator=(WriterInterface&&) -> WriterInterface& = default;

    // Destructor
    virtual ~WriterInterface() = default;

    // Methods
    /**
     * Writes the given data to the underlying medium
     * @param data
     * @param data_length
     */
    virtual auto write(char const* data, size_t data_length) -> void = 0;
    virtual auto flush() -> void = 0;
    virtual auto try_seek_from_begin(size_t pos) -> ErrorCode = 0;
    virtual auto try_seek_from_current(off_t offset) -> ErrorCode = 0;
    virtual auto try_get_pos(size_t& pos) const -> ErrorCode = 0;

    /**
     * Writes a numeric value
     * @param val Value to write
     */
    template <typename ValueType>
    auto write_numeric_value(ValueType value) -> void;

    /**
     * Writes a character to the underlying medium
     * @param c
     */
    auto write_char(char c) -> void;
    /**
     * Writes a string to the underlying medium
     * @param str
     */
    auto write_string(std::string const& str) -> void;

    /**
     * Seeks from the beginning to the given position
     * @param pos
     */
    auto seek_from_begin(size_t pos) -> void;

    /**
     * Offsets from the current position by the given amount
     * @param offset
     */
    auto seek_from_current(off_t offset) -> void;

    /**
     * Gets the current position of the write head
     * @return Position of the write head
     */
    [[nodiscard]] auto get_pos() const -> size_t;
};

template <typename ValueType>
auto WriterInterface::write_numeric_value(ValueType val) -> void {
    // Pointer casts to char* are safe
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    write(reinterpret_cast<char*>(&val), sizeof(val));
}
}  // namespace glt

#endif  // GLT_WRITERINTERFACE_HPP
