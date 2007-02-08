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


class DrawRectDemo : public Pt::Gui::Widget
{
    public:
        DrawRectDemo()
        : _image( )
        , _imagePainter( _image )
        , _angle(0 )
        {
            this->setTitle(L"DrawEllipseDemo");
        }

        virtual void _paintEvent(const Pt::Gui::PaintEvent& event)
        {
            Pt::Math::Rect rect1( Pt::Math::Point(10, 10), Pt::Math::Size(40, 40) );
            _imagePainter.setBrush( Pt::Gfx::Brush( Pt::Gfx::ARgbColor( 0, 0xffff, 0 ) ) );
            _imagePainter.setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor( 0xffff, 0xffff, 0xffff ) ) );
            _imagePainter.fillRect( rect1 ) ;
            _imagePainter.drawRect( rect1 ) ;
            painter().drawImage( Pt::Math::Point( 0, 0 ), _image );

            Pt::Math::Rect rect2( Pt::Math::Point(55, 10), Pt::Math::Size(40, 40) );
            this->painter().setBrush( Pt::Gfx::Brush( Pt::Gfx::ARgbColor( 0, 0xffff, 0 ) ) );
            this->painter().setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor( 0xffff, 0xffff, 0xffff ) ) );
            this->painter().fillRect( rect2 ) ;
            this->painter().drawRect( rect2 ) ;
        }

        virtual void _resizeEvent(const Pt::Gui::ResizeEvent& event)
        {
            _image.resize(  event.width(), event.height(), Pt::Gfx::ARgbColor( 0xffff, 0, 0 ) );
        }

    private:
        Pt::Gfx::ARgbImage _image;
        Pt::Gfx::ImagePainter _imagePainter;
        Pt::ssize_t _angle;
};


int main( int argc, char* argv[] )
{
    try
    {
        Pt::Gui::Application app;

        DrawRectDemo demo;
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
