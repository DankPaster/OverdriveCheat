#include "hooks.h"
#include "poly.h"

void Initialize()
{
	SetConsoleTitle("OVERDRIVE DEBUG CONSOLE");

	// Set console color if the console is allowed to be shown | WARNING: THIS IS A VERY GHETTO WAY TO DO THIS! YOU ARE ADVISED TO LEAVE NOW WHILST YOU STILL CAN
	system("color a");
	// CANT SIG THIS VOLVO

	// Hooking game functions
	g_Hooks.Hook();
	
	// Clearing the console from bullshit CSGO spews out when it starts up
	g_EngineClient->ClientCmd("clear");

	// CANT SIG THIS VOLVO
}

BOOL WINAPI DllMain(HINSTANCE Instance, DWORD Reason, LPVOID Reserved)
{
	switch (Reason)
	{
	case DLL_PROCESS_ATTACH:
		Interfaces.GetInterfaces();
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Initialize, NULL, NULL, NULL);
		break;
	case DLL_PROCESS_DETACH:
		exit(0);
		break;
	};

	return true;
}