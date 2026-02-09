/* Copyright (C) 2016 Marc Boris Duerner 
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/

#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/ImageView.h>
#include <Pt/Gfx/Argb32.h>
#include <Pt/Gfx/BlockScale.h>
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/RegisterTest.h>


class BlockScaleTest : public Pt::Unit::TestSuite
{
    public:
        BlockScaleTest()
        : Pt::Unit::TestSuite("BlockScaleTest")
        {
            Pt::Unit::TestSuite::registerMethod("ScaleUp", *this, &BlockScaleTest::ScaleUp);
            Pt::Unit::TestSuite::registerMethod("ScaleDown", *this, &BlockScaleTest::ScaleDown);
        }

    protected:
        void ScaleUp()
        {
            std::size_t fromWidth = 10;
            std::size_t fromHeight = 10;
            std::vector<Pt::uint32_t> from(fromWidth * fromHeight, 1);
            
            std::size_t toWidth = 20;
            std::size_t toHeight = 40;
            std::vector<Pt::uint32_t> to(toWidth * toHeight, 0);

            Pt::Gfx::blockScale(from.begin(), fromWidth, fromHeight, 
                                to.begin(), toWidth, toHeight);

            PT_UNIT_ASSERT(to.front() == from.front());
            PT_UNIT_ASSERT(to.back() == from.back());
        }

        void ScaleDown()
        {
            Pt::Gfx::Image from( Pt::Gfx::Argb32(), 100, 100 );
            Pt::Gfx::PixelView fromView(from);

            std::memset(from.data(), 123, from.stride() * from.height());

            Pt::Gfx::Image to( Pt::Gfx::Argb32(), 20, 40 );
            Pt::Gfx::PixelView toView(to);

            Pt::Gfx::blockScale(fromView.begin(), fromView.width(), fromView.height(), 
                                toView.begin(), toView.width(), toView.height());

            PT_UNIT_ASSERT(0 == std::memcmp(to.data(), from.data(), 
                                            to.stride() * to.height()));
        }
};

Pt::Unit::RegisterTest<BlockScaleTest> register_BlockScaleTest;
