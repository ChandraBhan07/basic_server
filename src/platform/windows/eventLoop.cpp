#include "eventLoop.h"

#include <winsock2.h>
#include <windows.h>
#include <iostream>

struct Context
{
    OVERLAPPED overlapped{};
    std::weak_ptr<ClientSession> client;
    WSABUF buffer;
    char data[1024];
    int operation; // 0 = recv, 1 = send
};

DWORD WINAPI WorkerThread(LPVOID param)
{
    HANDLE iocp = (HANDLE)param;

    DWORD bytes;
    ULONG_PTR key;
    OVERLAPPED *ov;

    while (true)
    {
        BOOL result = GetQueuedCompletionStatus(
            iocp,
            &bytes,
            &key,
            &ov,
            INFINITE);

        SOCKET sock = (SOCKET)key;
        Context *ctx = (Context *)ov;

        if (ctx->operation == 0)
        { // RECV complete
            std::cout << "Received: " << std::string(ctx->data, bytes) << "\n";

            // prepare send
            ctx->operation = 1;
            ctx->buffer.len = bytes;

            if (std::shared_ptr<ClientSession> sp = ctx->client.lock())
            {
                WSASend(sp->socket.native_handle(), &ctx->buffer, 1, NULL, 0, &ctx->overlapped, NULL);
            }
        }
    }
}

class EventLoop::pImpl
{
private:
    HANDLE iocp;

public:
    pImpl()
    {
        iocp = CreateIoCompletionPort(
            INVALID_HANDLE_VALUE,
            NULL,
            0,
            0);
        std::cout << "empty mail box ready.\n";

        // Create worker thread
        CreateThread(NULL, 0, WorkerThread, iocp, 0, NULL);
        std::cout << "worker thread created \n";
    }

    void registerClient(std::shared_ptr<ClientSession> client)
    {

        CreateIoCompletionPort(
            (HANDLE)client->socket.native_handle(),
            iocp,
            (ULONG_PTR)client->socket.native_handle(),
            0);

        // Create context
        Context *ctx = new Context{};
        ctx->operation = 0;
        ctx->buffer.buf = ctx->data;
        ctx->buffer.len = sizeof(ctx->data);
        ctx->client = client;

        DWORD flags = 0;

        // Start first recv (IMPORTANT)
        WSARecv(client->socket.native_handle(), &ctx->buffer, 1, NULL, &flags, &ctx->overlapped, NULL);
    }

    ~pImpl() = default;
};

EventLoop::EventLoop() : pimpl(std::make_unique<pImpl>()) {};

EventLoop::~EventLoop() = default;

void EventLoop::registerClient(std::shared_ptr<ClientSession> client)
{
    pimpl->registerClient(client);
}