#pragma once
#include <string>
class CmdTokener
{
public:
	CmdTokener(const std::string& command);

public:
	std::string NextToken();

private:
	std::string m_command;
	std::string::const_iterator m_cmdIter;
};

