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
#include "Pt/Gfx/ImagePainter.h"
#include "Pt/Gfx/ARgbImage.h"
#include "Pt/Gfx/Pen.h"
#include "Pt/Gfx/Font.h"
#include "Pt/Gfx/FontMetrics.h"
#include "Pt/Math/Point.h"

#include "Pt/Types.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestCase.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Unit/TestSuite.h"


class ImagePainterTest : public Pt::Unit::TestSuite
{
    public:
        ImagePainterTest()
        : TestSuite("ImagePainterTest")
        , _imagePainter( _image )
        , _bkColor( 0xffff, 0xffff, 0xffff )
        {
            this->registerMethod("drawLineTest", *this, &ImagePainterTest::drawLineTest);
            this->registerMethod("drawTextTest", *this, &ImagePainterTest::drawLineTest);
        }

        void drawLineTest()
        {
            _imagePainter.setPen( Pt::Gfx::Pen( 10, Pt::Gfx::ARgbColor( 0, 0, 0 ) ) );

            for( Pt::ssize_t size = 500; size >= 0; --size )
            {
                _image.resize(  size, size, _bkColor );

                _imagePainter.drawLine( Pt::Math::Point( 10,10 ), Pt::Math::Point (100,100 ) );
                _imagePainter.drawLine( Pt::Math::Point( 100,10 ), Pt::Math::Point ( 10,100 ) );
                _imagePainter.drawLine( Pt::Math::Point( 55,10 ), Pt::Math::Point ( 55,100 ) );
                _imagePainter.drawLine( Pt::Math::Point( 10,55 ), Pt::Math::Point ( 100,55 ) );
            }
        }

        void drawTextTest()
        {
            Pt::Gfx::ARgbColor  outlineColor( 0, 0xffff, 0);
            Pt::Text::String    text( L"Hallo Platinum!" );

            //Font metrics
            _imagePainter.setFont( Pt::Gfx::Font("Vera", 12, Pt::Gfx::Font::NormalStyle, 340 ) );

            Pt::Gfx::FontMetrics metrics  = _imagePainter.fontMetrics( text );

            PT_UNIT_ASSERT( metrics.ascent() == 12 );
            PT_UNIT_ASSERT( metrics.descent() == 3 );
            PT_UNIT_ASSERT( metrics.width() > 75 );
            PT_UNIT_ASSERT( metrics.height() > 8 );

            _imagePainter.setFont( Pt::Gfx::Font( "", 12, Pt::Gfx::Font::NormalStyle, 0) );

            //No clipping
            _image.resize( 800, 600, _bkColor );
            _imagePainter.drawText( Pt::Math::Point( 40, 40 ), text, &outlineColor );
            PT_UNIT_ASSERT( checkImage() );

            //Left clipping
            _image.resize( 800, 600, _bkColor );
            _imagePainter.drawText( Pt::Math::Point( -4, 40 ), text, &outlineColor );
            PT_UNIT_ASSERT( checkImage() );

            //Top cliping
            _image.resize( 800, 600, _bkColor );
            _imagePainter.drawText( Pt::Math::Point( 40, 1 ), text, &outlineColor );
            PT_UNIT_ASSERT( checkImage() );

            //Right clipping
            _image.resize( 800, 600, _bkColor );
            _imagePainter.drawText( Pt::Math::Point( 782, 40 ), text, &outlineColor );
            PT_UNIT_ASSERT( checkImage() ); 

            //Bottom clipping
            _image.resize( 800, 600, _bkColor );
            _imagePainter.drawText( Pt::Math::Point( 40, 601 ), text, &outlineColor );
            PT_UNIT_ASSERT( checkImage() );

            //Outside
            _image.resize( 800, 600, _bkColor );
            _imagePainter.drawText( Pt::Math::Point( 10, -40 ), text, &outlineColor );
            PT_UNIT_ASSERT( !checkImage() );
        }

    private:

    bool checkImage()
    {
        for( size_t i = 0; i < _image.width(); ++i )
            for( size_t j = 0; j < _image.height(); ++j )
                if( _image.pixel( i, j ) != _bkColor )
                    return true;
        
        return false;
    }

    Pt::Gfx::ARgbImage     _image;
    Pt::Gfx::ImagePainter  _imagePainter;
    Pt::Gfx::ARgbColor     _bkColor;
};

Pt::Unit::RegisterTest<ImagePainterTest> register_imagePainterTest;
