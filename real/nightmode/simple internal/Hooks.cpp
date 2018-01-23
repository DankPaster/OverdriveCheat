#include "Hooks.h"
#include "ImGUI\imgui.h"
#include "ImGUI\imgui_internal.h"
#include "ImGUI\imgui_impl_dx9.h"

extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Hooks
{
	HINSTANCE hThisModule;

	void FreeMemory()
	{
		FreeLibraryAndExitThread(hThisModule, NULL);
	}

	std::uint8_t* FindPattern(const char* modulename, const char* signature)
	{
		void* module = GetModuleHandle(modulename);

		static auto pattern_to_byte = [](const char* pattern) {
			auto bytes = std::vector<int>{};
			auto start = const_cast<char*>(pattern);
			auto end = const_cast<char*>(pattern) + strlen(pattern);

			for (auto current = start; current < end; ++current) {
				if (*current == '?') {
					++current;
					if (*current == '?')
						++current;
					bytes.push_back(-1);
				}
				else {
					bytes.push_back(strtoul(current, &current, 16));
				}
			}
			return bytes;
		};

		auto dosHeader = (PIMAGE_DOS_HEADER)module;
		auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

		auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
		auto patternBytes = pattern_to_byte(signature);
		auto scanBytes = reinterpret_cast<std::uint8_t*>(module);

		auto s = patternBytes.size();
		auto d = patternBytes.data();

		for (auto i = 0ul; i < sizeOfImage - s; ++i) {
			bool found = true;
			for (auto j = 0ul; j < s; ++j) {
				if (scanBytes[i + j] != d[j] && d[j] != -1) {
					found = false;
					break;
				}
			}
			if (found) {
				return &scanBytes[i];
			}
		}
		return nullptr;
	}

	void SetSky(const char* name)
	{
		static auto LoadNamedSky_Function = (void(__fastcall*)(const char*))(FindPattern("engine.dll", "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45"));
		LoadNamedSky_Function(name);
	}

	using DrawModelExecute = void(__thiscall*)(IVModelRender*, IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t*);
	using EndScene = long(__stdcall*)(IDirect3DDevice9*);
	using Reset = long(__stdcall*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
	DrawModelExecute oDrawModelExecute;
	EndScene oEndScene = nullptr;
	Reset oReset = nullptr;

	IDirect3DDevice9* g_D3DDevice9 = nullptr;
	IVModelRender* g_MdlRender = nullptr;
	IMaterialSystem* g_MatSystem = nullptr;
	ICVar* g_CVar = nullptr;

	CreateInterfaceFn get_module_factory(HMODULE module)
	{
		return reinterpret_cast<CreateInterfaceFn>(GetProcAddress(module, "CreateInterface"));
	}

	template<typename T>
	T* get_interface(CreateInterfaceFn f, const char* szInterfaceVersion)
	{
		auto result = reinterpret_cast<T*>(f(szInterfaceVersion, nullptr));

		if (!result) {
			throw std::runtime_error(std::string("[get_interface] Failed to GetOffset interface: ") + szInterfaceVersion);
		}

		return result;
	}

	typedef float color[3];
	static color ModelModulation = { 1.0f, 1.0f, 1.0f };
	static color WorldModulation = { 1.0f, 1.0f, 1.0f };
	static color StaticPropModulation = { 1.0f, 1.0f, 1.0f };

	static bool apply = false;
	static bool nightsky = false;
	void __stdcall hkDrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
	{
		oDrawModelExecute(g_MdlRender, ctx, state, pInfo, pCustomBoneToWorld);

		if (apply)
		{
			// Setting skybox
			if (nightsky) SetSky("sky_csgo_cloudy01");
			else SetSky("sky_dust");

			static ConVar* r_drawspecificstaticprop = g_CVar->FindVar("r_drawspecificstaticprop");
			r_drawspecificstaticprop->SetValue(false);

			// Doing nightmode
			for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
			{
				IMaterial* pMaterial = g_MatSystem->GetMaterial(i);

				if (!pMaterial)
					continue;

				if (strstr(pMaterial->GetTextureGroupName(), "Model")) 
				{
					pMaterial->ColorModulate(ModelModulation[0], ModelModulation[1], ModelModulation[2]);
				}

				if (strstr(pMaterial->GetTextureGroupName(), "World"))
				{
					pMaterial->SetMaterialVarFlag(MATERIAL_VAR_TRANSLUCENT, false);
					pMaterial->ColorModulate(WorldModulation[0], WorldModulation[1], WorldModulation[2]);
				}

				if (strstr(pMaterial->GetTextureGroupName(), "StaticProp"))
				{
					pMaterial->ColorModulate(StaticPropModulation[0], StaticPropModulation[1], StaticPropModulation[2]);
				}
			}
			apply = false;
		}

		g_MdlRender->ForcedMaterialOverride(nullptr);
	}

	static bool unload = false;
	WNDPROC oWndProc;
	static bool Initialized = false;
	static bool toggle = false;
	bool vecPressedKeys[256] = {};
	LRESULT __stdcall Hooked_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (!unload)
		{
			switch (uMsg)
			{
			case WM_LBUTTONDOWN:
				vecPressedKeys[VK_LBUTTON] = true;
				break;
			case WM_LBUTTONUP:
				vecPressedKeys[VK_LBUTTON] = false;
				break;
			case WM_RBUTTONDOWN:
				vecPressedKeys[VK_RBUTTON] = true;
				break;
			case WM_RBUTTONUP:
				vecPressedKeys[VK_RBUTTON] = false;
				break;
			case WM_KEYDOWN:
				vecPressedKeys[wParam] = true;
				break;
			case WM_KEYUP:
				vecPressedKeys[wParam] = false;
				break;
			default: break;
			}

			static bool isDown = false;
			static bool isClicked = false;
			if (vecPressedKeys[VK_INSERT])
			{
				isClicked = false;
				isDown = true;
			}
			else if (!vecPressedKeys[VK_INSERT] && isDown)
			{
				isClicked = true;
				isDown = false;
			}
			else
			{
				isClicked = false;
				isDown = false;
			}

			if (isClicked)
			{
				toggle = !toggle;
				static auto cl_mouseenable = g_CVar->FindVar("cl_mouseenable");
				cl_mouseenable->SetValue(!toggle);
			}

			if (Initialized && toggle && ImGui_ImplDX9_WndProcHandler(hWnd, uMsg, wParam, lParam))
				return true;

			return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
		}
	}

	ImGuiStyle& style = ImGui::GetStyle();
	void Theme_Grey()
	{
		style.WindowMinSize = ImVec2(0, 0);
		style.FramePadding = ImVec2(1, 1);
		style.ItemSpacing = ImVec2(4, 4);
		style.ItemInnerSpacing = ImVec2(6, 3);
		style.Alpha = 1.0f;
		style.WindowRounding = 0.00f;
		style.FrameRounding = 1.25f;
		style.IndentSpacing = 6.0f;
		style.ItemInnerSpacing = ImVec2(1, 1);
		style.ColumnsMinSpacing = 50.0f;
		style.GrabMinSize = 5.00f;
		style.GrabRounding = 100.0f;
		style.ScrollbarSize = 12.0f;
		style.ScrollbarRounding = 16.0f;
		style.AntiAliasedLines = true;
		style.AntiAliasedShapes = true;
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

		style.Colors[ImGuiCol_Text] = ImVec4(0.725f, 0.725f, 0.725f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.725f, 0.725f, 0.725f, 1.00f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.188f, 0.188f, 0.188f, 1.f);
		style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.188f, 0.188f, 0.188f, 1.f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.188f, 0.188f, 0.188f, 1.f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.05f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.05f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.228f, 0.228f, 0.228f, 1.f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.502f, 0.796f, 0.769f, 0.10f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.502f, 0.796f, 0.769f, 0.10f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.216f, 0.278f, 0.31f, 1.f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.216f, 0.278f, 0.31f, 1.f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.216f, 0.278f, 0.31f, 1.f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.188f, 0.188f, 0.188f, 1.f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.188f, 0.188f, 0.188f, 1.f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.725f, 0.725f, 0.725f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.502f, 0.796f, 0.769f, 0.10f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.502f, 0.796f, 0.769f, 0.10f);
		style.Colors[ImGuiCol_ComboBg] = ImVec4(0.228f, 0.228f, 0.228f, 1.f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.216f, 0.278f, 0.31f, 1.f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.725f, 0.725f, 0.725f, 1.00f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.725f, 0.725f, 0.725f, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.228f, 0.228f, 0.228f, 1.f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.502f, 0.796f, 0.769f, 0.10f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.502f, 0.796f, 0.769f, 0.10f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.228f, 0.228f, 0.228f, 1.f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.502f, 0.796f, 0.769f, 0.10f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.502f, 0.796f, 0.769f, 0.10f);
		style.Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
		style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
		style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.502f, 0.796f, 0.769f, 0.10f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.228f, 0.228f, 0.228f, 1.f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.502f, 0.796f, 0.769f, 0.10f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.502f, 0.796f, 0.769f, 0.10f);
		style.Colors[ImGuiCol_CloseButton] = ImVec4(0.82f, 0.82f, 0.82f, 0.00f);
		style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.82f, 0.82f, 0.82f, 0.00f);
		style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.502f, 0.796f, 0.769f, 0.00f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.07f);
	}

	void InitializeGUI(IDirect3DDevice9 * device)
	{
		HWND hWnd = FindWindowA("Valve001", 0);
		ImGui_ImplDX9_Init(hWnd, device);
		ImGuiIO& io = ImGui::GetIO();
		Theme_Grey();
		ImGui_ImplDX9_CreateDeviceObjects();
		oWndProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)Hooked_WndProc);
	}

	long __stdcall hkEndScene(IDirect3DDevice9* device)
	{
		if (!unload)
		{
			if (!Initialized)
			{
				InitializeGUI(device);
				Initialized = true;
			}
			if (toggle)
			{
				ImGui::GetIO().MouseDrawCursor = toggle;
				ImGui_ImplDX9_NewFrame();
				ImGui::Begin("Night Mode | Made By s1lenT", &toggle, ImVec2(-1, -1), 1.00f, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_AlwaysAutoResize);
				{
					ImGui::ColorEdit3("Model Color Modulation", ModelModulation, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
					ImGui::ColorEdit3("World Color Modulation", WorldModulation, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
					ImGui::ColorEdit3("Static Prop Color Modulation", StaticPropModulation, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
					ImGui::Checkbox("Night Sky", &nightsky);
					if (ImGui::Button("Apply")) apply = true;
					ImGui::SameLine();
					if (ImGui::Button("Reset"))
					{
						ModelModulation[0] = 1.0f;
						ModelModulation[1] = 1.0f;
						ModelModulation[2] = 1.0f;
						ModelModulation[3] = 1.0f;

						WorldModulation[0] = 1.0f;
						WorldModulation[1] = 1.0f;
						WorldModulation[2] = 1.0f;
						WorldModulation[3] = 1.0f;

						StaticPropModulation[0] = 1.0f;
						StaticPropModulation[1] = 1.0f;
						StaticPropModulation[2] = 1.0f;
						StaticPropModulation[3] = 1.0f;

						nightsky = false;

						apply = true;
					}
					/*ImGui::SameLine();
					if (ImGui::Button("Unload"))
					{
						ModelModulation[0] = 1.0f;
						ModelModulation[1] = 1.0f;
						ModelModulation[2] = 1.0f;
						ModelModulation[3] = 1.0f;

						WorldModulation[0] = 1.0f;
						WorldModulation[1] = 1.0f;
						WorldModulation[2] = 1.0f;
						WorldModulation[3] = 1.0f;

						StaticPropModulation[0] = 1.0f;
						StaticPropModulation[1] = 1.0f;
						StaticPropModulation[2] = 1.0f;
						StaticPropModulation[3] = 1.0f;

						nightsky = false;

						apply = true;

						unload = true;
					}*/
				}
				ImGui::End();
				ImGui::Render();
			}
		}
		return oEndScene(device);
	}

	// D3D Reset Hook
	long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		if (!Initialized) return oReset(device, pPresentationParameters);
		ImGui_ImplDX9_InvalidateDeviceObjects();
		auto hr = oReset(device, pPresentationParameters);
		ImGui_ImplDX9_CreateDeviceObjects();
		return hr;
	}

	void Hook()
	{
		auto engineFactory = get_module_factory(GetModuleHandle("engine.dll"));
		auto clientFactory = get_module_factory(GetModuleHandle("client.dll"));
		auto valveStdFactory = get_module_factory(GetModuleHandle("vstdlib.dll"));
		auto vguiFactory = get_module_factory(GetModuleHandle("vguimatsurface.dll"));
		auto vgui2Factory = get_module_factory(GetModuleHandle("vgui2.dll"));
		auto matSysFactory = get_module_factory(GetModuleHandle("materialsystem.dll"));
		auto dataCacheFactory = get_module_factory(GetModuleHandle("datacache.dll"));
		auto vphysicsFactory = get_module_factory(GetModuleHandle("vphysics.dll"));

		g_D3DDevice9 = **(IDirect3DDevice9***)(FindPattern("shaderapidx9.dll", "A1 ? ? ? ? 50 8B 08 FF 51 0C") + 1);
		g_MdlRender = get_interface<IVModelRender>(engineFactory, "VEngineModel016");
		g_MatSystem = get_interface<IMaterialSystem>(matSysFactory, "VMaterialSystem080");
		g_CVar = get_interface<ICVar>(valveStdFactory, "VEngineCvar007");

		CVMTHookManager* modelhook = new CVMTHookManager(g_MdlRender);
		CVMTHookManager* direct3d_hook = new CVMTHookManager(g_D3DDevice9);

		oDrawModelExecute = (DrawModelExecute)modelhook->HookFunction(21, hkDrawModelExecute);
		oEndScene = (EndScene)direct3d_hook->HookFunction(42, hkEndScene);
		oReset = (Reset)direct3d_hook->HookFunction(16, hkReset);

		modelhook->HookTable(true);		
		direct3d_hook->HookTable(true);

		while (!unload) Sleep(1000);

		ImGui_ImplDX9_Shutdown();

		static auto cl_mouseenable = g_CVar->FindVar("cl_mouseenable");
		cl_mouseenable->SetValue(false);

		modelhook->HookTable(false);
		direct3d_hook->HookTable(false);

		Sleep(2000);

		CreateRemoteThread(NULL, NULL, NULL, (LPTHREAD_START_ROUTINE)FreeMemory, NULL, NULL, NULL);
	}
}