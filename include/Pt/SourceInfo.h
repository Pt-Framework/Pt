/***************************************************************************
 *   Copyright (C) 2004-2008 Marc Boris Duerner                            *
 *   Copyright (C)      2006 Aloysius Indrayanto                           *
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

#define PT_SOURCEINFO_STR __FILE__ ":" PT_TOSTRING(__LINE__)

/** @brief Builds an error message including source information
    @ingroup Pt
*/
#define PT_ERROR_MSG(msg) __FILE__ ":" PT_TOSTRING(__LINE__) ": " #msg

/** @brief Construct a Pt::SourceInfo object
    @ingroup Pt
*/
#define PT_SOURCEINFO Pt::SourceInfo(__FILE__, __LINE__, PT_FUNCTION, \
                                     __FILE__ ":" PT_TOSTRING(__LINE__) )

namespace Pt {

/** @brief Source code info class
    @ingroup Pt

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
*/
class SourceInfo {
    public:
        /** @brief Copy constructor
        */
        inline SourceInfo(const SourceInfo& si)
        : _file(si._file), _line(si._line), _func(si._func), _msg(si._msg)
        { }

        /** @brief Constructor

            Do not use the constructor directly, but the PT_SOURCEINFO
            macro to take advantage of compiler specific macros to
            indicate the source file name, position and function name.
        */
        inline SourceInfo(const char* file, unsigned int line, const char* func, const char* msg)
        : _file(file), _line(line), _func(func), _msg(msg)
        { }

        /**  @brief Returns the filename
        */
        inline const char* file() const
        { return _file; }

        /** @brief Returns the line number
        */
        inline unsigned int line() const
        { return _line; }

        operator std::string() const
        { return std::string(_msg); }

        /** @brief Returns a string describing the location
        */
        inline const char* where() const
        { return _msg; }

        /** @brief Returns the function signature
        */
        inline const char* func() const
        { return _func; }

        /** @brief Assignment operator
        */
        SourceInfo& operator=(const SourceInfo& si)
        {
            _file = si._file;
            _line = si._line;
            _func = si._func;
            _msg = si._msg;
            return *this;
        }

    private:
        const char*  _file;
        unsigned int _line;
        const char*  _func;
        const char* _msg;
};


inline std::string operator+(const std::string& what, const SourceInfo& info)
{
    return std::string( info.where() ) + ": " + what;
}

inline std::string operator+(const char* what, const SourceInfo& info)
{
    return std::string( info.where() ) + ": " + what;
}

inline std::string operator+( const SourceInfo& info, const std::string& what)
{
    return std::string( info.where() ) + ": " + what;
}

inline std::string operator+(const SourceInfo& info, const char* what)
{
    return std::string( info.where() ) + ": " + what;
}

} // namespace Pt

#endif
