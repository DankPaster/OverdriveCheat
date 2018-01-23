#pragma once
#include "Options.h"
#include <xmmintrin.h>
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446095505822317253594081284811174502841027019385211055596446229489549303819644288109756659334461284756482337867831652712019091456485669234603486104543266482133936072602491412737245870066063155881748815209209628292540917153643678925903600113305305488204665213841469519415116094330572703657595919530921861173819326117931051185480744623799627495673518857527248912279381830119491298336733624406566430860213949463952247371907021798609437027705392171762931767523846748184676694051320005681271452635608277857713427577896091736371787214684409012249534301465495853710507922796892589235420199561121290219608640344181598136297747713099605187072113499999983729780499510597317328160963185950244594553469083026425223082533446850352619311881710100031378387528865875332083814206171776691473035982534904287554687311595628638823537875937519577818577805321712268066130019278766111959092164201989380952572010654858632788659361533818279682303019520353018529689957736225994138912497217752834791315155748572424541506959508295331168617278558890750983817546374649393192550604009277016711390098488240128583616035637076601047101819429555961989467678374494482553797747268471040475346462080466842590694912933136770289891521047521620569660240580f
#endif

#define M_PI_F		((float)(M_PI))	// Shouldn't collide with anything.
#ifndef RAD2DEG
#define RAD2DEG( x  )  ( (float)(x) * (float)(180.f / M_PI_F) )
#endif

#ifndef DEG2RAD
#define DEG2RAD( x  )  ( (float)(x) * (float)(M_PI_F / 180.f) )
#endif
enum
{
	PITCH = 0,	// up / down
	YAW,		// left / right
	ROLL		// fall over
};

// Math routines done in optimized assembly math package routines
void inline SinCos(float radians, float *sine, float *cosine)
{
	*sine = sin(radians);
	*cosine = cos(radians);

}

void AngleVectors(const QAngle &angles, Vector *forward)
{
	Assert(s_bMathlibInitialized);
	Assert(forward);

	float	sp, sy, cp, cy;

	sy = sin(DEG2RAD(angles[1]));
	cy = cos(DEG2RAD(angles[1]));

	sp = sin(DEG2RAD(angles[0]));
	cp = cos(DEG2RAD(angles[0]));

	forward->x = cp*cy;
	forward->y = cp*sy;
	forward->z = -sp;
}

