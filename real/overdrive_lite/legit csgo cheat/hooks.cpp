#include "hooks.h"
#include "ImGUI\imgui.h"
#include "ImGUI\imgui_impl_dx9.h"

IVEngineClient* g_EngineClient = nullptr;
IBaseClientDLL* g_CHLClient = nullptr;
CGlobalVarsBase* g_GlobalVars = nullptr;
IPanel* g_VGuiPanel = nullptr;
ISurface* g_VGuiSurface = nullptr;
IVModelRender* g_MdlRender = nullptr;
IMaterialSystem* g_MatSystem = nullptr;
CInput* g_Input = nullptr;
IClientEntityList* g_EntityList = nullptr;
IMDLCache* g_MdlCache = nullptr;
IVEngineTrace* g_EngineTrace = nullptr;
IClientMode* g_ClientMode = nullptr;
IVModelInfoClient* g_MdlInfo = nullptr;
IVDebugOverlay* g_DebugOverlay = nullptr;
IDirect3DDevice9* g_D3DDevice9 = nullptr;
ICVar* g_CVar = nullptr;
C_LocalPlayer g_LocalPlayer;
struct IDirect3DDevice9;

namespace Options
{
	bool Bhop = true;
	bool Chams = true;
	bool ChamsVisibleOnly = false;
	bool Backtracking = true;
	bool BacktrackingESP = true;
	bool GrenadePrediction = false;
	bool Radar = false;
	bool StreamProofing = false;
	bool visible = false;
}

namespace Names
{
	std::string hackNames[] = { "Bhop", "Chams", "VO Chams", "Backtracking", "BacktrackingESP", "GrenadePred", "Radar", "StreamProof", "visible" };
}
bool Initialized = false;

WNDPROC oWndProc = nullptr;

#define M_PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679f

struct lbyRecords
{
	int tick_count;
	float lby;
	Vector headPosition;
};
struct backtrackData
{
	float simtime;
	Vector hitboxPos;
};

class BackTrack
{
	int latest_tick;
	bool IsTickValid(int tick);
	void UpdateRecord(int i);
public:
	lbyRecords records[64];
	bool RunLBYBackTrack(int i, CUserCmd* cmd, Vector& aimPoint);
	void Update();
	void legitBackTrack(CUserCmd* cmd);
};

extern backtrackData headPositions[64][12];

extern BackTrack* backtracking;

void BackTrack::Update()
{
	if (Options::Backtracking)
	{
		latest_tick = g_GlobalVars->tickcount;
		for (int i = 0; i < 64; i++)
		{
			UpdateRecord(i);
		}
	}
}

bool BackTrack::IsTickValid(int tick)
{
	int delta = latest_tick - tick;
	float deltaTime = delta * g_GlobalVars->interval_per_tick;
	return (fabs(deltaTime) <= 0.2f);
}

void BackTrack::UpdateRecord(int i)
{
	C_BasePlayer* pEntity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);
	if (pEntity && pEntity->IsAlive() && !pEntity->IsDormant())
	{
		float lby = pEntity->m_flLowerBodyYawTarget();
		if (lby != records[i].lby)
		{
			records[i].tick_count = latest_tick;
			records[i].lby = lby;
			records[i].headPosition = pEntity->GetBonePos(8);
		}
	}
	else
	{
		records[i].tick_count = 0;
	}
}

bool BackTrack::RunLBYBackTrack(int i, CUserCmd* cmd, Vector& aimPoint)
{
	if (IsTickValid(records[i].tick_count))
	{
		aimPoint = records[i].headPosition;
		cmd->tick_count = records[i].tick_count;
		return true;
	}
	return false;
}

inline Vector angle_vector(Vector meme)
{
	auto sy = sin(meme.y / 180.f * static_cast<float>(M_PI));
	auto cy = cos(meme.y / 180.f * static_cast<float>(M_PI));

	auto sp = sin(meme.x / 180.f * static_cast<float>(M_PI));
	auto cp = cos(meme.x / 180.f* static_cast<float>(M_PI));

	return Vector(cp*cy, cp*sy, -sp);
}

inline float distance_point_to_line(Vector Point, Vector LineOrigin, Vector Dir)
{
	auto PointDir = Point - LineOrigin;

	auto TempOffset = PointDir.Dot(Dir) / (Dir.x * Dir.x + Dir.y * Dir.y + Dir.z *Dir.z);
	if (TempOffset < 0.000001f)
		return FLT_MAX;

	Vector PerpendicularPoint;
	LineOrigin + (Dir * TempOffset);

	return (Point - PerpendicularPoint).Length();
}

#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / g_GlobalVars->interval_per_tick ) )

