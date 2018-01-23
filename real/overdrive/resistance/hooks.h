#pragma once
#include <time.h>
#include "sdk\sdk.h"
#include "poly.h"
#include "utils.h"
#include "math.h"
#include "interfaces.h"
#include "definitions.h"
#include "menu.h"
#include "Options.h"
#include "ModelRec.h"

#define M_PHI 1.61803398874989484820f // golden ratio

#define RandomInt(min, max) (rand() % (max - min + 1) + min)

namespace Resolver
{
	int ResolverStages[64] = { 0 };
	CBaseEntity* pCurResolverEnt = nullptr;
	int shot = 0;

	void ResolvePlayer(CBaseEntity* pLocal, CBaseEntity* pEntity)
	{
		pCurResolverEnt = pEntity;
		
		switch (ResolverStages[pCurResolverEnt->Index()])
		{
		case 0: pCurResolverEnt->m_angEyeAngles() = QAngle(pEntity->m_angEyeAngles().pitch, pEntity->m_flLowerBodyYawTarget(), 0); break;
		case 1: pCurResolverEnt->m_angEyeAngles() = QAngle(pEntity->m_angEyeAngles().pitch, pEntity->m_flLowerBodyYawTarget() + 180.0f, 0); break;
		case 2: pCurResolverEnt->m_angEyeAngles() = QAngle(pEntity->m_angEyeAngles().pitch, pEntity->m_flLowerBodyYawTarget() + 90.0f, 0); break;
		case 3: pCurResolverEnt->m_angEyeAngles() = QAngle(pEntity->m_angEyeAngles().pitch, pEntity->m_flLowerBodyYawTarget() + 180.0f, 0); break;
		case 4: pCurResolverEnt->m_angEyeAngles() = QAngle(pEntity->m_angEyeAngles().pitch, pEntity->m_flLowerBodyYawTarget() - 90.0f, 0); break;
		case 5: pCurResolverEnt->m_angEyeAngles() = QAngle(pEntity->m_angEyeAngles().pitch, pEntity->m_flLowerBodyYawTarget(), 0); ResolverStages[pCurResolverEnt->Index()] = 0; break;
		}
		if (shot == 1) ResolverStages[pCurResolverEnt->Index()]++;
	}
};

namespace Backtracking
{
	class BacktrackInfo
	{
	public:
		Vector origin[100], headpos[100], velocity[100];
		QAngle viewangles[100];
		int m_fFlags[100], sequence[100], curtick_DO_NOT_SET_ONLY_USED_FOR_GETTING_INFO_FOR_INFORMATION_RESTORATION[100];
		float animtime[100], lby[100];
	};

	BacktrackInfo PlayerRecords[64];

	void StoreBacktrackInformation()
	{
		// Storing info
		for (int i = 0; i < g_EngineClient->GetMaxClients(); i++)
		{
			CBaseEntity* pEntity = g_EntityList->GetClientEntity(i);

			// Shifting player information for every tick
			for (int tick = 100 - 1; tick >= 0; tick--)
			{
				PlayerRecords[i].origin[tick + 1] = PlayerRecords[i].origin[tick];
				PlayerRecords[i].headpos[tick + 1] = PlayerRecords[i].headpos[tick];
				PlayerRecords[i].velocity[tick + 1] = PlayerRecords[i].velocity[tick];
				PlayerRecords[i].viewangles[tick + 1] = PlayerRecords[i].viewangles[tick];
				PlayerRecords[i].m_fFlags[tick + 1] = PlayerRecords[i].m_fFlags[tick];
				PlayerRecords[i].sequence[tick + 1] = PlayerRecords[i].sequence[tick];
				PlayerRecords[i].curtick_DO_NOT_SET_ONLY_USED_FOR_GETTING_INFO_FOR_INFORMATION_RESTORATION[tick + 1] = PlayerRecords[i].curtick_DO_NOT_SET_ONLY_USED_FOR_GETTING_INFO_FOR_INFORMATION_RESTORATION[tick];
				PlayerRecords[i].animtime[tick + 1] = PlayerRecords[i].animtime[tick];
				PlayerRecords[i].lby[tick + 1] = PlayerRecords[i].lby[tick];
			}

			// Storing new player info
			PlayerRecords[i].origin[0] = pEntity->m_vecAbsOrigin();
			PlayerRecords[i].headpos[0] = pEntity->GetBonePos(8);
			PlayerRecords[i].velocity[0] = pEntity->m_vecVelocity();
			PlayerRecords[i].viewangles[0] = pEntity->m_angEyeAngles();
			// missing sequence
			PlayerRecords[i].curtick_DO_NOT_SET_ONLY_USED_FOR_GETTING_INFO_FOR_INFORMATION_RESTORATION[0] = g_EngineClient->GetServerTick();
			// missing animtime
			PlayerRecords[i].lby[0] = pEntity->m_flLowerBodyYawTarget();
		}
	}

	void BacktrackPlayer(CBaseEntity* pEntity)
	{
		// Getting nearest backtrack record
		float closestdistance = 999999999.0f;
		int closesttick;

		for (int i = 0; i <= 100; i++)
		{
			QAngle vAngle;
			g_EngineClient->GetViewAngles(vAngle);
			if (GetDistanceFOV(Get3dDistance(g_LocalPlayer->GetEyePosition(), PlayerRecords[pEntity->Index()].headpos[i]), vAngle) < closestdistance) closesttick = PlayerRecords[pEntity->Index()].curtick_DO_NOT_SET_ONLY_USED_FOR_GETTING_INFO_FOR_INFORMATION_RESTORATION[i];
		}

		// Checking ticks for matching target tick
		for (int i = 0; i <= 100; i++)
		{
			// If its matching to the closest tick, restore player info
			if (PlayerRecords[pEntity->Index()].curtick_DO_NOT_SET_ONLY_USED_FOR_GETTING_INFO_FOR_INFORMATION_RESTORATION[i] == closesttick)
			{
				pEntity->m_vecAbsOrigin() = PlayerRecords[pEntity->Index()].origin[closesttick];
				pEntity->m_vecVelocity() = PlayerRecords[pEntity->Index()].velocity[closesttick];
				pEntity->m_angEyeAngles() = PlayerRecords[pEntity->Index()].viewangles[closesttick];
				*(int*)((uintptr_t)pEntity + 0x100) = PlayerRecords[pEntity->Index()].m_fFlags[closesttick];
				// missing sequence
				// missing animtime
				*(float*)((uintptr_t)pEntity + 0x39AC) = PlayerRecords[pEntity->Index()].lby[closesttick];

				break;
			}
		}
	}
};

float Distance(Vector a, Vector b)
{
	return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
}

QAngle LastTickViewAngles;

bool GetBestHeadAngle(QAngle& angle)
{
	Vector position = g_LocalPlayer->m_vecAbsOrigin() + g_LocalPlayer->m_vecViewOffset();

	float closest_distance = 100.0f;

	float radius = g_Options.aimbot_antiaim_edge_dist + 0.1f;
	float step = M_PI * 2.0 / 8;

	for (float a = 0; a < (M_PI * 2.0); a += step)
	{
		Vector location(radius * cos(a) + position.x, radius * sin(a) + position.y, position.z);

		Ray_t ray;
		trace_t tr;
		ray.Init(position, location);

		CTraceFilter traceFilter;

		//CTraceFilter traceFilter;
		traceFilter.pSkip = g_LocalPlayer;

		g_EngineTrace->TraceRay(ray, 0x4600400B, &traceFilter, &tr);

		float distance = Distance(position, tr.end);

		if (distance < closest_distance)
		{
			closest_distance = distance;
			angle.yaw = RAD2DEG(a);
		}
	}

	return closest_distance < g_Options.aimbot_antiaim_edge_dist;
}

