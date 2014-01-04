/*
 * Copyright (C) 2006-2007 Marc Boris Duerner
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
#include <Pt/Gfx/Point.h>
#include <Pt/System/Clock.h>
#include <Pt/System/Thread.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/Gfx/Font.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gui/Application.h>
#include <Pt/Gui/Painter.h>
#include <Pt/Gui/Widget.h>
#include <Pt/Gui/Pixmap.h>
#include <Pt/Gui/ResizeEvent.h>
#include <Pt/Gui/PaintEvent.h>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <typeinfo>


class UpdateEvent : public Pt::Event
{
    public:
        UpdateEvent()
        {}

        virtual const std::type_info& typeInfo() const
        { return TypeInfo; }

        virtual Event* clone() const
        { return new UpdateEvent(*this); }

        static const std::type_info& TypeInfo;
};

const std::type_info& UpdateEvent::TypeInfo = typeid(UpdateEvent);


class UpdateThread : public Pt::System::Thread, public Pt::Connectable
{
    public:
        UpdateThread()
        : _stop(false)
        {
        }

        void stop()
        { _stop = true; }

        Pt::Signal<const Pt::Event&> nextEvent;

    protected:
        virtual void run()
        {
            _stop = false;

            while(!_stop)
            {
                Thread::sleep(50);
                nextEvent.send( UpdateEvent() );
            }
        }

    private:
        bool _stop;
};

class DrawTextDemo : public Pt::Gui::Widget
{
    public:
    DrawTextDemo()
    : _image()
    , _pixmap(0)
    , _imagePainter( _image )
    , _angle(0)
    , _tickerText( L"+++ Can bees think? A new study confirms: No, they can not! +++")
    , _tickerTextPos(400)
    , _tickerTextWidth(0)
    {
        this->setTitle(L"DrawTextDemo");
        _imagePainter.setFont( Pt::Gfx::Font("Vera", 28) );
        _tickerTextWidth =  _imagePainter.fontMetrics( _tickerText ).width();

        _pixmap = new Pt::Gui::Pixmap(10, 10);
    }

    virtual ~DrawTextDemo()
    {}

    virtual void _paintEvent(const Pt::Gui::PaintEvent& event)
    {
        this->painter().drawPixmap(Pt::Gfx::Point(0,0), *_pixmap);
    }

    virtual void _resizeEvent(const Pt::Gui::ResizeEvent& event)
    {
        delete _pixmap;
        _pixmap = new Pt::Gui::Pixmap( event.width(), event.height() );
        _image.resize(  event.width(), event.height(), Pt::Gfx::ARgbColor( 0, 0, 0xcccc ) );

        Pt::Gfx::Brush blueBrush( Pt::Gfx::ARgbColor(0,0, 0xcccc) );
        _pixmap->painter().setBrush( blueBrush );
        _pixmap->painter().fillRect( Pt::Gfx::Rect( Pt::Gfx::Point(0,0), this->size() ) );
    }

    void nextFrame(const Pt::Event& event)
    {
        Pt::String text = L"P T V";
        _angle += 10;
        //Pt::System::TimeValue time;
        //Pt::System::Clock clock;
        //clock.start();

        Pt::Gfx::Brush blueBrush( Pt::Gfx::ARgbColor(0,0, 0xcccc) );
        _imagePainter.setBrush( blueBrush );
        _imagePainter.fillRect( Pt::Gfx::Rect(Pt::Gfx::Point(80, 0), Pt::Gfx::Size(280, 260)) );

        Pt::Gfx::ARgbColor yellow( 0xcccc, 0xbbbb, 0x0 );
        _imagePainter.setFont( Pt::Gfx::Font("Vera", 42 ,Pt::Gfx::Font::NormalStyle, _angle ) );
        _imagePainter.drawText( Pt::Gfx::Point(200, 135), text, &yellow );

        Pt::Gfx::Brush redBrush( Pt::Gfx::ARgbColor(0xdddd,0, 0) );
        _imagePainter.setBrush( redBrush );

        _imagePainter.fillRect( Pt::Gfx::Rect( Pt::Gfx::Point( 0, this->size().height() - 40 ),
                                                Pt::Gfx::Size( this->size().width(), 40) ) );

        if( --_tickerTextPos < -_tickerTextWidth )
        {
            _tickerTextPos = this->size().width();
        }

        Pt::Gfx::ARgbColor white( 0xffff, 0xffff, 0xffff );
        _imagePainter.setFont( Pt::Gfx::Font("Vera", 28) );
        _imagePainter.drawText( Pt::Gfx::Point( _tickerTextPos, this->size().height()-10 ), _tickerText, &white );

        _pixmap->painter().drawImage( Pt::Gfx::Point( 0, 0 ),
                                      _image,
                                      Pt::Gfx::Region( Pt::Gfx::Point(80,0),
                                                       Pt::Gfx::Size(280, 260) ) );

        _pixmap->painter().drawImage( Pt::Gfx::Point( 0, this->size().height() - 40 ),
                                      _image,
                                      Pt::Gfx::Region(  Pt::Gfx::Point( 0, this->size().height() - 40 ),
                                                        Pt::Gfx::Size(this->size().width(), 40) ) );

        this->painter().drawPixmap(Pt::Gfx::Point( 0, 0 ), *_pixmap);
        //time = clock.stop();
        //std::cerr << "Time per frame: " << time.seconds() + time.microSeconds() / 1000000.0 << " seconds" << std::endl;
    }

private:
    Pt::Gfx::ARgbImage _image;
    Pt::Gui::Pixmap* _pixmap;
    Pt::Gfx::ImagePainter _imagePainter;
    Pt::ssize_t _angle;
    Pt::String _tickerText;
    Pt::ssize_t _tickerTextPos;
    Pt::ssize_t _tickerTextWidth;
};


int main( int argc, char* argv[] )
{
    try
    {
        Pt::Gui::Application app;

        UpdateThread thread;
        connect( thread.nextEvent, app, &Pt::Gui::Application::commitEvent);

        thread.start();

        DrawTextDemo demo;
        connect( demo.closed, thread, &UpdateThread::stop );
        connect( demo.closed, app, &Pt::Gui::Application::exit );
        connect( app.event, demo, &DrawTextDemo::nextFrame);

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
