#pragma once
#include <Windows.h>
#include <cstdint>
#include <vector>

extern std::uint8_t* PatternScan(void* module, const char* signature);