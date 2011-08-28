/*
 * Copyright (C) 2004-2007 Marc Boris Duerner
 * Copyright (C) 2005-2007 Aloysius Indrayanto
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
#ifndef Pt_System_SystemError_h
#define Pt_System_SystemError_h

#include <Pt/System/Api.h>
#include <Pt/SourceInfo.h>
#include <stdexcept>
#include <string>

namespace Pt {

namespace System {

/** @brief Exception class indication a system error.
 */
class PT_SYSTEM_API SystemError : public std::runtime_error
{
    public:
        SystemError(const std::string& what, const SourceInfo& si);

        explicit SystemError(const char* what);

        ~SystemError() throw()
        {}
};

/** @brief Thrown, when a shared library could not be loaded
*/
class PT_SYSTEM_API OpenLibraryFailed : public SystemError
{
    public:
        //! @brief Constructs from a message string and source info
        OpenLibraryFailed(const std::string& msg, const Pt::SourceInfo& si);

        //! @brief Destructor
        ~OpenLibraryFailed() throw()
		{}
};

/** @brief Thrown, when a symbol is not found in a library
*/
class PT_SYSTEM_API SymbolNotFound : public SystemError
{
    std::string _symbol;

    public:
        SymbolNotFound(const std::string& sym, const Pt::SourceInfo& si);

        //! @brief Destructor
        ~SymbolNotFound() throw()
		{}

        //! @brief Returns the symbol, which was not found
        const std::string& symbol() const
        { return _symbol; }
};

} // namespace System

} // namespace Pt

#endif
