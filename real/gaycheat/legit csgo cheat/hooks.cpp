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
IGameEventManager2* g_GameEvents = nullptr;
struct IDirect3DDevice9;

#define STRINGIFY

float hitmarkerAlpha = 1.0f;

class cbullet_tracer_info
{
public:
	cbullet_tracer_info(Vector src, Vector dst, float time)
	{
		this->src = src;
		this->dst = dst;
		this->time = time;
	}

	Vector src, dst;
	float time;
};

std::vector<cbullet_tracer_info> logs;

float _flHurtTime = 0.0f;

namespace Options
{
	bool Bhop = false;
	bool Chams = false;
	bool ChamsVisibleOnly = false;
	bool Backtracking = false;
	bool BacktrackingESP = false;
	bool GrenadePrediction = false;
	bool Radar = false;
	bool Hitmarker = false;
	bool BulletTracers = false;

	int menuIndex = 0;
}

namespace Names
{
	std::string hackNames[] = { "Bhop", "Chams", "Legit Chams", "Backtracking", "Backtracking ESP", "Grenade Prediction", "Radar", "Hitmarkers", "Bullet Tracers" };
}

bool Initialized = false;

WNDPROC oWndProc = nullptr;

#define M_PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679f

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

struct BacktrackRecord
{
	C_BasePlayer* entity;
	Vector head;
	Vector origin;
};

struct BacktrackTick
{
	int tickcount;
	std::vector<BacktrackRecord> records;
};

class CBacktracking
{
	std::vector<BacktrackTick> ticks;
	C_BasePlayer* entity;
	Vector prevOrig;

public:
	void RegisterTick(CUserCmd* cmd)
	{
		ticks.insert(ticks.begin(), BacktrackTick{ cmd->tick_count });
		auto& cur = ticks[0];
		while (ticks.size() > 25) ticks.pop_back();
		for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
		{
			auto entity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);
			if (!entity ||
				entity->IsDormant() ||
				entity->Health() <= 0 ||
				entity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum() ||
				entity->m_bGunGameImmunity())
				continue;

			cur.records.emplace_back(BacktrackRecord{ entity, entity->GetBonePos(8), entity->m_vecAbsOrigin() });
		}
	}

	void Begin(CUserCmd* cmd)
	{
		entity = nullptr;

		float serverTime = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;
		auto weapon = g_LocalPlayer->m_hActiveWeapon();
		if (weapon) {
			if (cmd->buttons & IN_ATTACK && weapon->m_flNextPrimaryAttack() < serverTime + 0.001)
			{
				float fov = 170.5f;
				int tickcount = 0;
				bool hasTarget = false;
				Vector orig;

				for (auto& tick : ticks)
				{
					for (auto& record : tick.records)
					{
						QAngle angle = CalcAngle(g_LocalPlayer->GetEyePos(), record.head);
						float tmpFOV = GetFOV(cmd->viewangles, angle);

						if (tmpFOV < fov)
						{
							fov = tmpFOV;
							tickcount = tick.tickcount;
							hasTarget = true;
							entity = record.entity;
							orig = record.origin;

							static auto invalidateBoneCache = PatternScan(GetModuleHandleA("client.dll"), "80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81");
							unsigned long modelBoneCounter = **(unsigned long**)(invalidateBoneCache + 10);
							*(uintptr_t*)((uintptr_t)entity + 0x2914) = 0xFF7FFFFF;
							*(uintptr_t*)((uintptr_t)entity + 0x2680) = (modelBoneCounter - 1);
						}
					}
				}

				if (entity && hasTarget)
				{
					cmd->tick_count = tickcount;
					prevOrig = entity->m_vecAbsOrigin();
					entity->m_vecAbsOrigin() = orig;
				}
			}
		}
	}
	void End()
	{
		if (entity) entity->m_vecAbsOrigin() = prevOrig;
		entity = nullptr;
	}
	void Draw()
	{
		for (auto& tick : ticks)
		{
			for (auto& record : tick.records)
			{
				Vector screenPos;
				if (WorldToScreen(record.head, screenPos))
				{
					g_VGuiSurface->DrawSetColor(0, 0, 0, 50);
					g_VGuiSurface->DrawFilledRect(screenPos.x - 1, screenPos.y - 1, screenPos.x + 3, screenPos.y + 3);
					g_VGuiSurface->DrawSetColor(255, 0, 0, 255);
					g_VGuiSurface->DrawFilledRect(screenPos.x, screenPos.y, screenPos.x + 2, screenPos.y + 2);
				}
			}
		}
	}
};

