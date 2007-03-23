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

#include "Pt/Gfx/PlanarImage.h"
#include "Pt/Gfx/PlanarImage.tpp"
#include "Pt/Gfx/PlanarImageModel1x1.h"
#include "Pt/Gfx/Yv12Image.h"
#include "Pt/Gfx/ImageAlgo.h"
#include "Pt/Gfx/ColorAlgo.h"

#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Unit/TestSuite.h"

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
            this->registerMethod("ARgbModel", *this, &PlanarImageTest::ARgbModel);
            this->registerMethod("Yv12BlockScale", *this, &PlanarImageTest::Yv12BlockScale);
            this->registerMethod("Yv12PixelIterator", *this, &PlanarImageTest::Yv12PixelIterator);
            this->registerMethod("Yv12ConstPixelIterator", *this, &PlanarImageTest::Yv12ConstPixelIterator);
        }

        void ARgbModel()
        {
            // image data, could be 4 planes with two elements each
            uint16_t data[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };

            typedef Pt::Gfx::PlanarImageModel< Pt::Gfx::ARgbColorProxy, 1, 1 > Model;
            PT_UNIT_ASSERT( Model::NumberOfChannels == 4 )
            Model model;

            // Set a pointer to the first pixel. assume width = 2, height = 1
            // We expect  A=0, R= 2, G=4, B=6, each component at begin of plane
            Model::ChannelData chnStart = { data, data + 2, data + 4, data + 6 };
            Model::ColorPtrT ptr(chnStart, 2, 1, 0, 0);
            PT_UNIT_ASSERT( (*ptr).alpha() == 0 );
            PT_UNIT_ASSERT( (*ptr).red() == 2 );
            PT_UNIT_ASSERT( (*ptr).green() == 4 );
            PT_UNIT_ASSERT( (*ptr).blue() == 6 );

            // increment pointer we expect:
            //  A=1, R= 3, G=5, B=7, first element in each plane
            ++ptr;
            PT_UNIT_ASSERT( (*ptr).alpha() == 1 );
            PT_UNIT_ASSERT( (*ptr).red() == 3 );
            PT_UNIT_ASSERT( (*ptr).green() == 5 );
            PT_UNIT_ASSERT( (*ptr).blue() == 7 );
        }

        void Yv12BlockScale()
        {
            // 2x2 yv12 image data
            Pt::uint8_t yv12_out[] = { 0, 0,  // y
                                       0, 0,
                                       0,     // u
                                       0,  }; // v

            // Block scale to a 2x2 image ...
            Pt::Gfx::Yv12Model outmodel;
            outmodel.init(yv12_out, 2, 2);
            Pt::Gfx::Yv12Model::PixelIterator to(outmodel, 0, 0);
            Pt::Gfx::Yv12Model::PixelIterator toEnd(outmodel, 3, 1);

            // ... from a 4x4 image
            Pt::Gfx::Yv12Model model;
            model.init(yv12_data, 4, 4);
            Pt::Gfx::Yv12Model::PixelIterator it(model, 0, 0);
            Pt::Gfx::Yv12Model::PixelIterator end(model, 4, 3);

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
        }

        void Yv12PixelIterator()
        {
            Pt::Gfx::Yv12Model model;
            model.init(yv12_data, 4, 4);

            Pt::Gfx::Yv12Model::PixelIterator it(model, 0, 0);
            Pt::Gfx::Yv12Model::PixelIterator end(model, 4, 3);

            Pt::Math::Size size = end - it;
            PT_UNIT_ASSERT( size.width() == 4);
            PT_UNIT_ASSERT( size.height() == 4);

            Pt::Gfx::Yv12Model::ColorRef color = *it;
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
            Pt::Gfx::Yv12Model model;
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

            Pt::Gfx::Yv12Model::ConstPixelIterator it(model, 0, 0);
            Pt::Gfx::Yv12Model::ConstPixelIterator end(model, 4, 3);

            Pt::Math::Size size = end - it;
            PT_UNIT_ASSERT( size.width() == 4);
            PT_UNIT_ASSERT( size.height() == 4);

            Pt::Gfx::Yv12Model::ConstColorRef color = *it;
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
