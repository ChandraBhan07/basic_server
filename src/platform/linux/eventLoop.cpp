#include "eventLoop.h"

#include <iostream>

class EventLoop::pImpl
{
    public:
    pImpl() = default;
    ~pImpl() = default;

    void registerClient(std::shared_ptr<ClientSession> client)
    {
        std::cout << "client registration called, not responding now.. :( \n";
    }
};



EventLoop::EventLoop() : pimpl(std::make_unique<pImpl>()) {};

EventLoop::~EventLoop() = default;

void EventLoop::registerClient(std::shared_ptr<ClientSession> client)
{
    pimpl->registerClient(client);
}