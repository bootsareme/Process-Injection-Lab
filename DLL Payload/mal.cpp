#include "mal.h"

// replace this from what you discovered in debugger
constexpr static uintptr_t PLAYER_BALANCE_OFFSET = 0x60F0;

DWORD WINAPI MaliciousThread(LPVOID) 
{
	uintptr_t module_base = reinterpret_cast<uintptr_t>(GetModuleHandle(NULL));
	int* player_balance = reinterpret_cast<int*>(module_base + PLAYER_BALANCE_OFFSET);

	MessageBoxA(
		nullptr,
		"Don't worry, I gotchu! Take some bands...",
		"EliteHaxxor",
		MB_OK | MB_ICONEXCLAMATION
	);

	*player_balance = 999999;
	return 0;
}