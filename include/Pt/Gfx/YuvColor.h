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
#ifndef Pt_Gfx_YuvColor_h
#define Pt_Gfx_YuvColor_h

#include <Pt/Gfx/Color.h>


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


        class YuvColorRef : public PlanarColorRef<uint8_t, 3>
        {
            public:
                YuvColorRef(const YuvColorRef& c)
                : PlanarColorRef<uint8_t, 3>(c)
                { }

                YuvColorRef(ColorData& c)
                : PlanarColorRef<uint8_t, 3>(c)
                {  }

                YuvColorRef& operator=(const ConstColorRef& other)
                {
                    PlanarColorRef<uint8_t, 3>::operator=(other);
                    return *this;
                }

                Component y() const
                { return *_data[0]; }

                Component u() const
                { return *_data[1]; }

                Component v() const
                { return *_data[2]; }

                void setY(Component y)
                { *_data[0] = y; }

                void setU(Component u)
                { *_data[1] = u; }

                void setV(Component v)
                { *_data[2] = v; }
        };


        class YuvColorPtr : public PlanarColorPtr<uint8_t, 3>
        {
            public:
                YuvColorPtr()
                : PlanarColorPtr<uint8_t, 3>()
                { }

                YuvColorPtr(ColorData& data)
                : PlanarColorPtr<uint8_t, 3>(data)
                { }

                YuvColorRef operator*()
                { return YuvColorRef(_data); }
        };


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

    }

}

#endif
