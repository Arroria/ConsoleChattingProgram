#pragma once
#include <iostream>
using std::cin;
using std::cout;
using std::wcout;
using std::endl;

#include <mutex>
using RegionLocker = std::lock_guard<std::mutex>;


void gotoxy(int x, int y);

#include "../../arSocket.h"
#include "../../arJSON.h"
#include "../../JSON_Based_Socket.h"
#include "../../AsyncConnector.h"
#include "../../SocketReciver.h"

extern std::mutex g_coutMutex;

#include "Singleton.h"
