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

#include <Pt/Main.h>
#include <Pt/SourceInfo.h>
#include <Pt/Gfx/Point.h>
#include <Pt/System/Clock.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/Gui/Application.h>
#include <Pt/Gui/Painter.h>
#include <Pt/Gui/Widget.h>
#include <Pt/Gui/MouseMoveEvent.h>
#include <Pt/Gui/ResizeEvent.h>
#include <Pt/Gui/PaintEvent.h>
#include <iostream>
#include <fstream>
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


class GfxPerf : public Pt::Gui::Widget
{
    public:
        GfxPerf()
        : _image( )
        , _imagePainter( _image )
        , _out( "GfxPerf.txt" )
        {
            this->setTitle(L"GfxPerf");

            Pt::Gfx::ARgbImage texture(21, 21, Pt::Gfx::ARgbColor(0,0,0xdddd));
            std::copy( texture_data, texture_data + (21*21), texture.data() );

            _textureBrush = Pt::Gfx::Brush( &texture );
            _solidBrush   = Pt::Gfx::Brush( Pt::Gfx::ARgbColor( 0, 0xdddd, 0 ) );
        }

        void measurePolygon()
        {
            Pt::ssize_t x =  100;
            Pt::ssize_t y = 100;

            std::vector<Pt::Gfx::Point> points(5);
            points[0] = Pt::Gfx::Point( x-50, y-50);
            points[1] = Pt::Gfx::Point(100+ x, y -50);
            points[2] = Pt::Gfx::Point(100+ x, 100+ y);
            points[3] = Pt::Gfx::Point(50+ x, 150+ y);
            points[4] = Pt::Gfx::Point(x, 100+ y);

           _clock.start();
           _imagePainter.setBrush( _solidBrush );
           _imagePainter.fillPolygon( &points[0], points.size() );
           _time = _clock.stop();

           _out<<"ImagePainter-> FillPolygon SolidBrush :"<< _time.totalMSecs() <<std::endl;

           _clock.start();
           painter().setBrush( _solidBrush );
           painter().fillPolygon( &points[0], points.size() );
           _time = _clock.stop();

           _out<<"Native-> FillPolygon SolidBrush :"<< _time.totalMSecs()<<std::endl;
           _out<<std::endl;

            _clock.start();
           _imagePainter.setBrush( _textureBrush );
           _imagePainter.fillPolygon( &points[0], points.size() );
           _time = _clock.stop();

           _out<<"ImagePainter-> FillPolygon TextureBrush :"<< _time.totalMSecs()<<std::endl;

           _clock.start();
           painter().setBrush( _textureBrush );
           painter().fillPolygon( &points[0], points.size() );
           _time = _clock.stop();

           _out<<"Native-> FillPolygon TextureBrush :"<< _time.totalMSecs()<<std::endl;
           _out<<std::endl;
        }

        void measureText()
        {

            const Pt::String text(L"Hallo Platinum!");

            //Load cache with glyphs.
           _imagePainter.setFont( Pt::Gfx::Font( "Bitstream Vera Sans", 24, Pt::Gfx::Font::NormalStyle, 470 ) );
           _imagePainter.drawText( Pt::Gfx::Point( 30, 200) ,text);

            for( size_t i = 0; i < 20; i++ )
            {
                _imagePainter.setFont( Pt::Gfx::Font( "Bitstream Vera Sans", 24, Pt::Gfx::Font::NormalStyle, 370 * i) );
               _clock.start();
               _imagePainter.drawText( Pt::Gfx::Point( 30, 200), text );

               _time = _clock.stop();

               _out<<"ImagePainter-> DrawText :"<< _time.totalMSecs() <<std::endl;

               painter().setFont( Pt::Gfx::Font( "Bitstream Vera Sans", 24, Pt::Gfx::Font::NormalStyle, 370*i ) );

               _clock.start();
                painter().drawText( Pt::Gfx::Point( 30, 200) ,text);

               _time = _clock.stop();

               _out<<"Native-> DrawText :"<< _time.totalMSecs() <<std::endl;
               _out<<std::endl;
           }
        }

