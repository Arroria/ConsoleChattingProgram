#pragma once
#include "../../arSocket.h"
#include "../../RecvLoopManagementClass.h"

struct User
{
	SOCKET socket;
	sockaddr_in address;
	RecvLoopManagementClass recvMng;

	bool operator==(const User& ant) const
	{
		return
			//	address.sin_port == ant.address.sin_port &&
			//	memcmp(&address.sin_addr, &ant.address.sin_addr, sizeof(in_addr));
			socket == ant.socket;
	}

	inline bool operator<(const User& ant) const { return socket < ant.socket; }
};
