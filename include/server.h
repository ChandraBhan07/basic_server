#include "listenerSocket.h"
#include "uuid.h"
#include "clientSession.h"
#include "eventLoop.h"
#include <unordered_map>

class Server
{
public:
    void run();
    Server();
    ~Server();

private:
    ListenerSocket listener;
    std::unordered_map<uuid, std::shared_ptr<ClientSession>, uuidHash> sessions;

    EventLoop loop;
    void accept_new_clients();
};