void DoAntiAimX(QAngle& angle, int command_number, bool bFlip, bool& clamp) // pitch
{
	int aa_type = g_Options.aimbot_antiaim_pitch;

	static float pDance = 0.0f;

	switch (aa_type)
	{
		// lol
	case ANTIAIM_PITCH_NONE:
		break;

	case ANTIAIM_PITCH_DOWN:
		angle.pitch = 89.0f;
		break;
	case ANTIAIM_PITCH_UP:
		angle.pitch = -89.0f;
		break;
	}
}

void DoAntiAimY(QAngle& angle, bool bFlip, bool& clamp) // yaw
{
	int aa_type = bFlip ? g_Options.aimbot_antiaim_yaw_fake : g_Options.aimbot_antiaim_yaw;

	static bool yFlip;
	float temp;
	int random;
	int maxJitter;

	switch (aa_type)
	{
		// xD
	case ANTIAIM_YAW_NONE:
		break;

	case ANTIAIM_YAW_SIDE:
		yFlip ? angle.yaw += 90.f : angle.yaw -= 90.0f;
		yFlip = !yFlip;
		break;
	case ANTIAIM_YAW_BACKWARDS:
		angle.yaw += 180.0f;
		break;
	case ANTIAIM_YAW_RIGHT:
		angle.yaw -= 90.0f;
		break;
	case ANTIAIM_YAW_LEFT:
		angle.yaw += 90.0f;
		break;
	case ANTIAIM_YAW_STATIC_FORWARD:
		angle.yaw = 0.0f;
		break;
	case ANTIAIM_YAW_STATIC_BACKWARDS:
		angle.yaw = 180.0f;
		break;
	case ANTIAIM_YAW_STATIC_RIGHT:
		angle.yaw = -90.0f;
		break;
	case ANTIAIM_YAW_STATIC_LEFT:
		angle.yaw = 90.0f;
		break;
	case ANTIAIM_YAW_FAKE_LBY1:
		bFlip ? angle.yaw += g_LocalPlayer->m_flLowerBodyYawTarget() - 90.0f : angle.yaw -= g_LocalPlayer->m_flLowerBodyYawTarget() + 90.0f;
		break;
	case ANTIAIM_YAW_FAKE_LBY2:
		bFlip ? angle.yaw += g_LocalPlayer->m_flLowerBodyYawTarget() - RandomInt(180, 360) : angle.yaw -= g_LocalPlayer->m_flLowerBodyYawTarget() + RandomInt(30, 61);
		break;
	case ANTIAIM_YAW_SPIN:
		angle.yaw = fmodf(g_GlobalVars->curtime * ((360.0f / M_PHI) * g_Options.aimbot_antiaim_spin_speed), 360.0f);
		break;
	case ANTIAIM_YAW_RANDOM:
		angle.yaw = RandomInt(-180, 180);
		break;
	case ANTIAIM_YAW_JITTER:
		yFlip ? angle.yaw -= 90.0f : angle.yaw -= 270.0f;
		yFlip = !yFlip;
		break;
	case ANTIAIM_YAW_BACKJITTER:
		angle.yaw -= 180;
		random = rand() % 100;
		maxJitter = rand() % (85 - 70 + 1) + 70;
		temp = maxJitter - (rand() % maxJitter);
		if (random < 35 + (rand() % 15))
			angle.yaw -= temp;
		else if (random < 85 + (rand() % 15))
			angle.yaw += temp;
		break;
	case ANTIAIM_YAW_FAKE_SIDE_LBY:		
		break;
	}
}

int tabIndex = TAB_AIMBOT;
int aimbotIndex = AIMBOTTAB_HVH;
int legitbotIndex = Legit_Pistol;

bool Initialized = false;

std::unique_ptr<DrawManager> g_Renderer = nullptr;

extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool vecPressedKeys[256] = {};
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

	//Insert toggles the menu
	{
		//Maybe there is a better way to do this? Not sure
		//We want to toggle when the key is pressed (i.e when it receives a down and then up signal)
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

			visible = !visible;
			//Use cl_mouseenable convar to disable the mouse when the window is open 
			static auto cl_mouseenable = g_CVar->FindVar("cl_mouseenable");
			cl_mouseenable->SetValue(!visible);
		}
	}

	//Processes the user input using ImGui_ImplDX9_WndProcHandler
	if (Initialized && visible && ImGui_ImplDX9_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true; //Input was consumed, return

					 //Input was not consumed by the GUI, call original WindowProc to pass the input to the game
	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

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

void Theme_Red()
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
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.92f, 0.18f, 0.29f, 0.75f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.92f, 0.18f, 0.29f, 0.75f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.92f, 0.18f, 0.29f, 0.75f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.188f, 0.188f, 0.188f, 1.f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.188f, 0.188f, 0.188f, 1.f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.725f, 0.725f, 0.725f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.502f, 0.796f, 0.769f, 0.10f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.502f, 0.796f, 0.769f, 0.10f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.228f, 0.228f, 0.228f, 1.f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.92f, 0.18f, 0.29f, 0.75f);
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

void Theme_Black()
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
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.188f, 0.188f, 0.188f, 1.f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.188f, 0.188f, 0.188f, 1.f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.725f, 0.725f, 0.725f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.502f, 0.796f, 0.769f, 0.10f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.502f, 0.796f, 0.769f, 0.10f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.228f, 0.228f, 0.228f, 1.f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
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

void Theme_Blue()
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
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.188f, 0.188f, 0.188f, 1.f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.188f, 0.188f, 0.188f, 1.f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.725f, 0.725f, 0.725f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.502f, 0.796f, 0.769f, 0.10f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.502f, 0.796f, 0.769f, 0.10f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.228f, 0.228f, 0.228f, 1.f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 0.00f, 0.50f, 1.00f);
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

	ImFontConfig config;
	config.MergeMode = true;
	io.Fonts->AddFontDefault();
	static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFont* iconfont = io.Fonts->AddFontFromMemoryCompressedTTF(IconFont_compressed_data, IconFont_compressed_size, 20.f, &config, icon_ranges);
	ImFont* font = io.Fonts->AddFontFromMemoryCompressedTTF(Font_compressed_data, Font_compressed_size, 16.f, &config, io.Fonts->GetGlyphRangesDefault());
	iconfont->DisplayOffset.y = 4.5;

	Theme_Grey();

	g_Renderer->CreateObjects();

	ImGui_ImplDX9_CreateDeviceObjects();

	oWndProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)Hooked_WndProc);
}

