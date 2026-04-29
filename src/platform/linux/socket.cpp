#include "socket.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string>

namespace
{
    std::string get_error(const std::string& msg)
    {
        return msg + " (errno=" + std::to_string(errno) +
               "): " + std::strerror(errno);
    }
}

class Socket::pImpl
{
public:
    int s;

    explicit pImpl(bool create_socket)
        : s(-1)
    {
        if (create_socket)
        {
            s = ::socket(AF_INET, SOCK_STREAM, 0);
            if (s < 0)
                throw std::runtime_error(get_error("socket creation failed"));
        }
    }

    ~pImpl()
    {
        if (s >= 0)
        {
            ::close(s);
            s = -1;
        }
    }
};

Socket::Socket()
    : pimpl(std::make_unique<pImpl>(true))
{
}

Socket::~Socket() = default;

Socket::Socket(Socket &&) noexcept = default;

Socket &Socket::operator=(Socket &&) noexcept = default;

// adopt existing native socket
Socket::Socket(native_handle_type handle)
    : pimpl(std::make_unique<pImpl>(false))
{
    pimpl->s = static_cast<int>(handle);
}

Socket::native_handle_type Socket::native_handle() const noexcept
{
    return static_cast<native_handle_type>(pimpl->s);
}

void Socket::bind(unsigned short port)
{
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;
    if (setsockopt(pimpl->s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        throw std::runtime_error(get_error("setsockopt failed"));
    }

    if (::bind(pimpl->s,
               reinterpret_cast<sockaddr *>(&addr),
               sizeof(addr)) < 0)
    {
        throw std::runtime_error(get_error("bind failed"));
    }
}

void Socket::listen(int backlog)
{
    if (::listen(pimpl->s, backlog) < 0)
    {
        throw std::runtime_error(get_error("listen failed"));
    }
}

Socket Socket::accept()
{
    int client = ::accept(pimpl->s, nullptr, nullptr);

    if (client < 0)
        throw std::runtime_error(get_error("accept failed"));

    return Socket(static_cast<native_handle_type>(client));
}

void Socket::connect(const char *ip, unsigned short port)
{
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (::inet_pton(AF_INET, ip, &addr.sin_addr) <= 0)
        throw std::runtime_error(get_error("invalid IP address"));

    if (::connect(pimpl->s,
                  reinterpret_cast<sockaddr *>(&addr),
                  sizeof(addr)) < 0)
    {
        throw std::runtime_error(get_error("connect failed"));
    }
}