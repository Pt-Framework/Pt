/*
 * Copyright (C) 2006-2008 Marc Boris Duerner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef PT_SYSTEM_LIBRARY_H
#define PT_SYSTEM_LIBRARY_H

#include <Pt/System/Api.h>
#include <Pt/System/SystemError.h>
#include <Pt/System/Path.h>
#include <string>

namespace Pt {

namespace System {

class Symbol;

/** @brief Thrown, when a symbol is not found in a library

    @ingroup Pt-System-Libraries
*/
class PT_SYSTEM_API SymbolNotFound : public SystemError
{
    public:
        /** @brief Construct with symbol.
        */
        SymbolNotFound(const std::string& sym);

        /** @brief Construct with symbol.
        */
        SymbolNotFound(const char* sym);

        //! @brief Destructor
        ~SymbolNotFound() throw()
        {}

        //! @brief Returns the symbol, which was not found
        const std::string& symbol() const
        { return _symbol; }

    private:
        std::string _symbol;
};

/** @brief Shared library loader.

    A process can load a shared library after it has started and resolve
    symbols from it. %Library is the portable loader. It opens a library
    image and returns symbols by name.

    @code
    typedef int (*MyFunc)();

    Pt::System::Path libPath = "MyLib";
    Pt::System::Library library(libPath);

    Pt::System::Symbol symbol = library.getSymbol("myFunction");

    MyFunc func = reinterpret_cast<MyFunc>(symbol.sym());
    int result = func();
    @endcode

    The path "MyLib" is a basename. Construction and open() look for a
    file at the given path. If none is there, the path is extended by
    the platform suffix, then by the shared-library prefix. A basename
    is enough. An @link Pt::AccessFailed AccessFailed@endlink exception
    is thrown when no image is found. A second open() may close the
    image loaded before.

    prefix() and suffix() return the platform naming pieces so a
    portable library name can be built without relying on that search.
    suffix() is ".so" on Linux and ".dll" on Windows. prefix() is "lib"
    on Linux and empty on Windows.

    @code
    Pt::System::Path libPath = Pt::System::Library::prefix();
    libPath += "MyLib";
    libPath += Pt::System::Library::suffix();
    @endcode

    getSymbol() returns a %Symbol when the name exists. It throws
    %SymbolNotFound otherwise. The address is %Symbol::sym(). A %Symbol
    holds the %Library that produced it, so the image stays loaded while
    the symbol exists.

    The index operator and resolve() return a void pointer, or a null
    pointer when the name is missing. They do not throw. getSymbol()
    treats a missing name as an error. The index operator leaves that
    test to the caller.

    Standard C++ does not allow a cast from a void pointer to a function
    pointer. Nearly all runtimes implement that as an extension. The
    caller casts %Symbol::sym() to a function pointer to call it.

    @ingroup Pt-System-Libraries
*/
class PT_SYSTEM_API Library
{
    public:
        /** @brief Default Constructor which does not load a library.
         */
        Library();

        /** @brief Loads a shared library.

             If a file could not be found at the given path, the path will be
             extended by the platform-specific shared library extension first
             and then also by the shared library prefix. If still no file can
             be found an exception of type AccessFailed is thrown.
             Otherwise, the library is loaded immediately.
        */
        explicit Library(const Path& path);

        /** @brief Copy constructor.
        */
        Library(const Library& other);

        /** @brief Assignment operator.
        */
        Library& operator=(const Library& other);

        /** @brief The destructor unloads the shared library from memory.
         */
        ~Library();

        /** @brief Loads a shared library.

             If a file could not be found at the given path, the path will be
             extended by the platform-specific shared library extension first
             and then also by the shared library prefix. If still no file can
             be found an exception of type AccessFailed is thrown.
             Otherwise, the library is loaded immediately.
             Calling this method twice might close the previously loaded library.
        */
        Library& open(const Path& path);

        //! @brief Closes the shared library.
        void close();

        /** @brief Resolves the symbol \a symbol from the shared library
            Returns the address of the symbol or 0 if it was not found.
         */
        void* operator[](const char* symbol) const
        { return this->resolve(symbol); }

        /** @brief Resolves the symbol \a symbol from the shared library
            Returns the address of the symbol or 0 if it was not found.
         */
        void* resolve(const char* symbol) const;

        /** @brief Returns null if invalid
         */
        operator const void*() const;

        /** @brief Resolves the symbol \a symbol from the shared library.

            Throws SymbolNotFound if the symbol could not be resolved.
         */
        Symbol getSymbol(const char* symbol) const;

        /** @brief Returns true if invalid
         */
        bool operator!() const;

        /** @brief Returns the path to the shared library image
        */
        const Path& path() const;

        /** @brief Returns the extension for shared libraries

            Returns ".so" on Linux, ".dll" on Windows.
        */
        static const char* suffix();

        /**  @brief Returns the prefix for shared libraries

             Returns "lib" on Linux, "" on Windows 
        */
        static const char* prefix();

    protected:
        //! @internal
        void detach();

    private:
        //! @internal
        class LibraryImpl* _impl;

        //! @internal
        System::Path _path;
};

/** @brief Symbol resolved from a shared library

    @ingroup Pt-System-Libraries
*/
class Symbol
{
    public:
        /** @brief Default constructor.
        */
        Symbol()
        : _sym(0)
        { }

        /** @brief Construct with library and symbol address.
        */
        Symbol(const Library& lib, void* sym)
        : _lib(lib), _sym(sym)
        { }

        /** @brief Returns the symbol address
        */
        void* sym() const
        { return _sym; }

        /** @brief Returns the library where the symbol was resolved
        */
        const Library& library() const
        { return _lib; }

        /** @brief Returns the symbol address
        */
        operator void*() const
        { return _sym; }

    private:
        Library _lib;
        void* _sym;
};

} // namespace System

} // namespace Pt

#endif // PT_SYSTEM_LIBRARY_H
