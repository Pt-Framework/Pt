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
#include <string>
#include <Pt/Main.h>

#include <Pt/Exception.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gui/Application.h>
#include <Pt/Gui/Painter.h>
#include <Pt/Gui/Widget.h>
#include <Pt/Gui/ResizeEvent.h>
#include <Pt/Gui/PaintEvent.h>

#include <Pt/Math/Point.h>
#include <Pt/System/Clock.h>
#include <iostream>
#include <fstream>


class ImagePainterDemo : public Pt::Gui::Widget
{
	public:
	ImagePainterDemo()
	: _image( )
	, _imagePainter( _image )
	, _angle(0 )
	{
		this->setTitle(L"ImagePainterDemo");
	}

	virtual ~ImagePainterDemo()
	{}

	virtual void _paintEvent(const Pt::Gui::PaintEvent& event)
	{
/*
		std::vector<Pt::Math::Point> polygon;


		polygon.push_back( Pt::Math::Point( 10 - 50, 100 - 80));
		polygon.push_back( Pt::Math::Point( 50- 50, 10 - 80));
		polygon.push_back( Pt::Math::Point( 100- 50, 80 - 80));
		polygon.push_back( Pt::Math::Point( 150- 50, 10 - 80));
		polygon.push_back( Pt::Math::Point( 200- 50, 100 - 80));
		polygon.push_back( Pt::Math::Point( 100- 50, 300 - 80));
		polygon.push_back( Pt::Math::Point( 20- 50, 250- 80));

		polygon.push_back( Pt::Math::Point( -50, -50 ));
		polygon.push_back( Pt::Math::Point( 50, -50 ));
		polygon.push_back( Pt::Math::Point( 50, 100 ));
		polygon.push_back( Pt::Math::Point( -50, 100 ));
*/



		Pt::Gfx::ARgbColor outline( 0xffff, 0xffff, 0xffff );
		_imagePainter.setFont(Pt::Gfx::Font("Vera" , 12 ,Pt::Gfx::Font::NormalStyle, _angle ));
        Pt::System::TimeValue time;
        Pt::System::Clock clock;

        clock.start();
		_imagePainter.drawText( Pt::Math::Point(200, 100), L"Hello PTV!", &outline );
		time = clock.stop();

		std::cerr<<"Image Time: "<< time.seconds()+ time.microSeconds() / 1000000.0<<std::endl;


/*
		_imagePainter.setBrush( Pt::Gfx::Brush( Pt::Gfx::ARgbColor(0,0,0xffff) ));
		_imagePainter.fillPolygon( &polygon[0], polygon.size() );

		_imagePainter.setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor(0,0,0xffff) ));

		_imagePainter.drawLine( Pt::Math::Point( 10, 100 ), Pt::Math::Point( 10, 10 ));
//		_imagePainter.drawLine( Pt::Math::Point( 10, 100 ), Pt::Math::Point( 75, 10 ));
		_imagePainter.drawLine( Pt::Math::Point( 10, 100 ), Pt::Math::Point( 100, 10 ));
		_imagePainter.drawLine( Pt::Math::Point( 10, 100 ), Pt::Math::Point( 60, 10 ));
		_imagePainter.drawLine( Pt::Math::Point( 10, 100 ), Pt::Math::Point( 100, 60 ));
		_imagePainter.drawLine( Pt::Math::Point( 10, 100 ), Pt::Math::Point( 100, 100 ));


		_imagePainter.setPen( Pt::Gfx::Pen( 10, Pt::Gfx::ARgbColor(0xffff,0 ,0) ) );

		_imagePainter.drawLine( Pt::Math::Point( 10,10 ),Pt::Math::Point (15,15 ) );

		_imagePainter.drawLine( Pt::Math::Point( 100,10 ),Pt::Math::Point ( 10,100 ) );
		_imagePainter.drawLine( Pt::Math::Point( 55,10 ),Pt::Math::Point ( 55,100 ) );
		_imagePainter.drawLine( Pt::Math::Point( 10,55 ),Pt::Math::Point ( 100,55 ) );

		Pt::Gfx::ARgbColor color( 0xffff,0xffff, 0);

		_imagePainter.setFont( Pt::Gfx::Font( "Vera", 10, Pt::Gfx::Font::NormalStyle, -300));
		_imagePainter.drawText(Math::Point( 22, 180), L"Hallo Platinum!", &color  );

		_imagePainter.setFont( Pt::Gfx::Font( "Vera", 28, Pt::Gfx::Font::NormalStyle, 100) );
		_imagePainter.drawText(Math::Point( 220, 80), L"Hallo Platinum!");

		Pt::System::TimeValue delta = clock.stop();
		std::cout<<"Draw time: " << (delta.seconds() + delta.microSeconds()/1000000.0) << std::endl;
*/
		painter().drawImage( Pt::Math::Point( 0, 0 ), _image );



		painter().setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor( 0, 0, 0 ) ) );
		painter().setFont(Pt::Gfx::Font( "Tahoma", 12, Pt::Gfx::Font::NormalStyle, _angle));

		_angle += 10;
		if( _angle >= 3600 )
			_angle = 0;



		Pt::System::TimeValue time2;
		Pt::System::Clock clock2;
		clock2.start();
		painter().drawText(Pt::Math::Point(200, 130), L"Hello PTV!" );
		time2 = clock2.stop();
		std::cerr<<"System Time: "<< time2.seconds()+ time2.microSeconds() / 1000000.0<<std::endl;

/*
		painter().setBrush( Pt::Gfx::Brush( Pt::Gfx::ARgbColor(0,0,0xffff) ));


		polygon.clear();

		polygon.push_back( Pt::Math::Point( 10  + 50 , 100 ));
		polygon.push_back( Pt::Math::Point( 50 + 50 , 10));
		polygon.push_back( Pt::Math::Point( 100 + 50 , 80));
		polygon.push_back( Pt::Math::Point( 150 + 50 , 10));
		polygon.push_back( Pt::Math::Point( 200 + 50 , 100));
		polygon.push_back( Pt::Math::Point( 100 + 50 , 300));
		polygon.push_back( Pt::Math::Point( 20 + 50 , 250));

		//clock.start();
		//painter().fillPolygon( &polygon[0], polygon.size() );

		painter().setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor(0xffff,0,0) ));

		painter().drawLine( Pt::Math::Point( 10 + 4, 100  + 4), Pt::Math::Point( 10 + 4, 10 + 4));
//		painter().drawLine( Pt::Math::Point( 10 + 4, 100 + 4), Pt::Math::Point( 75 + 4, 10 ));
		painter().drawLine( Pt::Math::Point( 10 + 4, 100 + 4), Pt::Math::Point( 100 + 4, 10+ 4 ));
		painter().drawLine( Pt::Math::Point( 10 + 4, 100+ 4 ), Pt::Math::Point( 60 + 4, 10 + 4));
		painter().drawLine( Pt::Math::Point( 10 + 4, 100+ 4 ), Pt::Math::Point( 100 + 4, 60 + 4));
		painter().drawLine( Pt::Math::Point( 10 + 4, 100 + 4), Pt::Math::Point( 100 + 4, 100 + 4));
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

		ImagePainterDemo imageTest;
		connect( imageTest.closed, app, &Pt::Gui::Application::exit );

		imageTest.show();
		return app.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
