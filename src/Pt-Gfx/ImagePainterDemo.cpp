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
    {}

    virtual ~ImagePainterDemo()
    {}

    virtual void _paintEvent(const Pt::Gui::PaintEvent& event)
    {
        Pt::System::Clock clock;

        clock.start();

        _imagePainter.setPen( Pt::Gfx::Pen( 10, Pt::Gfx::ARgbColor(0xffff,0 ,0) ) );

        _imagePainter.drawLine( Pt::Math::Point( 10,10 ),Pt::Math::Point (100,100 ) );
        _imagePainter.drawLine( Pt::Math::Point( 100,10 ),Pt::Math::Point ( 10,100 ) );
        _imagePainter.drawLine( Pt::Math::Point( 55,10 ),Pt::Math::Point ( 55,100 ) );
        _imagePainter.drawLine( Pt::Math::Point( 10,55 ),Pt::Math::Point ( 100,55 ) );

        _imagePainter.setFont( Pt::Gfx::Font( "Vera", 28, Pt::Gfx::Font::NormalStyle, -300) );
        _imagePainter.drawText(Math::Point( 22, 180), L"Hallo Platinum!", &Pt::Gfx::ARgbColor(0xffff,0xffff,0) );

        _imagePainter.setFont( Pt::Gfx::Font( "Vera", 28, Pt::Gfx::Font::NormalStyle, 100) );
        _imagePainter.drawText(Math::Point( 220, 80), L"Hallo Platinum!");

        Pt::System::TimeValue delta = clock.stop();
        std::cout<<"Draw time: " << (delta.seconds() + delta.microSeconds()/1000000.0) << std::endl;

        painter().drawImage( Pt::Math::Point( 0, 0 ), _image );
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
    catch(const Pt::Exception& e)
    {
        std::cerr << "Exception: " << e.what() << "(" << e.sourceInfo().line() << " in " << e.sourceInfo().func() << ")" << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
