#ifndef PT_SYSTEM_WIN32_H
#define PT_SYSTEM_WIN32_H

#include "Pt/System/Api.h"
#include "Pt/SourceInfo.h"
#include <vector>
#include <string>
#include <stdexcept>
#include <string.h>
#include <windows.h>
#include <tchar.h>

namespace Pt {

namespace System {

namespace win32 {

    inline std::string toMultiByte(const wchar_t* from)
    {
        size_t length = WideCharToMultiByte(CP_ACP, 0, from, -1, NULL, 0, NULL, NULL);

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

#ifdef _UNICODE
    inline std::wstring fromMultiByte(const std::string& from)
    {
        size_t length = MultiByteToWideChar(CP_ACP, 0, from.c_str(), -1, NULL, 0);

        // condsider using a vector<wchar_t> as buffer
        wchar_t* wbuf = new wchar_t[length];
        length = MultiByteToWideChar(CP_ACP, 0, from.c_str(), -1, wbuf, length);
		if(length == 0)
			throw std::runtime_error(PT_SOURCEINFO + "MultiByteToWideChar failed");
        
		std::wstring wstr(wbuf, length-1);
        delete[] wbuf;
        return wstr;
    }
#else
    inline std::string fromMultiByte(const std::string& from)
    {
        return from;
    }
#endif

} // namespace win32

} // namespace System

} // namespace Pt

#endif


// old implementation using vector<TCHAR>
/*
#ifdef _UNICODE
    inline std::vector<wchar_t> fromMultiByte(const std::string& from)
    {
        size_t length = MultiByteToWideChar(CP_ACP, 0, from.c_str(), -1, NULL, 0);
        std::vector<wchar_t> wstr(length+1, L'\0'); // +1 should not be neccessary
        MultiByteToWideChar(CP_ACP, 0, from.c_str(), -1, &wstr[0], length);
        return wstr;
    }
#else
    inline std::vector<char> fromMultiByte(const std::string& from)
    {
        std::vector<char> to( from.begin(), from.end() );
        to.push_back('\0');
        return to;
    }
#endif
*/