//using InPrediction = bool(__thiscall*)(IPrediction*);
using FireEventClientSide = bool(__thiscall*)(IGameEventManager2*, IGameEvent*);
using CreateMove = void(__thiscall*)(CClient*, int, float, bool);
using PaintTraverse = void(__thiscall*)(CPanel*, vgui::VPANEL, bool, bool);
using FrameStageNotify = void(__thiscall*)(CClient*, ClientFrameStage_t);
using EndScene = long(__stdcall*)(IDirect3DDevice9*);
using Reset = long(__stdcall*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
using Present = long(__stdcall*)(IDirect3DDevice9*, RECT*, RECT*, HWND, RGNDATA*);
using DrawIndexedPrimitive = long(__stdcall*)(IDirect3DDevice9*, D3DPRIMITIVETYPE, int, unsigned int, unsigned int, unsigned int, unsigned int);

//InPrediction oInPrediction = nullptr;
FireEventClientSide oFireEventClientSide = nullptr;
CreateMove oCreateMove = nullptr;
PaintTraverse oPaintTraverse = nullptr;
FrameStageNotify oFrameStageNotify = nullptr;
EndScene oEndScene = nullptr;
Reset oReset = nullptr;
Present oPresent = nullptr;
DrawIndexedPrimitive oDrawIndexedPrimitive = nullptr;

float HitmarkerAlpha = 0.0f;

Vector PlayerHeads[64];
Vector PlayerOrigins[64];
Color PlayerColors[64];
std::string PlayerNames[64];
std::string PlayerHealths[64];
std::string PlayerWeapons[64];

/*bool __stdcall hkInPrediction()
{
	if (g_Options.visuals_NoVisualRecoil)
	{
		QAngle viewPunch = g_LocalPlayer->m_viewPunchAngle();
		QAngle aimPunch = g_LocalPlayer->m_aimPunchAngle();

		g_LocalPlayer->m_viewPunchAngle() -= (viewPunch + (aimPunch * 2 * 0.4499999f));
	}

	return oInPrediction(g_Prediction);
}*/

void RenderMenu()
{
	static int lastTheme = 0;
	if (g_Options.visuals_MenuStyle != lastTheme)
	{
		switch (g_Options.visuals_MenuStyle)
		{
		case THEME_GREY: Theme_Grey(); break;
		case THEME_RED: Theme_Red(); break;
		case THEME_BLACK: Theme_Black(); break;
		case THEME_BLUE: Theme_Blue(); break;
		}
		lastTheme = g_Options.visuals_MenuStyle;
	}

	ImGui::Begin("OVERDRIVE.ME", &visible, ImVec2(630, 375), 1.00f, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_AlwaysAutoResize);
	{
		if (ImGui::Button(ICON_FA_CROSSHAIRS, ImVec2((630.0f - 24.0f) / 5.0f, 25))) tabIndex = TAB_AIMBOT;
		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_EYE, ImVec2((630.0f - 24.0f) / 5.0f, 25))) tabIndex = TAB_VISUALS;
		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_SLIDERS, ImVec2((630.0f - 24.0f) / 5.0f, 25))) tabIndex = TAB_MISC;
		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_PAINT_BRUSH, ImVec2((630.0f - 24.0f) / 5.0f, 25))) tabIndex = TAB_SKINS;
		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_WRENCH, ImVec2((630.0f - 24.0f) / 5.0f, 25))) tabIndex = TAB_CONFIG;

		ImGui::Separator();

		switch (tabIndex)
		{
		case TAB_AIMBOT:
			if (ImGui::Button(ICON_FA_WARNING, ImVec2((630.0f - 12.0f) / 2.0f, 25))) aimbotIndex = AIMBOTTAB_HVH;
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_LOCK, ImVec2((630.0f - 12.0f) / 2.0f, 25))) aimbotIndex = AIMBOTTAB_AIMBOT;

			ImGui::Separator();

			switch (aimbotIndex)
			{
			case AIMBOTTAB_HVH:
				ImGuiCustom::Checkbox("ANTI RESOLVER", &g_Options.aimbot_antiaim_antiresolver);
				ImGuiCustom::Checkbox("RESOLVER", &g_Options.aimbot_resolver);
				ImGuiCustom::Checkbox("ANTIAIM ON KNIFE", &g_Options.aimbot_antiaim_knife);
				ImGuiCustom::Checkbox("ADAPTIVE FAKE LAG", &g_Options.aimbot_FakeLag_Adaptive);
				ImGui::Combo("PITCH", &g_Options.aimbot_antiaim_pitch, AntiAimPitch, ARRAYSIZE(AntiAimPitch));
				ImGui::Combo("REAL YAW", &g_Options.aimbot_antiaim_yaw, AntiAimYaw, ARRAYSIZE(AntiAimYaw));
				ImGui::Combo("FAKE YAW", &g_Options.aimbot_antiaim_yaw_fake, AntiAimYaw, ARRAYSIZE(AntiAimYaw));
				ImGui::Combo("THIRDPERSON ANGLE", &g_Options.aimbot_antiaim_thirdperson_angle, AntiAimThirdperson, ARRAYSIZE(AntiAimThirdperson));
				ImGui::SliderFloat("ANTIAIM EDGE DISTANCE", &g_Options.aimbot_antiaim_edge_dist, 0.0f, 100.0f);
				ImGui::SliderInt("FAKE LAG", &g_Options.aimbot_FakeLag, 0, 14);
				break;
			case AIMBOTTAB_AIMBOT:
				if (ImGui::Button("Pistols", ImVec2((630.0f - 24.0f) / 5.0f, 25))) legitbotIndex = Legit_Pistol;
				ImGui::SameLine();
				if (ImGui::Button("Rifles", ImVec2((630.0f - 24.0f) / 5.0f, 25))) legitbotIndex = Legit_Rifle;
				ImGui::SameLine();
				if (ImGui::Button("Snipers", ImVec2((630.0f - 24.0f) / 5.0f, 25))) legitbotIndex = Legit_Sniper;
				ImGui::SameLine();
				if (ImGui::Button("Shotguns", ImVec2((630.0f - 24.0f) / 5.0f, 25))) legitbotIndex = Legit_Shotgun;
				ImGui::SameLine();
				if (ImGui::Button("SMGs", ImVec2((630.0f - 24.0f) / 5.0f, 25))) legitbotIndex = Legit_SMG;
				switch (legitbotIndex) {
				case Legit_Pistol:
					ImGuiCustom::Checkbox("AIMKEY", &g_Options.aimbot_aimkey);
					ImGuiCustom::Checkbox("VISCHECK", &g_Options.aimbot_vischeck);
					ImGuiCustom::Checkbox("SILENT", &g_Options.aimbot_silent);
					ImGui::Combo("PISTOL HITBOX", &g_Options.aimbot_pistolhitbox, hitboxes, ARRAYSIZE(hitboxes));
					ImGui::SliderFloat("PISTOL FOV", &g_Options.aimbot_pistolFOV, 0.0f, 180.0f);
					ImGui::SliderFloat("PISTOL SMOOTH", &g_Options.aimbot_pistolsmooth, 0.0f, 1.0f);
					ImGui::SliderFloat("AIMBOT RCS PITCH", &g_Options.aimbot_RCS_pitch, 0.0f, 2.0f);
					ImGui::SliderFloat("AIMBOT RCS YAW", &g_Options.aimbot_RCS_yaw, 0.0f, 2.0f);
					break;
				case Legit_Rifle:
					ImGuiCustom::Checkbox("AIMKEY", &g_Options.aimbot_aimkey);
					ImGuiCustom::Checkbox("VISCHECK", &g_Options.aimbot_vischeck);
					ImGuiCustom::Checkbox("SILENT", &g_Options.aimbot_silent);
					ImGui::Combo("RIFLE HITBOX", &g_Options.aimbot_riflehitbox, hitboxes, ARRAYSIZE(hitboxes));
					ImGui::SliderFloat("RIFLE FOV", &g_Options.aimbot_rifleFOV, 0.0f, 180.0f);
					ImGui::SliderFloat("RIFLE SMOOTH", &g_Options.aimbot_riflesmooth, 0.0f, 1.0f);
					ImGui::SliderFloat("AIMBOT RCS PITCH", &g_Options.aimbot_RCS_pitch, 0.0f, 2.0f);
					ImGui::SliderFloat("AIMBOT RCS YAW", &g_Options.aimbot_RCS_yaw, 0.0f, 2.0f);
					ImGui::SliderInt("AIMBOT SHOTS (RIFLES)", &g_Options.aimbot_shots, 0, 30);
					break;
				case Legit_Sniper:
					ImGuiCustom::Checkbox("AIMKEY", &g_Options.aimbot_aimkey);
					ImGuiCustom::Checkbox("VISCHECK", &g_Options.aimbot_vischeck);
					ImGuiCustom::Checkbox("SILENT", &g_Options.aimbot_silent);
					ImGui::Combo("SNIPER HITBOX", &g_Options.aimbot_sniperhitbox, hitboxes, ARRAYSIZE(hitboxes));
					ImGui::SliderFloat("SNIPER FOV", &g_Options.aimbot_sniperFOV, 0.0f, 180.0f);
					ImGui::SliderFloat("SNIPER SMOOTH", &g_Options.aimbot_snipersmooth, 0.0f, 1.0f);
					ImGui::SliderFloat("AIMBOT RCS PITCH", &g_Options.aimbot_RCS_pitch, 0.0f, 2.0f);
					ImGui::SliderFloat("AIMBOT RCS YAW", &g_Options.aimbot_RCS_yaw, 0.0f, 2.0f);
					break;
				case Legit_Shotgun:
					ImGuiCustom::Checkbox("AIMKEY", &g_Options.aimbot_aimkey);
					ImGuiCustom::Checkbox("VISCHECK", &g_Options.aimbot_vischeck);
					ImGuiCustom::Checkbox("SILENT", &g_Options.aimbot_silent);
					ImGui::Combo("SHOTGUN HITBOX", &g_Options.aimbot_shotgunhitbox, hitboxes, ARRAYSIZE(hitboxes));
					ImGui::SliderFloat("SHOTGUN FOV", &g_Options.aimbot_shotgunFOV, 0.0f, 180.0f);
					ImGui::SliderFloat("SHOTGUN SMOOTH", &g_Options.aimbot_shotgunsmooth, 0.0f, 1.0f);
					ImGui::SliderFloat("AIMBOT RCS PITCH", &g_Options.aimbot_RCS_pitch, 0.0f, 2.0f);
					ImGui::SliderFloat("AIMBOT RCS YAW", &g_Options.aimbot_RCS_yaw, 0.0f, 2.0f);
					break;
				case Legit_SMG:
					ImGuiCustom::Checkbox("AIMKEY", &g_Options.aimbot_aimkey);
					ImGuiCustom::Checkbox("VISCHECK", &g_Options.aimbot_vischeck);
					ImGuiCustom::Checkbox("SILENT", &g_Options.aimbot_silent);
					ImGui::Combo("SMG HITBOX", &g_Options.aimbot_smghitbox, hitboxes, ARRAYSIZE(hitboxes));
					ImGui::SliderFloat("SMG FOV", &g_Options.aimbot_SMGFOV, 0.0f, 180.0f);
					ImGui::SliderFloat("SMG SMOOTH", &g_Options.aimbot_SMGsmooth, 0.0f, 1.0f);
					ImGui::SliderFloat("AIMBOT RCS PITCH", &g_Options.aimbot_RCS_pitch, 0.0f, 2.0f);
					ImGui::SliderFloat("AIMBOT RCS YAW", &g_Options.aimbot_RCS_yaw, 0.0f, 2.0f);
					ImGui::SliderInt("AIMBOT SHOTS (RIFLES)", &g_Options.aimbot_shots, 0, 30);
					break;
				}
				break;
			}
			break;
		case TAB_VISUALS:
			ImGuiCustom::Checkbox("WATERMARK", &g_Options.visuals_Watermark);
			ImGuiCustom::Checkbox("AIMBOT FOV CIRCLE", &g_Options.visuals_AimbotFOVCircle);
			ImGuiCustom::Checkbox("PLAYER ESP", &g_Options.visuals_ESP);
			ImGui::Combo("HITMARKER STYLES", &g_Options.visuals_hitmarker_style, hitmarker_styles, ARRAYSIZE(hitmarker_styles));
			break;
		case TAB_MISC:
			ImGuiCustom::Checkbox("BUNNYHOP", &g_Options.misc_Bhop);
			ImGuiCustom::Checkbox("STREAM PROOFING", &g_Options.visuals_StreamProofing);
			ImGuiCustom::Checkbox("NO VISUAL RECOIL", &g_Options.visuals_NoVisualRecoil);
			ImGuiCustom::Checkbox("NO FLASH", &g_Options.misc_noflash);
			ImGui::Combo("MENU THEME", &g_Options.visuals_MenuStyle, MenuThemes, ARRAYSIZE(MenuThemes));
			ImGui::SliderFloat("MENU FADE SPEED", &g_Options.visuals_MenuFadeSpeed, 0.50f, 5.0f);
			break;
		case TAB_SKINS:
			break;
		case TAB_CONFIG:
			break;
		}
	}
	ImGui::End();
}

