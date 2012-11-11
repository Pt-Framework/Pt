/*
 * Copyright (C) 2004-2008 Marc Boris Duerner
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
#ifndef Pt_SourceInfo_h
#define Pt_SourceInfo_h

#include <Pt/Api.h>
#include <string>

// GNU C++ compiler
#ifdef __GNUC__
    #define PT_FUNCTION __PRETTY_FUNCTION__
// Borland C++
#elif defined(__BORLANDC__)
    #define PT_FUNCTION __FUNC__
// Microsoft C++ compiler
#elif defined(_MSC_VER)
    // .NET 2003 support's demangled function names
    #if _MSC_VER >= 1300
        #define PT_FUNCTION __FUNCDNAME__
    #else
        #define PT_FUNCTION __FUNCTION__
    #endif
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
    #define PT_FUNCTION __func__
// otherwise use standard macro
#else
    #define PT_FUNCTION "unknown symbol"
#endif

#define PT_STRINGIFY(x) #x
#define PT_TOSTRING(x) PT_STRINGIFY(x)

/** @brief Builds a source info string literal
*/
#define PT_SOURCEINFO_STR __FILE__ ":" PT_TOSTRING(__LINE__)

/** @brief Builds a string literal containing an error message and source information
*/
#define PT_ERROR_MSG(msg) __FILE__ ":" PT_TOSTRING(__LINE__) ": " #msg

/** @brief Construct a Pt::SourceInfo object
*/
#define PT_SOURCEINFO Pt::SourceInfo(__FILE__, PT_TOSTRING(__LINE__), PT_FUNCTION)

namespace Pt {

/** @brief Source code info class

    This class is used to store information about a location in the source 
    code. The PT_SOURCEINFO macro can be used to construct a Pt::SourceInfo
    object conveniently.

    @code
        Pt::SourceInfo si(PT_SOURCEINFO);

        // print file, line and function
        std::cout << si.file() << std::endl;
        std::cout << si.line() << std::endl;
        std::cout << si.func() << std::endl;

        // print combined string
        std::cout << si.where() << std::endl;
    @endcode

    @ingroup CoreTypes
*/
class SourceInfo 
{
    public:
        /** @brief Constructor

            Do not use the constructor directly, but the PT_SOURCEINFO
            macro to take advantage of compiler specific macros to
            indicate the source file name, position and function name.
        */
        inline SourceInfo(const char* f, const char* ln, const char* fn)
        : _file(f), _line(ln), _func(fn)
        { }

        /**  @brief Returns the filename
        */
        inline const char* file() const
        { return _file; }

        /** @brief Returns the line number
        */
        inline const char* line() const
        { return _line; }

        /** @brief Returns the function signature
        */
        inline const char* func() const
        { return _func; }

    private:
        const char* _file;
        const char* _line;
        const char* _func;
};


inline std::string operator+(const std::string& what, const SourceInfo& info)
{
    return std::string( info.file() ) + ':' + info.line() + ": " += what;
}

inline std::string operator+(const char* what, const SourceInfo& info)
{
    return std::string( info.file() ) + ':' + info.line() + ": " += what;
}

inline std::string operator+( const SourceInfo& info, const std::string& what)
{
    return std::string( info.file() ) + ':' + info.line() + ": " += what;
}

inline std::string operator+(const SourceInfo& info, const char* what)
{
    return std::string( info.file() ) + ':' + info.line() + ": " += what;
}

} // namespace Pt

#endif
