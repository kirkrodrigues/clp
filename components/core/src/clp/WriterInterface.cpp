#include "WriterInterface.hpp"

#include "Defs.h"

namespace clp {
auto WriterInterface::write_char(char c) -> void {
    write(&c, 1);
}

auto WriterInterface::write_string(std::string const& str) -> void {
    write(str.c_str(), str.length());
}

auto WriterInterface::seek_from_begin(size_t pos) -> void {
    auto error_code = try_seek_from_begin(pos);
    if (ErrorCode_Success != error_code) {
        throw OperationFailed(error_code, __FILENAME__, __LINE__);
    }
}

auto WriterInterface::seek_from_current(off_t offset) -> void {
    auto error_code = try_seek_from_current(offset);
    if (ErrorCode_Success != error_code) {
        throw OperationFailed(error_code, __FILENAME__, __LINE__);
    }
}

auto WriterInterface::get_pos() const -> size_t {
    size_t pos;
    ErrorCode error_code = try_get_pos(pos);
    if (ErrorCode_Success != error_code) {
        throw OperationFailed(error_code, __FILENAME__, __LINE__);
    }

    return pos;
}
}  // namespace clp
