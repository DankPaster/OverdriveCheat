#pragma once

class CClientEntityList
{
public:

	CBaseEntity* GetClientEntity(int Index)
	{
		typedef CBaseEntity*(__thiscall* Fn)(void*, int);
		return ((Fn)VMT.GetFunction(this, 3))(this, Index);
	}

	int GetHighestEntityIndex()
	{
		typedef int(__thiscall* Fn)(void*);
		return ((Fn)VMT.GetFunction(this, 8))(this);
	}

	CBaseEntity* GetClientEntityFromHandle( HANDLE Handle )
	{
		typedef CBaseEntity*(__thiscall* Fn)(void*,HANDLE);
		return ((Fn)VMT.GetFunction(this, 7))(this, Handle);
	}
};