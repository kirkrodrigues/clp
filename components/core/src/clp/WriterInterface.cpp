#include "WriterInterface.hpp"

#include <sys/types.h>

#include <cstddef>
#include <string>

#include "ErrorCode.hpp"
#include "TraceableException.hpp"

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
    size_t pos{0};
    if (auto const error_code = try_get_pos(pos); ErrorCode_Success != error_code) {
        throw OperationFailed(error_code, __FILENAME__, __LINE__);
    }

    return pos;
}
}  // namespace clp
