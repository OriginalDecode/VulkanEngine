#include "HashString.h"
#include "Core/utilities/utilities.h"
namespace Core
{
	HashString::HashString(const HashString& str)
		: m_Hash(str.m_Hash)
#ifdef _DEBUG
		, m_DebugString(str.m_DebugString)
#endif
	{
	}

	HashString::HashString(const char* str) { m_Hash = Hash(str); }

	const char* HashString::debug_str() const
	{
#ifdef _DEBUG
		return m_DebugString;
#else
		return "no string in final";
#endif
	}

	HashString& HashString::operator=(const HashString& str)
	{
		m_Hash = str.m_Hash;
#ifdef _DEBUG
		m_DebugString = str.m_DebugString;
#endif
		return *this;
	}

	bool HashString::operator==(uint64 hash) const { return m_Hash == hash; }

	bool HashString::operator==(const HashString& hashStr) const { return m_Hash == hashStr.m_Hash; }

} // namespace Core
