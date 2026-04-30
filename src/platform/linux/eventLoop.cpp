#include "eventLoop.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <iostream>

class EventLoop::pImpl
{
private:
    int err;

public:
    pImpl() = default;
    ~pImpl() = default;

    void registerClient(std::shared_ptr<ClientSession> client)
    {
        std::cout << "client registration called, not responding now.. :( \n";

        char buffer[1024] = {};
        int bytesReceived = recv(client->socket.native_handle(), buffer, sizeof(buffer), 0);

        if (bytesReceived < 0)
        {
            err = errno;
            std::cerr << "Recv failed: "
                      << strerror(err)
                      << " (" << err << ")\n";

            // close(clientSocket);
            // continue;
        }

        if (bytesReceived == 0)
        {
            std::cout << "Client disconnected\n";
            // close(clientSocket);
            // continue;
        }

        std::cout << "Client says: " << buffer << "\n";

        const char *msg = "Hello client, greetings received.";

        int sent = send(client->socket.native_handle(), msg, strlen(msg), 0);
        if (sent < 0)
        {
            err = errno;
            std::cerr << "Send failed: "
                      << strerror(err)
                      << " (" << err << ")\n";
        }
    }
};

EventLoop::EventLoop() : pimpl(std::make_unique<pImpl>()) {};

EventLoop::~EventLoop() = default;

void EventLoop::registerClient(std::shared_ptr<ClientSession> client)
{
    pimpl->registerClient(client);
}