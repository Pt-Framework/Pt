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
#ifndef PT_FLOAT_H
#define PT_FLOAT_H

#include "Pt/Types.h"
#include <cmath>

namespace Pt
{

/** @brief This class holds methods to convert float values to IEEE754 single representation and vice versa.
@ingroup Pt
*/
class Float
{

public:

    /**
     * @brief Converts a float value to an IEEE754 single representation.
     *
     * @param val The float value to convert.
     * @return The IEEE754 single representation.
     */
    static inline const Pt::uint32_t toIEEE754Single(const float val)
    {
        Pt::uint32_t res = 0;
        float f = val;

        // calculate and set the sign bit
        if(f < 0.0f)
        {
            res |= 0x80000000;
            f *= -1.0f;
        }
        float frac = 0.0f;
        Pt::int8_t exp = 0;

        // calculate the exponent and the fraction
        if(f < 1.0f)
        {
            exp = -1;
            frac = static_cast<float>(f / (std::pow(2.0f, exp)));
            while(frac < 1 && exp > -126)
            {
                exp--;
                frac = static_cast<float>(f / (std::pow(2.0f, exp)));
            }
        }
        else
        {
            exp = 0;
            frac = static_cast<float>(f / (std::pow(2.0f, exp)));
            while(frac > 2 && exp < 127)
            {
                exp++;
                frac = static_cast<float>(f / (std::pow(2.0f, exp)));
            }
        }
        frac -= 1.0f;
        exp += SINGLE_PRECISION_BIAS;

        // set the exponent bits
        Pt::uint32_t shift = exp;
        shift &= 0x000000ff;
        shift = shift << ((sizeof(Pt::uint32_t)*8) - 1 - SIZEOF_SINGLE_PRECISSION_EXPONENT);
        res |= shift;

        // calculate and set the mantissa bits
        float mantissaValue = 0.5f;
        Pt::uint32_t mask = 0x400000;
        for(int i = 0; frac > 0.0f && i < SIZEOF_SINGLE_PRECISSION_MANTISSA; i++)
        {
            if(frac > mantissaValue)
            {
                res |= mask;
                frac -= mantissaValue;
            }
            mantissaValue /= 2;
            mask = mask >> 1;
        }
        return res;
    }

    /**
     * @brief Converts an IEEE754 single representation to a float value.
     *
     * @param ieee754 The IEEE754 single representation to convert.
     * @return The converted float value.
     */
    static inline const float fromIEEE754Single(const Pt::uint32_t ieee754)
    {
        float res = 0.0f;

        // calculate the sign bit
        bool negativeSign = (ieee754 & 0x80000000) > 0 ? true : false;

        // calculate the exponent
        Pt::int8_t exp = static_cast<Pt::int8_t>((ieee754 >> ((sizeof(Pt::uint32_t)*8) - 1 - SIZEOF_SINGLE_PRECISSION_EXPONENT)) & 0x000000ff);
        exp -= SINGLE_PRECISION_BIAS;

        // calculate the mantissa
        Pt::uint32_t mantissa = ieee754 & 0x7fffff;

        // calculate the fraction value
        float d = 0.5f;
        Pt::uint32_t mask = 0x400000;
        for(int i = 0; i < SIZEOF_SINGLE_PRECISSION_MANTISSA; i++)
        {
            if(mantissa & mask)
            {
                res += d;
            }
            d /= 2;
            mask = mask >> 1;
        }
        res += 1;
        res *= static_cast<float>(pow(2.0f, exp));

        // set the sign
        if(negativeSign)
        {
            res *= -1;
        }
        return res;
    }

private:
    static const Pt::uint8_t SIZEOF_SINGLE_PRECISSION_MANTISSA = 23;
    static const Pt::uint8_t SIZEOF_SINGLE_PRECISSION_EXPONENT = 8;
    static const Pt::int8_t SINGLE_PRECISION_BIAS = 127;
};


}

#endif