void VectorAngles(Vector forward, QAngle &angles)
{
	float tmp, yaw, pitch;

	if (forward[2] == 0 && forward[0] == 0)
	{
		yaw = 0;

		if (forward[2] > 0)
			pitch = 90;
		else
			pitch = 270;
	}
	else
	{
		yaw = (atan2(forward[1], forward[0]) * 180 / M_PI);

		if (yaw < 0)
			yaw += 360;

		tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
		pitch = (atan2(-forward[2], tmp) * 180 / M_PI);

		if (pitch < 0)
			pitch += 360;
	}

	if (pitch > 180)
		pitch -= 360;
	else if (pitch < -180)
		pitch += 360;

	if (yaw > 180)
		yaw -= 360;
	else if (yaw < -180)
		yaw += 360;

	if (pitch > 89)
		pitch = 89;
	else if (pitch < -89)
		pitch = -89;

	if (yaw > 180)
		yaw = 180;
	else if (yaw < -180)
		yaw = -180;

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

QAngle CalcAngle(Vector src, Vector dst)
{
	QAngle ret;

	Vector delta = src - dst;
	double hyp = delta.Length2D(); //delta.Length
	ret.yaw = (atan(delta.y / delta.x) * 57.295779513082f);
	ret.pitch = (atan(delta.z / hyp) * 57.295779513082f);
	ret[2] = 0.00;

	if (delta.x >= 0.0)
		ret.yaw += 180.0f;

	return ret;
}

void AngleVectorsTranspose(const QAngle &angles, Vector *forward, Vector *right, Vector *up)
{
	Assert(s_bMathlibInitialized);
	float sr, sp, sy, cr, cp, cy;

	SinCos(DEG2RAD(angles[YAW]), &sy, &cy);
	SinCos(DEG2RAD(angles[PITCH]), &sp, &cp);
	SinCos(DEG2RAD(angles[ROLL]), &sr, &cr);

	if (forward)
	{
		forward->x = cp*cy;
		forward->y = (sr*sp*cy + cr*-sy);
		forward->z = (cr*sp*cy + -sr*-sy);
	}

	if (right)
	{
		right->x = cp*sy;
		right->y = (sr*sp*sy + cr*cy);
		right->z = (cr*sp*sy + -sr*cy);
	}

	if (up)
	{
		up->x = -sp;
		up->y = sr*cp;
		up->z = cr*cp;
	}
}

void CrossProduct(const float* v1, const float* v2, float* cross)
{
	Assert(s_bMathlibInitialized);
	Assert(v1 != cross);
	Assert(v2 != cross);
	cross[0] = v1[1] * v2[2] - v1[2] * v2[1];
	cross[1] = v1[2] * v2[0] - v1[0] * v2[2];
	cross[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

inline void _SSE_RSqrtInline(float a, float* out)
{
	__m128  xx = _mm_load_ss(&a);
	__m128  xr = _mm_rsqrt_ss(xx);
	__m128  xt;
	xt = _mm_mul_ss(xr, xr);
	xt = _mm_mul_ss(xt, xx);
	xt = _mm_sub_ss(_mm_set_ss(3.f), xt);
	xt = _mm_mul_ss(xt, _mm_set_ss(0.5f));
	xr = _mm_mul_ss(xr, xt);
	_mm_store_ss(out, xr);
}

FORCEINLINE float VectorNormalize(Vector& vec)
{
#ifndef DEBUG // stop crashing my edit-and-continue!
#if defined(__i386__) || defined(_M_IX86)
#define DO_SSE_OPTIMIZATION
#endif
#endif

#if defined( DO_SSE_OPTIMIZATION )
	float sqrlen = vec.LengthSqr() + 1.0e-10f, invlen;
	_SSE_RSqrtInline(sqrlen, &invlen);
	vec.x *= invlen;
	vec.y *= invlen;
	vec.z *= invlen;
	return sqrlen * invlen;
#else
	extern float (FASTCALL *pfVectorNormalize)(Vector& v);
	return (*pfVectorNormalize)(vec);
#endif
}

void MatrixGetColumn(const matrix3x4_t& in, int column, Vector &out)
{
	out.x = in[0][column];
	out.y = in[1][column];
	out.z = in[2][column];
}

void MatrixSetColumn(const Vector &in, int column, matrix3x4_t& out)
{
	out[0][column] = in.x;
	out[1][column] = in.y;
	out[2][column] = in.z;
}
void SetIdentityMatrix(matrix3x4_t& matrix)
{
	memset(matrix.Base(), 0, sizeof(float) * 3 * 4);
	matrix[0][0] = 1.0;
	matrix[1][1] = 1.0;
	matrix[2][2] = 1.0;
}

void SetScaleMatrix(float x, float y, float z, matrix3x4_t &dst);
void MatrixBuildRotationAboutAxis(const Vector &vAxisOfRot, float angleDegrees, matrix3x4_t &dst);
void VectorTransform(class Vector const &, struct matrix3x4_t const &, class Vector &);

float QuaternionNormalize(Quaternion &q)
{
	Assert(s_bMathlibInitialized);
	float radius, iradius;

	Assert(q.IsValid());

	radius = q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3];

	if (radius) // > FLT_EPSILON && ((radius < 1.0f - 4*FLT_EPSILON) || (radius > 1.0f + 4*FLT_EPSILON))
	{
		radius = sqrt(radius);
		iradius = 1.0f / radius;
		q[3] *= iradius;
		q[2] *= iradius;
		q[1] *= iradius;
		q[0] *= iradius;
	}
	return radius;
}

void MatrixAngles(const matrix3x4_t &matrix, Quaternion &q, Vector &pos)
{
#ifdef _VPROF_MATHLIB
	VPROF_BUDGET("MatrixQuaternion", "Mathlib");
#endif
	float trace;
	trace = matrix[0][0] + matrix[1][1] + matrix[2][2] + 1.0f;
	if (trace > 1.0f + FLT_EPSILON)
	{
		// VPROF_INCREMENT_COUNTER("MatrixQuaternion A",1);
		q.x = (matrix[2][1] - matrix[1][2]);
		q.y = (matrix[0][2] - matrix[2][0]);
		q.z = (matrix[1][0] - matrix[0][1]);
		q.w = trace;
	}
	else if (matrix[0][0] > matrix[1][1] && matrix[0][0] > matrix[2][2])
	{
		// VPROF_INCREMENT_COUNTER("MatrixQuaternion B",1);
		trace = 1.0f + matrix[0][0] - matrix[1][1] - matrix[2][2];
		q.x = trace;
		q.y = (matrix[1][0] + matrix[0][1]);
		q.z = (matrix[0][2] + matrix[2][0]);
		q.w = (matrix[2][1] - matrix[1][2]);
	}
	else if (matrix[1][1] > matrix[2][2])
	{
		// VPROF_INCREMENT_COUNTER("MatrixQuaternion C",1);
		trace = 1.0f + matrix[1][1] - matrix[0][0] - matrix[2][2];
		q.x = (matrix[0][1] + matrix[1][0]);
		q.y = trace;
		q.z = (matrix[2][1] + matrix[1][2]);
		q.w = (matrix[0][2] - matrix[2][0]);
	}
	else
	{
		// VPROF_INCREMENT_COUNTER("MatrixQuaternion D",1);
		trace = 1.0f + matrix[2][2] - matrix[0][0] - matrix[1][1];
		q.x = (matrix[0][2] + matrix[2][0]);
		q.y = (matrix[2][1] + matrix[1][2]);
		q.z = trace;
		q.w = (matrix[1][0] - matrix[0][1]);
	}

	QuaternionNormalize(q);

#if 0
	// check against the angle version
	RadianEuler ang;
	MatrixAngles(matrix, ang);
	Quaternion test;
	AngleQuaternion(ang, test);
	float d = QuaternionDotProduct(q, test);
	Assert(fabs(d) > 0.99 && fabs(d) < 1.01);
#endif

	MatrixGetColumn(matrix, 3, pos);
}

FORCEINLINE vec_t DotProduct(const Vector& a, const Vector& b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
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

void CorrectMovement(QAngle vOldAngles, CUserCmd* pCmd, float fOldForward, float fOldSidemove)
{
	// side/forward move correction
	float deltaView;
	float f1;
	float f2;

	if (vOldAngles.yaw < 0.f)
		f1 = 360.0f + vOldAngles.yaw;
	else
		f1 = vOldAngles.yaw;

	if (pCmd->viewangles.yaw < 0.0f)
		f2 = 360.0f + pCmd->viewangles.yaw;
	else
		f2 = pCmd->viewangles.yaw;

	if (f2 < f1)
		deltaView = abs(f2 - f1);
	else
		deltaView = 360.0f - abs(f1 - f2);

	deltaView = 360.0f - deltaView;

	pCmd->forwardmove = cos(DEG2RAD(deltaView)) * fOldForward + cos(DEG2RAD(deltaView + 90.f)) * fOldSidemove;
	pCmd->sidemove = sin(DEG2RAD(deltaView)) * fOldForward + sin(DEG2RAD(deltaView + 90.f)) * fOldSidemove;
}

Color GetColorFromHealth(CBaseEntity* pEntity)
{
	const float multiplier = 2.55f;
	int R, G, B, A;
	int health = pEntity->m_iHealth();
	R = 255 - (health * multiplier);
	G = health * multiplier;
	B = 0;
	A = 255;
	return Color(R, G, B, A);
}

Color rainbow() 
{
	static float x = 0, y = 0;
	static float r = 0, g = 0, b = 0;

	if (y >= 0.0f && y < 255.0f)
	{
		r = 255.0f;
		g = 0.0f;
		b = x;
	}
	else if (y >= 255.0f && y < 510.0f) 
	{
		r = 255.0f - x;
		g = 0.0f;
		b = 255.0f;
	}
	else if (y >= 510.0f && y < 765.0f) 
	{
		r = 0.0f;
		g = x;
		b = 255.0f;
	}
	else if (y >= 765.0f && y < 1020.0f)
	{
		r = 0.0f;
		g = 255.0f;
		b = 255.0f - x;
	}
	else if (y >= 1020.0f && y < 1275.0f)
	{
		r = x;
		g = 255.0f;
		b = 0.0f;
	}
	else if (y >= 1275.0f && y < 1530.0f) 
	{
		r = 255.0f;
		g = 255.0f - x;
		b = 0.0f;
	}

	x += 0.25f; //increase this value to switch colors faster
	if (x >= 255.0f)
		x = 0.0f;

	y += 0.25f; //increase this value to switch colors faster
	if (y > 1530.0f)
		y = 0.0f;

	return Color((int)r, (int)g, (int)b, 255);
}

void NormalizeAngles(QAngle& angle)
{
	while (angle.pitch > 89.0f)
		angle.pitch -= 180.f;

	while (angle.pitch < -89.0f)
		angle.pitch += 180.f;

	while (angle.yaw > 180.f)
		angle.yaw -= 360.f;

	while (angle.yaw < -180.f)
		angle.yaw += 360.f;
}

void ClampAngles(QAngle& angle)
{
	if (angle.yaw > 180.0f)
		angle.yaw = 180.0f;
	else if (angle.yaw < -180.0f)
		angle.yaw = -180.0f;

	if (angle.pitch > 89.0f)
		angle.pitch = 89.0f;
	else if (angle.pitch < -89.0f)
		angle.pitch = -89.0f;
}

float GetDistanceFOV(float distance, QAngle angle)
{
	Vector aimingAt;
	QAngle angles;
	g_EngineClient->GetViewAngles(angles);
	AngleVectors(angles, &aimingAt);
	aimingAt.x *= distance;
	aimingAt.y *= distance;
	aimingAt.z *= distance;

	Vector aimAt;
	AngleVectors(angle, &aimAt);
	aimAt.x *= distance;
	aimAt.y *= distance;
	aimAt.z *= distance;

	return aimingAt.DistTo(aimAt);
}

float Get3dDistance(Vector local, Vector ent)
{
	return sqrt(pow(double(ent.x - local.x), 2.0) + pow(double(ent.y - local.y), 2.0) + pow(double(ent.z - local.z), 2.0));
}

float GetFOV(const QAngle& viewAngle, const QAngle& aimAngle)
{
	QAngle delta = aimAngle - viewAngle;
	NormalizeAngles(delta);

	return sqrtf(powf(delta.pitch, 2.0f) + powf(delta.yaw, 2.0f));
}

CBaseEntity* GetClosestPlayerToCrosshair()
{
	float bestFov;
	CBaseEntity* closestEntity = nullptr;
	if (g_LocalPlayer->m_hActiveWeapon()->IsPistol()) {
		 bestFov = g_Options.aimbot_pistolFOV;
	}
	if (g_LocalPlayer->m_hActiveWeapon()->IsRifle()) {
		 bestFov = g_Options.aimbot_rifleFOV;
	}
	if (g_LocalPlayer->m_hActiveWeapon()->IsSniper()) {
		 bestFov = g_Options.aimbot_sniperFOV;
	}
	if (g_LocalPlayer->m_hActiveWeapon()->IsSMG()) {
		bestFov = g_Options.aimbot_SMGFOV;
	}
	if (g_LocalPlayer->m_hActiveWeapon()->IsShotgun()) {
		bestFov = g_Options.aimbot_shotgunFOV;
	}
	if (g_LocalPlayer->m_hActiveWeapon()->IsKnife() || g_LocalPlayer->m_hActiveWeapon()->IsGrenade() || g_LocalPlayer->m_hActiveWeapon()->IsDefuseKit()) {
		bestFov = 0.f;
	}

	if (!g_LocalPlayer)
		return nullptr;

	for (int i = 1; i < g_EngineClient->GetMaxClients(); ++i)
	{
		CBaseEntity* player = g_EntityList->GetClientEntity(i);

		if (player
			&& player != g_LocalPlayer
			&& !player->m_bDormant()
			&& player->IsAlive()
			&& !player->IsImmune())
		{
			if (player->m_iTeamNum() != g_LocalPlayer->m_iTeamNum())
			{
				player_info_t entityInformation;
				g_EngineClient->GetPlayerInfo(i, &entityInformation);

				Vector eVecTarget = player->GetBonePos(head_0);
				Vector pVecTarget = g_LocalPlayer->GetEyePosition();

				QAngle viewAngles;
				g_EngineClient->GetViewAngles(viewAngles);

				float distance = pVecTarget.DistTo(eVecTarget);
				float fov = GetFOV(viewAngles, CalcAngle(pVecTarget, eVecTarget));
				int hp = player->m_iHealth();

				if (fov < bestFov)
				{
					closestEntity = player;
					bestFov = fov;
				}
			}
		}
	}
	return closestEntity;
}

typedef int(__fastcall* OriginalFn)(const Vector&, const Vector&, unsigned int, const CBaseEntity*, int, trace_t*);
bool IsBoneVisible(CBaseEntity* pEntity, int BoneID, bool smokeCheck)
{
	if (BoneID < 0) return false;

	trace_t Trace;
	Vector start = g_LocalPlayer->GetEyePosition();
	Vector end = pEntity->GetBonePos(BoneID);

	static OriginalFn TraceLine = (OriginalFn)pUtils::PatternScan(GetModuleHandleW(L"client.dll"), "55 8B EC 83 E4 F0 83 EC 7C 56 52");
	TraceLine(start, end, MASK_SOLID, g_LocalPlayer, 0, &Trace);

	// if line goes through smoke they arent visible
	if (smokeCheck && pUtils::LineGoesThroughSmoke(start, end)) return false;

	// if it hits the desired entity, they are visible
	if (Trace.m_pEnt == pEntity) return true;
	if (Trace.fraction == 1.0f) return true;

	return false;
}

QAngle Smooth(QAngle finalangle, QAngle viewangles) 
{

	float smooth;
	QAngle delta = finalangle - viewangles;
	NormalizeAngles(delta);
	ClampAngles(delta);
	if (g_LocalPlayer->m_hActiveWeapon()->IsPistol()) {
		smooth = powf(g_Options.aimbot_pistolsmooth, 0.4f);
	}
	if (g_LocalPlayer->m_hActiveWeapon()->IsRifle()) {
		smooth = powf(g_Options.aimbot_riflesmooth, 0.4f);
	}
	if (g_LocalPlayer->m_hActiveWeapon()->IsSniper()) {
		smooth = powf(g_Options.aimbot_snipersmooth, 0.4f);
	}
	if (g_LocalPlayer->m_hActiveWeapon()->IsSMG()) {
		smooth = powf(g_Options.aimbot_SMGsmooth, 0.4f);
	}
	if (g_LocalPlayer->m_hActiveWeapon()->IsShotgun()) {
		smooth = powf(g_Options.aimbot_shotgunsmooth, 0.4f);
	}
	if (g_LocalPlayer->m_hActiveWeapon()->IsKnife() || g_LocalPlayer->m_hActiveWeapon()->IsGrenade() || g_LocalPlayer->m_hActiveWeapon()->IsDefuseKit()) {
		smooth = powf(0.1, 0.4f);
	}
	 
	smooth = min(0.99f, smooth);
	QAngle toChange = delta - delta * smooth;
	return toChange;
}

bool IsVisible(CBaseEntity* pEntity, bool smokeCheck)
{
	// If any of the common bones in the body are visible, return true
	for (int bone = 2; bone < hand_R; bone++) if (IsBoneVisible(pEntity, bone, smokeCheck)) return true;
	// return that the entity isnt visible if no bones are visible
	return false;
}

bool CanFire(CBaseWeapon* pWeapon)
{
	if (pWeapon->IsReloading() || pWeapon->m_iClip1() <= 0)return false;
	if (!g_LocalPlayer) return false;
	float flServerTime = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;
	return pWeapon->m_flNextPrimaryAttack() <= flServerTime;
}