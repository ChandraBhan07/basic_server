#pragma once

#include <memory>
#include <cstdint>

class Socket
{
public:
    using native_handle_type = std::uintptr_t;

    Socket();
    ~Socket();

    Socket(Socket &&) noexcept;
    Socket &operator=(Socket &&) noexcept;

    Socket(const Socket &) = delete;
    Socket &operator=(const Socket &) = delete;

    // ----- native interop (event loops etc.) -----
    native_handle_type native_handle() const noexcept;

private:
    class pImpl;
    std::unique_ptr<pImpl> pimpl;

    explicit Socket(native_handle_type native_handle);

    // ----- OS operations (platform abstracted) -----
    void bind(unsigned short port);
    void listen(int backlog);
    Socket accept();
    void connect(const char *ip, unsigned short port);

    friend class ListenerSocket;
};