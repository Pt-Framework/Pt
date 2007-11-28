/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Dürner                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef PT_WIN32_H
#define PT_WIN32_H

#include <vector>
#include <string>
#include <string.h>
#include <windows.h>
#include <tchar.h>


namespace Pt {

namespace win32 {

    inline std::string toMultiByte(const wchar_t* from)
    {
        size_t length = WideCharToMultiByte(CP_ACP, 0, from, -1, NULL, 0, NULL, NULL);

        std::vector<char> str(length + 1);
        WideCharToMultiByte(CP_ACP, 0, from, -1, &str[0], length, NULL, NULL);

        std::string ret(&str[0], length);
        return ret;
    }


    inline std::string toUTF8(const wchar_t* from)
    {
        size_t length = WideCharToMultiByte(CP_UTF8, 0, from, -1, NULL, 0, NULL, NULL);

        std::vector<char> str(length + 1);
        WideCharToMultiByte(CP_UTF8, 0, from, -1, &str[0], length, NULL, NULL);

        std::string ret(&str[0], length);
        return ret;
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
        wchar_t* wbuf = new wchar_t[length + 1];
        MultiByteToWideChar(CP_ACP, 0, from.c_str(), -1, wbuf, length);
        std::wstring wstr(wbuf, length);
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

} // namespace Pt

#endif
