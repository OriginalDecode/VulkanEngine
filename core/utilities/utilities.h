#pragma once
#include "Core/Types.h"
#include <string>

#define LPE_DEFINE_HANDLE( object ) typedef struct object##_T* object;
namespace Core
{
    std::wstring ToWideString( const std::string& str );
    std::string ToString( const std::wstring& str );

    uint64 Hash( const std::string& str );
    uint64 Hash( const std::string& str, uint32 seed );

    void DebugPrintLastError();
    
}; //namespace Core