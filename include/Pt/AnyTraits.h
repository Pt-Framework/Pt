/***************************************************************************
 *   Copyright (C) 2004-2007 by Marc Boris Dürner                          *
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
#ifndef Pt_AnyTraits_h
#define Pt_AnyTraits_h

#include <Pt/Api.h>
#include <iosfwd>


namespace Pt {

    /** @brief Tune types for use with Any

        The behaviour of types used in Anys can be refined by specialising
        AnyTraits for the type. AnyTraits::input determines how a type is
        read from an std::istream, AnyTraits<>::output determines how a
        type is written to a std::osteam.
    */
    template <typename T>
    struct AnyTraits
    {
        /** @brief Write type to stream

            The value will be written to the std::ostream. The generic
            AnyTraits will use the operator<< defined for the type.

            @param os Output stream
            @param value value to write
        */
        static void output(std::ostream& os, const T& value)
        { os << value; }

        /** @brief Read type from stream

            The value will be read to the std::istream. The generic
            AnyTraits will use the operator>> defined for the type.

            @param os Input stream
            @param value value to read
        */
        static void input(std::istream& is, T& value)
        { is >> value; }
    };


    /** @internal */
    template <>
    struct PT_API AnyTraits<bool> {
        static void output(std::ostream& os, const bool& value);
        static void input(std::istream& is, bool& value);
    };


    /** @internal */
    template <>
    struct PT_API AnyTraits<int> {
        static void output(std::ostream& os, const int& value);
        static void input(std::istream& is, int& value);
    };


    /** @internal */
    template <>
    struct PT_API AnyTraits<float> {
        static void output(std::ostream& os, const float& value);
        static void input(std::istream& is, float& value);
    };


    /** @internal */
    template <>
    struct PT_API AnyTraits<double> {
        static void output(std::ostream& os, const double& value);
        static void input(std::istream& is, double& value);
    };

}


#endif
