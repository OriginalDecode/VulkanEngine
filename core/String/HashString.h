#pragma once
#include "Core/Types.h"
namespace Core
{
    class HashString
    {
    public:
        HashString( const HashString& str );
        HashString( const char* str );

        const char* debug_str() const;

        HashString& operator=( const HashString& str );

        bool operator==( const HashString& hashStr ) const;
        bool operator==( uint64 hash ) const;

    private:
        uint64 m_Hash = 0;
#ifdef _DEBUG
        const char* m_DebugString;
#endif
    };

}; // namespace Core