void BackTrack::legitBackTrack(CUserCmd* cmd)
{
	if (Options::Backtracking)
	{
		int bestTargetIndex = -1;
		float bestFov = FLT_MAX;
		player_info_t info;
		if (!g_LocalPlayer->IsAlive())
			return;

		for (int i = 0; i < g_EngineClient->GetMaxClients(); i++)
		{
			C_BasePlayer* entity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);

			if (!entity || !g_LocalPlayer)
				continue;

			if (entity == g_LocalPlayer)
				continue;

			if (!g_EngineClient->GetPlayerInfo(i, &info))
				continue;

			if (entity->IsDormant())
				continue;

			if (entity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
				continue;

			if (entity->IsAlive())
			{
				float simtime = entity->m_flSimulationTime();
				Vector hitboxPos = entity->GetBonePos(8);

				headPositions[i][cmd->command_number % 13] = backtrackData{ simtime, hitboxPos };
				Vector ViewDir = angle_vector(cmd->viewangles + (g_LocalPlayer->m_aimPunchAngle() * 2.f));
				float FOVDistance = distance_point_to_line(hitboxPos, g_LocalPlayer->GetEyePos(), ViewDir);

				if (bestFov > FOVDistance)
				{
					bestFov = FOVDistance;
					bestTargetIndex = i;
				}
			}
		}

		float bestTargetSimTime;
		if (bestTargetIndex != -1)
		{
			float tempFloat = FLT_MAX;
			Vector ViewDir = angle_vector(cmd->viewangles + (g_LocalPlayer->m_aimPunchAngle() * 2.f));
			for (int t = 0; t < 12; ++t)
			{
				float tempFOVDistance = distance_point_to_line(headPositions[bestTargetIndex][t].hitboxPos, g_LocalPlayer->GetEyePos(), ViewDir);
				if (tempFloat > tempFOVDistance && headPositions[bestTargetIndex][t].simtime > g_LocalPlayer->m_flSimulationTime() - 1)
				{
					tempFloat = tempFOVDistance;
					bestTargetSimTime = headPositions[bestTargetIndex][t].simtime;
				}
			}
			if (cmd->buttons & IN_ATTACK)
			{
				cmd->tick_count = TIME_TO_TICKS(bestTargetSimTime);
			}
		}
	}
}

BackTrack* backtracking = new BackTrack();
backtrackData headPositions[64][12];

extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

ImGuiStyle& style = ImGui::GetStyle();

void Theme_Grey()
{
	style.WindowMinSize = ImVec2(630, 375);
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

bool vecPressedKeys[256] = { 0 };
LRESULT __stdcall Hooked_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//Captures the keys states
	switch (uMsg) {
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
	if (vecPressedKeys[VK_INSERT]) {
		isClicked = false;
		isDown = true;
	}
	else if (!vecPressedKeys[VK_INSERT] && isDown) {
		isClicked = true;
		isDown = false;
	}
	else {
		isClicked = false;
		isDown = false;
	}

	if (isClicked)
	{
		Options::visible = !Options::visible;
		//Use cl_mouseenable convar to disable the mouse when the window is open 
		static auto cl_mouseenable = g_CVar->FindVar("cl_mouseenable");
		cl_mouseenable->SetValue(!Options::visible);
	}

	//Processes the user input using ImGui_ImplDX9_WndProcHandler
	if (Initialized && Options::visible && ImGui_ImplDX9_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true; //Input was consumed, return

					 //Input was not consumed by the GUI, call original WindowProc to pass the input to the game
	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

void InitializeGUI(IDirect3DDevice9 * device)
{
	HWND hWnd = FindWindowA("Valve001", 0);
	ImGui_ImplDX9_Init(hWnd, device);
	ImGuiIO& io = ImGui::GetIO();
	ImFontConfig config;
	config.MergeMode = true;
	io.Fonts->AddFontDefault();
	ImFont* font = io.Fonts->AddFontFromMemoryCompressedTTF(Font_compressed_data, Font_compressed_size, 16.f, &config, io.Fonts->GetGlyphRangesDefault());
	Theme_Grey();
	ImGui_ImplDX9_CreateDeviceObjects();
	oWndProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)Hooked_WndProc);
}

/*
		ImGui::Checkbox("bunnyhop", &Options::Bhop);
		ImGui::Checkbox("chams", &Options::Chams);
		ImGui::Checkbox("chams visible only", &Options::ChamsVisibleOnly);
		ImGui::Checkbox("backtracking", &Options::Backtracking);
		ImGui::Checkbox("show backtracking positions", &Options::BacktrackingESP);
		ImGui::Checkbox("grenade prediction", &Options::GrenadePrediction);
		ImGui::Checkbox("radar", &Options::Radar);
		ImGui::Checkbox("stream proofind", &Options::StreamProofing);
*/

void RenderFrame(IDirect3DDevice9* device)
{
	if (!Initialized)
	{
		InitializeGUI(device);
		Initialized = true;
	}

	ImGui::GetIO().MouseDrawCursor = Options::visible;
	ImGui_ImplDX9_NewFrame();

	static float menuFadeTimer = 0.0f;
	if (Options::visible)
	{
		if (menuFadeTimer <= 1.0f - (1.2f / 100.0f)) menuFadeTimer += (1.2f / 100.0f);
		style.Alpha = menuFadeTimer;
		//RenderMenu();
	}
	else if (menuFadeTimer > (1.2f / 100.0f) && !Options::visible)
	{
		menuFadeTimer -= (1.2f / 100.0f);
		style.Alpha = menuFadeTimer;
		//RenderMenu();
	}

	//ImGui::Render();
}

static CreateClientClassFn GetWearableCreateFn()
{
	auto pClass = g_CHLClient->GetAllClasses();

	while (strcmp(pClass->m_pNetworkName, "CEconWearable"))
		pClass = pClass->m_pNext;

	return pClass->m_pCreateFn;
}

// Function to apply skin data to weapons.
inline bool ApplyCustomSkin(C_BaseWeapon* pWeapon, int nWeaponIndex, int skin)
{
	// Apply our changes to the fallback variables.
	pWeapon->m_nFallbackPaintKit() = skin;
	pWeapon->m_nFallbackSeed() = 0;
	pWeapon->m_nFallbackStatTrak() = -1;
	pWeapon->m_flFallbackWear() = 0.000000001f;

	pWeapon->m_iItemDefinitionIndex() = nWeaponIndex;

	pWeapon->m_iItemIDHigh() = -1;

	return true;
}

// Function to apply custom view models to weapons.
inline bool ApplyCustomModel(C_BaseWeapon* pWeapon, const char* vMdl)
{
	auto pLocal = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());

	// Get the view model of this weapon.
	C_BaseViewModel* pViewModel = pLocal->m_hViewModel().Get();

	if (!pViewModel)
		return false;

	// Get the weapon belonging to this view model.
	auto hViewModelWeapon = pViewModel->m_hWeapon();
	auto pViewModelWeapon = (C_BaseWeapon*)g_EntityList->GetClientEntityFromHandle(hViewModelWeapon);

	if (pViewModelWeapon != pWeapon)
		return false;

	// Check if an override exists for this view model.
	int nViewModelIndex = pViewModel->m_nModelIndex();

	// Set the replacement model.
	pViewModel->m_nModelIndex() = g_MdlInfo->GetModelIndex(vMdl);

	return true;
}