void RenderFrame(IDirect3DDevice9* device)
{
	if (!Initialized)
	{
		// Getting device
		g_Renderer = make_unique<DrawManager>(device);
		InitializeGUI(device);
		Initialized = true;
	}
	// Rendering the menu
	ImGui::GetIO().MouseDrawCursor = visible;
	ImGui_ImplDX9_NewFrame();
	if (g_EngineClient->IsInGame() && g_LocalPlayer)
	{
		g_Renderer->BeginRendering();
		// Getting screen resolution
		int w, h;
		g_EngineClient->GetScreenSize(w, h);

		if (g_Options.visuals_ESP)
		{
			if (g_EngineClient->IsInGame() && g_LocalPlayer)
			{
				for (int i = 0; i < g_EngineClient->GetMaxClients(); i++)
				{
					CBaseEntity* pEntity = g_EntityList->GetClientEntity(i);
					if (pEntity && pEntity != g_LocalPlayer
						&& pEntity->IsAlive()
						&& pEntity->m_iTeamNum() != g_LocalPlayer->m_iTeamNum()
						&& pEntity->IsPlayer()
						&& !pEntity->m_bDormant())
					{
						g_Renderer->AddCircle(ImVec2(PlayerHeads[i].x, PlayerHeads[i].y), 5, D3DCOLOR_RGBA(0, PlayerColors[i].g(), PlayerColors[i].r(), 255), 25);
						g_Renderer->AddText(ImVec2(PlayerOrigins[i].x, PlayerOrigins[i].y + 10), D3DCOLOR_RGBA(0, PlayerColors[i].g(), PlayerColors[i].r(), 255), drop_shadow | centered_x | centered_y, PlayerHealths[i].c_str());
						g_Renderer->AddText(ImVec2(PlayerOrigins[i].x, PlayerOrigins[i].y + 25), D3DCOLOR_RGBA(0, PlayerColors[i].g(), PlayerColors[i].r(), 255), drop_shadow | centered_x | centered_y, PlayerNames[i].c_str());
						g_Renderer->AddText(ImVec2(PlayerOrigins[i].x, PlayerOrigins[i].y + 40), D3DCOLOR_RGBA(0, PlayerColors[i].g(), PlayerColors[i].r(), 255), drop_shadow | centered_x | centered_y, PlayerWeapons[i].c_str());
					}
				}
			}
		}
		// Hitmarker
		if (g_Options.visuals_hitmarker_style > 0 && HitmarkerAlpha >= 0.006f && g_LocalPlayer->IsAlive())
		{
			g_Renderer->AddLine(ImVec2(w / 2 - 5, h / 2 - 10), ImVec2(w / 2 - 5, h / 2 - 5), D3DCOLOR_RGBA(255, 255, 255, (int)(HitmarkerAlpha * 255)));
			g_Renderer->AddLine(ImVec2(w / 2 - 5, h / 2 + 10), ImVec2(w / 2 - 5, h / 2 + 5), D3DCOLOR_RGBA(255, 255, 255, (int)(HitmarkerAlpha * 255)));
			g_Renderer->AddLine(ImVec2(w / 2 + 5, h / 2 - 10), ImVec2(w / 2 + 5, h / 2 - 5), D3DCOLOR_RGBA(255, 255, 255, (int)(HitmarkerAlpha * 255)));
			g_Renderer->AddLine(ImVec2(w / 2 + 5, h / 2 + 10), ImVec2(w / 2 + 5, h / 2 + 5), D3DCOLOR_RGBA(255, 255, 255, (int)(HitmarkerAlpha * 255)));
			HitmarkerAlpha -= 0.006f;
		}
		if (g_Options.visuals_AimbotFOVCircle)
		{
			if (g_EngineClient->IsInGame() && g_LocalPlayer->IsAlive())
			{
				float Radius;
				float fov;
				if (g_LocalPlayer->m_hActiveWeapon()->IsPistol()) {
					fov = g_Options.aimbot_pistolFOV;
				}
				if (g_LocalPlayer->m_hActiveWeapon()->IsRifle()) {
					fov = g_Options.aimbot_rifleFOV;
				}
				if (g_LocalPlayer->m_hActiveWeapon()->IsSniper()) {
					fov = g_Options.aimbot_sniperFOV;
				}
				if (g_LocalPlayer->m_hActiveWeapon()->IsSMG()) {
					fov = g_Options.aimbot_SMGFOV;
				}
				if (g_LocalPlayer->m_hActiveWeapon()->IsShotgun()) {
					fov = g_Options.aimbot_shotgunFOV;
				}
				if (g_LocalPlayer->m_hActiveWeapon()->IsKnife() || g_LocalPlayer->m_hActiveWeapon()->IsGrenade() || g_LocalPlayer->m_hActiveWeapon()->IsDefuseKit()) {
					fov = 0.f;
				}
				Radius = tanf((DEG2RAD(fov)) / 6) / tanf(97) * w;
				g_Renderer->AddCircle(ImVec2(w / 2, h / 2), Radius, D3DCOLOR_RGBA(0, 255, 0, 255), fov * 10.0f, 0.5f);
			}
		}
		// Watermark
		if (g_Options.visuals_Watermark)
		{
			g_Renderer->AddText(ImVec2(25, 10), D3DCOLOR_RGBA(rainbow().r(), rainbow().g(), rainbow().b(), rainbow().a()), drop_shadow, "OVERDRIVE.ME v1.0");
		}

		g_Renderer->EndRendering();
	}

	static float menuFadeTimer = 0.0f;
	if (visible)
	{
		if (menuFadeTimer <= 1.0f - (g_Options.visuals_MenuFadeSpeed / 100.0f)) menuFadeTimer += (g_Options.visuals_MenuFadeSpeed / 100.0f);
		style.Alpha = menuFadeTimer;
		RenderMenu();
	}
	else if (menuFadeTimer > (g_Options.visuals_MenuFadeSpeed / 100.0f) && !visible)
	{
		menuFadeTimer -= (g_Options.visuals_MenuFadeSpeed / 100.0f);
		style.Alpha = menuFadeTimer;
		RenderMenu();
	}
	ImGui::Render();
}

