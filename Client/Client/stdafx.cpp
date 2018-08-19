#include "stdafx.h"

std::mutex g_coutMutex;

#include <Windows.h>
void gotoxy(int x, int y)
{
	COORD Pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}