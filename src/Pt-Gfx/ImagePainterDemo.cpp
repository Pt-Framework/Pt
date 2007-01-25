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
    {
        this->setTitle(L"ImagePainterDemo");
    }

    virtual ~ImagePainterDemo()
    {}

    virtual void _paintEvent(const Pt::Gui::PaintEvent& event)
    {

        std::vector<Pt::Math::Point> polygon;


        polygon.push_back( Pt::Math::Point( 10, 100 ));
        polygon.push_back( Pt::Math::Point( 50, 10));
        polygon.push_back( Pt::Math::Point( 100, 80));
        polygon.push_back( Pt::Math::Point( 150, 10));
        polygon.push_back( Pt::Math::Point( 200, 100));
        polygon.push_back( Pt::Math::Point( 100, 300));
        polygon.push_back( Pt::Math::Point( 20, 250));

/*
        polygon.push_back( Pt::Math::Point( 50, 20 ));
        polygon.push_back( Pt::Math::Point( 70, 40 ));
        polygon.push_back( Pt::Math::Point( 50, 60 ));
        polygon.push_back( Pt::Math::Point( 30, 40 ));
        */
		Pt::Gfx::ARgbColor outline(0xffff,0,0);
		Pt::Gfx::Font f("Vera", 60);
		_imagePainter.setFont(f);
		_imagePainter.drawText( Pt::Math::Point(300, 200), L"Hello PTV!" );
		_imagePainter.drawText( Pt::Math::Point(300, 270), L"Hello PTV!", &outline );
        _imagePainter.setBrush( Pt::Gfx::Brush( Pt::Gfx::ARgbColor(0xffff,0xffff,0xffff) ));
        //Pt::System::Clock clock;

        //clock.start();
        _imagePainter.fillPolygon( &polygon[0], polygon.size() );
        //Pt::System::TimeValue time = clock.stop();
        //std::cerr<<"Image Time: "<< time.seconds()+ time.microSeconds() / 1000000.0<<std::endl;


/*
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
        painter().setBrush( Pt::Gfx::Brush( Pt::Gfx::ARgbColor(0,0,0xffff) ));
        painter().drawImage( Pt::Math::Point( 0, 0 ), _image );

        polygon.clear();

        polygon.push_back( Pt::Math::Point( 10  + 50 , 100 ));
        polygon.push_back( Pt::Math::Point( 50 + 50 , 10));
        polygon.push_back( Pt::Math::Point( 100 + 50 , 80));
        polygon.push_back( Pt::Math::Point( 150 + 50 , 10));
        polygon.push_back( Pt::Math::Point( 200 + 50 , 100));
        polygon.push_back( Pt::Math::Point( 100 + 50 , 300));
        polygon.push_back( Pt::Math::Point( 20 + 50 , 250));



        //clock.start();
        painter().fillPolygon( &polygon[0], polygon.size() );

        //time = clock.stop();
        //std::cerr<<"System Time: "<< time.seconds()+ time.microSeconds() / 1000000.0<<std::endl;


    }

    virtual void _resizeEvent(const Pt::Gui::ResizeEvent& event)
    {
        _image.resize(  event.width(), event.height(), Pt::Gfx::ARgbColor( 0xffff, 0xffff, 0xffff ) );
    }

private:
    Pt::Gfx::ARgbImage    _image;
    Pt::Gfx::ImagePainter _imagePainter;
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
