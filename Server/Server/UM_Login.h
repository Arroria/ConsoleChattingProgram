#pragma once
class UM_Login
{
public:
	static bool Reciver(AsyncConnector& client, int recvResult, SocketBuffer& socketBuffer);

private:
	static bool Act_Register(AsyncConnector& user, const arJSON& jsonRoot);
	static bool Act_Login(AsyncConnector& user, const arJSON& jsonRoot);
	static bool Act_Quit(AsyncConnector& user);
};

#include <map>
#include <string>
#include "Singleton.h"
class TestIDDB
	: public Singleton<TestIDDB>
{
public:
	bool RegistID(const std::string& id, const std::string& password);
	bool Login(const std::string& id, const std::string& password);

private:
	std::map<std::string, std::string> m_id;
};
