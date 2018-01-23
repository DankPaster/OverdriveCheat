#pragma once

#include "baseentity.h"

class C_LocalPlayer
{
	friend bool operator==(const C_LocalPlayer& lhs, void* rhs);
public:
	C_LocalPlayer() : m_local(nullptr) {}

	operator bool() const { return *m_local != nullptr; }
	operator CBaseEntity*() const { return *m_local; }

	CBaseEntity* operator->() { return *m_local; }

private:
	CBaseEntity** m_local;
};

extern C_LocalPlayer g_LocalPlayer;