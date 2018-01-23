#pragma once
#include <Windows.h>

enum MenuTabs_t
{
	TAB_AIMBOT,
	TAB_VISUALS,
	TAB_MISC,
	TAB_SKINS,
	TAB_CONFIG
};

enum AimbotTabs_t
{
	AIMBOTTAB_HVH,
	AIMBOTTAB_AIMBOT
};

enum Legitbot_Tabs
{
	Legit_Pistol,
	Legit_Rifle,
	Legit_Sniper,
	Legit_Shotgun,
	Legit_SMG
};

enum MenuThemes_t
{
	THEME_GREY,
	THEME_RED,
	THEME_BLACK,
	THEME_BLUE
};

static char* MenuThemes[] =
{
	"GREY",
	"RED",
	"BLACK",
	"BLUE"
};

enum AntiAimYaw_t
{
	ANTIAIM_YAW_NONE,
	ANTIAIM_YAW_SPIN,
	ANTIAIM_YAW_STATIC_FORWARD,
	ANTIAIM_YAW_STATIC_RIGHT,
	ANTIAIM_YAW_STATIC_BACKWARDS,
	ANTIAIM_YAW_STATIC_LEFT,
	ANTIAIM_YAW_BACKWARDS,
	ANTIAIM_YAW_LEFT,
	ANTIAIM_YAW_RIGHT,
	ANTIAIM_YAW_SIDE,
	ANTIAIM_YAW_FAKE_LBY1,
	ANTIAIM_YAW_FAKE_LBY2,
	ANTIAIM_YAW_JITTER,
	ANTIAIM_YAW_BACKJITTER,
	ANTIAIM_YAW_FAKE_SIDE_LBY,
	ANTIAIM_YAW_RANDOM
};

static char* AntiAimYaw[] =
{
	"NONE",
	"SPIN",
	"STATIC FORWARD",
	"STATIC RIGHT",
	"STATIC BACKWARDS",
	"STATIC LEFT",
	"BACKWARDS",
	"LEFT",
	"RIGHT",
	"SIDE",
	"FAKE LBY",
	"FAKE LBY JITTER",
	"JITTER",
	"BACKJITTER",
	"FAKE SIDE LBY",
	"RANDOM"
};

enum AntiAimPitch_t
{
	ANTIAIM_PITCH_NONE,
	ANTIAIM_PITCH_DOWN,
	ANTIAIM_PITCH_UP,
};

static char* AntiAimPitch[] =
{
	"NONE",
	"DOWN",
	"UP",
};

enum AntiAimThirdperson_t
{
	ANTIAIM_THIRDPERSON_NONE,
	ANTIAIM_THIRDPERSON_REAL,
	ANTIAIM_THIRDPERSON_FAKE,
	ANTIAIM_THIRDPERSON_BOTH
};

static char* AntiAimThirdperson[] =
{
	"NONE",
	"REAL",
	"FAKE",
	"ALL"
};

static char* hitboxes[] =
{
	"HEAD",
	"NECK",
	"SPINE",
	"BODY",
	"PELVIS"
};
static char* hitmarker_styles[] =
{
	"NONE",
	"NORMAL",
	"1TAPPY"
};
extern bool aimstepInProgress;
typedef bool bSendPacket;
extern bSendPacket SendPacket;
extern WNDPROC oWndProc;
extern bool visible;
class Options 
{
public:

	bool antiuntrusted = true;

	bool aimbot_aimkey = false;
	bool aimbot_vischeck = false;


	float aimbot_pistolFOV = 0.0f;
	float aimbot_sniperFOV = 0.0f;
	float aimbot_rifleFOV = 0.0f;
	float aimbot_shotgunFOV = 0.0f;
	float aimbot_SMGFOV = 0.0f;


	float aimbot_RCS_pitch = 0.0f;
	float aimbot_RCS_yaw = 0.0f;

	float aimbot_pistolsmooth = 0.0f;
	float aimbot_snipersmooth = 0.0f;
	float aimbot_riflesmooth = 0.0f;
	float aimbot_shotgunsmooth = 0.0f;
	float aimbot_SMGsmooth = 0.0f;

	int aimbot_pistolhitbox = 0;
	int aimbot_riflehitbox = 0;
	int aimbot_sniperhitbox = 0;
	int aimbot_shotgunhitbox = 0;
	int aimbot_smghitbox = 0;


	int aimbot_shots = 0;


	float aimbot_antiaim_edge_dist = 0.0f;
	int aimbot_antiaim_pitch = ANTIAIM_PITCH_NONE;
	int aimbot_antiaim_yaw = ANTIAIM_YAW_NONE;
	int aimbot_antiaim_yaw_fake = ANTIAIM_YAW_NONE;
	float aimbot_antiaim_spin_speed = 0.0f;
	bool aimbot_antiaim_knife = false;
	int aimbot_antiaim_thirdperson_angle = ANTIAIM_THIRDPERSON_REAL;
	bool aimbot_antiaim_antiresolver = false;
	int aimbot_FakeLag = 0;
	bool aimbot_FakeLag_Adaptive = false;
	bool aimbot_resolver = false;
	bool aimbot_silent = false;


	bool visuals_Watermark = false;
	bool visuals_AimbotFOVCircle = false;
	bool visuals_ESP = false;
	bool visuals_NoVisualRecoil = false;
	int visuals_hitmarker_style = 0;
	bool visuals_StreamProofing = false;
	float visuals_MenuFadeSpeed = 2.0f;
	int visuals_MenuStyle = THEME_GREY;


	bool misc_Bhop = false;
	bool misc_noflash = false;
};

extern Options g_Options;