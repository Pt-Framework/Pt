/***************************************************************************
 *   Copyright (C) 2006 Aloysius Indrayanto                                *
 *   Copyright (C) 2006 Marc Boris Duerner                                  *
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
#undef PT_API_EXPORT

#include "Pt/SourceInfo.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestCase.h"
#include "Pt/Unit/RegisterTest.h"

#include <stdexcept>

class ExceptionTest : public Pt::Unit::TestCase
{
    public:
        ExceptionTest()
        : TestCase("ExceptionTest")
        {}

        void test()
        {
            try {
                throw std::invalid_argument( "test" + PT_SOURCEINFO);
            }
            catch(const std::exception& ex)
            {
                PT_UNIT_ASSERT(std::string( ex.what() ).find("test") != std::string::npos);
                return;
            }

            PT_UNIT_ASSERT(false);
        }
};

Pt::Unit::RegisterTest<ExceptionTest> register_ExceptionTest;
