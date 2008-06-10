/***************************************************************************
 *   Copyright (C) 2006-2008 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 Tobias Mueller                                *
 *   Copyright (C) 2006-2007 PTV AG                                        *
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

#include "Pt/Atomicity.h"
#include "Pt/System/SharedLib.h"
#include "Pt/System/SystemError.h"
#include <string>
#include <dlfcn.h>

namespace Pt {

namespace System {

class SharedLibImpl
{
    public:
        SharedLibImpl()
        : _refs(1)
        , _handle(0)
        { }

        SharedLibImpl(const std::string& path)
        : _refs(1)
        , _handle(0)
        {
            this->open(path);
        }

        ~SharedLibImpl()
        {
            if(_handle)
                ::dlclose(_handle);
        }

        Pt::atomic_t refs() const
        {
            return _refs;
        }

        Pt::atomic_t ref()
        {
            return Pt::atomicIncrement(_refs);
        }

        Pt::atomic_t unref()
        {
            return Pt::atomicDecrement(_refs);
        }

        void open(const std::string& path)
        {
            if(_handle)
                return;

            /* RTLD_NOW: since lazy loading is not supported by every target platform
               RTLD_GLOBAL: make the external symbols in the loaded library available for subsequent libraries.
                           see also http://gcc.gnu.org/faq.html#dso
            */
            int flags = RTLD_NOW | RTLD_GLOBAL;

            _handle = ::dlopen(path.c_str(), flags);
            if( !_handle ) {
                throw SystemError(dlerror(), PT_SOURCEINFO);
            }
        }

        void* resolve(const char* symbol)
        {
            if(_handle)
            {
                void* p =  ::dlsym(_handle, symbol);
                return p;
            }

            return 0;
        }

        bool failed()
        { return _handle == 0; }

        static std::string suffix()
        {
            return ".so";
        }

        static std::string prefix()
        {
            return "lib";
        }

    private:
        Pt::atomic_t _refs;
        void* _handle;
};

} // namespace System

} // namespace Pt

#endif
