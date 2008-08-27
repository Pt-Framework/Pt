/***************************************************************************
*   Copyright (C) 2006 by Sven Falk                                       *
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
#ifndef PT_DOUBLE_H
#define PT_DOUBLE_H

#include "Pt/Types.h"
#include <cmath>

namespace Pt 
{

/** @brief This class holds methods to convert double values to IEEE754 double representation and vice versa.
@ingroup Pt
*/
class Double
{

public:

    /**
    * @brief Converts a double value to an IEEE754 double representation.
    *
    * @param val The double value to convert.
    * @return The IEEE754 double representation.
    */
    static inline const Pt::uint64_t toIEEE754Double(const double val)
    {
        Pt::uint64_t res = 0;
        double d = val;

        // calculate and set the sign bit
        if(d < 0.0)
        {
            res |= 0x8000000000000000ull;
            d *= -1.0;
        }

        // calculate the exponent and the fraction
        Pt::int16_t exp = 0;
        double frac = 0.0;
        if(d < 1.0)
        {
            exp = -1;
            frac = d / (std::pow(2.0, exp));
            while(frac < 1 && exp > - 1022)
            {
                exp--;
                frac = d / (std::pow(2.0, exp));
            }
        }
        else
        {
            exp = 0;
            frac = d / (std::pow(2.0, exp));
            while(frac > 2 && exp < 1023)
            {
                exp++;
                frac = d / (std::pow(2.0, exp));
            }
        }
        frac -= 1.0;
        exp += DOUBLE_PRECISION_BIAS;

        // set the exponent bits
        Pt::uint64_t shift = exp;
        shift &= 0x000000000000ffffull;
        shift = shift << ((sizeof(Pt::uint64_t)*8) - 1 - SIZEOF_DOUBLE_PRECISSION_EXPONENT);
        res |= shift;

        // calculate and set the mantissa bits
        double maskValue = 0.5;
        Pt::uint64_t mask = 0x8000000000000ull;
        for(int i = 0; frac > 0.0f && i < SIZEOF_DOUBLE_PRECISSION_MANTISSA; i++)
        {
            if(frac > maskValue)
            {
                res |= mask;
                frac -= maskValue;
            }
            maskValue /= 2;
            mask = mask >> 1;
        }
        return res;
    }

    /**
    * @brief Converts an IEEE754 double representation to a double value.
    *
    * @param ieee754 The IEEE754 double representation to convert.
    * @return The converted double value.
    */
    static inline const double fromIEEE754Double(const Pt::uint64_t ieee754)
    {
        double res = 0.0; 

        // calculate the sign bit
        bool negativeSign = (ieee754 & 0x8000000000000000ull) > 0 ? true : false;

        // calculate the exponent
        Pt::int16_t exp = static_cast<Pt::int16_t>((ieee754 >> ((sizeof(Pt::uint64_t)*8) - 1 - SIZEOF_DOUBLE_PRECISSION_EXPONENT)) & 0x00000000000007ff);
        exp -= DOUBLE_PRECISION_BIAS;

        // calculate the mantissa
        Pt::uint64_t mantissa = ieee754 & 0xFFFFFFFFFFFFFull;

        // calculate the fraction value
        double d = 0.5;
        Pt::uint64_t mask = 0x8000000000000ull;
        for(int i = 0; i < SIZEOF_DOUBLE_PRECISSION_MANTISSA; i++)
        {
            if(mantissa & mask)
            {
                res += d;
            }
            d /= 2;
            mask = mask >> 1;
        }
        res += 1;
        res *= pow(2.0, exp);
        if(negativeSign)
        {
            res *= -1;
        }
        return res;
    }

private:
    static const Pt::uint8_t SIZEOF_DOUBLE_PRECISSION_MANTISSA = 52;
    static const Pt::uint8_t SIZEOF_DOUBLE_PRECISSION_EXPONENT = 11;
    static const Pt::int16_t DOUBLE_PRECISION_BIAS = 1023;
};


}

#endif