CBacktracking Backtracking;

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
	FireEventClientSide oFireEventClientSide = nullptr;

	void __stdcall hkCreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket)
	{
		oCreateMove(g_CHLClient, sequence_number, input_sample_frametime, active);

		auto cmd = g_Input->GetUserCmd(sequence_number);
		auto verified = g_Input->GetVerifiedCmd(sequence_number);

		if (!cmd || !cmd->command_number)
			return;

		if (g_LocalPlayer && g_EngineClient->IsInGame())
		{
			Backtracking.RegisterTick(cmd);
			Backtracking.Begin(cmd);
		}

		if (Options::Backtracking && g_EngineClient->IsInGame() && g_LocalPlayer && g_LocalPlayer->IsAlive())
		{
			Backtracking.End();
		}

		if (g_LocalPlayer && g_EngineClient->IsInGame() && g_LocalPlayer->IsAlive() && Options::Bhop)
			if (cmd->buttons & IN_JUMP && !(g_LocalPlayer->m_fFlags() & FL_ONGROUND)) cmd->buttons &= ~IN_JUMP;

		//grenade_prediction::instance().Tick(cmd->buttons);

		if ((cmd->buttons & IN_SCORE) && g_LocalPlayer && g_EngineClient->IsInGame()) RankRevealAll();

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

	static int fov = 0.0f;
	void __stdcall hkPaintTraverse(vgui::VPANEL panel, bool forceRepaint, bool allowForce)
	{
		static auto panelId = vgui::VPANEL{ 0 };

		oPaintTraverse(g_VGuiPanel, panel, forceRepaint, allowForce);

		static vgui::HFont aCoolFont;

		static bool FontsCreated = false;
		if (!FontsCreated)
		{
			aCoolFont = g_VGuiSurface->Create_Font();
			g_VGuiSurface->SetFontGlyphSet(aCoolFont, "Quicksand", 16, 500, 0, 0, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);
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
			static wchar_t* watermark = L"cheathak_ware.dll";
			g_VGuiSurface->DrawPrintText(watermark, wcslen(watermark));	
			for (int i = 0; i < ARRAYSIZE(Names::hackNames); i++) 
			{
				switch (i)
				{
				case 0:
					if (Options::Bhop) g_VGuiSurface->DrawSetTextColor(220, 253, 242, 255);
					if (Options::Bhop && i == Options::menuIndex) g_VGuiSurface->DrawSetTextColor(0, 200, 0, 255);
					if (!Options::Bhop) g_VGuiSurface->DrawSetTextColor(150, 200, 200, 255);
					break;
				case 1:
					if (Options::Chams) g_VGuiSurface->DrawSetTextColor(220, 253, 242, 255);
					if (Options::Chams && i == Options::menuIndex) g_VGuiSurface->DrawSetTextColor(0, 200, 0, 255);
					if (!Options::Chams) g_VGuiSurface->DrawSetTextColor(150, 200, 200, 255);
					break;
				case 2:
					if (Options::ChamsVisibleOnly) g_VGuiSurface->DrawSetTextColor(220, 253, 242, 255);
					if (Options::ChamsVisibleOnly && i == Options::menuIndex) g_VGuiSurface->DrawSetTextColor(0, 200, 0, 255);
					if (!Options::ChamsVisibleOnly) g_VGuiSurface->DrawSetTextColor(150, 200, 200, 255);
					break;
				case 3:
					if (Options::Backtracking) g_VGuiSurface->DrawSetTextColor(220, 253, 242, 255);
					if (Options::Backtracking && i == Options::menuIndex) g_VGuiSurface->DrawSetTextColor(0, 200, 0, 255);
					if (!Options::Backtracking) g_VGuiSurface->DrawSetTextColor(150, 200, 200, 255);
					break;
				case 4:
					if (Options::BacktrackingESP) g_VGuiSurface->DrawSetTextColor(220, 253, 242, 255);
					if (Options::BacktrackingESP && i == Options::menuIndex) g_VGuiSurface->DrawSetTextColor(0, 200, 0, 255);
					if (!Options::BacktrackingESP) g_VGuiSurface->DrawSetTextColor(150, 200, 200, 255);
					break;
				case 5:
					if (Options::GrenadePrediction) g_VGuiSurface->DrawSetTextColor(220, 253, 242, 255);
					if (Options::GrenadePrediction && i == Options::menuIndex) g_VGuiSurface->DrawSetTextColor(0, 200, 0, 255);
					if (!Options::GrenadePrediction) g_VGuiSurface->DrawSetTextColor(150, 200, 200, 255);
					break;
				case 6:
					if (Options::Radar) g_VGuiSurface->DrawSetTextColor(220, 253, 242, 255);
					if (Options::Radar && i == Options::menuIndex) g_VGuiSurface->DrawSetTextColor(0, 200, 0, 255);
					if (!Options::Radar) g_VGuiSurface->DrawSetTextColor(150, 200, 200, 255);
					break;
				case 7:
					if (Options::Hitmarker) g_VGuiSurface->DrawSetTextColor(220, 253, 242, 255);
					if (Options::Hitmarker && i == Options::menuIndex) g_VGuiSurface->DrawSetTextColor(0, 200, 0, 255);
					if (!Options::Hitmarker) g_VGuiSurface->DrawSetTextColor(150, 200, 200, 255);
					break;
				case 8:
					if (Options::BulletTracers) g_VGuiSurface->DrawSetTextColor(220, 253, 242, 255);
					if (Options::BulletTracers && i == Options::menuIndex) g_VGuiSurface->DrawSetTextColor(0, 200, 0, 255);
					if (!Options::BulletTracers) g_VGuiSurface->DrawSetTextColor(150, 200, 200, 255);
					break;
				default:
					g_VGuiSurface->DrawSetTextColor(100, 133, 122, 255);
					break;
				}

				if (i == Options::menuIndex) g_VGuiSurface->DrawSetTextColor(0, 255, 75, 255);

				g_VGuiSurface->DrawSetTextFont(aCoolFont);
				g_VGuiSurface->DrawSetTextPos(10, (i * 25) + 35);
				std::string hn = Names::hackNames[i];
				std::wstring wide_string = std::wstring(hn.begin(), hn.end());
				const wchar_t* result = wide_string.c_str();
				g_VGuiSurface->DrawPrintText(result, wcslen(result));
			}
			
			static bool press = false;
			if ((GetAsyncKeyState(VK_UP) & 0x8000) && !press) press = true;
			else if (!(GetAsyncKeyState(VK_UP) & 0x8000) && press)
			{
				Options::menuIndex--;
				press = false;
			}

			static bool press1 = false;
			if ((GetAsyncKeyState(VK_DOWN) & 0x8000) && !press1) press1 = true;
			else if (!(GetAsyncKeyState(VK_DOWN) & 0x8000) && press1)
			{
				Options::menuIndex++;
				press1 = false;
			}

			static bool press2 = false;
			if ((GetAsyncKeyState(VK_RETURN) & 0x8000) && !press2) press2 = true;
			else if (!(GetAsyncKeyState(VK_RETURN) & 0x8000) && press2)
			{
				switch (Options::menuIndex)
				{
				case 0: Options::Bhop = !Options::Bhop; break;
				case 1: Options::Chams = !Options::Chams; break;
				case 2: Options::ChamsVisibleOnly = !Options::ChamsVisibleOnly; break;
				case 3: Options::Backtracking = !Options::Backtracking; break;
				case 4: Options::BacktrackingESP = !Options::BacktrackingESP; break;
				case 5: Options::GrenadePrediction = !Options::GrenadePrediction; break;
				case 6: Options::Radar = !Options::Radar; break;
				case 7: Options::Hitmarker = !Options::Hitmarker; break;
				case 8: Options::BulletTracers = !Options::BulletTracers; break;
				}
				press2 = false;
			}

			if (Options::menuIndex > ARRAYSIZE(Names::hackNames) - 1) Options::menuIndex = ARRAYSIZE(Names::hackNames) - 1;
			else if (Options::menuIndex < 0) Options::menuIndex = 0;

			if (Options::BacktrackingESP && g_EngineClient->IsInGame()) Backtracking.Draw();

			if (g_EngineClient->IsInGame() && g_LocalPlayer && g_LocalPlayer->IsAlive())
			{
				if (Options::Hitmarker)
				{
					int lineSize = 8;

					int screenSizeX, screenCenterX;
					int screenSizeY, screenCenterY;
					g_EngineClient->GetScreenSize(screenSizeX, screenSizeY);

					screenCenterX = screenSizeX / 2;
					screenCenterY = screenSizeY / 2;

					g_VGuiSurface->DrawSetColor(200, 200, 200, hitmarkerAlpha * 255.0f);
					g_VGuiSurface->DrawLine(screenCenterX - lineSize, screenCenterY - lineSize, screenCenterX - (lineSize / 4), screenCenterY - (lineSize / 4));
					g_VGuiSurface->DrawLine(screenCenterX - lineSize, screenCenterY + lineSize, screenCenterX - (lineSize / 4), screenCenterY + (lineSize / 4));
					g_VGuiSurface->DrawLine(screenCenterX + lineSize, screenCenterY + lineSize, screenCenterX + (lineSize / 4), screenCenterY + (lineSize / 4));
					g_VGuiSurface->DrawLine(screenCenterX + lineSize, screenCenterY - lineSize, screenCenterX + (lineSize / 4), screenCenterY - (lineSize / 4));

					if (hitmarkerAlpha >= 0.006f) hitmarkerAlpha -= 0.006f;
				}

				if (Options::BulletTracers)
				{
					for (size_t i = 0; i < logs.size(); i++)
					{
						auto current = logs.at(i);

						g_VGuiSurface->DrawSetColor(255, 0, 255, 255);
						g_VGuiSurface->DrawLine(current.src.x, current.src.y, current.dst.x, current.dst.y);
						g_VGuiSurface->DrawOutlinedRect(current.dst.x - 1, current.dst.y - 1, 3, 3);
						g_DebugOverlay->AddBoxOverlay(current.dst, Vector(-2, -2, -2), Vector(2, 2, 2), QAngle(0, 0, 0), 255, 0, 0, 127, -1.f);

						if (fabs(g_GlobalVars->curtime - current.time) > 3.0f)
							logs.erase(logs.begin() + i);
					}
				}
			}
		}
	}

	void __stdcall hkOverrideView(CViewSetup* setup)
	{
		if (g_EngineClient && g_EngineClient->IsInGame())
			if (!g_LocalPlayer->m_bIsScoped()) setup->fov += fov;

		if (GetAsyncKeyState(VK_END)) fov = 90.0f;
		if (GetAsyncKeyState(VK_LEFT)) fov--;
		if (GetAsyncKeyState(VK_RIGHT)) fov++;
		setup->fovViewmodel += 40;
		oOverrideView(g_ClientMode, setup);
	}

	void __stdcall hkFrameStageNotify(ClientFrameStage_t stage)
	{
		auto pLocal = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());

		if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		{
			if (Options::Radar && g_LocalPlayer && g_EngineClient->IsInGame())
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
		}

		oFrameStageNotify(g_CHLClient, stage);
	}

	bool __stdcall hkFireEventClientSide(IGameEvent* pEvent)
	{
		if (!pEvent)
			return oFireEventClientSide(g_GameEvents, pEvent);

		if (!strcmp(pEvent->GetName(), "player_hurt"))
		{
			if (g_EngineClient->GetPlayerForUserID(pEvent->GetInt("attacker")) == g_EngineClient->GetLocalPlayer())
			{
				hitmarkerAlpha = 1.0f;
				_flHurtTime = g_GlobalVars->curtime;

				if (Options::Hitmarker)
				{
					g_EngineClient->ExecuteClientCmd(R"(play buttons\arena_switch_press_02.wav)");
				}
			}
		}

		if (strstr(pEvent->GetName(), "bullet_impact"))
		{
			if (g_EngineClient->GetPlayerForUserID(pEvent->GetInt("userid")) == g_EngineClient->GetLocalPlayer())
			{
				Vector position(pEvent->GetFloat("x"), pEvent->GetFloat("y"), pEvent->GetFloat("z"));

				Ray_t ray;
				ray.Init(g_LocalPlayer->GetEyePos(), position);

				CTraceFilter filter;
				filter.pSkip = g_LocalPlayer;

				trace_t tr;
				g_EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);

				logs.push_back(cbullet_tracer_info(g_LocalPlayer->GetEyePos(), position, g_GlobalVars->curtime));
			}
		}

		return oFireEventClientSide(g_GameEvents, pEvent);
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
		g_GameEvents = get_interface<IGameEventManager2>(engineFactory, "GAMEEVENTSMANAGER002");
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
		CVMTHookManager* events_hook = new CVMTHookManager(g_GameEvents);

		oCreateMove = (CreateMove)chlclient_hook->HookFunction(21, hkCreateMove_Proxy);
		oPlaySound = (PlaySound)vguisurface_hook->HookFunction(82, hkPlaySound);
		oDrawModelExecute = (DrawModelExecute)mdlrender_hook->HookFunction(21, hkDrawModelExecute);
		oPaintTraverse = (PaintTraverse)vguipanel_hook->HookFunction(41, hkPaintTraverse);
		oOverrideView = (OverrideView)clientmode_hook->HookFunction(18, hkOverrideView);
		oFrameStageNotify = (FrameStageNotify)chlclient_hook->HookFunction(36, hkFrameStageNotify);
		oFireEventClientSide = (FireEventClientSide)events_hook->HookFunction(9, hkFireEventClientSide);

		chlclient_hook->HookTable(true);
		vguisurface_hook->HookTable(true);
		mdlrender_hook->HookTable(true);
		vguipanel_hook->HookTable(true);
		clientmode_hook->HookTable(true);
		events_hook->HookTable(true);
	}
}