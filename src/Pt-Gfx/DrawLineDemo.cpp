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


class DrawLineDemo : public Pt::Gui::Widget
{
	public:
	DrawLineDemo()
	: _image( )
	, _imagePainter( _image )
	, _angle(0 )
	{
		this->setTitle(L"DrawLineDemo");
	}

	virtual ~DrawLineDemo()
	{}

	virtual void _paintEvent(const Pt::Gui::PaintEvent& event)
	{
        _imagePainter.setPen( Pt::Gfx::Pen( 3, Pt::Gfx::ARgbColor(0, 0xdddd, 0) ));
        _imagePainter.drawLine( Pt::Math::Point( 20, 10 ), Pt::Math::Point( 160, 30 ));

        _imagePainter.setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor(0, 0, 0) ));
        _imagePainter.drawLine( Pt::Math::Point( 20, 10 ), Pt::Math::Point( 160, 30 ));


        _imagePainter.setPen( Pt::Gfx::Pen( 4, Pt::Gfx::ARgbColor(0, 0xdddd, 0) ));
        _imagePainter.drawLine( Pt::Math::Point( 20, 20 ), Pt::Math::Point( 160, 40 ));

        _imagePainter.setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor(0, 0, 0) ));
        _imagePainter.drawLine( Pt::Math::Point( 20, 20 ), Pt::Math::Point( 160, 40 ));

        _imagePainter.setPen( Pt::Gfx::Pen( 5, Pt::Gfx::ARgbColor(0, 0xdddd, 0) ));
        _imagePainter.drawLine( Pt::Math::Point( 20, 30 ), Pt::Math::Point( 160, 50 ));

        _imagePainter.setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor(0, 0, 0) ));
        _imagePainter.drawLine( Pt::Math::Point( 20, 30 ), Pt::Math::Point( 160, 50 ));

        painter().drawImage( Pt::Math::Point( 0, 0 ), _image );

/*
		//
		// draw wide lines of varying widths using the ImagePainter
		//
		_imagePainter.setPen( Pt::Gfx::Pen( 2, Pt::Gfx::ARgbColor(0, 0xffff,0 ) ));
        _imagePainter.drawLine( Pt::Math::Point( 10, 10 ), Pt::Math::Point( 55, -155 ));

        _imagePainter.setPen( Pt::Gfx::Pen( 3, Pt::Gfx::ARgbColor(0, 0xffff, 0) ));
        _imagePainter.drawLine( Pt::Math::Point( 20, 10 ), Pt::Math::Point( 160, 150 ));

        _imagePainter.setPen( Pt::Gfx::Pen( 4, Pt::Gfx::ARgbColor(0, 0xffff, 0) ));
        _imagePainter.drawLine( Pt::Math::Point( 30, 10 ), Pt::Math::Point( 70, 50 ));

        _imagePainter.setPen( Pt::Gfx::Pen( 5, Pt::Gfx::ARgbColor(0, 0xffff, 0) ));
        _imagePainter.drawLine( Pt::Math::Point( 40, 10 ), Pt::Math::Point( 80, 50 ));

        _imagePainter.setPen( Pt::Gfx::Pen( 6, Pt::Gfx::ARgbColor(0, 0xffff, 0) ));
        _imagePainter.drawLine( Pt::Math::Point( 52, 10 ), Pt::Math::Point( 92, 50 ));

        _imagePainter.setPen( Pt::Gfx::Pen( 7, Pt::Gfx::ARgbColor(0, 0xffff, 0) ));
        _imagePainter.drawLine( Pt::Math::Point( 65, 10 ), Pt::Math::Point( 105, 50 ));

		//
		// draw thin lines over the wide lines to mark center using the ImagePainter
		//
        _imagePainter.setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor(0, 0, 0) ));
        _imagePainter.drawLine( Pt::Math::Point( 10, 10 ), Pt::Math::Point( 150, 150 ));
        _imagePainter.drawLine( Pt::Math::Point( 20, 10 ), Pt::Math::Point( 60, 50 ));
        _imagePainter.drawLine( Pt::Math::Point( 30, 10 ), Pt::Math::Point( 70, 50 ));
        _imagePainter.drawLine( Pt::Math::Point( 40, 10 ), Pt::Math::Point( 80, 50 ));
        _imagePainter.drawLine( Pt::Math::Point( 52, 10 ), Pt::Math::Point( 92, 50 ));
        _imagePainter.drawLine( Pt::Math::Point( 65, 10 ), Pt::Math::Point( 105, 50 ));

        painter().drawImage( Pt::Math::Point( 0, 0 ), _image );


		//
		// draw wide lines of varying widths using the Gui::Painter
		//
        painter().setPen( Pt::Gfx::Pen( 2, Pt::Gfx::ARgbColor(0, 0, 0xffff) ));
        painter().drawLine( Pt::Math::Point( 55, 55 ), Pt::Math::Point( 100, 100 ));

        painter().setPen( Pt::Gfx::Pen( 3, Pt::Gfx::ARgbColor(0, 0, 0xffff) ));
        painter().drawLine( Pt::Math::Point( 65, 55 ), Pt::Math::Point( 110, 100 ));

        painter().setPen( Pt::Gfx::Pen( 4, Pt::Gfx::ARgbColor(0, 0, 0xffff) ));
        painter().drawLine( Pt::Math::Point( 75, 55 ), Pt::Math::Point( 120, 100 ));

        painter().setPen( Pt::Gfx::Pen( 5, Pt::Gfx::ARgbColor(0, 0, 0xffff) ));
        painter().drawLine( Pt::Math::Point( 85, 55 ), Pt::Math::Point( 130, 100 ));

        painter().setPen( Pt::Gfx::Pen( 6, Pt::Gfx::ARgbColor(0, 0, 0xffff) ));
        painter().drawLine( Pt::Math::Point( 97, 55 ), Pt::Math::Point( 142, 100 ));

        painter().setPen( Pt::Gfx::Pen( 7, Pt::Gfx::ARgbColor(0, 0, 0xffff) ));
        painter().drawLine( Pt::Math::Point( 110, 55 ), Pt::Math::Point( 155, 100 ));

		//
		// draw thin lines over the wide lines to mark center using the Gui::Painter
		//
        painter().setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor(0, 0, 0) ));
        painter().drawLine( Pt::Math::Point( 55, 55 ), Pt::Math::Point( 100, 100 ));
        painter().drawLine( Pt::Math::Point( 65, 55 ), Pt::Math::Point( 110, 100 ));
        painter().drawLine( Pt::Math::Point( 75, 55 ), Pt::Math::Point( 120, 100 ));
        painter().drawLine( Pt::Math::Point( 85, 55 ), Pt::Math::Point( 130, 100 ));
        painter().drawLine( Pt::Math::Point( 97, 55 ), Pt::Math::Point( 142, 100 ));
        painter().drawLine( Pt::Math::Point( 110, 55 ), Pt::Math::Point( 155, 100 ));
*/
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

		DrawLineDemo demo;
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
