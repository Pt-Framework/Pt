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
#ifndef Pt_Gfx_Yv12Image_h
#define Pt_Gfx_Yv12Image_h

#include <Pt/Gfx/PlanarImage.h>


namespace Pt {

    namespace Gfx {

        struct Yuv {};

        template <>
        class Color<Yuv>
        {
            public:
                static const size_t NumberOfChannels = 3;

                typedef uint8_t ComponentT;

            public:
                inline Color()
                : _y(0), _u(0), _v(0)
                {}

                inline Color(const Color& c)
                : _y(c._y), _u(c._u), _v(c._v)
                {}

                inline Color(ComponentT y, ComponentT u, ComponentT v)
                : _y(y), _u(u), _v(v)
                {}

                inline ComponentT y() const
                { return _y; }

                inline ComponentT u() const
                { return _u; }

                inline ComponentT v() const
                { return _v; }

                inline void setY(ComponentT y)
                { _y = y; }

                inline void setU(ComponentT u)
                { _u = u; }

                inline void setV(ComponentT v)
                { _v = v; }

            protected:
                ComponentT _y, _u, _v;
        };

        typedef Color<Yuv> YuvColor;


        class YuvConstColorRef : public PlanarConstColorRef<uint8_t, 3>
        {
            public:
                YuvConstColorRef(const YuvConstColorRef& c)
                : PlanarConstColorRef<uint8_t, 3>(c)
                { }

                YuvConstColorRef(ConstColorData& data)
                : PlanarConstColorRef<uint8_t, 3>(data)
                { }

                Component y() const
                { return *_data[0]; }

                Component u() const
                { return *_data[1]; }

                Component v() const
                { return *_data[2]; }
        };


        typedef PlanarColorRef<uint8_t, 3> YuvColorRef;


        class YuvConstColorPtr : public PlanarConstColorPtr<uint8_t, 3>
        {
            public:
                YuvConstColorPtr()
                : PlanarConstColorPtr<uint8_t, 3>()
                { }

                YuvConstColorPtr(const ColorPtr& data)
                : PlanarConstColorPtr<uint8_t, 3>(data)
                { }

                YuvConstColorPtr(ConstColorData& data)
                : PlanarConstColorPtr<uint8_t, 3>(data)
                { }

                YuvConstColorRef operator*()
                { return YuvConstColorRef(_data); }
        };


        inline void assign(YuvColorRef to, const YuvColorRef& from)
        { to = from; }

        inline void assign(YuvColorRef to, const YuvConstColorRef& from)
        { to = from; }


        /** @brief Color model for planar YUV images
        */
        struct PlanarYuv
        {
            static const size_t NumberOfChannels = 3;

            typedef uint8_t Component;

            typedef YuvColor Color;

            typedef const YuvColor ConstColor;

            typedef YuvColorRef ColorRef;

            typedef YuvConstColorRef ConstColorRef;

            typedef PlanarColorPtr<Component, 3> ColorPtr;

            typedef YuvConstColorPtr ConstColorPtr;

            typedef Component* ColorData [ NumberOfChannels ];

            typedef const Component* ConstColorData [ NumberOfChannels ];
        };

        typedef PlanarImageView< PlanarYuv, 2, 2> Yv12View;


        typedef PlanarImage< Yv12View > Yv12Image;

    }

}

#endif
