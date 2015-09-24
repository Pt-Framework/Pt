/*
 * Copyright (C) 2007 by Marc Boris Duerner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
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
            enum FillStyle
           {
                SolidFill = 0,
                TextureFill,
                LinearGradientFill,
                RadialGradientFill
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
