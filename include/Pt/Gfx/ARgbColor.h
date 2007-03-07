/***************************************************************************
 *   Copyright (C) 2006-2007 by Aloysius Indrayanto                        *
 *   Copyright (C) 2006-2007 by Marc Boris Dürner                          *
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
#ifndef Pt_Gfx_ARgbColor_h
#define Pt_Gfx_ARgbColor_h

#include <Pt/String.h>
#include <Pt/Unicode.h>
#include <Pt/Gfx/Color.h>
#include <Pt/AnyTraits.h>
#include <Pt/SourceInfo.h>
#include <limits>


namespace Pt {

    namespace Gfx {

        /** @brief An empty structure used for tagging 64-bit ARGB color class.
         */
        struct ARgb {};


#pragma pack(push, 1)
        /** @brief 64-Bit ARGB color class.
         *  @ingroup Gfx
         *
         *  This is the master color model for Pt::Gfx.
         *  \n\n
         *  Valid range of the color components for this color model:
         *  <TABLE>
         *    <TR> <TD>Alpha</TD> <TD>0</TD> <TD>to</TD> <TD>65535 (0xFFFF)</TD> </TR>
         *    <TR> <TD>Red  </TD> <TD>0</TD> <TD>to</TD> <TD>65535 (0xFFFF)</TD> </TR>
         *    <TR> <TD>Green</TD> <TD>0</TD> <TD>to</TD> <TD>65535 (0xFFFF)</TD> </TR>
         *    <TR> <TD>Blue </TD> <TD>0</TD> <TD>to</TD> <TD>65535 (0xFFFF)</TD> </TR>
         *  </TABLE>
         */
        template <>
        class PT_GFX_API Color<ARgb> {
            public:
                /** @brief The default constructor, will generate the default color (black).
                 */
                inline Color()
                : _a(0xFFFF), _r(0), _g(0), _b(0)
                {}

                /** @brief Copy constructor.
                 */
                inline Color(const Color& c)
                : _a(c._a), _r(c._r), _g(c._g), _b(c._b)
                {}

                /** @brief Construct color using the given components.
                 */
                inline Color(uint16_t a, uint16_t r, uint16_t g, uint16_t b)
                : _a(a), _r(r), _g(g), _b(b)
                {}

                /** @brief Construct color using the given components.
                 */
                inline Color(uint16_t r, uint16_t g, uint16_t b)
                : _a(0xFFFF), _r(r), _g(g), _b(b)
                {}

                /** @brief Assignment operator.
                 */
                inline Color& operator=(const Color& c)
                { _a = c._a; _r = c._r; _g = c._g; _b = c._b; return *this; }

                /** @brief Assignment operator.
                 *
                 *  This assigns color with different type to this one by calling
                 *  assign(), which can be overloaded to allow new color types to
                 *  be assigned to this one.
                 */
                template <typename ColorT>
                inline Color& operator=(const ColorT& color)
                { assign(*this, color); return *this; }


                /** @brief Assignment-addition operator (beware of overflow).
                 */
                inline Color& operator+=(const Color& c)
                { _a += c._a; _r += c._r; _g += c._g; _b += c._b; return *this; }

                /** @brief Assignment-substraction operator (beware of underflow).
                 */
                inline Color& operator-=(const Color& c)
                { _a -= c._a; _r -= c._r; _g -= c._g; _b -= c._b; return *this; }


                /** @brief Return the alpha component of this color (range 0 to 65535).
                 */
                inline uint16_t alpha() const
                { return _a; }

                /** @brief Return the red component of this color (range 0 to 65535).
                 */
                inline uint16_t red() const
                { return _r; }

                /** @brief Return the green component of this color (range 0 to 65535).
                 */
                inline uint16_t green() const
                { return _g; }

                /** @brief Return the blue component of this color (range 0 to 65535).
                 */
                inline uint16_t blue() const
                { return _b; }


                /** @brief Set the alpha component of this color (range 0 to 65535).
                 */
                inline void setAlpha(uint16_t a)
                { _a = a; }

                /** @brief Set the red component of this color (range 0 to 65535).
                 */
                inline void setRed(uint16_t r)
                { _r = r; }

                /** @brief Set the green component of this color (range 0 to 65535).
                 */
                inline void setGreen(uint16_t g)
                { _g = g; }

                /** @brief Set the blue component of this color (range 0 to 65535).
                 */
                inline void setBlue(uint16_t b)
                { _b = b; }

            protected:
                uint16_t _a, _r, _g, _b;
        };