long GenerateShader(IDirect3DDevice9* device, IDirect3DPixelShader9** Shader, float r, float g, float b)
{
	char ShaderAsmBuf[256];
	ID3DXBuffer* ShaderBuf = NULL;
	sprintf_s(ShaderAsmBuf, "ps_3_0\ndef c0, %f, %f, %f, %f\nmov oC0,c0", r, g, b, 1.0f);
	if (FAILED(D3DXAssembleShader(ShaderAsmBuf, (strlen(ShaderAsmBuf) + 1), NULL, NULL, 0, &ShaderBuf, NULL))) return E_FAIL;
	return device->CreatePixelShader((const DWORD*)ShaderBuf->GetBufferPointer(), Shader);
}

long GenerateTexture(IDirect3DDevice9* device, IDirect3DTexture9** ppD3Dtex, DWORD color)
{
	HRESULT hr = device->CreateTexture(8, 8, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, ppD3Dtex, NULL);
	if (FAILED(hr)) return hr;
	D3DLOCKED_RECT d3dlr;
	(*ppD3Dtex)->LockRect(0, &d3dlr, 0, 0);
	BYTE* pDstRow = (BYTE*)d3dlr.pBits;
	DWORD* pDst32;
	for (int y = 0; y < 8; y++) 
	{
		pDst32 = (DWORD*)pDstRow;
		for (int x = 0; x < 8; x++) *pDst32++ = color;
		pDstRow += d3dlr.Pitch;
	}
	(*ppD3Dtex)->UnlockRect(0);
	return S_OK;
}

