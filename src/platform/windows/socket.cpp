#include "socket.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdexcept>
#include <string>

namespace
{
    class WSAContext
    {
    public:
        WSAContext()
        {
            WSADATA data;
            if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
                throw std::runtime_error("WSAStartup failed");
        }

        ~WSAContext()
        {
            WSACleanup();
        }

        WSAContext(const WSAContext &) = delete;
        WSAContext &operator=(const WSAContext &) = delete;
    };

    inline void ensure_wsa()
    {
        static WSAContext ctx;
        (void)ctx;
    }
}

class Socket::pImpl
{
public:
    SOCKET s;

    explicit pImpl(bool create_socket)
        : s(INVALID_SOCKET)
    {
        ensure_wsa();

        if (create_socket)
        {
            s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (s == INVALID_SOCKET)
                throw std::runtime_error("socket creation failed");
        }
    }

    ~pImpl()
    {
        if (s != INVALID_SOCKET)
        {
            ::closesocket(s);
            s = INVALID_SOCKET;
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

// adopt existing native socket (used by accept) - later use case
Socket::Socket(native_handle_type handle)
    : pimpl(std::make_unique<pImpl>(false))
{
    pimpl->s = static_cast<SOCKET>(handle);
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

    if (::bind(pimpl->s,
               reinterpret_cast<sockaddr *>(&addr),
               sizeof(addr)) == SOCKET_ERROR)
    {
        throw std::runtime_error(
            "bind failed: " + std::to_string(WSAGetLastError()));
    }
}

void Socket::listen(int backlog)
{
    if (::listen(pimpl->s, backlog) == SOCKET_ERROR)
    {
        throw std::runtime_error(
            "listen failed: " + std::to_string(WSAGetLastError()));
    }
}

Socket Socket::accept()
{
    SOCKET client = ::accept(pimpl->s, nullptr, nullptr);

    if (client == INVALID_SOCKET)
        throw std::runtime_error("accept failed");

    return Socket(static_cast<native_handle_type>(client));
}

void Socket::connect(const char *ip, unsigned short port)
{
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (::inet_pton(AF_INET, ip, &addr.sin_addr) != 1)
        throw std::runtime_error("invalid IP address");

    if (::connect(pimpl->s,
                  reinterpret_cast<sockaddr *>(&addr),
                  sizeof(addr)) == SOCKET_ERROR)
    {
        throw std::runtime_error(
            "connect failed: " + std::to_string(WSAGetLastError()));
    }
}
