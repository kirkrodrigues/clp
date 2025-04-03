#include "socket_utils.hpp"

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdio>
#include <string>

#include "../ErrorCode.hpp"
#include "../TraceableException.hpp"
#include "SocketOperationFailed.hpp"

namespace clp::networking {
auto connect_to_server(std::string const& host, std::string const& port) -> int {
    // Get address info
    struct addrinfo hints = {};
    // Address can be IPv4 or IPV6
    hints.ai_family = AF_UNSPEC;
    // TCP socket
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;
    struct addrinfo* addresses_head = nullptr;
    auto error = getaddrinfo(host.c_str(), port.c_str(), &hints, &addresses_head);
    if (0 != error) {
        return -1;
    }

    // Try each address until a socket can be created and connected to
    int socket_fd = -1;
    for (auto* curr = addresses_head; nullptr != curr; curr = curr->ai_next) {
        // Create socket
        socket_fd = socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol);
        if (-1 == socket_fd) {
            continue;
        }

        // Connect to address
        if (connect(socket_fd, curr->ai_addr, curr->ai_addrlen) != -1) {
            break;
        }

        // Failed to connect, so close socket
        close(socket_fd);
        socket_fd = -1;
    }
    freeaddrinfo(addresses_head);

    return socket_fd;
}

auto try_send(int fd, char const* buf, size_t buf_len) -> ErrorCode {
    if (fd < 0 || nullptr == buf) {
        return ErrorCode_BadParam;
    }

    ssize_t const num_bytes_sent = ::send(fd, buf, buf_len, 0);
    if (-1 == num_bytes_sent) {
        return ErrorCode_errno;
    }

    return ErrorCode_Success;
}

auto send(int fd, char const* buf, size_t buf_len) -> void {
    auto error_code = try_send(fd, buf, buf_len);
    if (ErrorCode_Success != error_code) {
        throw SocketOperationFailed(error_code, __FILENAME__, __LINE__);
    }
}

auto try_receive(int fd, char* buf, size_t buf_len, size_t& num_bytes_received) -> ErrorCode {
    if (fd < 0 || nullptr == buf) {
        return ErrorCode_BadParam;
    }

    ssize_t const result = recv(fd, buf, buf_len, 0);
    if (result < 0) {
        return ErrorCode_errno;
    }
    if (0 == result) {
        return ErrorCode_EndOfFile;
    }
    num_bytes_received = result;

    return ErrorCode_Success;
}

auto receive(int fd, char* buf, size_t buf_len, size_t& num_bytes_received) -> void {
    auto error_code = try_receive(fd, buf, buf_len, num_bytes_received);
    if (ErrorCode_Success != error_code) {
        throw SocketOperationFailed(error_code, __FILENAME__, __LINE__);
    }
}
}  // namespace clp::networking
