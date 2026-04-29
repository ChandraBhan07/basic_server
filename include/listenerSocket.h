#pragma once
#include "socket.h"

#include <iostream>
class ListenerSocket
{
public:
    void start(unsigned short port)
    {
        std::cout << "server is starting up for port " << port << "\n";
        socket.bind(port);
        socket.listen(100); // SOMAXCONN
        std::cout << "server listens at " << port << "\n";
    }

    Socket accept()
    {
        return socket.accept();
    }

private:
    Socket socket;
};