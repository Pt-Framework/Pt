/***************************************************************************
 *   Copyright (C) 2006-2007 Aloysius Indrayanto                           *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
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

#include "Pt/Gfx/ARgbColor.h"
#include "Pt/Gfx/PlanarImage.h"
#include "Pt/Gfx/PlanarImage.tpp"

#include "Pt/Gfx/Yv12Image.h"
#include "Pt/Gfx/ImageAlgo.h"
#include "Pt/Gfx/ColorAlgo.h"

#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Unit/TestSuite.h"


namespace Pt {

namespace Gfx {


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

        Component a() const
        { return *_data[0]; }

        Component r() const
        { return *_data[1]; }

        Component g() const
        { return *_data[2]; }

        Component b() const
        { return *_data[3]; }

        void setA(Component a)
        { *_data[0] = a; }

        void setR(Component r)
        { *_data[1] = r; }

        void setG(Component g)
        { *_data[2] = g; }

        void setB(Component b)
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


struct PlanarARgb
{
    static const size_t NumberOfChannels = 4;

    typedef uint16_t Component;

    typedef ARgbColor Color;

    typedef const ARgbColor ConstColor;

    typedef ARgbColorRef ColorRef;

    typedef PlanarConstColorRef<Component, 4> ConstColorRef;

    typedef ARgbColorPtr ColorPtr;

    typedef PlanarConstColorPtr<Component, 4> ConstColorPtr;
};


typedef PlanarImageView< PlanarARgb, 1, 1> ARgbView;


typedef PlanarImage< ARgbView > PlanarARgbImage;

}

}



// 4x2 yuv12 data
Pt::uint8_t yv12_data[] = { 0, 1, 2, 3,       // y
                            4, 5, 6, 7,
                            8, 9, 10, 11,
                            12, 13, 14, 15,
                            16, 17, 18, 19,   // u
                            20, 21, 22, 23 }; // v


class PlanarImageTest : public Pt::Unit::TestSuite
{
    public:
        PlanarImageTest()
        : TestSuite( "PlanarImageTest" )
        {
            this->registerMethod("ARgbPixelIterator", *this, &PlanarImageTest::ARgbPixelIterator);
            this->registerMethod("Yv12BlockScale", *this, &PlanarImageTest::Yv12BlockScale);
            this->registerMethod("Yv12PixelIterator", *this, &PlanarImageTest::Yv12PixelIterator);
            this->registerMethod("Yv12ConstPixelIterator", *this, &PlanarImageTest::Yv12ConstPixelIterator);
        }

        void ARgbPixelIterator()
        {
            // 2x2 ARGB planar data
            Pt::uint16_t argb_data[] = { 0, 1, 2, 3,
                                         4, 5, 6, 7,
                                         8, 9, 10, 11,
                                         12, 13, 14, 15 };

            PT_UNIT_ASSERT( Pt::Gfx::PlanarARgb::NumberOfChannels == 4 )
            Pt::Gfx::ARgbView view;
            view.init( (unsigned char*)argb_data, 2, 2);
            PT_UNIT_ASSERT( view.size(2,2) == 32 )

            Pt::Gfx::ARgbView::PixelIterator it(view, 0, 0);
            Pt::Gfx::ARgbView::PixelIterator end(view, 3, 1);

            Pt::Math::Size size = end - it;
            PT_UNIT_ASSERT( size.width() == 2 );
            PT_UNIT_ASSERT( size.height() == 2 );

            Pt::Gfx::ARgbView::ColorRef color = *it;
            PT_UNIT_ASSERT( color.a() == 0);
            PT_UNIT_ASSERT( color.r() == 4);
            PT_UNIT_ASSERT( color.g() == 8);
            PT_UNIT_ASSERT( color.b() == 12);

            ++it;
            color = *it;
            PT_UNIT_ASSERT( color.a() == 1);
            PT_UNIT_ASSERT( color.r() == 5);
            PT_UNIT_ASSERT( color.g() == 9);
            PT_UNIT_ASSERT( color.b() == 13);

            it += 2;
            color = *it;
            PT_UNIT_ASSERT( color.a() == 3);
            PT_UNIT_ASSERT( color.r() == 7);
            PT_UNIT_ASSERT( color.g() == 11);
            PT_UNIT_ASSERT( color.b() == 15);
        }

        void Yv12BlockScale()
        {
            // 2x2 yv12 image data
            Pt::uint8_t yv12_out[] = { 0, 0,  // y
                                       0, 0,
                                       0,     // u
                                       0,  }; // v

            // Block scale to a 2x2 image ...
            Pt::Gfx::Yv12View outmodel;
            outmodel.init(yv12_out, 2, 2);
            Pt::Gfx::Yv12View::PixelIterator to(outmodel, 0, 0);
            Pt::Gfx::Yv12View::PixelIterator toEnd(outmodel, 3, 1);

            // ... from a 4x4 image
            Pt::Gfx::Yv12View model;
            model.init(yv12_data, 4, 4);
            Pt::Gfx::Yv12View::PixelIterator it(model, 0, 0);
            Pt::Gfx::Yv12View::PixelIterator end(model, 4, 3);

            Pt::Gfx::blockScale(it, end, to, toEnd);

            PT_UNIT_ASSERT( yv12_out[0] == 0 ); // y
            PT_UNIT_ASSERT( yv12_out[1] == 2 );
            PT_UNIT_ASSERT( yv12_out[2] == 8 );
            PT_UNIT_ASSERT( yv12_out[3] == 10 );
            PT_UNIT_ASSERT( yv12_out[4] == 19 ); // u
            PT_UNIT_ASSERT( yv12_out[5] == 23 ); // v

            //std::cerr << std::endl;
            //for(int x = 0; x < 6; ++x)
            //     std::cerr << (int)yv12_out[x] << std::endl;
            Pt::Gfx::Yv12View::ConstPixelIterator cit(model, 0, 0);
            Pt::Gfx::Yv12View::ConstPixelIterator cend(model, 4, 3);
            Pt::Gfx::blockScale(cit, cend, to, toEnd);
        }

        void Yv12PixelIterator()
        {
            Pt::Gfx::Yv12View model;
            model.init(yv12_data, 4, 4);

            Pt::Gfx::Yv12View::PixelIterator it(model, 0, 0);
            Pt::Gfx::Yv12View::PixelIterator end(model, 4, 3);

            Pt::Math::Size size = end - it;
            PT_UNIT_ASSERT( size.width() == 4);
            PT_UNIT_ASSERT( size.height() == 4);

            Pt::Gfx::Yv12View::ColorRef color = *it;
            PT_UNIT_ASSERT( color.y() == 0);
            PT_UNIT_ASSERT( color.u() == 16);
            PT_UNIT_ASSERT( color.v() == 20);

            it += 10;
            PT_UNIT_ASSERT( (*it).y() == 10);
            PT_UNIT_ASSERT( (*it).u() == 19);
            PT_UNIT_ASSERT( (*it).v() == 23);

            //std::cerr << std::endl;
            //for(int y = 0; y < 4; ++y)
            //    for(int x = 0; x < 4; ++x)
            //    {
            //        Pt::Gfx::Yv12Color color = *it;
            //        std::cerr << "[" << x << ", " << y << "]: "
            //                  << (int)color.y() << ":" << (int)color.u() << ":" << (int)color.v()
            //                  << std::endl;
            //        ++it;
            //    }
        }

        void Yv12ConstPixelIterator()
        {
            Pt::Gfx::Yv12View model;
            model.init(yv12_data, 4, 4);

            //Pt::Gfx::PlanarPixelIterator2x2<Pt::Gfx::Yv12Model, Pt::Gfx::Yv12Color, uint8_t> pit(model, 0, 0);
            //Pt::Gfx::PlanarPixelIterator2x2<Pt::Gfx::Yv12Model, Pt::Gfx::Yv12Color, uint8_t> end(model, 4, 3);
            //std::cerr  << std::endl;
            //while(pit != end)
            //{
            //    Pt::Gfx::Yv12Color col = *pit;
            //    std::cerr   << (int)col.y() << ":" << (int)col.u() << ":" << (int)col.v()
            //                << std::endl;
            //
            //    ++pit;
            //}

            Pt::Gfx::Yv12View::ConstPixelIterator it(model, 0, 0);
            Pt::Gfx::Yv12View::ConstPixelIterator end(model, 4, 3);

            Pt::Math::Size size = end - it;
            PT_UNIT_ASSERT( size.width() == 4);
            PT_UNIT_ASSERT( size.height() == 4);

            Pt::Gfx::Yv12View::ConstColorRef color = *it;
            PT_UNIT_ASSERT( color.y() == 0);
            PT_UNIT_ASSERT( color.u() == 16);
            PT_UNIT_ASSERT( color.v() == 20);

            ++it; ++it; ++it; ++it;
            color = *it;

            PT_UNIT_ASSERT( color.y() == 4);
            PT_UNIT_ASSERT( color.u() == 16);
            PT_UNIT_ASSERT( color.v() == 20);

            ++it; ++it; ++it; ++it;
            color = *it;

            PT_UNIT_ASSERT( color.y() == 8);
            PT_UNIT_ASSERT( color.u() == 18);
            PT_UNIT_ASSERT( color.v() == 22);
        }
};

Pt::Unit::RegisterTest<PlanarImageTest> register_planarImageTest;
