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

        void drawStar(Gfx::Painter& painter, const Pt::Math::Point& center, size_t lineWidth)
        {
            painter.setPen( Pt::Gfx::Pen( lineWidth, Pt::Gfx::ARgbColor(0, 0xcccc, 0) ));
            painter.drawLine( Pt::Math::Point( center.x(), center.y()-120 ),
                                    Pt::Math::Point( center.x(), center.y()-50 ));

            painter.setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor(0, 0, 0) ));
            painter.drawLine( Pt::Math::Point( center.x(), center.y()-120 ),
                                    Pt::Math::Point( center.x(), center.y()-50 )); 

            painter.setPen( Pt::Gfx::Pen( lineWidth, Pt::Gfx::ARgbColor(0, 0xcccc, 0) ));
            painter.drawLine( Pt::Math::Point( center.x() - 50, center.y()-120 ),
                                    Pt::Math::Point( center.x(), center.y()-30 ));

            painter.setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor(0, 0, 0) ));
            painter.drawLine( Pt::Math::Point( center.x() - 50, center.y()-120 ),
                                    Pt::Math::Point( center.x(), center.y()-30 ));

            painter.setPen( Pt::Gfx::Pen( lineWidth, Pt::Gfx::ARgbColor(0, 0xcccc, 0) ));
            painter.drawLine( Pt::Math::Point( center.x() - 50, center.y()-70 ),
                                    Pt::Math::Point( center.x(), center.y()-20 ));

            painter.setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor(0, 0, 0) ));
            painter.drawLine( Pt::Math::Point( center.x() - 50, center.y()-70 ),
                                    Pt::Math::Point( center.x(), center.y()-20 ));

            painter.setPen( Pt::Gfx::Pen( lineWidth, Pt::Gfx::ARgbColor(0, 0xcccc, 0) ));
            painter.drawLine( Pt::Math::Point( center.x() - 50, center.y()-20 ),
                                    Pt::Math::Point( center.x(), center.y()-10 ));

            painter.setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor(0, 0, 0) ));
            painter.drawLine( Pt::Math::Point( center.x() - 50, center.y()-20 ),
                                    Pt::Math::Point( center.x(), center.y()-10 ));

            painter.setPen( Pt::Gfx::Pen( lineWidth, Pt::Gfx::ARgbColor(0, 0xcccc, 0) ));
            painter.drawLine( Pt::Math::Point( center.x() - 50, center.y() ), center);

            painter.setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor(0, 0, 0) ));
            painter.drawLine( Pt::Math::Point( center.x() - 50, center.y() ), center);

            painter.setPen( Pt::Gfx::Pen( lineWidth, Pt::Gfx::ARgbColor(0, 0xcccc, 0) ));
            painter.drawLine( Pt::Math::Point( center.x() - 50, center.y()+20 ),
                                    Pt::Math::Point( center.x(), center.y()+10 ));

            painter.setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor(0, 0, 0) ));
            painter.drawLine( Pt::Math::Point( center.x() - 50, center.y()+20 ),
                                    Pt::Math::Point( center.x(), center.y()+10 ));

            painter.setPen( Pt::Gfx::Pen( lineWidth, Pt::Gfx::ARgbColor(0, 0xcccc, 0) ));
            painter.drawLine( Pt::Math::Point( center.x() - 50, center.y()+70 ),
                                    Pt::Math::Point( center.x(), center.y()+20 ));

            painter.setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor(0, 0, 0) ));
            painter.drawLine( Pt::Math::Point( center.x() - 50, center.y()+70 ),
                                    Pt::Math::Point( center.x(), center.y()+20 ));

            painter.setPen( Pt::Gfx::Pen( lineWidth, Pt::Gfx::ARgbColor(0, 0xcccc, 0) ));
            painter.drawLine( Pt::Math::Point( center.x() - 50, center.y()+120 ),
                                    Pt::Math::Point( center.x(), center.y()+30 ));

            painter.setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor(0, 0, 0) ));
            painter.drawLine( Pt::Math::Point( center.x() - 50, center.y()+120 ),
                                    Pt::Math::Point( center.x(), center.y()+30 ));

           painter.setPen( Pt::Gfx::Pen( lineWidth, Pt::Gfx::ARgbColor(0, 0xcccc, 0) ));
            painter.drawLine( Pt::Math::Point( center.x(), center.y()+120 ),
                                    Pt::Math::Point( center.x(), center.y()+50 ));

            painter.setPen( Pt::Gfx::Pen( 1, Pt::Gfx::ARgbColor(0, 0, 0) ));
            painter.drawLine( Pt::Math::Point( center.x(), center.y()+120 ),
                                    Pt::Math::Point( center.x(), center.y()+50 ));
        }

        virtual void _paintEvent(const Pt::Gui::PaintEvent& event)
        {
            this->drawStar( _imagePainter, Pt::Math::Point(60, 150), 2);
            this->drawStar( _imagePainter, Pt::Math::Point(120, 150), 3);
            this->drawStar( _imagePainter, Pt::Math::Point(180, 150), 4);
            this->drawStar( _imagePainter, Pt::Math::Point(240, 150), 5);
            this->drawStar( _imagePainter, Pt::Math::Point(300, 150), 6);
            this->drawStar( _imagePainter, Pt::Math::Point(360, 150), 8);
            painter().drawImage( Pt::Math::Point( 0, 0 ), _image );

            Gui::Painter widgetPainter = painter();
            this->drawStar( widgetPainter, Pt::Math::Point(60, 450), 2);
            this->drawStar( widgetPainter,Pt::Math::Point(120, 450), 3);
            this->drawStar( widgetPainter,Pt::Math::Point(180, 450), 4);
            this->drawStar( widgetPainter,Pt::Math::Point(240, 450), 5);
            this->drawStar( widgetPainter,Pt::Math::Point(300, 450), 6);
            this->drawStar( widgetPainter,Pt::Math::Point(360, 450), 8);
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

        demo.resize(400, 600);
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
