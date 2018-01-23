#pragma once
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <fstream>
#include "sdk.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

extern IVEngineClient* g_EngineClient;
extern IBaseClientDLL* g_CHLClient;
extern CGlobalVarsBase* g_GlobalVars;
extern IPanel* g_VGuiPanel;
extern ISurface* g_VGuiSurface;
extern IVModelRender* g_MdlRender;
extern IMaterialSystem* g_MatSystem;
extern CInput* g_Input;
extern IClientEntityList* g_EntityList;
extern IMDLCache* g_MdlCache;
extern C_LocalPlayer g_LocalPlayer;
extern IVEngineTrace* g_EngineTrace;
extern IClientMode* g_ClientMode;
extern IVModelInfoClient* g_MdlInfo;
extern IVDebugOverlay* g_DebugOverlay;
extern IDirect3DDevice9* g_D3DDevice9;
extern ICVar* g_CVar;
extern struct IDirect3DDevice9;

extern WNDPROC oWndProc;

namespace Hooks
{
	using PaintTraverse = void(__thiscall*)(IPanel*, vgui::VPANEL, bool, bool);
	using CreateMove = void(__thiscall*)(IBaseClientDLL*, int, float, bool);
	using DrawModelExecute = void(__thiscall*)(IVModelRender*, IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t*);
	using PlaySound = void(__thiscall*)(ISurface*, const char* name);
	using PaintTraverse = void(__thiscall*)(IPanel*, vgui::VPANEL, bool, bool);
	using OverrideView = void(__thiscall*)(IClientMode*, CViewSetup*);
	using FrameStageNotify = void(__thiscall*)(IBaseClientDLL*, ClientFrameStage_t);
	using EndScene = long(__stdcall *)(IDirect3DDevice9*);
	using Reset = long(__stdcall *)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
	using Present = long(__stdcall*)(IDirect3DDevice9*, RECT*, RECT*, HWND, RGNDATA*);

	extern CreateMove oCreateMove;
	extern PlaySound oPlaySound;
	extern DrawModelExecute oDrawModelExecute;
	extern PaintTraverse oPaintTraverse;
	extern OverrideView oOverrideView;
	extern FrameStageNotify oFrameStageNotify;
	extern EndScene oEndScene;
	extern Reset oReset;
	extern Present oPresent;

	CreateInterfaceFn get_module_factory(HMODULE module);
	template<typename T>
	T* get_interface(CreateInterfaceFn f, const char* szInterfaceVersion);

	void __stdcall hkCreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket);
	void __stdcall hkPlaySound(const char* name);
	void __stdcall hkDrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld);
	void __stdcall hkPaintTraverse(vgui::VPANEL panel, bool forceRepaint, bool allowForce);
	void __stdcall hkOverrideView(CViewSetup* setup);
	void __stdcall hkFrameStageNotify(ClientFrameStage_t stage);
	long __stdcall hkEndScene(IDirect3DDevice9* device);
	long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters);
	long __stdcall hkPresent(IDirect3DDevice9* device, RECT* src, RECT* dest, HWND wnd_override, RGNDATA* dirty_region);

	void HookFunctions();
}