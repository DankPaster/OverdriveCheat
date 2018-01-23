#pragma once
#include "baseweapon.h"

class CBaseEntity
{
public:

	float m_flFriction()
	{
		return *(float*)((uintptr_t)this + 0x0140);
	}

	int m_iTeamNum()
	{
		return *(int*)((uintptr_t)this + 0xF0);
	}

	Vector m_vecVelocity()
	{
		return *(Vector*)((uintptr_t)this + 0x110);
	}

	int m_iHealth()
	{
		return *(int*)((uintptr_t)this + 0xFC);
	}
	
	int Index()
	{
		void* networkable = (void*)(this + 0x8);
		typedef int(__thiscall* OriginalFn)(void*);
		return ((OriginalFn)VMT.GetFunction(networkable, 9))(networkable);
		//return *(int*)(this + 64);
	}

	int m_fFlags()
	{
		return *(int*)((uintptr_t)this + 0x100);
	}

	Vector& m_vecAbsOrigin()
	{
		typedef Vector& (__thiscall* OriginalFn)(void*);
		return ((OriginalFn)VMT.GetFunction(this, 10))(this);
	}

	Vector GetEyePosition()
	{
		return m_vecAbsOrigin() + *(Vector*)((uintptr_t)this + 0x104);
	}
	bool IsMoving()
	{
		if (m_vecVelocity().Length2D() > 1) {
			return true;
		}
		else {
			return false;
		}
	}
	Vector m_vecViewOffset()
	{
		return *(Vector*)((uintptr_t)this + 0x104);
	}

	QAngle& m_vecAbsAngles()
	{
		typedef QAngle& (__thiscall* OriginalFn)(void*);
		return ((OriginalFn)VMT.GetFunction(this, 11))(this);
	}

	ClientClass* GetClientClass()
	{
		ClientClass* Networkable = (ClientClass*)(this + 0x8);
		typedef ClientClass* (__thiscall* OriginalFn)(ClientClass*);
		return ((OriginalFn)VMT.GetFunction(Networkable, 2))(Networkable);
	}

	bool m_bDormant()
	{
		return *(bool*)((uintptr_t)this + 0xE9);
	}

	float m_flMaxSpeed()
	{
		return *(float*)((uintptr_t)this + 0x160C);
	}

	bool IsAlive()
	{
		BYTE lifestate = *(BYTE*)((uintptr_t)this + 0x025B);
		return (lifestate == 0);
	}

	bool SetupBones(matrix3x4_t* pBoneMatrix, int nMaxBones, int nBoneMask, float flCurTime = 0)
	{
		void* pRenderable = (void*)(this + 0x4);
		typedef bool(__thiscall* OriginalFn)(void*, matrix3x4_t*, int, int, float);
		return ((OriginalFn)VMT.GetFunction(pRenderable, 13))(pRenderable, pBoneMatrix, nMaxBones, nBoneMask, flCurTime);
	}

	void* GetModel()
	{
		void *pRenderable = (void*)(this + 0x4);
		typedef void*(__thiscall* OriginalFn)(void*);
		return VMT.getvfunc<OriginalFn>(pRenderable, 9)(pRenderable);
	}

	int m_nTickBase()
	{
		return *(int*)((uintptr_t)this + 0x3404);
	}

	Vector GetBonePos(int boneIndex)
	{
		matrix3x4_t BoneMatrix[MAXSTUDIOBONES];

		if (!this->SetupBones(BoneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0))
			return this->m_vecAbsOrigin();

		matrix3x4_t hitbox = BoneMatrix[boneIndex];

		return Vector(hitbox[0][3], hitbox[1][3], hitbox[2][3]);
	}

	bool IsPlayer()
	{
		return VMT.getvfunc<bool(__thiscall*)(CBaseEntity*)>(this, 152)(this);
	}

	bool IsWeapon()
	{
		return VMT.getvfunc<bool(__thiscall*)(CBaseEntity*)>(this, 160)(this);
	}

	bool IsImmune()
	{
		return *(bool*)((uintptr_t)this + 0x38B0);
	}

	QAngle m_viewPunchAngle()
	{
		return *(QAngle*)((uintptr_t)this + 0x3010);
	}

	QAngle m_aimPunchAngle()
	{
		return *(QAngle*)((uintptr_t)this + 0x301C);
	}

	int m_iShotsFired()
	{
		return *(int*)((uintptr_t)this + 0xA2A0);
	}

	float m_flLowerBodyYawTarget()
	{
		return *(float*)((uintptr_t)this + 0x39AC);
	}

	QAngle m_angEyeAngles()
	{
		return *(QAngle*)((uintptr_t)this + 0xAA08);
	}

	CBaseEntity* m_hActiveWeapon()
	{
		return *(CBaseEntity**)((uintptr_t)this + 0x2EE8);
	}

	CBaseEntity* m_hMyWeapons()
	{
		return *(CBaseEntity**)((uintptr_t)this + 0x2DE8);
	}

	int m_iObserverMode()
	{
		return *(int*)((uintptr_t)this + 0x334C);
	}

	CBaseEntity* m_hObserverTarget()
	{
		return *(CBaseEntity**)((uintptr_t)this + 0x3360);
	}
};