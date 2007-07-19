/***************************************************************************
 *   Copyright (C) 2004-2006 Marc Boris Duernr                             *
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
#include <sstream>


// GNU C++ compiler
#ifdef __GNUC__
    #define PT_PRETTY_FUNCTION __PRETTY_FUNCTION__
// Borland C++
#elif defined(__BORLANDC__)
    #define PT_PRETTY_FUNCTION __FUNC__
// Microsoft C++ compiler
#elif defined(_MSC_VER)
    // .NET 2003 support's demangled function names
    #if _MSC_VER >= 1300
        #define PT_PRETTY_FUNCTION __FUNCDNAME__
    #else
        #define PT_PRETTY_FUNCTION __FUNCTION__
    #endif
// otherwise use standard macro
#else
    #define PT_PRETTY_FUNCTION __FUNCTION__
#endif


/** @brief super macro to construct a Pt::SourceInfo object
    @ingroup Pt
*/
#define PT_SOURCEINFO Pt::SourceInfo(__FILE__,__LINE__,PT_PRETTY_FUNCTION)


namespace Pt {

/** @brief Source code info class
    @ingroup Pt

    This class is used by exception classes for storing information
    about the location in the source code where the error occured.
    The PT_SOURCEINFO macro can be used to construct a Pt::SourceInfo
    object conveniently.\n
    \n
    Example:
    @code
    int main()
    {
        try
        {
            const Pt::SourceInfo& si = PT_SOURCEINFO;
            throw Pt::Exception("Some error occured", si);

        }
        catch(const Pt::Exception& e) {
            cerr << "Error    : " << e.what() << endl;
            cerr << "File     : " << e.sourceInfo().file() << endl;
            cerr << "Line     : " << e.sourceInfo().line() << endl;
            cerr << "Function : " << e.sourceInfo().func() << endl;
            return 1;
        }
        return 0;
    }
    @endcode
*/
class SourceInfo {
    public:
        /** @brief Copy constructor
        */
        inline SourceInfo(const SourceInfo& si) throw()
        : _file(si._file), _line(si._line), _func(si._func)
        { }

        /** @brief Constructor

                Do not use the constructor directly, but the PT_SOURCEINFO
                macro to take advantage of compiler specific macros to
                indicate the source file name, position and function name.

                @param file filename of the source
                @param line line number of the source
                @param func function name of the source
        */
        inline SourceInfo(const char* file, unsigned int line, const char* func) throw()
        : _file(file), _line(line), _func(func)
        {
            std::stringstream ss;
            ss << line;
            ss >> _lineNo;
        }

        /**  @brief Returns the filename

                Returns the name of the file where the exception has
                been thrown.

                @return name of the file where the exception was thrown
        */
        inline const char* file() const throw()
        { return _file.c_str(); }

        /** @brief Returns the line number

                Returns the line number of the file where the exception
                has been thrown.

                @return line number where the exception was thrown
        */
        inline unsigned int line() const throw()
        { return _line; }

        inline std::string str() const
        { return _file + ":" + _lineNo; }

        operator std::string() const
        { return this->str(); }

        /** @brief Returns the function signature

                Returns the signature of the function where the exception
                has been thrown.

                @return the function signature
        */
        inline const char* func() const throw()
        { return _func.c_str(); }

        /** @brief Assignment operator
        */
        SourceInfo& operator=(const SourceInfo& si) throw()
        {
            _file = si._file;
            _line = si._line;
            _func = si._func;
            return *this;
        }

    private:
        std::string  _file;
        unsigned int _line;
        std::string _lineNo;
        std::string  _func;
};


inline std::string operator+(const std::string& what, const SourceInfo& info)
{
    return info.str() + ": " + what;
}

} // namespace Pt

#endif
