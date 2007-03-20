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

#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Unit/TestSuite.h"


class PlanarImageTest : public Pt::Unit::TestSuite
{
    public:
        PlanarImageTest()
        : TestSuite( "PlanarImageTest" )
        {
            this->registerMethod("ARgbModel", *this, &PlanarImageTest::ARgbModel);
            this->registerMethod("Yv12Model", *this, &PlanarImageTest::Yv12Model);
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

        void Yv12Model()
        {
            // 4x2 yuv12 data
            Pt::uint8_t data[] = { 0, 1, 2, 3,       // y
                                   4, 5, 6, 7,
                                   8, 9, 10, 11,
                                   12, 13, 14, 15,
                                   16, 17, 18, 19,   // u
                                   20, 21, 22, 23 }; // v

            Pt::Gfx::Yv12Model model;
            model.init(data, 4, 4);

            Pt::Gfx::Yv12ColorPtr ptr(model, 0, 0);

            std::cerr << std::endl;

            for(int y = 0; y < 4; ++y)
                for(int x = 0; x < 4; ++x)
                {
                    Pt::Gfx::Yv12ColorRef ref = *ptr;
                    std::cerr << "[" << x << ", " << y << "]: "
                              << (int)ref.y() << ":" << (int)ref.u() << ":" << (int)ref.v()
                              << std::endl;
                    ++ptr;
                }
        }

    private:

};

Pt::Unit::RegisterTest<PlanarImageTest> register_planarImageTest;
