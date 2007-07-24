/***************************************************************************
 *   Copyright (C) 2007 by Marc Boris Dürner                               *
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
#ifndef PT_GFX_PRUSH_H
#define PT_GFX_PRUSH_H

#include <Pt/SmartPtr.h>
#include <Pt/Gfx/ARgbImage.h>


namespace Pt {

namespace Gfx {

    class BrushData;

    class PT_GFX_API Brush
    {
        public:
            enum FillStyle {
                SolidFill = 0, TextureFill
            };

        public:
            Brush( const ARgbColor& color = ARgbColor(0,0,0) );

            Brush(const ARgbImage* texture);

            FillStyle fillStyle() const;

            const ARgbColor& color() const;

            const ARgbImage& texture() const;

            friend PT_GFX_API bool operator==(const Brush& a, const Brush& b);

            friend PT_GFX_API bool operator<(const Brush& a, const Brush& b);


        private:
            SmartPtr<BrushData> _brushData;
    };


    PT_GFX_API SerializationNode& insert(SerializationData& data, const Brush& pen);


    PT_GFX_API const SerializationNode& operator>>(const SerializationNode& node, Brush& x);


    class PT_GFX_API BrushData
    {
        public:
            BrushData(Brush::FillStyle fillStyle, const ARgbColor& color, const ARgbImage* texture);

            ~BrushData();

            Brush::FillStyle fillStyle() const;

            const ARgbColor& color() const;

            const ARgbImage& texture() const;

        private:
            Brush::FillStyle _fillStyle;
            ARgbColor        _color;
            ARgbImage*       _texture;
    };

} // namespace Gfx


template <>
struct AnyTraits<Gfx::Brush> {
	static void output(std::ostream& os, const Gfx::Brush& value);
	static void input(std::istream& is, Gfx::Brush& value);
	static void output(std::basic_ostream<Pt::Char>& os, const Gfx::Brush& value);
	static void input(std::basic_istream<Pt::Char>& is, Gfx::Brush& value);
};


template <typename CharT>
inline void outputGeneric(std::basic_ostream<CharT>& os, const Gfx::Brush& value)
{
    if(value.fillStyle() == Gfx::Brush::SolidFill)
    {
        os << '(';
        AnyTraits<Gfx::ARgbColor >::output(os, value.color());
        os << ')';
    }
    else
    {
//TODO write url for texture image path
        throw std::runtime_error("Textured brushes are not supported so far.");
    }
}


inline void Pt::AnyTraits<Gfx::Brush>::output(std::ostream& os, const Gfx::Brush& value)
{
	outputGeneric(os, value);
}


inline void Pt::AnyTraits<Gfx::Brush>::output(std::basic_ostream<Pt::Char>& os, const Gfx::Brush& value)
{
	outputGeneric(os, value);
}


template <typename CharT>
inline void inputGeneric(std::basic_istream<CharT>& is, Gfx::Brush& value)
{
    CharT ch;

    is >> ch;
    if (ch != '(')
    {
	    throw std::runtime_error("Could not read Brush value" + PT_SOURCEINFO);
    }

    Gfx::ARgbColor  brushColor;

//ToDo -> read texture as property
// extend the url class with AnyTraits for image pathes -> load an ARgbImage
//    Gfx::ARgbImage* brushTexture;

    AnyTraits<Gfx::ARgbColor >::input(is, brushColor);

    is >> ch;
    if (ch != ')')
    {
	    throw std::runtime_error("Could not read Brush value" + PT_SOURCEINFO);
    }

    // solid brush
    value = Gfx::Brush(brushColor);

//ToDo -> read texture as property
    // texture brush
//    value = Gfx::Brush(brushTexture);
}


inline void Pt::AnyTraits<Gfx::Brush>::input(std::istream& is, Gfx::Brush& value)
{
	inputGeneric(is, value);
}


inline void Pt::AnyTraits<Gfx::Brush>::input(std::basic_istream<Pt::Char>& is, Gfx::Brush& value)
{
	inputGeneric(is, value);
}

} // namespace Pt

#endif
