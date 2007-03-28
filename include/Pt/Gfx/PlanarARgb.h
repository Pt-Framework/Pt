/***************************************************************************
 *   Copyright (C) 2006-2007 by Aloysius Indrayanto                        *
 *   Copyright (C) 2006-2007 by Marc Boris Duerner                          *
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
#ifndef Pt_Gfx_PlanarARgb_h
#define Pt_Gfx_PlanarARgb_h

#include <Pt/Gfx/ARgbColor.h>
#include <Pt/Gfx/PlanarImageView.h>


namespace Pt {

    namespace Gfx {

        class ARgbConstColorRef : public PlanarConstColorRef<uint16_t, 4>
        {
            public:
                ARgbConstColorRef(const ARgbConstColorRef& c)
                : PlanarConstColorRef<uint16_t, 4>(c)
                { }

                ARgbConstColorRef(ConstColorData& data)
                : PlanarConstColorRef<uint16_t, 4>(data)
                { }

                Component alpha() const
                { return *_data[0]; }

                Component red() const
                { return *_data[1]; }

                Component green() const
                { return *_data[2]; }

                Component blue() const
                { return *_data[3]; }
        };


        class ARgbColorRef : public PlanarColorRef<uint16_t, 4>
        {
            public:
                ARgbColorRef(const ARgbColorRef& c)
                : PlanarColorRef<uint16_t, 4>(c)
                { }

                ARgbColorRef(ColorData& c)
                : PlanarColorRef<uint16_t, 4>(c)
                {  }

                ARgbColorRef& operator=(const ConstColorRef& other)
                {
                    PlanarColorRef<uint16_t, 4>::operator=(other);
                    return *this;
                }

                Component alpha() const
                { return *_data[0]; }

                Component red() const
                { return *_data[1]; }

                Component green() const
                { return *_data[2]; }

                Component blue() const
                { return *_data[3]; }

                void setAlpha(Component a)
                { *_data[0] = a; }

                void setRed(Component r)
                { *_data[1] = r; }

                void setGreen(Component g)
                { *_data[2] = g; }

                void setBlue(Component b)
                { *_data[3] = b; }
        };


        class ARgbColorPtr : public PlanarColorPtr<uint16_t, 4>
        {
            public:
                ARgbColorPtr()
                : PlanarColorPtr<uint16_t, 4>()
                { }

                ARgbColorPtr(ColorData& data)
                : PlanarColorPtr<uint16_t, 4>(data)
                { }

                ARgbColorRef operator*()
                { return ARgbColorRef(_data); }
        };


        class ARgbConstColorPtr : public PlanarConstColorPtr<uint16_t, 4>
        {
            public:
                ARgbConstColorPtr()
                : PlanarConstColorPtr<uint16_t, 4>()
                { }

                ARgbConstColorPtr(const ColorPtr& data)
                : PlanarConstColorPtr<uint16_t, 4>(data)
                { }

                ARgbConstColorPtr(ConstColorData& data)
                : PlanarConstColorPtr<uint16_t, 4>(data)
                { }

                ARgbConstColorRef operator*()
                { return ARgbConstColorRef(_data); }
        };


        struct PlanarARgb
        {
            static const size_t NumberOfChannels = 4;

            typedef uint16_t Component;

            typedef ARgbColor Color;

            typedef const ARgbColor ConstColor;

            typedef ARgbColorRef ColorRef;

            typedef ARgbConstColorRef ConstColorRef;

            typedef ARgbColorPtr ColorPtr;

            typedef ARgbConstColorPtr ConstColorPtr;
        };

    }

}

#endif