// Chams textures and shaders
static bool ShadersCreated = false;
IDirect3DPixelShader9* RedShader;
IDirect3DPixelShader9* GreenShader;
IDirect3DTexture9* RedTexture;
IDirect3DTexture9* GreenTexture;
IDirect3DTexture9* TerrorVisible;
IDirect3DTexture9* Terror;
IDirect3DTexture9* CTerrorVisible;
IDirect3DTexture9* CTerror;
IDirect3DTexture9* WeaponCham;
long __stdcall hkDrawIndexedPrimitive(IDirect3DDevice9* device, D3DPRIMITIVETYPE PrimType, int BaseVertexIndex, unsigned int MinVertexIndex, unsigned int NumVertices, unsigned int startIndex, unsigned int primCount)
{
	IDirect3DVertexBuffer9* sdata;
	unsigned int Offset, mStride;

	if (device->GetStreamSource(0, &sdata, &Offset, &mStride) == D3D_OK) sdata->Release();

	if (!ShadersCreated)
	{
		GenerateShader(device, &RedShader, 1.0f, 0.0f, 0.0f);
		GenerateShader(device, &GreenShader, 0.0f, 1.0f, 0.0f);
		GenerateTexture(device, &CTerrorVisible, D3DCOLOR_ARGB(255, 154, 205, 50));
		GenerateTexture(device, &CTerror, D3DCOLOR_ARGB(255, 70, 130, 180));
		GenerateTexture(device, &TerrorVisible, D3DCOLOR_ARGB(255, 255, 255, 0));
		GenerateTexture(device, &Terror, D3DCOLOR_ARGB(255, 220, 20, 60));
		GenerateTexture(device, &WeaponCham, D3DCOLOR_ARGB(255, 255, 255, 255));
		ShadersCreated = true;
	}

	if (PLAYERS)
	{
		device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		device->SetRenderState(D3DRS_ZFUNC, D3DCMP_NEVER);
		device->SetTexture(0, CTerrorVisible);
		oDrawIndexedPrimitive(device, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
		device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		device->SetTexture(0, CTerror);
	}

	if (WEAPONS)
	{
		device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		device->SetRenderState(D3DRS_ZFUNC, D3DCMP_NEVER);
		device->SetTexture(0, WeaponCham);
		oDrawIndexedPrimitive(device, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
		device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		device->SetTexture(0, WeaponCham);
	}

	return oDrawIndexedPrimitive(device, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}

long __stdcall hkPresent(IDirect3DDevice9* device, RECT* src, RECT* dest, HWND wnd_override, RGNDATA* dirty_region)
{
	if (g_Options.visuals_StreamProofing) RenderFrame(device);
	return oPresent(device, src, dest, wnd_override, dirty_region);
}

long __stdcall hkEndScene(IDirect3DDevice9* device)
{
	if (!g_Options.visuals_StreamProofing) RenderFrame(device);
	return oEndScene(device);
}

// D3D Reset Hook
long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	if (!Initialized) return oReset(device, pPresentationParameters);
	if (ShadersCreated)
	{
		RedTexture->Release();
		GreenTexture->Release();
		RedShader->Release();
		GreenShader->Release();
		ShadersCreated = false;
	}
	ImGui_ImplDX9_InvalidateDeviceObjects();
	g_Renderer->InvalidateObjects();
	ImGui_ImplDX9_CreateDeviceObjects();
	g_Renderer->CreateObjects();
	return oReset(device, pPresentationParameters);
}

bool hasAimbotTarget = false;

void DoAimbot(CUserCmd* pCmd)
{
	if (g_Options.aimbot_aimkey && !GetAsyncKeyState(VK_LBUTTON)) return;
	CBaseEntity* pClosest = GetClosestPlayerToCrosshair();
	if (pClosest && pClosest->m_iTeamNum() != g_LocalPlayer->m_iTeamNum() && !pClosest->m_bDormant() && pClosest->IsAlive() && !pClosest->IsImmune())
	{
		if (g_Options.aimbot_vischeck && !IsVisible(pClosest, true)) return;
		Vector BonePos;
		if (g_LocalPlayer->m_hActiveWeapon()->IsPistol()) {
			switch (g_Options.aimbot_pistolhitbox)
			{
			case 0: BonePos = pClosest->GetBonePos(head_0); break;
			case 1: BonePos = pClosest->GetBonePos(neck_0); break;
			case 2: BonePos = pClosest->GetBonePos(spine_2); break;
			case 3: BonePos = pClosest->GetBonePos(spine_3); break;
			case 4: BonePos = pClosest->GetBonePos(pelvis); break;
			}
		}
		if (g_LocalPlayer->m_hActiveWeapon()->IsRifle()) {
			switch (g_Options.aimbot_riflehitbox)
			{
			case 0: BonePos = pClosest->GetBonePos(head_0); break;
			case 1: BonePos = pClosest->GetBonePos(neck_0); break;
			case 2: BonePos = pClosest->GetBonePos(spine_2); break;
			case 3: BonePos = pClosest->GetBonePos(spine_3); break;
			case 4: BonePos = pClosest->GetBonePos(pelvis); break;
			}
		}
		if (g_LocalPlayer->m_hActiveWeapon()->IsSniper()) {
			switch (g_Options.aimbot_sniperhitbox)
			{
			case 0: BonePos = pClosest->GetBonePos(head_0); break;
			case 1: BonePos = pClosest->GetBonePos(neck_0); break;
			case 2: BonePos = pClosest->GetBonePos(spine_2); break;
			case 3: BonePos = pClosest->GetBonePos(spine_3); break;
			case 4: BonePos = pClosest->GetBonePos(pelvis); break;
			}
		}
		if (g_LocalPlayer->m_hActiveWeapon()->IsSMG()) {
			switch (g_Options.aimbot_smghitbox)
			{
			case 0: BonePos = pClosest->GetBonePos(head_0); break;
			case 1: BonePos = pClosest->GetBonePos(neck_0); break;
			case 2: BonePos = pClosest->GetBonePos(spine_2); break;
			case 3: BonePos = pClosest->GetBonePos(spine_3); break;
			case 4: BonePos = pClosest->GetBonePos(pelvis); break;
			}
		}
		if (g_LocalPlayer->m_hActiveWeapon()->IsShotgun()) {
			switch (g_Options.aimbot_shotgunhitbox)
			{
			case 0: BonePos = pClosest->GetBonePos(head_0); break;
			case 1: BonePos = pClosest->GetBonePos(neck_0); break;
			case 2: BonePos = pClosest->GetBonePos(spine_2); break;
			case 3: BonePos = pClosest->GetBonePos(spine_3); break;
			case 4: BonePos = pClosest->GetBonePos(pelvis); break;
			}
		}
		if (g_LocalPlayer->m_hActiveWeapon()->IsKnife() || g_LocalPlayer->m_hActiveWeapon()->IsGrenade() || g_LocalPlayer->m_hActiveWeapon()->IsDefuseKit()) {
			BonePos = pClosest->GetBonePos(head_0);
		}

		QAngle FinalAngle = CalcAngle(g_LocalPlayer->GetEyePosition(), BonePos);
		if (!g_Options.aimbot_silent)
		{
			QAngle ViewAngles;
			g_EngineClient->GetViewAngles(ViewAngles);
			QAngle ToChange = Smooth(FinalAngle, ViewAngles);
			QAngle Final = ViewAngles + ToChange;
			NormalizeAngles(Final);
			ClampAngles(Final);
			g_EngineClient->SetViewAngles(Final);
		}
		else if (g_Options.aimbot_silent) pCmd->viewangles = FinalAngle;
		NormalizeAngles(pCmd->viewangles);
		ClampAngles(pCmd->viewangles);
	}
}

// CreateMove Hook
void __stdcall hkCreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket)
{
	oCreateMove(g_CHLClient, sequence_number, input_sample_frametime, active);

	auto pCmd = g_Input->GetUserCmd(sequence_number);
	auto pCmdVerified = g_Input->GetVerifiedCmd(sequence_number);

	if (!g_LocalPlayer || !pCmd)
		return;

	// Old pCmd
	QAngle oldAngle = pCmd->viewangles;
	float oldForward = pCmd->forwardmove;
	float oldSideMove = pCmd->sidemove;

	// Reseting aimbot target
	hasAimbotTarget = false;

	// Bhop
	if (g_Options.misc_Bhop
		&& (pCmd->buttons & IN_JUMP)
		&& !(g_LocalPlayer->m_fFlags() & FL_ONGROUND)
		&& g_EngineClient->IsInGame()
		&& g_LocalPlayer->IsAlive())
		pCmd->buttons &= ~IN_JUMP;

	// Aimbot
	if (g_EngineClient->IsInGame() && g_LocalPlayer->IsAlive() && g_LocalPlayer->m_iShotsFired() >= g_Options.aimbot_shots) DoAimbot(pCmd);
	else if (g_EngineClient->IsInGame() && g_LocalPlayer->IsAlive() && g_Options.aimbot_shots <= 0) DoAimbot(pCmd);

	// RCS
	if ((g_Options.aimbot_RCS_pitch > 0.0f || g_Options.aimbot_RCS_yaw > 0.0f) && g_EngineClient->IsInGame() && g_LocalPlayer->IsAlive())
	{
		QAngle punchAngles = g_LocalPlayer->m_aimPunchAngle();
		if (punchAngles.pitch != 0.0f || punchAngles.yaw != 0.0f)
		{
			pCmd->viewangles.pitch -= g_LocalPlayer->m_aimPunchAngle().pitch * g_Options.aimbot_RCS_pitch;
			pCmd->viewangles.yaw -= g_LocalPlayer->m_aimPunchAngle().yaw * g_Options.aimbot_RCS_yaw;
		}
		ClampAngles(pCmd->viewangles);
		NormalizeAngles(pCmd->viewangles);
	}

	static int ticks = 0;
	static int ticksMax = 16;
	if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND) && !g_Options.aimbot_FakeLag_Adaptive && g_EngineClient->IsInGame() && g_LocalPlayer->IsAlive())
	{
		if (!(pCmd->buttons & IN_ATTACK))
		{
			if (ticks >= ticksMax)
			{
				SendPacket = true;
				ticks = 0;
			}
			else
			{
				SendPacket = ticks < 16 - g_Options.aimbot_FakeLag;
			}
		}
		else if (pCmd->buttons & IN_ATTACK)
		{
			SendPacket = true;
		}
	}
	else if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND) && g_Options.aimbot_FakeLag_Adaptive && g_EngineClient->IsInGame() && g_LocalPlayer->IsAlive())
	{
		if (ticks >= ticksMax)
		{
			SendPacket = true;
			ticks = 0;
		}
		else
		{
			int packetsToChoke;
			if (g_LocalPlayer->m_vecVelocity().Length() > 0.f)
			{
				packetsToChoke = (int)((64.f / g_GlobalVars->interval_per_tick) / g_LocalPlayer->m_vecVelocity().Length()) + 1;
				if (packetsToChoke >= 15) packetsToChoke = 14;
				if (packetsToChoke < g_Options.aimbot_FakeLag) packetsToChoke = g_Options.aimbot_FakeLag;
			}
			else packetsToChoke = 0;
			if (ticks < 16 - packetsToChoke) SendPacket = true;
			else SendPacket = false;
		}
	}
	else if (g_EngineClient->IsInGame() && g_LocalPlayer->IsAlive())
	{
		SendPacket = true;
	}
	ticks++;

	static bool press = false;
	if (GetAsyncKeyState(VK_MENU))
	{
		press = true;
	}
	else if (!GetAsyncKeyState(VK_MENU) && press)
	{
		if (g_Options.aimbot_antiaim_yaw == ANTIAIM_YAW_LEFT) g_Options.aimbot_antiaim_yaw = ANTIAIM_YAW_RIGHT;
		else if (g_Options.aimbot_antiaim_yaw == ANTIAIM_YAW_RIGHT) g_Options.aimbot_antiaim_yaw = ANTIAIM_YAW_LEFT;
		press = false;
	}

	// Antiaim
	if (g_EngineClient->IsInGame() && g_LocalPlayer->IsAlive())
	{
		if (!aimstepInProgress)
		{
			if (!(pCmd->buttons & IN_USE) && !(pCmd->buttons & IN_ATTACK) && !(pCmd->buttons & IN_ATTACK2))
			{
				QAngle edge_angle = pCmd->viewangles;
				bool edging_head = g_Options.aimbot_antiaim_edge_dist > 0.0f && GetBestHeadAngle(edge_angle);

				static bool bFlip;
				bFlip = !bFlip;

				// Untrusted Anti-Aim Check // SET TO NONE IF ANTIAIM IS UNTRUSTED
				if (g_Options.antiuntrusted)
				{
					// UT yaws
					// real yaws
				}

				if (g_Options.aimbot_antiaim_yaw > 0 || g_Options.aimbot_antiaim_yaw_fake > 0)
				{
					DoAntiAimY(pCmd->viewangles, bFlip, g_Options.antiuntrusted);
					NormalizeAngles(pCmd->viewangles);
					if (g_Options.aimbot_FakeLag <= 0)
						SendPacket = bFlip;
					else bFlip = SendPacket;
					if (g_Options.aimbot_antiaim_edge_dist > 0.0f && edging_head && !bFlip)
						pCmd->viewangles.yaw = edge_angle.yaw;
				}

				if (g_Options.aimbot_antiaim_pitch > 0)
					DoAntiAimX(pCmd->viewangles, pCmd->command_number, bFlip, g_Options.antiuntrusted);

				if (g_Options.antiuntrusted)
				{
					NormalizeAngles(pCmd->viewangles);
					ClampAngles(pCmd->viewangles);
				}

				if (g_Options.aimbot_antiaim_antiresolver)
				{
					static bool antiResolverFlip = false;
					if (pCmd->viewangles.yaw == g_LocalPlayer->m_flLowerBodyYawTarget())
					{
						if (antiResolverFlip) pCmd->viewangles.yaw += 60.f;
						else pCmd->viewangles.yaw -= 60.f;

						if (g_Options.antiuntrusted)
						{
							NormalizeAngles(pCmd->viewangles);
							ClampAngles(pCmd->viewangles);
						}

						antiResolverFlip = !antiResolverFlip;
					}
				}

				if (g_Options.aimbot_antiaim_thirdperson_angle == ANTIAIM_THIRDPERSON_BOTH)
					LastTickViewAngles = pCmd->viewangles;

				if (g_Options.aimbot_antiaim_thirdperson_angle == ANTIAIM_THIRDPERSON_NONE)
				{
					QAngle vAnglesxd;
					g_EngineClient->GetViewAngles(vAnglesxd);
					LastTickViewAngles = vAnglesxd;
				}
			}
		}
	}

	if (g_EngineClient->IsInGame() && g_LocalPlayer->IsAlive())
	{
		NormalizeAngles(pCmd->viewangles);
		ClampAngles(pCmd->viewangles);
	}

	if (!(pCmd->buttons & IN_ATTACK) && g_EngineClient->IsInGame() && g_LocalPlayer->IsAlive()) bSendPacket = SendPacket;
	else if ((pCmd->buttons & IN_ATTACK) && g_EngineClient->IsInGame() && g_LocalPlayer->IsAlive()) bSendPacket = true;
	else if (g_EngineClient->IsInGame() && !g_LocalPlayer->IsAlive()) bSendPacket = true;

	if (!bSendPacket && g_Options.aimbot_antiaim_thirdperson_angle == ANTIAIM_THIRDPERSON_REAL && (g_Options.aimbot_antiaim_yaw > 0 || g_Options.aimbot_antiaim_pitch > 0)) LastTickViewAngles = pCmd->viewangles;
	else if (bSendPacket && g_Options.aimbot_antiaim_thirdperson_angle == ANTIAIM_THIRDPERSON_FAKE && (g_Options.aimbot_antiaim_yaw_fake > 0 || g_Options.aimbot_antiaim_pitch > 0)) LastTickViewAngles = pCmd->viewangles;

	if ((pCmd->buttons & IN_ATTACK) 
		|| (pCmd->buttons & IN_ATTACK2) 
		|| (pCmd->buttons & IN_ATTACK3) 
		|| (pCmd->buttons & IN_USE) 
		|| (pCmd->buttons & IN_GRENADE1) 
		|| (pCmd->buttons & IN_GRENADE2))
	{
		QAngle vAngles;
		g_EngineClient->GetViewAngles(vAngles);
		LastTickViewAngles = vAngles;
	}

	// Movement correction
	CorrectMovement(oldAngle, pCmd, oldForward, oldSideMove);

	pCmdVerified->m_cmd = *pCmd;
	pCmdVerified->m_crc = pCmd->GetChecksum();
}

