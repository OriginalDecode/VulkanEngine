#include "utilities.h"

namespace Core
{
    std::wstring ToWString(const std::string& str)
    {
        return std::wstring(str.begin(), str.end());
    }

    std::string ToString(const std::wstring& str)
    {
        return std::string(str.begin(), str.end());
    }


}; //namespace Core