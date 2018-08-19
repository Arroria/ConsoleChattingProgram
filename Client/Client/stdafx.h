#pragma once
#include <iostream>
using std::cin;
using std::cout;
using std::wcout;
using std::endl;

#include <mutex>
extern std::mutex g_coutMutex;


void gotoxy(int x, int y);

#include "../../arSocket.h"
#include "../../arJSON.h"