void SetClantag(const char* tag)
{
	static auto fnClantagChanged = (int(__fastcall*)(const char*, const char*))PatternScan(GetModuleHandleW(L"engine.dll"), "53 56 57 8B DA 8B F9 FF 15");
	fnClantagChanged(tag, tag);
}

void SetName(const char* name_)
{
	static auto name = g_CVar->FindVar("name");
	name->fnChangeCallback = 0;
	static bool once = false;
	if (!once) name->SetValue("\n");
	once = true;
	name->SetValue(name_);
}

void RankRevealAll()
{
	using ServerRankRevealAll = char(__cdecl*)(int*);
	static auto fnServerRankRevealAll = PatternScan(GetModuleHandleW(L"client.dll"), "55 8B EC 8B 0D ? ? ? ? 68");
	int v[3] = { 0,0,0 };
	reinterpret_cast<ServerRankRevealAll>(fnServerRankRevealAll)(v);
}

void SetSky(const char* name)
{
	static auto LoadNamedSky_Function = (void(__fastcall*)(const char*))(PatternScan(GetModuleHandleW(L"engine.dll"), "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45"));
	LoadNamedSky_Function(name);
}

static bool screen_transform(const Vector& in, Vector& out)
{
	static auto& w2sMatrix = g_EngineClient->WorldToScreenMatrix();

	out.x = w2sMatrix.m[0][0] * in.x + w2sMatrix.m[0][1] * in.y + w2sMatrix.m[0][2] * in.z + w2sMatrix.m[0][3];
	out.y = w2sMatrix.m[1][0] * in.x + w2sMatrix.m[1][1] * in.y + w2sMatrix.m[1][2] * in.z + w2sMatrix.m[1][3];
	out.z = 0.0f;

	float w = w2sMatrix.m[3][0] * in.x + w2sMatrix.m[3][1] * in.y + w2sMatrix.m[3][2] * in.z + w2sMatrix.m[3][3];

	if (w < 0.001f) {
		out.x *= 100000;
		out.y *= 100000;
		return false;
	}

	out.x /= w;
	out.y /= w;

	return true;
}

bool WorldToScreen(const Vector& in, Vector& out)
{
	if (screen_transform(in, out)) {
		int w, h;
		g_EngineClient->GetScreenSize(w, h);

		out.x = (w / 2.0f) + (out.x * w) / 2.0f;
		out.y = (h / 2.0f) - (out.y * h) / 2.0f;

		return true;
	}
	return false;
}

QAngle CalcAngle(Vector src, Vector dst)
{
	QAngle ret;
	Vector delta = src - dst;
	double hyp = delta.Length2D(); //delta.Length
	ret.yaw = (atan(delta.y / delta.x) * 57.295779513082f);
	ret.pitch = (atan(delta.z / hyp) * 57.295779513082f);
	ret[2] = 0.00;
	if (delta.x >= 0.0) ret.yaw += 180.0f;
	return ret;
}

void NormalizeAngles(QAngle& angle)
{
	while (angle.pitch > 89.0f) angle.pitch -= 180.f;
	while (angle.pitch < -89.0f) angle.pitch += 180.f;
	while (angle.yaw > 180.f) angle.yaw -= 360.f;
	while (angle.yaw < -180.f) angle.yaw += 360.f;
}

void ClampAngles(QAngle& angle)
{
	if (angle.yaw > 180.0f) angle.yaw = 180.0f;
	else if (angle.yaw < -180.0f) angle.yaw = -180.0f;
	if (angle.pitch > 89.0f) angle.pitch = 89.0f;
	else if (angle.pitch < -89.0f) angle.pitch = -89.0f;
}

