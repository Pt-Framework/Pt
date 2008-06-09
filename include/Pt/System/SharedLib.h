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
#ifndef PT_SYSTEM_SHAREDLIB_H
#define PT_SYSTEM_SHAREDLIB_H

#include <Pt/NonCopyable.h>
#include <Pt/System/Api.h>
#include <Pt/System/SystemError.h>
#include <string>

namespace Pt {

namespace System {

/** @brief Thrown, when a symbol is not found in a library
*/
class PT_SYSTEM_API SymbolNotFound : public SystemError
{
    std::string _symbol;

    public:
        SymbolNotFound(const std::string& sym, const Pt::SourceInfo& si);

        //! @brief Destructor
        ~SymbolNotFound() throw();

        //! @brief Returns the symbol, which was not found
        const std::string& symbol() const
        { return _symbol; }
};

/** @brief Shared library loader

    This class can be used to dynamically load shared libraries and
    resolve symbols from it. The example below shows how to retrieve
    the address of the function 'myProcedure' in library 'MySharedLib':

    @code
        SharedLib shlib("MySharedLib.dll");
        void* procAddr = shlib["myProcedure"];

        typedef int (*MyProcType)();
        MyProcType proc =(MyProcType)procAddr;
        int result = proc();
    @endcode
*/
class PT_SYSTEM_API SharedLib : private NonCopyable
{
    public:

        /** @brief Default Constructor which does not load a library.
         */
        SharedLib();

        /** @brief Loads a shared library.

             If a file could not be found at the given path, the path will be extended
             by the platform-specific shared library extension first and then also by the
             shared library prefix. If still no file can be found an exception is thrown.

             The library is loaded immediately.
        */
        SharedLib(const std::string& path);

        /** @brief The destructor unloads the shared library from memory.
         */
        ~SharedLib();

        /** @brief Loads a shared library.

             If a file could not be found at the given path, the path will be extended
             by the platform-specific shared library extension first and then also by the
             shared library prefix. If still no file can be found an exception is thrown.

             If a library was previously loaded by calling the constructor with a library file
             or by calling one of the open()-methods, this method fails and throws a SystemError.
        */
        SharedLib& open(const std::string& path);

        /** @brief Resolves the symbol \a symbol from the shared library
            Returns the address of the symbol or 0 if it was not found.
         */
        void* operator[](const char* symbol);

        /** @brief Resolves the symbol \a symbol from the shared library
            Returns the address of the symbol or 0 if it was not found.
         */
        void* resolve(const char* symbol);

        /** @brief Returns null if invalid
         */
        operator void*();

        /** @brief Returns true if invalid
         */
        bool operator!();

        /** @brief Returns the path to the shared library image
         */
        const std::string& path() const;

        static std::string find(const std::string& path);

        /** @brief Returns the extension for shared libraries

            Returns ".so" on Linux, ".dll" on Windows.
        */
        static std::string suffix();

        /**  @brief Returns the prefix for shared libraries

             Returns "lib" on Linux, "" on Windows 
        */
        static std::string prefix();

    private:
        //! @internal
        class SharedLibImpl* _impl;

        //! @internal
        std::string _path;
};

} // namespace System

} // namespace Pt

extern "C" {
    //! @internal
    PT_SYSTEM_API void pt_system_testSharedLib();

}

#endif
