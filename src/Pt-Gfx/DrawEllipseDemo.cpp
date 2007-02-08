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
#include <Pt/Main.h>
#include <Pt/SourceInfo.h>
#include <Pt/Math/Point.h>
#include <Pt/System/Clock.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/Gui/Application.h>
#include <Pt/Gui/Painter.h>
#include <Pt/Gui/Widget.h>
#include <Pt/Gui/ResizeEvent.h>
#include <Pt/Gui/PaintEvent.h>

#include <iostream>
#include <stdexcept>
#include <string>

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


class DrawEllipseDemo : public Pt::Gui::Widget
{
    public:
        DrawEllipseDemo()
        : _texture( 21, 21, Pt::Gfx::ARgbColor(0,0,0xdddd) )
        , _image( )
        , _imagePainter( _image )
        , _angle(0 )
        {
            this->setTitle(L"DrawEllipseDemo");

            std::copy( texture_data, texture_data + (21*21), _texture.data() );
        }

        virtual ~DrawEllipseDemo()
        {}

    virtual void _paintEvent(const Pt::Gui::PaintEvent& event)
    {
        size_t pens = 1;

        Size  size1( 40, 60 );
        Size  size2( 10, 10 );

        Point point1( 10 ,50 );
        Point point2( 50 ,50 );
        Point point3( 20 ,80 );
        Point point4( 60 ,80 );

        _imagePainter.setBrush( Brush( ARgbColor( 0,0xffff,0 ) ) );
        _imagePainter.fillEllipse( point1, size1 );

        _imagePainter.setPen( Pen( pens, ARgbColor( 0,0,0 ) ) );
        _imagePainter.drawEllipse( point1, size1 ) ;

        _imagePainter.setBrush( Brush( &_texture ) );
        _imagePainter.fillEllipse( point2, size1 );

        _imagePainter.setPen( Pen( pens, ARgbColor( 0,0,0 ) ) );
        _imagePainter.drawEllipse( point2, size1 ) ;

        _imagePainter.setBrush( Brush( &_texture ) );
        _imagePainter.fillEllipse( point3, size2 );

        _imagePainter.setBrush( Brush( ARgbColor( 0,0,0 ) ) );
        _imagePainter.fillEllipse( point4, size2 );

        painter().drawImage( Point( 0, 0 ), _image );
    }

    virtual void _resizeEvent(const Pt::Gui::ResizeEvent& event)
    {
        _image.resize(  event.width(), event.height(), Pt::Gfx::ARgbColor( 0xffff, 0, 0 ) );
    }

    private:
        Pt::Gfx::ARgbImage _texture;
        Pt::Gfx::ARgbImage		_image;
        Pt::Gfx::ImagePainter	_imagePainter;
        Pt::ssize_t				_angle;
};


int main( int argc, char* argv[] )
{
	try
	{
		Pt::Gui::Application app;

		DrawEllipseDemo demo;
		connect( demo.closed, app, &Pt::Gui::Application::exit );

		demo.resize(400, 300);
		demo.show();
		return app.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
