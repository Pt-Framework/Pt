/***************************************************************************
 *   Copyright (C) 2004-2006 by Marc Boris Dürner                          *
 *   Copyright (C) 2004-2006 by Stepan Beal                                *
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
#ifndef Pt_VariantTraits_h
#define Pt_VariantTraits_h

#include <Pt/Api.h>
#include <Pt/String.h>
#include <Pt/StringStream.h>
#include <string>
#include <sstream>
#include <iomanip>


namespace Pt {

    template <typename T>
    struct VariantTraits
    {
        static const int isUnicode = 0;

        static void toData(std::string& data, const T& value)
        {
            std::ostringstream os;
            os << value;
            data = os.str();
        }

        static bool fromData(T& value, const std::string& data)
        {
            std::istringstream is(data);
            is >> value;
            return !is.fail();
        }

        static bool fromData(T& value, const Pt::String& data)
        {
            Pt::StringStream is(data);
            is >> value;
            return !is.fail();
        }
    };

    template <>
    struct VariantTraits<char>
    {
        static const int isUnicode = 0;

        static void toData(std::string& data, char value)
        {
            std::ostringstream os;
            os << value;
            data = os.str();
        }

        static bool fromData(char& value, const std::string& data)
        {
            std::istringstream is(data);
            is >> value;
            return !is.fail();
        }

        static bool fromData(char& value, const Pt::String& data)
        {
            if( data.empty() )
                return false;

            value = data[0].narrow('*');
            return true;
        }
    };

    template <>
    struct VariantTraits<std::string>
    {
        static const int isUnicode = 0;

        static void toData(std::string& data, const std::string& value)
        { data = value; }

        static bool fromData(std::string& value, const std::string& data)
        {
            value = data;
            return true;
        }

        static bool fromData(std::string& value, const Pt::String& data)
        {
            value = data.narrow();
            return true;
        }
    };


    template <>
    struct VariantTraits<Pt::String>
    {
        static const int isUnicode = 1;

        static void toData(std::string& data, const Pt::String& value)
        {
            data = value.narrow();
        }

        static bool fromData(Pt::String& value, const std::string& data)
        {
            value = Pt::String::widen(data);
            return true;
        }

        static bool fromData(Pt::String& value, const Pt::String& data)
        {
            value = data;
            return true;
        }
    };


    /** \brief Special trait for double.

        VariantTraits for double.
    */
    template <>
    struct VariantTraits<double>
    {
        static const int isUnicode = 0;

        static void toData(std::string& data, const double& value)
        {
            std::ostringstream os;
            os << std::scientific << std::setprecision(15) << value;
            data = os.str();
        }

        static bool fromData(double& value, const std::string& data)
        {
            std::istringstream is(data);
            is >> std::scientific >> std::setprecision(15) >> value;
            return !is.fail();
        }

        static bool fromData(double& value, const Pt::String& data)
        {
            Pt::StringStream is(data);
            is >> std::scientific >> std::setprecision(15) >> value;
            return !is.fail();
        }
    };

} // namespace Pt


#endif

