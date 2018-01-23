#pragma once
#include <vector>
#include <d3d9.h>
#include <d3dx9.h>
#include "CSGOClasses.h"
#include "VMT.h"

namespace Hooks
{
	extern HINSTANCE hThisModule;

	std::uint8_t* FindPattern(const char* modulename, const char* signature);

	void Hook();
}