#pragma once
#include <iostream>
using std::cout;
using std::wcout;
using std::endl;

#include <mutex>
using RegionLocker = std::lock_guard<std::mutex>;


#include "../../arSocket.h"
#include "../../arJSON.h"
#include "../../JSON_Based_Socket.h"
#include "../../AsyncConnector.h"


extern std::mutex g_coutMutex;

AsyncConnector* ClientRegister(SOCKET clientSocket, const sockaddr_in& clientAddress);
void ClientDisconnect(AsyncConnector* client);
