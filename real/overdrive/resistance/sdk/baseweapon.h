#pragma once
#include "../utils.h"

class CBaseWeapon
{
public:
	ClientClass* GetClientClass()
	{
		ClientClass* Networkable = (ClientClass*)(this + 0x8);
		typedef ClientClass* (__thiscall* OriginalFn)(ClientClass*);
		return ((OriginalFn)VMT.GetFunction(Networkable, 2))(Networkable);
	}

	int m_iItemDefinitionIndex()
	{
		return *(int*)((uintptr_t)this + 0x2F88);
	}

	int m_iItemIDHigh()
	{
		return *(int*)((uintptr_t)this + 0x2FA0);
	}

	float m_flNextPrimaryAttack()
	{
		return *(float*)((uintptr_t)this + 0x31D8);
	}

	int m_iClip1()
	{
		return *(int*)((uintptr_t)this + 0x3204);
	}

	float m_flFallbackWear()
	{
		return *(float*)((uintptr_t)this + 0x3178);
	}

	int m_nFallbackPaintKit()
	{
		return *(int*)((uintptr_t)this + 0x3170);
	}

	int m_nFallbackSeed()
	{
		return *(int*)((uintptr_t)this + 0x3174);
	}

	int m_nFallbackStatTrak()
	{
		return *(int*)((uintptr_t)this + 0x317C);
	}

	bool IsWeapon()
	{
		return VMT.getvfunc<bool(__thiscall*)(CBaseWeapon*)>(this, 160)(this);
	}

	bool IsPlantedC4()
	{
		return GetClientClass()->m_ClassID == ClassId_CPlantedC4;
	}

	bool IsDefuseKit()
	{
		return GetClientClass()->m_ClassID == ClassId_CBaseAnimating;
	}

	CCSWeaponInfo* GetCSWeaponData()
	{
		static auto fnGetWpnData = (CCSWeaponInfo*(__thiscall*)(void*))(pUtils::PatternScan(GetModuleHandleW(L"client.dll"), "55 8B EC 81 EC ? ? ? ? 53 8B D9 56 57 8D 8B"));
		return fnGetWpnData(this);
	}

	bool HasBullets()
	{
		return !IsReloading() && m_iClip1() > 0;
	}

	bool IsGrenade()
	{
		return GetCSWeaponData()->m_WeaponType == WEAPONTYPE_GRENADE;
	}

	bool IsKnife()
	{
		return GetCSWeaponData()->m_WeaponType == WEAPONTYPE_KNIFE;
	}

	bool IsShotgun()
	{
		switch (GetCSWeaponData()->m_WeaponType)
		{
		case WEAPONTYPE_SHOTGUN:
			return true;
		default:
			return false;
		}
	}

	bool IsSMG()
	{
		switch (GetCSWeaponData()->m_WeaponType)
		{
		case WEAPONTYPE_SUBMACHINEGUN:
			return true;
		default:
			return false;
		}
	}

	bool IsRifle()
	{
		switch (GetCSWeaponData()->m_WeaponType)
		{
		case WEAPONTYPE_RIFLE:
			return true;
		case WEAPONTYPE_MACHINEGUN:
			return true;
		default:
			return false;
		}
	}

	bool IsPistol()
	{
		switch (GetCSWeaponData()->m_WeaponType)
		{
		case WEAPONTYPE_PISTOL:
			return true;
		default:
			return false;
		}
	}

	bool IsSniper()
	{
		switch (GetCSWeaponData()->m_WeaponType)
		{
		case WEAPONTYPE_SNIPER_RIFLE:
			return true;
		default:
			return false;
		}
	}

	bool IsReloading()
	{
		static auto inReload = *(uint32_t*)(pUtils::PatternScan(GetModuleHandleW(L"client.dll"), "C6 87 ? ? ? ? ? 8B 06 8B CE FF 90") + 2);
		return *(bool*)((uintptr_t)this + inReload);
	}
};