// CreateMove Hook Proxy
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
		call hkCreateMove
		pop  ebx
		pop  ebp
		retn 0Ch
	}
}

// PaintTraverse Hook | NOT STREAM SAFE
void __stdcall hkPaintTraverse(vgui::VPANEL panel, bool forceRepaint, bool allowForce)
{
	static auto panelId = vgui::VPANEL{ 0 };

	oPaintTraverse(g_VGuiPanel, panel, forceRepaint, allowForce);

	// Fonts
	static vgui::HFont watermark_font;
	static vgui::HFont esp_font;
	static bool FontsCreated = false;
	if (!FontsCreated)
	{
		watermark_font = g_VGuiSurface->CreateFont_();
		esp_font = g_VGuiSurface->CreateFont_();
		// Segoe UI Light is the best font IMO like this... no one uses and it seems  am the only person who uses it. It IS THE BEST FONT THERE IS FOR USER INTERFACES LIKE THIS
		g_VGuiSurface->SetFontGlyphSet(watermark_font, "Segoe UI Light", 20, 200, 0, 0, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);
		g_VGuiSurface->SetFontGlyphSet(esp_font, "Segoe UI Light", 16, 200, 0, 0, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);

		FontsCreated = true;
	}

	int w, h;
	g_EngineClient->GetScreenSize(w, h);

	if (!panelId)
	{
		const auto panelName = g_VGuiPanel->GetName(panel);
		if (!strcmp(panelName, "MatSystemTopPanel"))
		{
			panelId = panel;
		}
	}
	else if (panelId == panel)
	{
		for (int i = 0; i < g_EngineClient->GetMaxClients(); i++)
		{
			for (int tick = 0; tick <= 100; tick++)
			{
				g_VGuiSurface->DrawSetColor(255, 255, 255, 255);
				Vector scrn;
				WorldToScreen(Backtracking::PlayerRecords[i].headpos[tick], scrn);
				g_VGuiSurface->DrawFilledRect(scrn.x - 2, scrn.y - 2, scrn.x + 2, scrn.y + 2);
			}
		}
	}
}

