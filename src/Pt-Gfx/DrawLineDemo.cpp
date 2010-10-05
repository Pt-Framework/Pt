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
#include <Pt/Gui/ResizeEvent.h>
#include <Pt/Gui/PaintEvent.h>

#include <iostream>
#include <stdexcept>
#include <string>


using namespace Pt;
using namespace Pt::Gfx;

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

        void drawStar(Gfx::Painter& painter, const Pt::Gfx::Point& center, size_t lineWidth)
        {

            painter.setPen( Pt::Gfx::Pen( lineWidth, Pt::Gfx::ARgbColor(0, 0xcccc, 0) ));
            painter.drawLine( Pt::Gfx::Point( center.x(), center.y()-120 ),
                                    Pt::Gfx::Point( center.x(), center.y()-50 ));

            painter.setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor(0, 0, 0) ));
            painter.drawLine( Pt::Gfx::Point( center.x(), center.y()-120 ),
                                    Pt::Gfx::Point( center.x(), center.y()-50 ));

            painter.setPen( Pt::Gfx::Pen( lineWidth, Pt::Gfx::ARgbColor(0, 0xcccc, 0) ));
            painter.drawLine( Pt::Gfx::Point( center.x() - 50, center.y()-120 ),
                                    Pt::Gfx::Point( center.x(), center.y()-30 ));

            painter.setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor(0, 0, 0xcccc) , Pen::SolidStyle, Pen::TriangularCap));
            painter.drawLine( Pt::Gfx::Point( center.x() - 50, center.y()-120 ),
                                    Pt::Gfx::Point( center.x(), center.y()-30 ));

            painter.setPen( Pt::Gfx::Pen( lineWidth, Pt::Gfx::ARgbColor(0, 0, 0xcccc),Pen::SolidStyle, Pen::TriangularCap ));
            painter.drawLine( Pt::Gfx::Point( center.x() - 50, center.y()-70 ),
                                    Pt::Gfx::Point( center.x(), center.y()-20 ));

            painter.setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor(0, 0, 0) ));
            painter.drawLine( Pt::Gfx::Point( center.x() - 50, center.y()-70 ),
                                    Pt::Gfx::Point( center.x(), center.y()-20 ));

            painter.setPen( Pt::Gfx::Pen( lineWidth, Pt::Gfx::ARgbColor(0, 0xcccc, 0) ));
            painter.drawLine( Pt::Gfx::Point( center.x() - 50, center.y()-20 ),
                                    Pt::Gfx::Point( center.x(), center.y()-10 ));

            painter.setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor(0, 0, 0) ));
            painter.drawLine( Pt::Gfx::Point( center.x() - 50, center.y()-20 ),
                                    Pt::Gfx::Point( center.x(), center.y()-10 ));

            painter.setPen( Pt::Gfx::Pen( lineWidth, Pt::Gfx::ARgbColor(0, 0xcccc, 0) ));
            painter.drawLine( Pt::Gfx::Point( center.x() - 50, center.y() ), center);

            painter.setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor(0, 0, 0) ));
            painter.drawLine( Pt::Gfx::Point( center.x() - 50, center.y() ), center);

            painter.setPen( Pt::Gfx::Pen( lineWidth, Pt::Gfx::ARgbColor(0, 0xcccc, 0) ));
            painter.drawLine( Pt::Gfx::Point( center.x() - 50, center.y()+20 ),
                                    Pt::Gfx::Point( center.x(), center.y()+10 ));

            painter.setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor(0, 0, 0) ));
            painter.drawLine( Pt::Gfx::Point( center.x() - 50, center.y()+20 ),
                                    Pt::Gfx::Point( center.x(), center.y()+10 ));

            painter.setPen( Pt::Gfx::Pen( lineWidth, Pt::Gfx::ARgbColor(0, 0xcccc, 0),Pen::DashStyle ));
            painter.drawLine( Pt::Gfx::Point( center.x() - 50, center.y()+70 ),
                                    Pt::Gfx::Point( center.x(), center.y()+20 ));

            painter.setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor(0, 0, 0),Pen::DoubleDash ));
            painter.drawLine( Pt::Gfx::Point( center.x() - 50, center.y()+70 ),
                                    Pt::Gfx::Point( center.x(), center.y()+20 ));

            painter.setPen( Pt::Gfx::Pen( lineWidth, Pt::Gfx::ARgbColor(0, 0xcccc, 0),Pen::DoubleDash ));
            painter.drawLine( Pt::Gfx::Point( center.x() - 50, center.y()+120 ),
                                    Pt::Gfx::Point( center.x(), center.y()+30 ));


            painter.setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor(0, 0, 0),Pen::DoubleDash ));
            painter.drawLine( Pt::Gfx::Point( center.x() - 50, center.y()+120 ),
                                    Pt::Gfx::Point( center.x(), center.y()+30 ));

            painter.setPen( Pt::Gfx::Pen( lineWidth, Pt::Gfx::ARgbColor(0, 0xcccc, 0),Pen::DoubleDash ));
            painter.drawLine( Pt::Gfx::Point( center.x(), center.y()+120 ),
                                    Pt::Gfx::Point( center.x(), center.y()+50 ));

            painter.setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor(0, 0, 0),Pen::DoubleDash));
            painter.drawLine( Pt::Gfx::Point( center.x(), center.y()+120 ),
                                    Pt::Gfx::Point( center.x(), center.y()+50 ));
        }

        virtual void _paintEvent(const Pt::Gui::PaintEvent& event)
        {
            this->drawStar( _imagePainter, Pt::Gfx::Point(60, 150), 2);
            this->drawStar( _imagePainter, Pt::Gfx::Point(120, 150), 3);
            this->drawStar( _imagePainter, Pt::Gfx::Point(180, 150), 4);
            this->drawStar( _imagePainter, Pt::Gfx::Point(240, 150), 5);
            this->drawStar( _imagePainter, Pt::Gfx::Point(300, 150), 6);
            this->drawStar( _imagePainter, Pt::Gfx::Point(360, 150), 8);

            painter().drawImage( Pt::Gfx::Point( 0, 0 ), _image );

            Gui::Painter widgetPainter = painter();

            this->drawStar( widgetPainter, Pt::Gfx::Point(60, 450), 2);
            this->drawStar( widgetPainter,Pt::Gfx::Point(120, 450), 3);
            this->drawStar( widgetPainter,Pt::Gfx::Point(180, 450), 4);
            this->drawStar( widgetPainter,Pt::Gfx::Point(240, 450), 5);
            this->drawStar( widgetPainter,Pt::Gfx::Point(300, 450), 6);
            this->drawStar( widgetPainter,Pt::Gfx::Point(360, 450), 8);
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

        DrawLineDemo demo;
        connect( demo.closed, app, &Pt::Gui::Application::exit );

        demo.resize(600, 800);
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
