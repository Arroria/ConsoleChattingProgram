#pragma once
#include <iostream>
using std::cout;
using std::wcout;
using std::endl;

#include <mutex>
extern std::mutex g_coutMutex;

#include "../../arSocket.h"
