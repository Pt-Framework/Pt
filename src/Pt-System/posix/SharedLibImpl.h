/***************************************************************************
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
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

#include "Pt/System/SharedLib.h"
#include "Pt/System/SystemError.h"

#include <string>
#include <iostream>

#include <dlfcn.h>


namespace Pt {

namespace System {

//! @brief Implementation of SharedLib class for POSIX systems
/**
    This class represents the implementation of the bridge pattern of
    the SharedLib class for POSIX systems. It implements the specific
    procedures of SharedLib in a system dependend manner. The behaviour
    of the shared library loading is adapted to the behaviour of shared
    library loading under MS Windows operating systems.
    This means that the mode of loading shared libraries is limitted
    to the RTLD_NOW mode only.

    @see SharedLib documentation
*/
class SharedLibImpl {
    public:
        //! @brief default Constructor
        SharedLibImpl()
        : _handle(0)
        { }

        /**
         * @brief Constructor which takes the path to the shared library to load
         * @see SharedLib#SharedLib()
         * @param libraryFile A File object referencing the library in the file system.
         */
        SharedLibImpl(const File& libraryFile)
        : _handle(0)
        {
            this->open(libraryFile);
        }

        //! @brief Destructor
        ~SharedLibImpl()
        {
            if(_handle)
                ::dlclose(_handle);
        }

        /**
         * @brief Loads the shared library specified by path
         *
         * This method holds the operating system dependend code to actually
         * load the shared library.
         * @see SharedLib#open()
         * @param libraryFile A File object referencing the library in the file system.
         */
        void open(const File& libraryFile)
        {
            if(_handle)
                return;

            /* RTLD_NOW: since lazy loading is not supported by every target platform
               RTLD_GLOBAL: make the external symbols in the loaded library available for subsequent libraries.
                           see also http://gcc.gnu.org/faq.html#dso
            */
            int flags = RTLD_NOW | RTLD_GLOBAL;

            _handle = ::dlopen(libraryFile.path().c_str(), flags);
            if( !_handle ) {
                throw SystemError(dlerror(), PT_SOURCEINFO);
            }
        }

        /**
         * @brief Resolves the symbol specified by symbol
         *
         * This method holds the operating system dependend code to actually
         * resolve the symbol within the shared library.
         * @see SharedLib#resolve()
         * @param symbol the symbol to resolve
         * @return the resolved symbol or 0 if the symbol cannot be resolved
         */
        void* resolve(const char* symbol)
        {
            if(_handle)
                return ::dlsym(_handle, symbol);

            return 0;
        }

        /**
         * @brief Returns if the loading of the shared library was successful or not
         * @see SharedLib#failed()
         * @return true if the loading of the shared library has failed, false otherwise.
         */
        bool failed()
        { return _handle == 0; }

    public:
        /**
         * @brief Implicitely loads the shared library specified by path and tries to resolve the symbol specified by symbol
         * 
         * This method contains the operating system dependend code to load the
         * shared library and to resolve the desired symbol.
         * @see SharedLib::failed()
         * @param libraryFile A File object referencing the library in the file system.
         * @param symbol The symbol to resolve within the loaded library.
         * @return The resolved symbol or 0 if the loading of the shared library has failed.
         */
        static void* openResolve(const File& libraryFile, const char* symbol)
        {
            void* handle = ::dlopen(libraryFile.path().c_str(), RTLD_NOW);
            if(handle)
                return ::dlsym(handle, symbol);

            return 0;
        }

    private:
        void* _handle;
};

} // namespace System

} // namespace Pt

#endif
