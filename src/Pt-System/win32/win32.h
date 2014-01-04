#ifndef PT_SYSTEM_WIN32_H
#define PT_SYSTEM_WIN32_H

#include "Pt/System/Api.h"
#include "Pt/SourceInfo.h"
#include <vector>
#include <string>
#include <stdexcept>
#include <cstddef>
#include <windows.h>

namespace Pt {

namespace System {

namespace win32 {

inline std::string toMultiByte(const wchar_t* from)
{
    int length = WideCharToMultiByte(CP_ACP, 0, from, -1, NULL, 0, NULL, NULL);

    std::vector<char> str(length);
    int ret = WideCharToMultiByte(CP_ACP, 0, from, -1, &str[0], length, NULL, NULL);
    if(ret == 0)
        throw std::runtime_error(PT_SOURCEINFO + "WideCharToMultiByte failed");

    return std::string(&str[0], length-1);
}


inline std::string toMultiByte(const char* from)
{
    return std::string(from);
}


inline void fromMultiByte(const std::string& from, std::wstring& to)
{
    int length = MultiByteToWideChar(CP_ACP, 0, from.c_str(), -1, NULL, 0);

    std::vector<wchar_t> wbuf(length);
    length = MultiByteToWideChar(CP_ACP, 0, from.c_str(), -1, &wbuf[0], length);
    if(length == 0)
    {
        throw std::runtime_error(PT_SOURCEINFO + "MultiByteToWideChar failed");
    }

    to.assign(&wbuf[0], length-1);
}


inline void fromMultiByte(const char* from, std::wstring& to)
{
    int length = MultiByteToWideChar(CP_ACP, 0, from, -1, NULL, 0);

    std::vector<wchar_t> wbuf(length);
    length = MultiByteToWideChar(CP_ACP, 0, from, -1, &wbuf[0], length);
    if(length == 0)
    {
        throw std::runtime_error(PT_SOURCEINFO + "MultiByteToWideChar failed");
    }

    to.assign(&wbuf[0], length-1);
}


inline void fromMultiByte(const std::string& from, std::string& to)
{
    to = from;
}

inline void fromMultiByte(const char* from, std::string& to)
{
    to = from;
}

} // namespace win32

} // namespace System

} // namespace Pt

#endif
