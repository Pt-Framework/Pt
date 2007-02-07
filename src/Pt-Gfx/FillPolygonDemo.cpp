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

Pt::Gfx::ARgbColor G(158*255, 158*255, 158*255);
Pt::Gfx::ARgbColor A(188*255, 188*255, 188*255);
Pt::Gfx::ARgbColor W(0xffff, 0xffff, 0xffff);

Pt::Gfx::ARgbColor texture_data[]  = {W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,
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


class FillPolygonDemo : public Pt::Gui::Widget
{
    public:
        FillPolygonDemo()
        : _f(0), _image( )
        , _imagePainter( _image )
        , _angle(0 )
        {
            this->setTitle(L"FillPolygonDemo");

            Pt::Gfx::ARgbImage texture(21, 21, Pt::Gfx::ARgbColor(0,0,0xdddd));
            std::copy( texture_data, texture_data + (21*21), texture.begin() );

            _brush = Pt::Gfx::Brush(&texture);
        }

        virtual void _paintEvent(const Pt::Gui::PaintEvent& event)
        {
            std::vector<Pt::Math::Point> points(5);
            points[0] = Pt::Math::Point(10 + _f, 10+ _f);
            points[1] = Pt::Math::Point(100+ _f, 10+ _f);
            points[2] = Pt::Math::Point(100+ _f, 100+ _f);
            points[3] = Pt::Math::Point(50+ _f, 150+ _f);
            points[4] = Pt::Math::Point(10+ _f, 100+ _f);

            if(++_f > 200) _f = 0;

            _imagePainter.setBrush(_brush);
            _imagePainter.fillPolygon( &points[0], points.size() );

            painter().drawImage( Pt::Math::Point( 0, 0 ), _image );
        }

        virtual void _resizeEvent(const Pt::Gui::ResizeEvent& event)
        {
            _image.resize(  event.width(), event.height(), Pt::Gfx::ARgbColor( 0xffff, 0, 0 ) );
        }

    private:
        Pt::size_t _f;
        Pt::Gfx::Brush _brush;
        Pt::Gfx::ARgbImage _image;
        Pt::Gfx::ImagePainter _imagePainter;
        Pt::ssize_t _angle;
};


int main( int argc, char* argv[] )
{
    try
    {
        Pt::Gui::Application app;

        FillPolygonDemo demo;
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
