#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <cstdlib>
#include <cstdint>
#include "Hooks.h"

void Initialize()
{
	Hooks::Hook();
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, uintptr_t fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH: 
		Hooks::hThisModule = hinstDLL;
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Initialize, NULL, NULL, NULL); 
		break;
	case DLL_PROCESS_DETACH: 
		break;
	}

	return true;
}