float GetFOV(const Vector& viewAngle, const QAngle& aimAngle)
{
	QAngle delta;
	delta.pitch = aimAngle.pitch - viewAngle.x;
	delta.yaw = aimAngle.yaw - viewAngle.y;
	NormalizeAngles(delta);
	return sqrtf(powf(delta.pitch, 2.0f) + powf(delta.yaw, 2.0f));
}

template<typename T>
class singleton
{
protected:
	singleton() {}
	~singleton() {}

	singleton(const singleton&) = delete;
	singleton& operator=(const singleton&) = delete;

	singleton(singleton&&) = delete;
	singleton& operator=(singleton&&) = delete;
public:
	static T& instance()
	{
		static T inst{};
		return inst;
	}
};

class grenade_prediction
	: public singleton<grenade_prediction>
{
private:
	std::vector<Vector> path;

	int type = 0;
	int act = 0;

	void Setup(Vector& vecSrc, Vector& vecThrow, Vector viewangles);
	void Simulate(CViewSetup* setup);

	int Step(Vector& vecSrc, Vector& vecThrow, int tick, float interval);
	bool CheckDetonate(const Vector& vecThrow, const trace_t& tr, int tick, float interval);

	void TraceHull(Vector& src, Vector& end, trace_t& tr);
	void AddGravityMove(Vector& move, Vector& vel, float frametime, bool onground);
	void PushEntity(Vector& src, const Vector& move, trace_t& tr);
	void ResolveFlyCollisionCustom(trace_t& tr, Vector& vecVelocity, float interval);
	int PhysicsClipVelocity(const Vector& in, const Vector& normal, Vector& out, float overbounce);
public:
	void Tick(int buttons);
	void View(CViewSetup* setup);
	void Paint();
};

void grenade_prediction::Tick(int buttons)
{
	if (!Options::GrenadePrediction)
		return;
	bool in_attack = buttons & IN_ATTACK;
	bool in_attack2 = buttons & IN_ATTACK2;

	act = (in_attack && in_attack2) ? ACT_LOB :
		(in_attack2) ? ACT_DROP :
		(in_attack) ? ACT_THROW :
		ACT_NONE;
}
void grenade_prediction::View(CViewSetup* setup)
{
	if (!Options::GrenadePrediction)
		return;
	if (g_LocalPlayer && g_LocalPlayer->IsAlive())
	{
		auto weapon = g_LocalPlayer->m_hActiveWeapon();
		if (weapon && weapon->IsGrenade())
		{
			type = weapon->m_iItemDefinitionIndex();
			Simulate(setup);
		}
		else type = 0;
	}
}

void grenade_prediction::Paint()
{
	if (!Options::GrenadePrediction)
		return;
	if (type && path.size() > 1)
	{
		Vector nadeStart, nadeEnd;
		Vector prev = path[0];
		for (auto it = path.begin(), end = path.end(); it != end; ++it)
		{
			if (WorldToScreen(prev, nadeStart) && WorldToScreen(*it, nadeEnd))
			{
				g_VGuiSurface->DrawSetColor(255, 255, 255, 255);
				g_VGuiSurface->DrawLine((int)nadeStart.x, (int)nadeStart.y, (int)nadeEnd.x, (int)nadeEnd.y);
			}
			prev = *it;
		}

		if (WorldToScreen(prev, nadeEnd))
		{
			g_VGuiSurface->DrawSetColor(255, 0, 0, 255);
			g_VGuiSurface->DrawOutlinedCircle((int)nadeEnd.x, (int)nadeEnd.y, 10, 48);
		}
	}
}

static const constexpr auto PIRAD = 0.01745329251f;
void angle_vectors2(const Vector &angles, Vector *forward, Vector *right, Vector *up)
{
	float sr, sp, sy, cr, cp, cy;

	sp = static_cast<float>(sin(double(angles.x) * PIRAD));
	cp = static_cast<float>(cos(double(angles.x) * PIRAD));
	sy = static_cast<float>(sin(double(angles.y) * PIRAD));
	cy = static_cast<float>(cos(double(angles.y) * PIRAD));
	sr = static_cast<float>(sin(double(angles.z) * PIRAD));
	cr = static_cast<float>(cos(double(angles.z) * PIRAD));

	if (forward)
	{
		forward->x = cp*cy;
		forward->y = cp*sy;
		forward->z = -sp;
	}

	if (right)
	{
		right->x = (-1 * sr*sp*cy + -1 * cr*-sy);
		right->y = (-1 * sr*sp*sy + -1 * cr*cy);
		right->z = -1 * sr*cp;
	}

	if (up)
	{
		up->x = (cr*sp*cy + -sr*-sy);
		up->y = (cr*sp*sy + -sr*cy);
		up->z = cr*cp;
	}
}

