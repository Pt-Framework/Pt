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

#include "Pt/Api.h"
#include "Pt/System/SystemError.h"
#include "Pt/System/SharedLib.h"
#include "Pt/System/File.h"

#include "win32.h"

#include <vector>
#include <iostream>
#include <sstream>
#include <windows.h>


namespace Pt {

namespace System {

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
        ~SharedLibImpl() throw()
        {
            if(_handle != 0) {
                ::FreeLibrary(_handle);
            }
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
            if(_handle != 0) {
                return;
            }

            std::basic_string<TCHAR> tpath = win32::fromMultiByte(libraryFile.path());
            _handle = ::LoadLibrary( tpath.c_str() );

            if(_handle == 0)
            {
                Pt::uint32_t errorCode = GetLastError();
                std::stringstream ss;
                ss << "Could not open shared library(" << libraryFile.path() << "), Error code: " << errorCode;
                throw SystemError(ss.str(), PT_SOURCEINFO);
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
            if(_handle == 0) {
                return 0;
            }

            std::basic_string<TCHAR> tsymbol = win32::fromMultiByte(symbol);
            return (void*) ( ::GetProcAddress( _handle, tsymbol.c_str() ) );
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
            std::basic_string<TCHAR> tpath = win32::fromMultiByte(libraryFile.path());
            HMODULE handle = ::LoadLibrary( tpath.c_str() );

            // TODO: throw different exception when path does not exist
            if(handle == 0)
                throw SystemError("Could not open shared library", PT_SOURCEINFO);

            std::basic_string<TCHAR> tsymbol = win32::fromMultiByte(symbol);
            return (void*) ::GetProcAddress( handle, tsymbol.c_str() );
        }

    private:
        HMODULE _handle;
};

} // namespace System

} // namespace Pt

#endif
