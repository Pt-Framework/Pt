/***************************************************************************
 *   Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan                     *
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 PTV AG                                        *
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
#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestMain.h"

#include <Pt/Gfx/ImagePainter.h>
#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Gfx/Pen.h>

using namespace Pt;

class ImagePainterTest : public CPPUNIT_NS::TestFixture
{
public:

    ImagePainterTest()
    : _imagePainter( _image )
    {}

    CPPUNIT_TEST_SUITE( ImagePainterTest );

    CPPUNIT_TEST( test );
    CPPUNIT_TEST_SUITE_END();

protected:
    void test();

private:
    Pt::Gfx::ARgbImage     _image;
    Pt::Gfx::ImagePainter  _imagePainter;
};


CPPUNIT_TEST_SUITE_REGISTRATION( ImagePainterTest );


void ImagePainterTest::test()
{
    _imagePainter.setPen( Ptv::Gfx::Pen( 10, Ptv::Gfx::ARgbColor( 0xffff, 0, 0 ) ) );

    for( ptv::ssize_t size = 500; size >= 0; --size )
    {
        _image.resize(  size, size, Ptv::Gfx::ARgbColor( 0xffff, 0xffff, 0xffff ) );

        _imagePainter.drawLine( Pt::Math::Point( 10,10 ), Pt::Math::Point (100,100 ) );
        _imagePainter.drawLine( Pt::Math::Point( 100,10 ), Pt::Math::Point ( 10,100 ) );
        _imagePainter.drawLine( Pt::Math::Point( 55,10 ), Pt::Math::Point ( 55,100 ) );
        _imagePainter.drawLine( Pt::Math::Point( 10,55 ), Pt::Math::Point ( 100,55 ) );
    }
}