#pragma pack(pop)


        /** @brief Convenience access to the 64-Bit ARGB color model.
         *  @ingroup Gfx
         */
        typedef Color<ARgb> ARgbColor;


        /** @brief Convert a Color<ARgb> to a Color<ARgb>.
         *
         *  This function is implemented just for the sake of completeness.
         */
        inline const Color<ARgb>& toARgb(Color<ARgb>& to, const Color<ARgb>& from)
        { to = from; return to; }

        /** @brief Convert a Color<ARgb> to a Color<ARgb>.
         *
         *  This function is implemented just for the sake of completeness.
         */
        inline void fromARgb(Color<ARgb>& to, const Color<ARgb>& from)
        { to = from; }


        /** @brief Equality operator for Color<ARgb> comparison.
         */
        inline bool operator==(const Color<ARgb>& c1, const Color<ARgb>& c2)
        { return c1.alpha()==c2.alpha() && c1.red()==c2.red() && c1.green()==c2.green() && c1.blue()==c2.blue(); }

        /** @brief Less-than operator for Color<ARgb> comparison.
         */
        inline bool operator<(const Color<ARgb>& c1, const Color<ARgb>& c2)
        { return c1.alpha()<c2.alpha() || c1.red()<c2.red() || c1.green()<c2.green() || c1.blue()<c2.blue(); }

        /** @brief Greater-than operator for Color<ARgb> comparison.
         */
        inline bool operator>(const Color<ARgb>& c1, const Color<ARgb>& c2)
        { return c1.alpha()>c2.alpha() || c1.red()>c2.red() || c1.green()>c2.green() || c1.blue()>c2.blue(); }


        /** @brief Make the greyscale version of the source Color<ARgb>.
         */
        inline Color<ARgb>& greyscale(Color<ARgb>& to, const Color<ARgb>& from)
        {
            const uint16_t s = (from.red()*77 + from.green()*128 + from.blue()*51) >> 8;

            to.setAlpha(from.alpha());
            to.setRed  (s);
            to.setGreen(s);
            to.setBlue (s);

            return to;
        }


        /** @brief Mix two Color<ARgb>s using the given mixing factor.
         */
        inline void blend(Color<ARgb>& dst, const Color<ARgb>& src, uint16_t factor)
        {
            typedef uint32_t ValueT;

            const ValueT oF = factor;
            const ValueT rF = std::numeric_limits<uint16_t>::max() - factor;

            const ValueT dA = ValueT( dst.alpha() ) * rF;
            const ValueT dR = ValueT( dst.red()   ) * rF;
            const ValueT dG = ValueT( dst.green() ) * rF;
            const ValueT dB = ValueT( dst.blue()  ) * rF;

            const ValueT sA = ValueT( src.alpha() ) * oF;
            const ValueT sR = ValueT( src.red()   ) * oF;
            const ValueT sG = ValueT( src.green() ) * oF;
            const ValueT sB = ValueT( src.blue()  ) * oF;

            dst.setAlpha( (dA + sA) >> (8*sizeof(factor)) );
            dst.setRed  ( (dR + sR) >> (8*sizeof(factor)) );
            dst.setGreen( (dG + sG) >> (8*sizeof(factor)) );
            dst.setBlue ( (dB + sB) >> (8*sizeof(factor)) );
        }

        /*
           Note: mixing equ for true ARgb alpha blending without mask:

           A: upper layer (the color to be put)
           B: lower layer (the color already in the image)
           D: the result  (mixing result)

           aD = 1 - (1-aA) * (1-aB)
           [rD, gD, bD] = [rA, gA, bA] * aA/aD        +
                          [rB, gB, bB] * aB*(1-aA)/aD
        */

    } // namespace Gfx




template<>
struct AnyTraits<Gfx::Color<Gfx::ARgb> > {
	static void output(std::ostream& os, const Gfx::Color<Gfx::ARgb>& value);
	static void input(std::istream& is, Gfx::Color<Gfx::ARgb>& value);
	static void output(std::basic_ostream<Pt::Char>& os, const Gfx::Color<Gfx::ARgb>& value);
	static void input(std::basic_istream<Pt::Char>& is, Gfx::Color<Gfx::ARgb>& value);
};


inline void Pt::AnyTraits<Gfx::Color<Gfx::ARgb> >::output(std::ostream& os, const Gfx::Color<Gfx::ARgb>& value)
{
    os << "ARGB(" << value.alpha() << ' ' << value.red() << ' ' << value.green() << ' ' << value.blue() << ')';
}


inline void Pt::AnyTraits<Gfx::Color<Gfx::ARgb> >::output(std::basic_ostream<Pt::Char>& os, const Gfx::Color<Gfx::ARgb>& value)
{
    os << L"ARGB(" << value.alpha() << ' ' << value.red() << ' ' << value.green() << ' ' << value.blue() << ')';
}


template <typename CharT>
inline void inputGeneric(std::basic_istream<CharT>& is, Gfx::Color<Gfx::ARgb>& value)
{
    uint16_t alpha;
    uint16_t red;
    uint16_t green;
    uint16_t blue;

    is >> alpha;
    is >> red;
    is >> green;
    is >> blue;

    CharT endChar;
    is >> endChar;

    if (endChar != ')')
    {
		throw std::runtime_error("Could not read ARgbColor value" + PT_SOURCEINFO);
    }

    value.setAlpha(alpha);
    value.setRed(red);
    value.setGreen(green);
    value.setBlue(blue);
}


inline void Pt::AnyTraits<Gfx::Color<Gfx::ARgb> >::input(std::istream& is, Gfx::Color<Gfx::ARgb>& value)
{
    while ( isspace( is.peek() ) )
    {
        is.get();
    }

    char ch[6];
	is.get(ch, 6);

	if (ch != "ARGB(")
	{
		throw std::runtime_error("Could not read ARgbColor value" + PT_SOURCEINFO);
	}

    inputGeneric(is, value);
}


inline void Pt::AnyTraits<Gfx::Color<Gfx::ARgb> >::input(std::basic_istream<Pt::Char>& is, Gfx::Color<Gfx::ARgb>& value)
{
    while (Unicode::isSpace(is.peek()))
    {
        is.get();
    }

    Pt::Char ch[6];
	is.get(ch, 6);

    if (!(Pt::String(ch) == L"ARGB("))
	{
		throw std::runtime_error("Could not read ARgbColor value" + PT_SOURCEINFO);
	}

    inputGeneric(is, value);
}


} // namespace Pt

#endif

