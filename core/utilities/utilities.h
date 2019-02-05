#pragma once
#include <string>
namespace Core
{
    std::wstring ToWideString(const std::string& str);
    std::string ToString(const std::wstring& str);


}; //namespace Core