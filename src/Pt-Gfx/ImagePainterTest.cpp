/*
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2007 Marc Boris Duerner
 * Copyright (C) 2006-2007 PTV AG
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

using namespace Pt;
using namespace Pt::Gfx;
using namespace Pt::Math;


Pt::Gfx::ARgbColor G(158*255, 158*255, 158*255);
Pt::Gfx::ARgbColor A(188*255, 188*255, 188*255);
Pt::Gfx::ARgbColor W(0xffff, 0xffff, 0xffff);

Pt::Gfx::ARgbColor texture_data[]  =   {W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,
                                        W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,
                                        W,W,G,G,G,G,G,G,G,G,G,G,G,G,G,G,G,G,G,W,W,
                                        W,W,G,G,G,G,G,G,G,G,G,G,G,G,G,G,G,G,G,W,W,
                                        W,W,G,G,G,G,G,G,G,G,G,G,G,G,G,G,G,G,G,W,W,
                                        W,W,G,G,G,G,G,G,G,W,W,W,G,G,G,G,G,G,G,W,W,
                                        W,W,G,G,G,G,G,G,G,W,W,W,G,G,G,G,G,G,G,W,W,
                                        W,W,G,G,G,G,G,G,G,W,W,W,G,G,G,G,G,G,G,W,W,
                                        W,W,G,G,G,G,G,G,G,W,W,W,G,G,G,G,G,G,G,W,W,
                                        W,W,G,G,G,W,W,W,W,W,W,W,W,W,W,W,G,G,G,W,W,
                                        W,W,G,G,G,W,W,W,W,W,W,W,W,W,W,W,G,G,G,W,W,
                                        W,W,G,G,G,W,W,W,W,W,W,W,W,W,W,W,G,G,G,W,W,
                                        W,W,G,G,G,G,G,G,G,W,W,W,G,G,G,G,G,G,G,W,W,
                                        W,W,G,G,G,G,G,G,G,W,W,W,G,G,G,G,G,G,G,W,W,
                                        W,W,G,G,G,G,G,G,G,W,W,W,G,G,G,G,G,G,G,W,W,
                                        W,W,G,G,G,G,G,G,G,W,W,W,G,G,G,G,G,G,G,W,W,
                                        W,W,G,G,G,G,G,G,G,G,G,G,G,G,G,G,G,G,G,W,W,
                                        W,W,G,G,G,G,G,G,G,G,G,G,G,G,G,G,G,G,G,W,W,
                                        W,W,G,G,G,G,G,G,G,G,G,G,G,G,G,G,G,G,G,W,W,
                                        W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,
                                        W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W};


class ImagePainterTest : public Pt::Unit::TestSuite
{
    public:
        ImagePainterTest()
        : TestSuite( "ImagePainterTest" )
        , _imagePainter( _image )
        , _texture( 21, 21, Pt::Gfx::ARgbColor(0,0,0xdddd) )
        , _bkColor( 0xffff, 0xffff, 0xffff )
        {
            std::copy( texture_data, texture_data + (21*21), _texture.data() );

            this->registerMethod("drawThinLineTest", *this, &ImagePainterTest::drawThinLineTest);
            this->registerMethod("drawThickLineTest", *this, &ImagePainterTest::drawThickLineTest);
            this->registerMethod("drawRectTest", *this, &ImagePainterTest::drawRectTest);
            this->registerMethod("drawCircleTest", *this, &ImagePainterTest::drawCircleTest);
            this->registerMethod("drawThinEllipseTest", *this, &ImagePainterTest::drawThinEllipseTest);
            this->registerMethod("drawThickEllipseTest", *this, &ImagePainterTest::drawThickEllipseTest);
            this->registerMethod("drawPolylineTest", *this, &ImagePainterTest::drawPolylineTest);
            this->registerMethod("drawImageTest", *this, &ImagePainterTest::drawImageTest);
            this->registerMethod("fontMetricTest", *this, &ImagePainterTest::fontMetricTest);
            this->registerMethod("drawTextTest", *this, &ImagePainterTest::drawTextTest);

            this->registerMethod("fillEllipseTest", *this, &ImagePainterTest::fillEllipseTest);
            this->registerMethod("fillCircleTest", *this, &ImagePainterTest::fillCircleTest);
            this->registerMethod("fillPolygonTest", *this, &ImagePainterTest::fillPolygonTest);
            this->registerMethod("fillRectTest", *this, &ImagePainterTest::fillRectTest);
        }

        void drawThinLineTest()
        {
            _imagePainter.setPen( Pen( 1, ARgbColor( 0, 0, 0 ) ) );

            for( Pt::ssize_t size = 500; size >= 0; --size )
            {
                _image.resize(  size, size, _bkColor );

                _imagePainter.drawLine( Point( 10,10 ), Point (100,100 ) );
                _imagePainter.drawLine( Point( 100,10 ), Point ( 10,100 ) );
                _imagePainter.drawLine( Point( 55,10 ), Point ( 55,100 ) );
                _imagePainter.drawLine( Point( 10,55 ), Point ( 100,55 ) );
            }
        }

        void drawThickLineTest()
        {
            _imagePainter.setPen( Pen( 10, ARgbColor( 0, 0, 0 ) ) );

            for( Pt::ssize_t size = 500; size >= 0; --size )
            {
                _image.resize(  size, size, _bkColor );

                _imagePainter.drawLine( Point( 10,10 ), Point (100,100 ) );
                _imagePainter.drawLine( Point( 100,10 ), Point ( 10,100 ) );
                _imagePainter.drawLine( Point( 55,10 ), Point ( 55,100 ) );
                _imagePainter.drawLine( Point( 10,55 ), Point ( 100,55 ) );
            }

            _image.resize(  300, 300, _bkColor );
            _imagePainter.setPen( Pen( Pen::DashStyle ) );
            _imagePainter.drawLine( Point( 55,10 ), Point ( 55,100 ) );
            PT_UNIT_ASSERT( checkImage() );
        }

        void fillPolygonTest()
        {
            _image.resize(  800, 600, _bkColor );
            std::vector<Point> polygon;

            _imagePainter.setBrush( Brush( ARgbColor( 0, 0, 0 )));

            polygon.push_back( Point( 10,200 ) );
            polygon.push_back( Point( 40,10 ) );
            polygon.push_back( Point( 80,100 ) );
            polygon.push_back( Point( 160,10 ) );
            polygon.push_back( Point( 200,200 ) );

            _imagePainter.fillPolygon( &polygon[0], polygon.size() );
            PT_UNIT_ASSERT( checkImage() );

            _image.clear();
            _image.resize(  800, 600, _bkColor );
            _imagePainter.setBrush( Brush( &_texture ) );
            _imagePainter.fillPolygon( &polygon[0], polygon.size() );
            PT_UNIT_ASSERT( checkImage() );
        }

        void drawRectTest()
        {
            _imagePainter.setPen( Pen( 1, ARgbColor( 0,0,0) ) );

            _image.resize(  800, 600, _bkColor );
            _imagePainter.drawRect( Gfx::Rect( Point( 10, 10), Size( 100,100) ) );
            PT_UNIT_ASSERT( checkImage() );

            _image.resize(  800, 600, _bkColor );
            _imagePainter.drawRect( Gfx::Rect( Point( -10, -10), Size( 1000,1000) ) );
            PT_UNIT_ASSERT( !checkImage() );

            _image.resize(  800, 600, _bkColor );
            _imagePainter.drawRect( Gfx::Rect( Point( -10, -10), Size( 1000,40) ) );
            PT_UNIT_ASSERT( checkImage() );
        }

        void fillRectTest()
        {
            _imagePainter.setBrush( Brush( ARgbColor( 0,0,0) ) );

            _image.resize(  800, 600, _bkColor );
            _imagePainter.fillRect( Gfx::Rect( Point( 10, 10), Size( 100,100) ) );
            PT_UNIT_ASSERT( checkImage() );

            _image.resize(  800, 600, _bkColor );
            _imagePainter.fillRect( Gfx::Rect( Point( -10, -10), Size( 1000, 1000 ) ) );
            PT_UNIT_ASSERT( checkImage() );

            _image.resize(  800, 600, _bkColor );
            _imagePainter.fillRect( Gfx::Rect( Point( -10, -10), Size( 1000, 50) ) );
            PT_UNIT_ASSERT( checkImage() );
        }

        void drawCircleTest()
        {
            _imagePainter.setPen( Pen( 1, ARgbColor( 0, 0, 0 ) ) );

            _image.resize(  800, 600, _bkColor );
            _imagePainter.drawCircle( Point( 10,10), 100 );
            PT_UNIT_ASSERT( checkImage() );

            _image.resize(  800, 600, _bkColor );
            _imagePainter.drawCircle( Point( -10,10), 100 );
            PT_UNIT_ASSERT( checkImage() );
        }

        void fillCircleTest()
        {
            _imagePainter.setBrush( Brush( ARgbColor( 0, 0, 0 ) ) );

            _image.clear();
            _image.resize(  800, 600, _bkColor );
            _imagePainter.fillCircle( Point( 10,10), 100 );
            PT_UNIT_ASSERT( checkImage() );

            _image.clear();
            _image.resize(  800, 600, _bkColor );
            _imagePainter.fillCircle( Point( -10, 10 ), 100 );
            PT_UNIT_ASSERT( checkImage() );
        }

        void drawThickEllipseTest()
        {
            _imagePainter.setPen( Pen( 10, ARgbColor( 0, 0, 0 ) ) );

            _image.resize(  800, 600, _bkColor );
            _imagePainter.drawEllipse( Point( 10,10), Size( 100,100) );
//            PT_UNIT_ASSERT( checkImage() );

            _image.resize(  800, 600, _bkColor );
            _imagePainter.drawEllipse( Point( -10,10), Size( 100,100) );
//            PT_UNIT_ASSERT( checkImage() );

            _image.resize(  800, 600, _bkColor );
            _imagePainter.drawEllipse( Point( -10,10), Size( 1000,100) );
            //PT_UNIT_ASSERT( checkImage() );
        }

        void drawThinEllipseTest()
        {
            _imagePainter.setPen( Pen( 1, ARgbColor( 0, 0, 0 ) ) );

            _image.resize(  800, 600, _bkColor );
            _imagePainter.drawEllipse( Point( 10,10), Size( 100,100) );
            PT_UNIT_ASSERT( checkImage() );

            _image.resize(  800, 600, _bkColor );
            _imagePainter.drawEllipse( Point( -10,10), Size( 100,100) );
            PT_UNIT_ASSERT( checkImage() );

            _image.resize(  800, 600, _bkColor );
            _imagePainter.drawEllipse( Point( -10,10), Size( 1000,100) );
            PT_UNIT_ASSERT( checkImage() );
        }

        void fillEllipseTest()
        {
            _imagePainter.setBrush( Brush( ARgbColor( 0, 0, 0 ) ) );

            _image.resize(  800, 600, _bkColor );
            _imagePainter.fillEllipse( Point( 10,10), Size( 100,100) );
            PT_UNIT_ASSERT( checkImage() );

            _image.resize(  800, 600, _bkColor );
            _imagePainter.fillEllipse( Point( -10,10), Size( 100,100) );
            PT_UNIT_ASSERT( checkImage() );

            _image.resize(  800, 600, _bkColor );
            _imagePainter.fillEllipse( Point( -10,10), Size( 1000,100) );
            PT_UNIT_ASSERT( checkImage() );
            _image.resize(  800, 600, _bkColor );

            _image.clear();
            _image.resize(  800, 600, _bkColor );
            _imagePainter.setBrush( Brush(&_texture) );
            _imagePainter.fillEllipse( Point( -10,10), Size( 1000,100) );
            PT_UNIT_ASSERT( checkImage() );
        }

        void drawPolylineTest()
        { }

        void drawImageTest()
        { }

        void fontMetricTest()
        {
            ARgbColor  outlineColor( 0, 0xffff, 0 );
            String     text( L"Hallo Platinum!" );

            _imagePainter.setFont( Font("Vera", 12, Font::NormalStyle, 340 ) );
            FontMetrics metrics  = _imagePainter.fontMetrics( text );
            PT_UNIT_ASSERT( metrics.ascent() == 12 );
            PT_UNIT_ASSERT( metrics.descent() == 3 );
            PT_UNIT_ASSERT( metrics.width() >= 80 && metrics.width() <= 100 );
            PT_UNIT_ASSERT( metrics.height() >= 10 && metrics.height() <= 18 );

            _imagePainter.setFont( Font("Vera", 14, Font::NormalStyle, 340 ) );
            metrics  = _imagePainter.fontMetrics( text );
            PT_UNIT_ASSERT( metrics.ascent() == 13 );
            PT_UNIT_ASSERT( metrics.descent() == 4 );
            PT_UNIT_ASSERT( metrics.width() >= 90 && metrics.width() <= 110 );
            PT_UNIT_ASSERT( metrics.height() >= 12 && metrics.height() <= 20 );

            _imagePainter.setFont( Font("Vera", 12, Font::NormalStyle, 340 ) );
            metrics  = _imagePainter.fontMetrics( text );
            PT_UNIT_ASSERT( metrics.ascent() == 12 );
            PT_UNIT_ASSERT( metrics.descent() == 3 );
            PT_UNIT_ASSERT( metrics.width() >= 80 && metrics.width() <= 100 );
            PT_UNIT_ASSERT( metrics.height() >= 10 && metrics.height() <= 18 );
        }

        void drawTextTest()
        {
            ARgbColor  backgroundColor( 0, 0xffff, 0);
            String     text( L"Hallo Platinum!" );

            _imagePainter.setFont( Font( "Vera", 12, Font::NormalStyle, 0) );

            //No clipping
            _image.resize( 800, 600, _bkColor );
            _imagePainter.drawText( Point( 40, 40 ), text, &backgroundColor );
            PT_UNIT_ASSERT( checkImage() );

            //Left clipping
            _image.resize( 800, 600, _bkColor );
            _imagePainter.drawText( Point( -4, 40 ), text, &backgroundColor );
            PT_UNIT_ASSERT( checkImage() );

            //Top cliping
            _image.resize( 800, 600, _bkColor );
            _imagePainter.drawText( Point( 40, 1 ), text, &backgroundColor );
            PT_UNIT_ASSERT( checkImage() );

            //Right clipping
            _image.resize( 800, 600, _bkColor );
            _imagePainter.drawText( Point( 782, 40 ), text, &backgroundColor );
            PT_UNIT_ASSERT( checkImage() );

            //Bottom clipping
            _image.resize( 800, 600, _bkColor );
            _imagePainter.drawText( Point( 40, 601 ), text, &backgroundColor );
            PT_UNIT_ASSERT( checkImage() );

            //Outside
            _image.resize( 800, 600, _bkColor );
            _imagePainter.drawText( Point( 10, -40 ), text, &backgroundColor );
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

    ARgbImage     _image;
    ImagePainter  _imagePainter;
    Pt::Gfx::ARgbImage _texture;
    ARgbColor     _bkColor;
};

Pt::Unit::RegisterTest<ImagePainterTest> register_imagePainterTest;