// FrameStageNotify Hook
void __stdcall hkFrameStageNotify(ClientFrameStage_t stage)
{
	CBaseEntity* pLocal = g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());

	if (stage == FRAME_RENDER_START)
	{
		static auto viewmodel_fov = g_CVar->FindVar("viewmodel_fov");
		
		if (g_EngineClient->IsInGame() && g_LocalPlayer->IsAlive())
		{
			if (*(bool*)((uintptr_t)g_Input + 0xA5))
				*(QAngle*)((uintptr_t)pLocal + 0x31C8) = LastTickViewAngles;
		}

		if (g_EngineClient->IsInGame())
		{
			if (viewmodel_fov->nValue != 110) viewmodel_fov->SetValue(126);
		}

		if (g_Options.visuals_ESP)
		{
			if (g_EngineClient->IsInGame() && g_LocalPlayer)
			{
				for (int i = 0; i < g_EngineClient->GetMaxClients(); i++)
				{
					CBaseEntity* pEntity = g_EntityList->GetClientEntity(i);

					if (pEntity
						&& pEntity != g_LocalPlayer
						&& pEntity->IsAlive()
						&& pEntity->m_iTeamNum() != g_LocalPlayer->m_iTeamNum()
						&& pEntity->IsPlayer()
						&& !pEntity->m_bDormant())
					{
						player_info_s entityinfo;
						g_EngineClient->GetPlayerInfo(i, &entityinfo);

						Vector headpos = pEntity->GetBonePos(ECSPlayerBones::head_0);
						Vector origin = pEntity->m_vecAbsOrigin();
						Vector screen, screen1;
						WorldToScreen(headpos, screen);
						WorldToScreen(origin, screen1);

						PlayerHeads[i] = screen;
						PlayerOrigins[i] = screen1;
						PlayerNames[i] = entityinfo.szName;
						PlayerHealths[i] = std::to_string(pEntity->m_iHealth());
						PlayerWeapons[i] = std::to_string(*pEntity->m_hActiveWeapon()->GetCSWeaponData()->szPrintName);
						PlayerColors[i] = Color(GetColorFromHealth(pEntity).r(), GetColorFromHealth(pEntity).g(), 0, 255);
					}
				}
			}
		}
	}

	if (stage == FRAME_NET_UPDATE_START)
	{
		
	}

	if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
	{
		if (g_EngineClient->IsInGame() && g_LocalPlayer)
		{
			if (g_Options.misc_noflash) *(float*)((uintptr_t)pLocal + 0xA2E8) = 0.0f;

			if (g_Options.aimbot_resolver)
			{
				for (int i = 1; i < g_EngineClient->GetMaxClients(); ++i)
				{
					CBaseEntity* pEntity = g_EntityList->GetClientEntity(i);
					static float OldYawDeltas[64] = { 0.0f };

					if (pEntity
						&& pEntity != g_LocalPlayer
						&& !pEntity->m_bDormant()
						&& pEntity->IsAlive()
						&& !pEntity->IsImmune()
						&& pEntity->m_iTeamNum() != g_LocalPlayer->m_iTeamNum())
					{
						Resolver::ResolvePlayer(pLocal, pEntity);
					}
				}
			}
		}
	}

	oFrameStageNotify(g_CHLClient, stage);
}

// Events Hook
bool __stdcall hkFireEventClientSide(IGameEvent* pEvent)
{
	if (!pEvent) return oFireEventClientSide(g_GameEvents, pEvent);

	if (g_EngineClient->IsInGame())
	{
		CBaseEntity* pLocal = g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());

		if (!strcmp(pEvent->GetName(), "player_hurt"))
		{
			if (g_EngineClient->GetPlayerForUserID(pEvent->GetInt("attacker")) == g_EngineClient->GetLocalPlayer())
			{
				CBaseEntity* pEntity = g_EntityList->GetClientEntity(g_EngineClient->GetPlayerForUserID(pEvent->GetInt("userid")));

				if (pLocal
					&& pEntity
					&& pEntity->m_iTeamNum() != g_LocalPlayer->m_iTeamNum())
				{
					switch (g_Options.visuals_hitmarker_style)
					{
					case 1: PlaySound("C:\\Program Files (x86)\\Steam\\steamapps\\common\\Counter-Strike Global Offensive\\hitsound.wav", NULL, SND_FILENAME | SND_ASYNC); break;
					case 2: PlaySound("C:\\Program Files (x86)\\Steam\\steamapps\\common\\Counter-Strike Global Offensive\\1tap.wav", NULL, SND_FILENAME | SND_ASYNC); break;
					}

					HitmarkerAlpha = 1.0f;
				}
			}
		}

		if (pLocal->m_iShotsFired() > 0)
		{
			if (!strcmp(pEvent->GetName(), "player_hurt"))
			{
				CBaseEntity* pEntity = g_EntityList->GetClientEntity(g_EngineClient->GetPlayerForUserID(pEvent->GetInt("userid")));

				if (pLocal
					&& pEntity
					&& Resolver::pCurResolverEnt
					&& Resolver::pCurResolverEnt->m_iTeamNum() != g_LocalPlayer->m_iTeamNum()
					&& pEntity == Resolver::pCurResolverEnt)
				{
					if (g_EngineClient->GetPlayerForUserID(pEvent->GetInt("attacker")) == g_EngineClient->GetLocalPlayer()) Resolver::shot = 2;
				}
			}
			else Resolver::shot = 1;
		}
		else Resolver::shot = 0;
	}

	return oFireEventClientSide(g_GameEvents, pEvent);
}

class CHooks
{
public:

	void Hook()
	{
		// CANT SIG THIS VOLVO

		// INITIALIZING THE HOOKS HERE

		auto hlclient_hook = new CVMTHookManager(g_CHLClient);
		auto vguipanel_hook = new CVMTHookManager(g_VGuiPanel);
		auto direct3d_hook = new CVMTHookManager(g_D3DDevice9);
		auto events_hook = new CVMTHookManager(g_GameEvents);
		//auto prediction_hook = new CVMTHookManager(g_Prediction);

		oFireEventClientSide = (FireEventClientSide)events_hook->HookFunction(9, hkFireEventClientSide);
		oCreateMove = (CreateMove)hlclient_hook->HookFunction(21, hkCreateMove_Proxy);
		oPaintTraverse = (PaintTraverse)vguipanel_hook->HookFunction(41, hkPaintTraverse);
		oFrameStageNotify = (FrameStageNotify)hlclient_hook->HookFunction(36, hkFrameStageNotify);
		oEndScene = (EndScene)direct3d_hook->HookFunction(42, hkEndScene);
		oReset = (Reset)direct3d_hook->HookFunction(16, hkReset);
		oPresent = (Present)direct3d_hook->HookFunction(17, hkPresent);
		oDrawIndexedPrimitive = (DrawIndexedPrimitive)direct3d_hook->HookFunction(82, hkDrawIndexedPrimitive);
		//oInPrediction = (InPrediction)prediction_hook->HookFunction(14, hkInPrediction);

		hlclient_hook->HookTable(true);
		vguipanel_hook->HookTable(true);
		direct3d_hook->HookTable(true);
		events_hook->HookTable(true);
		//prediction_hook->HookTable(true);

		// CANT SIG THIS VOLVO

		// UNLOADING THE CHEAT HERE

		/*
		
		while (!GetAsyncKeyState(VK_F1))
		{
		Sleep(100);
		}

		hlclient_hook->HookTable(false);
		vguipanel_hook->HookTable(false);
		direct3d_hook->HookTable(false);
		events_hook->HookTable(false);
		//prediction_hook->HookTable(false);

		static auto cl_mouseenable = g_CVar->FindVar("cl_mouseenable");
		cl_mouseenable->SetValue(true);

		static auto crosshair = g_CVar->FindVar("crosshair");
		crosshair->SetValue(true);

		static auto mat_postprocess_enable = g_CVar->FindVar("mat_postprocess_enable");
		mat_postprocess_enable->SetValue(true);

		static auto viewmodel_fov = g_CVar->FindVar("viewmodel_fov");
		viewmodel_fov->SetValue(68);
		
		*/
	}
};
CHooks g_Hooks;