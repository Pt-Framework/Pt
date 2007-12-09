/***************************************************************************
 *   Copyright (C) 2004-2007 by Marc Boris Duerner                         *
 *   Copyright (C) 2004-2007 by Stepan Beal                                *
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
#ifndef Pt_Convert_h
#define Pt_Convert_h

#include <Pt/Api.h>
#include <Pt/String.h>
#include <Pt/Exception.h>
#include <Pt/StringStream.h>
#include <string>
#include <stdexcept>
#include <iomanip>
#include <limits>

namespace Pt {

    template <typename T>
    inline void convert(Pt::String& s, const T& value)
    {
        Pt::StringStream os;
        os << value;
        s = os.str();
    }


    template <typename T>
    inline void convert(T& t, const Pt::String& str)
    {
        Pt::StringStream is(str);
        is >> t;
    }


    inline void convert(Pt::String& s, const Pt::String& str)
    {
        s = str;
    }


    inline void convert(Pt::String& s, bool value)
    {
        static const Pt::String istrue(L"true");
        static const Pt::String isfalse(L"false");
        s = value ? istrue : isfalse;
    }


    inline void convert(bool& n, const Pt::String& str)
    {
        static const Pt::String istrue(L"true");
        n = (str == istrue);
    }


    inline void convert(Pt::String& s, char value)
    {
        s = Pt::String( 1, Pt::Char(value) );
    }


    inline void convert(char& n, const Pt::String& str)
    {
        if( str.empty() )
            throw ConversionError("char", PT_SOURCEINFO);

        n = str[0].narrow('*');
    }


    inline void convert(Pt::String& s, unsigned char value)
    {
        Pt::StringStream ss;
        unsigned int i = static_cast<unsigned int>(value);
        ss << i;
        s = ss.str();
    }


    inline void convert(unsigned char& n, const Pt::String& str)
    {
        if( str.empty() )
            throw ConversionError("unsigned char", PT_SOURCEINFO);

        // interpret as numeric value
        Pt::StringStream ss(str);
        unsigned int i = 0;
        ss >> i;
        n = static_cast<unsigned char>(i);
    }


    inline void convert(Pt::String& s, signed char value)
    {
        Pt::StringStream ss;
        signed int i = static_cast<signed int>(value);
        ss << i;
        s = ss.str();
    }


    inline void convert(signed char& n, const Pt::String& str)
    {
        if( str.empty() )
            throw ConversionError("signed char", PT_SOURCEINFO);

        // interpret as numeric value
        Pt::StringStream ss(str);
        signed int i = 0;
        ss >> i;
        n = static_cast<signed char>(i);
    }


    inline void convert(Pt::String& s, const std::string& value)
    {
        s = Pt::String::widen(value);
    }


    inline void convert(std::string& s,const Pt::String& str)
    {
        s = str.narrow();
    }


    inline void convert(Pt::String& s, float value)
    {
        // not a number
        if(value != value)
        {
            s = L"NAN";
            return;
        }

        Pt::StringStream os;
        os << value;
        s = os.str();
    }


    inline void convert(float& n, const Pt::String& str)
    {
        // not a number
        if(str == L"NAN")
        {
            n = std::numeric_limits<float>::quiet_NaN();
            return;
        }

        Pt::StringStream is(str);
        is >> n;

        if( is.fail() )
            throw ConversionError("float", PT_SOURCEINFO);
    }


    inline void convert(Pt::String& s, double value)
    {
        // not a number
        if(value != value)
        {
            s = L"NAN";
            return;
        }

        Pt::StringStream os;
        os << std::fixed << std::setprecision(15) << value;
        s = os.str();
    }


    inline void convert(double& n, const Pt::String& str)
    {
        // not a number
        if(str == L"NAN")
        {
            n = std::numeric_limits<float>::quiet_NaN();
            return;
        }

        Pt::StringStream is(str);
        is >> std::fixed >> std::setprecision(15) >> n;

        if( is.fail() )
            throw ConversionError("float", PT_SOURCEINFO);
    }


    template<typename T, typename S>
    void convert(T& to, const S& from)
    {
        Pt::StringStream ss;
        if( !(ss << from && ss >> to) )
            throw std::bad_cast("bad lexical cast");
    }


    template<typename T, typename S>
    struct Convert
    {
        T operator()(const S& from) const
        {
            T value = T();
            convert(value, from);
            return value;
        }
    };


    template<typename T, typename S>
    T convert(const S& from)
    {
        T value = T();
        convert(value, from);
        return value;
    }

} // namespace Pt

#endif
