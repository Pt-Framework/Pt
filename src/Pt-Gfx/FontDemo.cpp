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
#include <Pt/Gui/Application.h>
#include <Pt/Exception.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gui/Painter.h>
#include <Pt/Gui/Widget.h>
#include <Pt/Gui/ResizeEvent.h>
#include <Pt/Gui/PaintEvent.h>
#include <Pt/Math/Point.h>
#include <Pt/Gfx/Font.h>
#include <Pt/System/Clock.h>

#include <iostream>
#include <fstream>


class FontDemo : public Pt::Gui::Widget
{
   public:
    FontDemo()
    : _image( )
    , _imagePainter( _image )
    {
        setTitle("FontDemo");
    }

    virtual ~FontDemo()
    {}

    virtual void _paintEvent(const Pt::Gui::PaintEvent& event)
    {
        Pt::System::Clock clock;

        
        _imagePainter.setFont( Pt::Gfx::Font( "", 28, Pt::Gfx::Font::NormalStyle,-899) );

        Pt::Gfx::FontMetrics metrics = _imagePainter.fontMetrics(L"Hallo Platinum!");
                
        clock.start();
        _imagePainter.drawText(Math::Point( 40, 40), L"Hallo Platinum!");
        
        
        Pt::System::TimeValue time = clock.stop();
        
        std::cout<<" FreeType Text time: "<< ( time.seconds() + time.microSeconds() / 1000000.0 ) << std::endl;
        
        painter().drawImage( Pt::Math::Point( 0, 0 ), _image );
  
  
        //-------------------------GDI
        
//        painter().setBrush(Pt::Gfx::Brush(Pt::Gfx::ARgbColor( 0xffff, 0xffff, 0xffff )) );
//        painter().fillRect( Pt::Math::Rect( Pt::Math::Point(0,0), this->size() ) ); 

        this->painter().setFont( Pt::Gfx::Font( "Tahoma", 28, Pt::Gfx::Font::NormalStyle,-899) );
        
        this->painter().setPen( Pt::Gfx::Pen(1,  Pt::Gfx::ARgbColor( 0x0000,0xffff,0)));
        metrics = this->painter().fontMetrics(L"Hallo PTV!");
        
        clock.start();       

        this->painter().drawText( Math::Point( 42, 40), L"Hallo Platinum!" ) ;
        time = clock.stop();

        std::cout<<" GDI Text time: "<< ( time.seconds() + time.microSeconds() / 1000000.0 ) << std::endl;        
    }

    virtual void _resizeEvent(const Pt::Gui::ResizeEvent& event)
    {
        _image.resize(  event.width(), event.height(), Pt::Gfx::ARgbColor(  0xffff,0x0, 0x0 ) );
        
        std::cout<<"Width: " << event.width() << "  Height: "<< event.height()<<std::endl;
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

		FontDemo fontDemo;
		connect( fontDemo.closed, app, &Pt::Gui::Application::exit );

		fontDemo.show();
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
