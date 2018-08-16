#pragma once
#include <string>
class UserCommand
{
public:
	UserCommand(const std::string& command);

public:
	std::string NextToken();

private:
	std::string m_command;
	std::string::const_iterator m_cmdIter;
};

