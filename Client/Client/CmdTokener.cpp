#include "stdafx.h"
#include "CmdTokener.h"


CmdTokener::CmdTokener(const std::string & command)
	: m_command(command)
	, m_cmdIter(m_command.begin())
{
}



std::string CmdTokener::NextToken()
{
	std::string token;
	while (m_cmdIter != m_command.end())
	{
		char data = *(m_cmdIter++);
		if (data <= 32)
		{
			if (token.size())
				break;
			continue;
		}
		token += data;
	}
	return token;
}