void grenade_prediction::Setup(Vector& vecSrc, Vector& vecThrow, Vector viewangles)
{
	if (!Options::GrenadePrediction)
		return;

	Vector angThrow = viewangles;
	C_BasePlayer* local = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());
	float pitch = angThrow.x;

	if (pitch <= 90.0f)
	{
		if (pitch<-90.0f)
		{
			pitch += 360.0f;
		}
	}
	else
	{
		pitch -= 360.0f;
	}
	float a = pitch - (90.0f - fabs(pitch)) * 10.0f / 90.0f;
	angThrow.x = a;

	// Gets ThrowVelocity from weapon files
	// Clamped to [15,750]
	float flVel = 750.0f * 0.9f;

	// Do magic on member of grenade object [esi+9E4h]
	// m1=1  m1+m2=0.5  m2=0
	static const float power[] = { 1.0f, 1.0f, 0.5f, 0.0f };
	float b = power[act];
	// Clamped to [0,1]
	b = b * 0.7f;
	b = b + 0.3f;
	flVel *= b;

	Vector vForward, vRight, vUp;
	angle_vectors2(angThrow, &vForward, &vRight, &vUp); //angThrow.ToVector(vForward, vRight, vUp);

	vecSrc = local->GetEyePos();
	float off = (power[act] * 12.0f) - 12.0f;
	vecSrc.z += off;

	// Game calls UTIL_TraceHull here with hull and assigns vecSrc tr.endpos
	trace_t tr;
	Vector vecDest = vecSrc;
	vecDest += vForward * 22.0f; //vecDest.MultAdd(vForward, 22.0f);

	TraceHull(vecSrc, vecDest, tr);

	// After the hull trace it moves 6 units back along vForward
	// vecSrc = tr.endpos - vForward * 6
	Vector vecBack = vForward; vecBack *= 6.0f;
	vecSrc = tr.end;
	vecSrc -= vecBack;

	// Finally calculate velocity
	vecThrow = local->m_vecVelocity(); 
	vecThrow *= 1.25f;
	vecThrow += vForward * flVel; //	vecThrow.MultAdd(vForward, flVel);
}

void grenade_prediction::Simulate(CViewSetup* setup)
{
	if (!Options::GrenadePrediction)
		return;
	Vector vecSrc, vecThrow;
	Vector angles;
	g_EngineClient->GetViewAngles(angles);
	Setup(vecSrc, vecThrow, angles);

	float interval = g_GlobalVars->interval_per_tick;

	// Log positions 20 times per sec
	int logstep = static_cast<int>(0.05f / interval);
	int logtimer = 0;

	path.clear();
	for (unsigned int i = 0; i < path.max_size() - 1; ++i)
	{
		if (!logtimer) path.push_back(vecSrc);
		int s = Step(vecSrc, vecThrow, i, interval);
		if ((s & 1)) break;
		if ((s & 2) || logtimer >= logstep) logtimer = 0;
		else ++logtimer;
	}
	path.push_back(vecSrc);
}

int grenade_prediction::Step(Vector& vecSrc, Vector& vecThrow, int tick, float interval)
{

	// Apply gravity
	Vector move;
	AddGravityMove(move, vecThrow, interval, false);

	// Push entity
	trace_t tr;
	PushEntity(vecSrc, move, tr);

	int result = 0;
	// Check ending conditions
	if (CheckDetonate(vecThrow, tr, tick, interval))
	{
		result |= 1;
	}

	// Resolve collisions
	if (tr.fraction != 1.0f)
	{
		result |= 2; // Collision!
		ResolveFlyCollisionCustom(tr, vecThrow, interval);
	}

	// Set new position
	vecSrc = tr.end;

	return result;
}

bool grenade_prediction::CheckDetonate(const Vector& vecThrow, const trace_t& tr, int tick, float interval)
{
	switch (type)
	{
	case WEAPON_SMOKE:
	case WEAPON_DECOY:
		// Velocity must be <0.1, this is only checked every 0.2s
		if (vecThrow.Length2D() < 0.1f)
		{
			int det_tick_mod = static_cast<int>(0.2f / interval);
			return !(tick % det_tick_mod);
		}
		return false;

	case WEAPON_MOLOTOV:
	case WEAPON_INC:
		// Detonate when hitting the floor
		if (tr.fraction != 1.0f && tr.plane.normal.z > 0.7f)
			return true;
		// OR we've been flying for too long

	case WEAPON_FLASH:
	case WEAPON_HE:
		// Pure timer based, detonate at 1.5s, checked every 0.2s
		return static_cast<float>(tick)*interval>1.5f && !(tick%static_cast<int>(0.2f / interval));

	default:
		assert(false);
		return false;
	}
}

void grenade_prediction::TraceHull(Vector& src, Vector& end, trace_t& tr)
{
	if (!Options::GrenadePrediction)
		return;
	Ray_t ray;
	ray.Init(src, end, Vector(-2.0f, -2.0f, -2.0f), Vector(2.0f, 2.0f, 2.0f));

	CTraceFilterWorldAndPropsOnly filter;

	g_EngineTrace->TraceRay(ray, 0x200400B, &filter, &tr);
}

void grenade_prediction::AddGravityMove(Vector& move, Vector& vel, float frametime, bool onground)
{
	if (!Options::GrenadePrediction)
		return;
	Vector basevel(0.0f, 0.0f, 0.0f);

	move.x = (vel.x + basevel.x) * frametime;
	move.y = (vel.y + basevel.y) * frametime;

	if (onground)
	{
		move.z = (vel.z + basevel.z) * frametime;
	}
	else
	{
		// Game calls GetActualGravity( this );
		float gravity = 800.0f * 0.4f;

		float newZ = vel.z - (gravity * frametime);
		move.z = ((vel.z + newZ) / 2.0f + basevel.z) * frametime;

		vel.z = newZ;
	}
}

