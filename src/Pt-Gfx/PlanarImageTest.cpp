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
        }

        void ARgbModel()
        {
            typedef Pt::Gfx::PlanarImageModel< Pt::Gfx::ARgbColorProxy, 1, 1 > Model;
            Model model;
            PT_UNIT_ASSERT( Model::NumberOfChannels == 4 )
        }

    private:

};

Pt::Unit::RegisterTest<PlanarImageTest> register_planarImageTest;
