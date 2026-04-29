#include "server.h"
#include "socket.h"
#include <iostream>

Server::Server() = default;
Server::~Server() = default;

void Server::run()
{
    listener.start(5347);

    while (true)
    {
        accept_new_clients();
    }
}

void Server::accept_new_clients()
{
    uuid id;
    std::cout << "uuid created " << id.toString() << " \n";

    std::shared_ptr<ClientSession> session = std::make_shared<ClientSession>(listener.accept(), id);
    sessions.emplace(id, session);

    loop.registerClient(session);

    std::cout << "clients connected " << sessions.size();
}