void grenade_prediction::PushEntity(Vector& src, const Vector& move, trace_t& tr)
{
	if (!Options::GrenadePrediction)
		return;
	Vector vecAbsEnd = src;
	vecAbsEnd += move;

	// Trace through world
	TraceHull(src, vecAbsEnd, tr);
}

void grenade_prediction::ResolveFlyCollisionCustom(trace_t& tr, Vector& vecVelocity, float interval)
{
	if (!Options::GrenadePrediction)
		return;
	// Calculate elasticity
	float flSurfaceElasticity = 1.0;  // Assume all surfaces have the same elasticity
	float flGrenadeElasticity = 0.45f; // GetGrenadeElasticity()
	float flTotalElasticity = flGrenadeElasticity * flSurfaceElasticity;
	if (flTotalElasticity>0.9f) flTotalElasticity = 0.9f;
	if (flTotalElasticity<0.0f) flTotalElasticity = 0.0f;

	// Calculate bounce
	Vector vecAbsVelocity;
	PhysicsClipVelocity(vecVelocity, tr.plane.normal, vecAbsVelocity, 2.0f);
	vecAbsVelocity *= flTotalElasticity;

	// Stop completely once we move too slow
	float flSpeedSqr = vecAbsVelocity.LengthSqr();
	static const float flMinSpeedSqr = 20.0f * 20.0f; // 30.0f * 30.0f in CSS
	if (flSpeedSqr<flMinSpeedSqr)
	{
		//vecAbsVelocity.Zero();
		vecAbsVelocity.x = 0.0f;
		vecAbsVelocity.y = 0.0f;
		vecAbsVelocity.z = 0.0f;
	}

	// Stop if on ground
	if (tr.plane.normal.z>0.7f)
	{
		vecVelocity = vecAbsVelocity;
		vecAbsVelocity *= ((1.0f - tr.fraction) * interval); //vecAbsVelocity.Mult((1.0f - tr.fraction) * interval);
		PushEntity(tr.end, vecAbsVelocity, tr);
	}
	else
	{
		vecVelocity = vecAbsVelocity;
	}
}

int grenade_prediction::PhysicsClipVelocity(const Vector& in, const Vector& normal, Vector& out, float overbounce)
{
	static const float STOP_EPSILON = 0.1f;

	float    backoff;
	float    change;
	float    angle;
	int        i, blocked;

	blocked = 0;

	angle = normal[2];

	if (angle > 0)
	{
		blocked |= 1;        // floor
	}
	if (!angle)
	{
		blocked |= 2;        // step
	}

	backoff = in.Dot(normal) * overbounce;

	for (i = 0; i<3; i++)
	{
		change = normal[i] * backoff;
		out[i] = in[i] - change;
		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
		{
			out[i] = 0;
		}
	}

	return blocked;
}

inline IHandleEntity* CBaseHandle::Get() const
{
	return g_EntityList->GetClientEntityFromHandle(*this);
}

namespace Hooks
{
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

	CreateMove oCreateMove = nullptr;
	PlaySound oPlaySound = nullptr;
	DrawModelExecute oDrawModelExecute = nullptr;
	PaintTraverse oPaintTraverse = nullptr;
	OverrideView oOverrideView = nullptr;
	FrameStageNotify oFrameStageNotify = nullptr;
	EndScene oEndScene = nullptr;
	Reset oReset = nullptr;
	Present oPresent = nullptr;

