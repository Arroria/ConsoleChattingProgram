#include "stdafx.h"

std::mutex g_coutMutex;

#include <set>
std::set<AsyncConnector*> g_clientList;
std::mutex g_clientListMutex;

AsyncConnector* ClientRegister(SOCKET clientSocket, const sockaddr_in& clientAddress)
{
	AsyncConnector* client = new AsyncConnector(clientSocket, clientAddress);
	{
		RegionLocker locker(g_clientListMutex);
		g_clientList.insert(client);
	}
	return client;
}

void ClientDisconnect(AsyncConnector * client)
{
	{
		RegionLocker locker(g_clientListMutex);
		g_clientList.erase(client);
	}
	delete client;
}
