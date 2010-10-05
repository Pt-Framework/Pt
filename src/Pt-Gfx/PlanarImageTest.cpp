/*
 * Copyright (C) 2006-2007 Aloysius Indrayanto
 * Copyright (C) 2006-2007 Marc Boris Duerner
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

#include "Pt/Gfx/ARgbPlanarImage.h"
#include "Pt/Gfx/Yv12Image.h"
#include "Pt/Gfx/Algorithm.h"

#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Unit/TestSuite.h"


// 4x2 yuv12 data
Pt::uint8_t image_data[] = { 0, 1, 2, 3,       // y
                            4, 5, 6, 7,
                            8, 9, 10, 11,
                            12, 13, 14, 15,
                            16, 17, 18, 19,   // u
                            20, 21, 22, 23 }; // v

// 4x2 yuv12 data
Pt::uint16_t image_data_16[] = { 0, 1, 2, 3,
                            4, 5, 6, 7,
                            8, 9, 10, 11,
                            12, 13, 14, 15,
                            16, 17, 18, 19,
                            20, 21, 22, 23 };


class PlanarImageTest : public Pt::Unit::TestSuite
{
    public:
        PlanarImageTest()
        : TestSuite( "PlanarImageTest" )
        {
            this->registerMethod("ARgbConstPixelIterator", *this, &PlanarImageTest::ARgbConstPixelIterator);
            this->registerMethod("ARgbPixelIterator", *this, &PlanarImageTest::ARgbPixelIterator);
            this->registerMethod("PlanarARgbView", *this, &PlanarImageTest::PlanarARgbView);
            this->registerMethod("Yv12BlockScale", *this, &PlanarImageTest::Yv12BlockScale);
            this->registerMethod("Yv12ConstPixelIterator", *this, &PlanarImageTest::Yv12ConstPixelIterator);
            this->registerMethod("Yv12PixelIterator", *this, &PlanarImageTest::Yv12PixelIterator);
            this->registerMethod("Yv12View", *this, &PlanarImageTest::Yv12View);
        }

        void PlanarARgbView()
        {
            PT_UNIT_ASSERT( Pt::Gfx::PlanarARgb::NumberOfChannels == 4 )

            Pt::Gfx::PlanarARgbView view;
            PT_UNIT_ASSERT( view.size(2,2) == 32 )
        }


        void ARgbPixelIterator()
        {
            Pt::Gfx::PlanarARgbView view;
            view.init( (unsigned char*)image_data_16, 2, 2);

            Pt::Gfx::PlanarARgbView::PixelIterator it(view, 0, 0);
            Pt::Gfx::PlanarARgbView::PixelIterator end(view, 3, 1);

            Pt::Gfx::Size size = end - it;
            PT_UNIT_ASSERT( size.width() == 2 );
            PT_UNIT_ASSERT( size.height() == 2 );

            Pt::Gfx::PlanarARgbView::ColorRef color = *it;
            PT_UNIT_ASSERT( color.alpha() == 0);
            PT_UNIT_ASSERT( color.red() == 4);
            PT_UNIT_ASSERT( color.green() == 8);
            PT_UNIT_ASSERT( color.blue() == 12);

            ++it;
            color = *it;
            PT_UNIT_ASSERT( color.alpha() == 1);
            PT_UNIT_ASSERT( color.red() == 5);
            PT_UNIT_ASSERT( color.green() == 9);
            PT_UNIT_ASSERT( color.blue() == 13);

            it += 2;
            color = *it;
            PT_UNIT_ASSERT( color.alpha() == 3);
            PT_UNIT_ASSERT( color.red() == 7);
            PT_UNIT_ASSERT( color.green() == 11);
            PT_UNIT_ASSERT( color.blue() == 15);
        }

        void ARgbConstPixelIterator()
        {
            Pt::Gfx::PlanarARgbView view;
            view.init( (unsigned char*)image_data_16, 2, 2);

            Pt::Gfx::PlanarARgbView::ConstPixelIterator it(view, 0, 0);
            Pt::Gfx::PlanarARgbView::ConstPixelIterator end(view, 3, 1);

            Pt::Gfx::Size size = end - it;
            PT_UNIT_ASSERT( size.width() == 2 );
            PT_UNIT_ASSERT( size.height() == 2 );

            Pt::Gfx::PlanarARgbView::ConstColorRef color = *it;
            PT_UNIT_ASSERT( color.alpha() == 0);
            PT_UNIT_ASSERT( color.red() == 4);
            PT_UNIT_ASSERT( color.green() == 8);
            PT_UNIT_ASSERT( color.blue() == 12);

            ++it;
            color = *it;
            PT_UNIT_ASSERT( color.alpha() == 1);
            PT_UNIT_ASSERT( color.red() == 5);
            PT_UNIT_ASSERT( color.green() == 9);
            PT_UNIT_ASSERT( color.blue() == 13);

            it += 2;
            color = *it;
            PT_UNIT_ASSERT( color.alpha() == 3);
            PT_UNIT_ASSERT( color.red() == 7);
            PT_UNIT_ASSERT( color.green() == 11);
            PT_UNIT_ASSERT( color.blue() == 15);
        }

        void Yv12View()
        {
            PT_UNIT_ASSERT( Pt::Gfx::Yv12View::NumberOfChannels == 3 )

            Pt::Gfx::Yv12View view;
            PT_UNIT_ASSERT( view.size(4, 4) == 24 )
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
            model.init(image_data, 4, 4);
            Pt::Gfx::Yv12View::PixelIterator it(model, 0, 0);
            Pt::Gfx::Yv12View::PixelIterator end(model, 4, 3);

            Pt::Gfx::blockScale(it, end, to, toEnd);

            PT_UNIT_ASSERT( yv12_out[0] == 0 ); // y
            PT_UNIT_ASSERT( yv12_out[1] == 2 );
            PT_UNIT_ASSERT( yv12_out[2] == 8 );
            PT_UNIT_ASSERT( yv12_out[3] == 10 );
            PT_UNIT_ASSERT( yv12_out[4] == 19 ); // u
            PT_UNIT_ASSERT( yv12_out[5] == 23 ); // v

            Pt::Gfx::Yv12View::ConstPixelIterator cit(model, 0, 0);
            Pt::Gfx::Yv12View::ConstPixelIterator cend(model, 4, 3);
            Pt::Gfx::blockScale(cit, cend, to, toEnd);
        }

        void Yv12PixelIterator()
        {
            Pt::Gfx::Yv12View model;
            model.init(image_data, 4, 4);

            Pt::Gfx::Yv12View::PixelIterator it(model, 0, 0);
            Pt::Gfx::Yv12View::PixelIterator end(model, 4, 3);

            Pt::Gfx::Size size = end - it;
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
        }

        void Yv12ConstPixelIterator()
        {
            Pt::Gfx::Yv12View model;
            model.init(image_data, 4, 4);

            Pt::Gfx::Yv12View::ConstPixelIterator it(model, 0, 0);
            Pt::Gfx::Yv12View::ConstPixelIterator end(model, 4, 3);

            Pt::Gfx::Size size = end - it;
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