	void __stdcall hkCreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket)
	{
		oCreateMove(g_CHLClient, sequence_number, input_sample_frametime, active);

		auto cmd = g_Input->GetUserCmd(sequence_number);
		auto verified = g_Input->GetVerifiedCmd(sequence_number);

		if (!cmd || !cmd->command_number)
			return;

		backtracking->legitBackTrack(cmd);

		if (g_LocalPlayer && g_EngineClient->IsInGame() && g_LocalPlayer->IsAlive() && Options::Bhop)
			if (cmd->buttons & IN_JUMP && !(g_LocalPlayer->m_fFlags() & FL_ONGROUND)) cmd->buttons &= ~IN_JUMP;

		//grenade_prediction::instance().Tick(cmd->buttons);

		if (cmd->buttons & IN_SCORE) RankRevealAll();	

		verified->m_cmd = *cmd;
		verified->m_crc = cmd->GetChecksum();
	}

	__declspec(naked) void __stdcall hkCreateMove_Proxy(int sequence_number, float input_sample_frametime, bool active)
	{
		__asm
		{
			push ebp
			mov  ebp, esp
			push ebx
			lea  ecx, [esp]
			push ecx
			push dword ptr[active]
			push dword ptr[input_sample_frametime]
			push dword ptr[sequence_number]
			call Hooks::hkCreateMove
			pop  ebx
			pop  ebp
			retn 0Ch
		}
	}

	void __stdcall hkPlaySound(const char* name)
	{
		oPlaySound(g_VGuiSurface, name);

		if (strstr(name, "UI/competitive_accept_beep.wav")) 
		{
			static auto fnAccept = (void(*)())PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F8 83 EC 08 56 8B 35 ? ? ? ? 57 83 BE");
			fnAccept();
		}
	}

	void __stdcall hkDrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
	{
		oDrawModelExecute(g_MdlRender, ctx, state, pInfo, pCustomBoneToWorld);

		static IMaterial* chams_material = nullptr;
		static IMaterial* chams_material_XQZ = nullptr;

		static bool Initialized = false;
		if (!Initialized)
		{
			std::ofstream("csgo\\materials\\chams_material_XQZ.vmt") << R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "1"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";

			std::ofstream("csgo\\materials\\chams_material.vmt") << R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "0"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";

			chams_material_XQZ = g_MatSystem->FindMaterial("chams_material_XQZ", TEXTURE_GROUP_MODEL);
			chams_material = g_MatSystem->FindMaterial("chams_material", TEXTURE_GROUP_MODEL);
			Initialized = true;
		}

		if (g_EngineClient->IsInGame() && g_LocalPlayer && Options::Chams)
		{
			const auto mdl = pInfo.pModel;

			if (strstr(mdl->szName, "models/player"))
			{
				auto pEntity = (C_BasePlayer*)g_EntityList->GetClientEntity(pInfo.entity_index);
				if (pEntity && pEntity->m_iTeamNum() != g_LocalPlayer->m_iTeamNum() && pEntity->IsAlive())
				{
					if (!Options::ChamsVisibleOnly)
					{
						chams_material_XQZ->AlphaModulate(1.0f);
						chams_material_XQZ->ColorModulate(0.0f, 0.0f, 1.0f);
						g_MdlRender->ForcedMaterialOverride(chams_material_XQZ);
						oDrawModelExecute(g_MdlRender, ctx, state, pInfo, pCustomBoneToWorld);
					}

					chams_material->AlphaModulate(1.0f);
					chams_material->ColorModulate(0.0f, 1.0f, 0.0f);
					g_MdlRender->ForcedMaterialOverride(chams_material);
					oDrawModelExecute(g_MdlRender, ctx, state, pInfo, pCustomBoneToWorld);
				}
			}
			
		}

		g_MdlRender->ForcedMaterialOverride(nullptr);
	}

	void __stdcall hkPaintTraverse(vgui::VPANEL panel, bool forceRepaint, bool allowForce)
	{
		static auto panelId = vgui::VPANEL{ 0 };

		oPaintTraverse(g_VGuiPanel, panel, forceRepaint, allowForce);

		static vgui::HFont aCoolFont;

		static bool FontsCreated = false;
		if (!FontsCreated)
		{
			aCoolFont = g_VGuiSurface->Create_Font();
			g_VGuiSurface->SetFontGlyphSet(aCoolFont, "Tahoma", 24, 500, 0, 0, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);
			FontsCreated = true;
		}

		if (!panelId)
		{
			const char* panelName = g_VGuiPanel->GetName(panel);
			if (!strcmp(panelName, "FocusOverlayPanel")) panelId = panel;
		}
		else if (panelId == panel) 
		{
			g_VGuiSurface->DrawSetTextFont(aCoolFont);
			g_VGuiSurface->DrawSetTextColor(255, 0, 0, 255);
			g_VGuiSurface->DrawSetTextPos(10, 10);
			static wchar_t* watermark = L"overdrivecheat.me lite";
			g_VGuiSurface->DrawPrintText(watermark, wcslen(watermark));	
			for (int i = 0; i < ARRAYSIZE(Names::hackNames); i++) {
				g_VGuiSurface->DrawSetTextFont(aCoolFont);
				g_VGuiSurface->DrawSetTextColor(100, 133, 122, 255);
				g_VGuiSurface->DrawSetTextPos(10, (i*25) + 35);
				std::string hn = Names::hackNames[i];
				std::wstring wide_string = std::wstring(hn.begin(), hn.end());
				const wchar_t* result = wide_string.c_str();
				g_VGuiSurface->DrawPrintText(result, wcslen(result));
			}
		}
	}

	int fov = 0.0f;
	void __stdcall hkOverrideView(CViewSetup* setup)
	{
		if (g_EngineClient && g_EngineClient->IsInGame())
			if (!g_LocalPlayer->m_bIsScoped()) setup->fov += fov;

		if (GetAsyncKeyState(VK_LEFT)) fov--;
		if (GetAsyncKeyState(VK_RIGHT)) fov++;

		oOverrideView(g_ClientMode, setup);
	}

	void __stdcall hkFrameStageNotify(ClientFrameStage_t stage)
	{
		auto pLocal = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());

		static bool InGameCheck = false;
		if (!InGameCheck)
		{
			if (g_EngineClient->IsInGame()) exit(0);
			InGameCheck = true;
		}

		if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		{
			if (Options::Radar)
			{
				for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
				{
					auto pEntity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);

					if (pEntity &&
						pEntity->IsAlive() &&
						pEntity->m_iTeamNum() != g_LocalPlayer->m_iTeamNum())
						*(bool*)(pEntity + 0x939) = true;
				}
			}

			backtracking->Update();
		}

		oFrameStageNotify(g_CHLClient, stage);
	}

	long __stdcall hkEndScene(IDirect3DDevice9* device)
	{
		RenderFrame(device);
		return oEndScene(device);
	}

	long __stdcall hkPresent(IDirect3DDevice9* device, RECT* src, RECT* dest, HWND wnd_override, RGNDATA* dirty_region)
	{
		return oPresent(device, src, dest, wnd_override, dirty_region);
	}

	long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		if (!Initialized) return oReset(device, pPresentationParameters);

		ImGui_ImplDX9_InvalidateDeviceObjects();
		ImGui_ImplDX9_CreateDeviceObjects();
		return oReset(device, pPresentationParameters);
	}

	void HookFunctions()
	{
		CreateInterfaceFn engineFactory = get_module_factory(GetModuleHandleW(L"engine.dll"));
		CreateInterfaceFn clientFactory = get_module_factory(GetModuleHandleW(L"client.dll"));
		CreateInterfaceFn valveStdFactory = get_module_factory(GetModuleHandleW(L"vstdlib.dll"));
		CreateInterfaceFn vguiFactory = get_module_factory(GetModuleHandleW(L"vguimatsurface.dll"));
		CreateInterfaceFn vgui2Factory = get_module_factory(GetModuleHandleW(L"vgui2.dll"));
		CreateInterfaceFn matSysFactory = get_module_factory(GetModuleHandleW(L"materialsystem.dll"));
		CreateInterfaceFn dataCacheFactory = get_module_factory(GetModuleHandleW(L"datacache.dll"));
		CreateInterfaceFn vphysicsFactory = get_module_factory(GetModuleHandleW(L"vphysics.dll"));

		g_CHLClient = get_interface<IBaseClientDLL>(clientFactory, "VClient018");
		g_EntityList = get_interface<IClientEntityList>(clientFactory, "VClientEntityList003");
		g_MdlCache = get_interface<IMDLCache>(dataCacheFactory, "MDLCache004");
		g_EngineClient = get_interface<IVEngineClient>(engineFactory, "VEngineClient014");
		g_MdlRender = get_interface<IVModelRender>(engineFactory, "VEngineModel016");
		g_MatSystem = get_interface<IMaterialSystem>(matSysFactory, "VMaterialSystem080");
		g_VGuiPanel = get_interface<IPanel>(vgui2Factory, "VGUI_Panel009");
		g_VGuiSurface = get_interface<ISurface>(vguiFactory, "VGUI_Surface031");
		g_EngineTrace = get_interface<IVEngineTrace>(engineFactory, "EngineTraceClient004");
		g_MdlInfo = get_interface<IVModelInfoClient>(engineFactory, "VModelInfoClient004");
		g_DebugOverlay = get_interface<IVDebugOverlay>(engineFactory, "VDebugOverlay004");
		g_CVar = get_interface<ICVar>(valveStdFactory, "VEngineCvar007");
		g_ClientMode = **(IClientMode***)((*(uintptr_t**)g_CHLClient)[10] + 0x5);

		HMODULE client = GetModuleHandleW(L"client.dll");
		HMODULE engine = GetModuleHandleW(L"engine.dll");
		HMODULE dx9api = GetModuleHandleW(L"shaderapidx9.dll");

		g_GlobalVars = **(CGlobalVarsBase***)(PatternScan(client, "A1 ? ? ? ? 5E 8B 40 10") + 1);
		g_Input = *(CInput**)(PatternScan(client, "B9 ? ? ? ? 8B 40 38 FF D0 84 C0 0F 85") + 1);
		g_LocalPlayer = *(C_LocalPlayer*)(PatternScan(client, "8B 0D ? ? ? ? 83 FF FF 74 07") + 2);
		g_D3DDevice9 = **(IDirect3DDevice9***)(PatternScan(dx9api, "A1 ? ? ? ? 50 8B 08 FF 51 0C") + 1);

		CVMTHookManager* chlclient_hook = new CVMTHookManager(g_CHLClient);
		CVMTHookManager* vguisurface_hook = new CVMTHookManager(g_VGuiSurface);
		CVMTHookManager* vguipanel_hook = new CVMTHookManager(g_VGuiPanel);
		CVMTHookManager* mdlrender_hook = new CVMTHookManager(g_MdlRender);
		CVMTHookManager* clientmode_hook = new CVMTHookManager(g_ClientMode);
		CVMTHookManager* direct3d_hook = new CVMTHookManager(g_D3DDevice9);

		oCreateMove = (CreateMove)chlclient_hook->HookFunction(21, hkCreateMove_Proxy);
		oPlaySound = (PlaySound)vguisurface_hook->HookFunction(82, hkPlaySound);
		oDrawModelExecute = (DrawModelExecute)mdlrender_hook->HookFunction(21, hkDrawModelExecute);
		oPaintTraverse = (PaintTraverse)vguipanel_hook->HookFunction(41, hkPaintTraverse);
		oOverrideView = (OverrideView)clientmode_hook->HookFunction(18, hkOverrideView);
		oFrameStageNotify = (FrameStageNotify)chlclient_hook->HookFunction(36, hkFrameStageNotify);
		oPresent = (Present)direct3d_hook->HookFunction(17, hkPresent);
		oEndScene = (EndScene)direct3d_hook->HookFunction(42, hkEndScene);
		oReset = (Reset)direct3d_hook->HookFunction(16, hkReset);

		chlclient_hook->HookTable(true);
		vguisurface_hook->HookTable(true);
		mdlrender_hook->HookTable(true);
		vguipanel_hook->HookTable(true);
		clientmode_hook->HookTable(true);
		direct3d_hook->HookTable(true);
	}
}