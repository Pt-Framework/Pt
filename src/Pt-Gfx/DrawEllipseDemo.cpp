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


class DrawEllipseDemo : public Pt::Gui::Widget
{
	public:
	DrawEllipseDemo()
	: _image( )
	, _imagePainter( _image )
	, _angle(0 )
	{
		this->setTitle(L"DrawEllipseDemo");
	}

	virtual ~DrawEllipseDemo()
	{}

	virtual void _paintEvent(const Pt::Gui::PaintEvent& event)
	{
		size_t pens = 1;
		Pt::Math::Point point( 40 ,50 );
		Pt::Math::Point point2( -10 ,100 );
		Pt::Math::Size	size( 70, 70 );
		
		_imagePainter.setPen( Pt::Gfx::Pen( pens, Pt::Gfx::ARgbColor( 0,0,0 ) ) );		
		_imagePainter.setBrush( Pt::Gfx::Brush( Pt::Gfx::ARgbColor( 0,0,0 ) ) );		
		_imagePainter.drawEllipse( point, size ) ;
		_imagePainter.fillEllipse( point2, size );
		
		painter().drawImage( Pt::Math::Point( 0, 0 ), _image );
		painter().setPen( Pt::Gfx::Pen( pens, Pt::Gfx::ARgbColor( 0, 0xffff, 0 ) ) );
		painter().setBrush( Pt::Gfx::Brush( Pt::Gfx::ARgbColor( 0, 0xffff, 0 ) ) );
		painter().drawEllipse( point, size ) ;
		//painter().fillEllipse( point2, size );
		
		
    }

	virtual void _resizeEvent(const Pt::Gui::ResizeEvent& event)
	{
		_image.resize(  event.width(), event.height(), Pt::Gfx::ARgbColor( 0xffff, 0, 0 ) );
	}

private:
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
