/***************************************************************************
 *   Copyright (C) 2006-2008 Marc Boris Duerner                            *
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
#ifndef PT_SHAREDLIBIMPL_H
#define PT_SHAREDLIBIMPL_H

#include "Pt/System/SystemError.h"
#include "Pt/System/SharedLib.h"
#include "win32.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <windows.h>

namespace Pt {

namespace System {

class SharedLibImpl
{
    public:
        SharedLibImpl()
        : _handle(0)
        { }

        SharedLibImpl(const std::string& path)
        : _handle(0)
        {
            this->open(path);
        }

        ~SharedLibImpl() throw()
        {
            if(_handle != 0) {
                ::FreeLibrary(_handle);
            }
        }

        void open(const std::string& path)
        {
            if(_handle != 0) {
                return;
            }

            std::basic_string<TCHAR> tpath = win32::fromMultiByte(path);
            _handle = ::LoadLibrary( tpath.c_str() );

            if(_handle == 0)
            {
                Pt::uint32_t errorCode = GetLastError();
                std::stringstream ss;
                ss << "Could not open shared library " << path << " error " << errorCode;
                throw SystemError(ss.str(), PT_SOURCEINFO);
            }
        }

        void* resolve(const char* symbol)
        {
            if(_handle == 0) {
                return 0;
            }

            std::basic_string<TCHAR> tsymbol = win32::fromMultiByte(symbol);
            return (void*) ( ::GetProcAddress( _handle, tsymbol.c_str() ) );
        }

        bool failed()
        { return _handle == 0; }

    private:
        HMODULE _handle;
};

} // namespace System

} // namespace Pt

#endif
