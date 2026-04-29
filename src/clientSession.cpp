#include "clientSession.h"

ClientSession::ClientSession(Socket s, const uuid &id_) : socket(std::move(s)),
                                                          id(id_),
                                                          createdAt(std::chrono::system_clock::now()),
                                                          lastActivity(std::chrono::steady_clock::now()),
                                                          errorCount(0),
                                                          state(SessionState::Handshake),
                                                          type(ClientType::Unknown)
{
}