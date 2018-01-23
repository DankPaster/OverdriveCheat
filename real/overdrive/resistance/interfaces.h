#pragma once
#include "utils.h"
#include "sdk\cvar.h"
#include "sdk\sdk.h"

struct IDirect3DDevice9;

//IPrediction* g_Prediction = nullptr;
CEngine *g_EngineClient = nullptr;
CClient *g_CHLClient = nullptr;
CInput *g_Input = nullptr;
CClientEntityList *g_EntityList = nullptr;
CModelInfo *g_MdlInfo = nullptr;
CPanel* g_VGuiPanel = nullptr;
ISurface* g_VGuiSurface = nullptr;
CTrace* g_EngineTrace = nullptr;
CGlobalVarsBase* g_GlobalVars = nullptr;
IDirect3DDevice9* g_D3DDevice9 = nullptr;
IGameEventManager2* g_GameEvents = nullptr;
//CGlowObjectManager*   g_GlowObjManager = nullptr;
ICVar* g_CVar;
C_LocalPlayer g_LocalPlayer;

template<typename... Args>
void ConMsg(const char* pMsg, Args... args)
{
	static auto import = (void(*)(const char*, ...))GetProcAddress(GetModuleHandleW(L"tier0.dll"), "?ConMsg@@YAXPBDZZ");
	return import(pMsg, args...);
}

template<typename... Args>
void ConColorMsg(const Color& clr, const char* pMsg, Args... args)
{
	static auto import = (void(*)(const Color&, const char*, ...))GetProcAddress(GetModuleHandleW(L"tier0.dll"), "?ConColorMsg@@YAXABVColor@@PBDZZ");
	return import(clr, pMsg, args...);
}

class CInterfaces
{
public:

	CreateInterfaceFn get_module_factory(HMODULE module)
	{
		return reinterpret_cast<CreateInterfaceFn>(GetProcAddress(module, "CreateInterface"));
	}

	template<typename T>
	T* get_interface(CreateInterfaceFn f, const char* szInterfaceVersion)
	{
		auto result = reinterpret_cast<T*>(f(szInterfaceVersion, nullptr));

		if (!result) throw std::runtime_error(std::string("Failed To Get The Offset For Interface: ") + szInterfaceVersion);

		return result;
	}

	void GetInterfaces()
	{
		auto engineFactory = get_module_factory(GetModuleHandleW(L"engine.dll"));
		auto clientFactory = get_module_factory(GetModuleHandleW(L"client.dll"));
		auto valveStdFactory = get_module_factory(GetModuleHandleW(L"vstdlib.dll"));
		auto vguiFactory = get_module_factory(GetModuleHandleW(L"vguimatsurface.dll"));
		auto vgui2Factory = get_module_factory(GetModuleHandleW(L"vgui2.dll"));
		auto matSysFactory = get_module_factory(GetModuleHandleW(L"materialsystem.dll"));
		auto dataCacheFactory = get_module_factory(GetModuleHandleW(L"datacache.dll"));
		auto vphysicsFactory = get_module_factory(GetModuleHandleW(L"vphysics.dll"));

		//g_Prediction = get_interface<IPrediction>(clientFactory, "VClientPrediction001");
		g_EngineClient = get_interface<CEngine>(engineFactory, "VEngineClient014");
		g_EngineTrace = get_interface<CTrace>(engineFactory, "EngineTraceClient004");
		g_CHLClient = get_interface<CClient>(clientFactory, "VClient018");
		g_EntityList = get_interface<CClientEntityList>(clientFactory, "VClientEntityList003");
		g_MdlInfo = get_interface<CModelInfo>(engineFactory, "VModelInfoClient004");
		g_VGuiPanel = get_interface<CPanel>(vgui2Factory, "VGUI_Panel009");
		g_VGuiSurface = get_interface<ISurface>(vguiFactory, "VGUI_Surface031");
		g_GameEvents = get_interface<IGameEventManager2>(engineFactory, "GAMEEVENTSMANAGER002");
		g_CVar = get_interface<ICVar>(valveStdFactory, "VEngineCvar007");
		g_Input = *(CInput**)(pUtils::PatternScan(GetModuleHandleW(L"client.dll"), "B9 ? ? ? ? 8B 40 38 FF D0 84 C0 0F 85") + 1);
		g_D3DDevice9 = **(IDirect3DDevice9***)(pUtils::PatternScan(GetModuleHandleW(L"shaderapidx9.dll"), "A1 ? ? ? ? 50 8B 08 FF 51 0C") + 1);
		g_LocalPlayer = *(C_LocalPlayer*)(pUtils::PatternScan(GetModuleHandleW(L"client.dll"), "8B 0D ? ? ? ? 83 FF FF 74 07") + 2);
		g_GlobalVars = **(CGlobalVarsBase***)(pUtils::PatternScan(GetModuleHandleW(L"client.dll"), "A1 ? ? ? ? 5E 8B 40 10") + 1);
		//g_GlowObjManager = *(CGlowObjectManager**)(pUtils::PatternScan(GetModuleHandleW(L"client.dll"), "0F 11 05 ? ? ? ? 83 C8 01") + 3);

		printf("DEBUG INFORMATION :");
		printf("\n");
		printf("_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-\n");
		printf("\n");
		printf("Input : 0x%X\n", (uintptr_t)g_Input);
		printf("Direct3D : 0x%X\n", (uintptr_t)g_D3DDevice9);
		printf("LocalPlayer : 0x%X\n", (uintptr_t)g_LocalPlayer);
		printf("Globals : 0x%X\n", (uintptr_t)g_GlobalVars);
		//printf("GlowObjectManager : 0x%X\n", (uintptr_t)g_GlowObjManager);
		printf("\n");
		printf("_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-\n");
		printf("\n");
		//printf("VClientPrediction001 : %s\n", (uintptr_t)g_Prediction);
		printf("VEngineClient014 : 0x%X\n", (uintptr_t)g_EngineClient);
		printf("EngineTraceClient004 : 0x%X\n", (uintptr_t)g_EngineTrace);
		printf("VClient018 : 0x%X\n", (uintptr_t)g_CHLClient);
		printf("VClientEntityList003 : 0x%X\n", (uintptr_t)g_EntityList);
		printf("VModelInfoClient004 : 0x%X\n", (uintptr_t)g_MdlInfo);
		printf("VGUI_Panel009 : 0x%X\n", (uintptr_t)g_VGuiPanel);
		printf("VGUI_Surface031 : 0x%X\n", (uintptr_t)g_VGuiSurface);
		printf("GAMEEVENTSMANAGER002 : 0x%X\n", (uintptr_t)g_GameEvents);
		printf("VEngineCvar007 : 0x%X\n", (uintptr_t)g_CVar);
	}
}; 

CInterfaces Interfaces;