#pragma once
#include "Singleton.h"

#include "ServerUser.h"
#include <string>
#include <map>

#include <thread>
#include <mutex>

class UM_Login
	: public Singleton<UM_Login>
{
public:
	UM_Login();
	~UM_Login();

public:
	bool Join(const User& user);
	bool Quit(const User& user);

private:
	static void RecvLoop(User user);

private:
	std::map<User, std::thread> m_user;
	std::mutex m_userMutex;
};

class TestIDDB
	: public Singleton<TestIDDB>
{
public:
	bool RegistID(const std::string& id, const std::string& password);
	bool Login(const std::string& id, const std::string& password);

private:
	std::map<std::string, std::string> m_id;
};