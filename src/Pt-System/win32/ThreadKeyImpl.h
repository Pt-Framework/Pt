/***************************************************************************
 *   Copyright (C) 2005-2006 by Marc Boris Dürner                          *
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

#include "Pt/Api.h"
#include "Pt/System/SystemError.h"

#include <windows.h>

// TLS_OUT_OF_INDEXES not defined on WinCE
#ifndef TLS_OUT_OF_INDEXES
#define TLS_OUT_OF_INDEXES 0xffffffff
#endif

namespace Pt {

namespace System {

class ThreadKeyImpl {
    public:
        ThreadKeyImpl() throw(SystemError)
        {
            DWORD handle = TlsAlloc();
            if( handle == TLS_OUT_OF_INDEXES )
                throw SystemError("Could not create thread-key", PT_SOURCEINFO);

            _handle = handle;
        }

        ~ThreadKeyImpl() throw()
        {
            TlsFree(_handle);
        }

        void set(void* ptr) throw(SystemError)
        {
            if( !TlsSetValue(_handle, ptr) )
                throw SystemError("Could not set thread-key", PT_SOURCEINFO);
        }

        void* get() const throw(SystemError)
        {
            return TlsGetValue(_handle);
        }

    private:
        DWORD _handle;
};

} // namespace System

} // !namespace Pt
