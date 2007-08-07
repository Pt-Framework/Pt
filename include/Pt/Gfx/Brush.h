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

    PT_GFX_API void operator >>=( const SerializationInfo& si, Gfx::Brush& x );

    PT_GFX_API void operator <<=( SerializationInfo& si, const Gfx::Brush& x );

} // namespace Gfx

} // namespace Pt

#endif
