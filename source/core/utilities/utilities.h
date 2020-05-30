#pragma once
#include "Core/Types.h"
#include <string>

namespace Core
{
	float RadToDegree(float rad);
	float DegreeToRad(float degree);

	std::wstring ToWString(const std::string& str);
	std::string ToString(const std::wstring& str);

	uint64 Hash(const std::string& str);
	uint64 Hash(const std::string& str, uint32 seed);

	void DebugPrintLastError();

	void OutputDebugStr(const std::string& str);
	void OutputDebugStr(const char* fmt, ...);

}; // namespace Core
