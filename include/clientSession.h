#pragma once

#include "socket.h"
#include "uuid.h"

#include <string>
#include <vector>
#include <queue>
#include <chrono>

enum class ClientType
{
    Unknown,
    Chat,
    Telemetry,
    File
};

enum class SessionState
{
    Handshake,
    Connected,
    Active,
    Closed
};

class ClientSession
{
public:
    uuid id;
    Socket socket;
    ClientType type;
    SessionState state;

    std::vector<char> readBuffer;
    std::queue<std::string> writeQueue;

    std::chrono::system_clock::time_point createdAt;
    std::chrono::steady_clock::time_point lastActivity;

    int errorCount;

    ClientSession(Socket s, const uuid &id_);

    // for adding authentication logic later
};
