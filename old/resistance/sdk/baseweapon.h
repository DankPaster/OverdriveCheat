#pragma once

class CBaseWeapon
{
public:
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
};