        void measureLine()
        {
            _clock.start();

            _imagePainter.setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor( 0 ,0 ,0 ) ) );
            _imagePainter.drawLine( Pt::Gfx::Point( 10,10 ), Pt::Gfx::Point( 40 , 100 ) );
            _imagePainter.drawLine( Pt::Gfx::Point( 10,10 ), Pt::Gfx::Point( 100 , 40 ) );
            _imagePainter.drawLine( Pt::Gfx::Point( 10,10 ), Pt::Gfx::Point( 40 , 40 ) );

            _time = _clock.stop();

            _out<<"ImagePainter-> DrawThinLine:"<< _time.totalMSecs() <<std::endl;

            _clock.start();

            painter().setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor( 0 ,0 ,0 ) ) );
            painter().drawLine( Pt::Gfx::Point( 10,10 ), Pt::Gfx::Point( 40 , 100 ) );
            painter().drawLine( Pt::Gfx::Point( 10,10 ), Pt::Gfx::Point( 100 , 40 ) );
            painter().drawLine( Pt::Gfx::Point( 10,10 ), Pt::Gfx::Point( 40 , 40 ) );

            _time = _clock.stop();

            _out<<"Native-> DrawThinLine:"<< _time.totalMSecs() <<std::endl;

            _out<<std::endl;

            _clock.start();

            _imagePainter.setPen( Pt::Gfx::Pen( 10, Pt::Gfx::ARgbColor( 0 ,0 ,0 ) ) );
            _imagePainter.drawLine( Pt::Gfx::Point( 10,10 ), Pt::Gfx::Point( 40 , 100 ) );
            _imagePainter.drawLine( Pt::Gfx::Point( 10,10 ), Pt::Gfx::Point( 100 , 40 ) );
            _imagePainter.drawLine( Pt::Gfx::Point( 10,10 ), Pt::Gfx::Point( 40 , 40 ) );

            _time = _clock.stop();

            _out<<"ImagePainter-> DrawThickLine:"<< _time.totalMSecs() <<std::endl;

            _clock.start();

            painter().setPen( Pt::Gfx::Pen( 10, Pt::Gfx::ARgbColor( 0 ,0 ,0 ) ) );
            painter().drawLine( Pt::Gfx::Point( 10,10 ), Pt::Gfx::Point( 40 , 100 ) );
            painter().drawLine( Pt::Gfx::Point( 10,10 ), Pt::Gfx::Point( 100 , 40 ) );
            painter().drawLine( Pt::Gfx::Point( 10,10 ), Pt::Gfx::Point( 40 , 40 ) );

            _time = _clock.stop();

            _out<<"Native-> DrawThickLine:"<< _time.totalMSecs() <<std::endl;
            _out<<std::endl;
        }

        virtual void _paintEvent(const Pt::Gui::PaintEvent& event)
        {

            painter().setBrush(Pt::Gfx::Brush( Pt::Gfx::ARgbColor(0, 0, 0x7fff) ) );
            painter().fillRect( Pt::Gfx::Rect( Pt::Gfx::Point(0, 0), Pt::Gfx::Size(1000,1000) ) );

            measureLine();
            measurePolygon();
            measureText();
        }

        virtual void _resizeEvent(const Pt::Gui::ResizeEvent& event)
        {
            _image.resize(  event.width(), event.height(), Pt::Gfx::ARgbColor( 0xffff, 0, 0 ) );
        }

    private:
        Pt::Gfx::Brush          _textureBrush;
        Pt::Gfx::Brush          _solidBrush;
        Pt::Gfx::ARgbImage      _image;
        Pt::Gfx::ImagePainter   _imagePainter;
        std::ofstream            _out;
        Pt::System::Clock       _clock;
        Pt::Timespan    _time;
};


int main( int argc, char* argv[] )
{
    try
    {
        Pt::Gui::Application app;

        GfxPerf demo;
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
