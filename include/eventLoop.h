#pragma once

#include "socket.h"
#include "uuid.h"
#include "clientSession.h"

#include <memory>

class EventLoop
{
public:
    void registerClient(std::shared_ptr<ClientSession> client);

    EventLoop();
    ~EventLoop();

private:
    class pImpl;
    std::unique_ptr<pImpl> pimpl;
};
