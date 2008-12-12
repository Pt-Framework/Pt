/*
 * Copyright (C) 2005-2007 by Marc Boris Duerner
 * Copyright (C) 2005-2007 by Aloysius Indrayanto
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
#include "PainterImpl.h"
#include "WidgetImpl.h"
#include "ApplicationImpl.h"

#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Gfx/ImagePainter.h>
#include <Pt/Gui/Painter.h>
#include <Pt/Gui/Widget.h>
#include <Pt/Gui/KeyEvent.h>
#include <Pt/System/Thread.h>

#include <iostream>

class DemoWidget : public Pt::Gui::Widget
{
    public:
        DemoWidget()
        : _pos(50, 50)
        {}

        Pt::Signal<> escape;

    protected:
        void _paintEvent(const Pt::Gui::PaintEvent& pev)
        {
            this->refresh();
        }

        void _keyEvent(const Pt::Gui::KeyEvent& kev)
        {
            if(kev.type() == Pt::Gui::KeyEvent::Release)
                return;

            switch( kev.code() )
            {
                case Pt::Gui::KeyEvent::Left:
                    this->erase();
                    _pos = Pt::Math::Point(_pos.x() - 10, _pos. y());
                    this->refresh();
                    break;
                case Pt::Gui::KeyEvent::Right:
                    this->erase();
                    _pos = Pt::Math::Point(_pos.x() + 10, _pos. y());
                    this->refresh();
                    break;
                case Pt::Gui::KeyEvent::Up:
                    this->erase();
                    _pos = Pt::Math::Point(_pos.x(), _pos. y() -10);
                    this->refresh();
                    break;
                case Pt::Gui::KeyEvent::Down:
                    this->erase();
                    _pos = Pt::Math::Point(_pos.x(), _pos. y()+ 10);
                    this->refresh();
                    break;

                case Pt::Gui::KeyEvent::Escape:
                    escape.send();
                    break;

                default:
                    break;
	    }
        }

        void erase()
        {
            // erase old "sprite"
            Pt::Gfx::ARgbImage image(220, 50, Pt::Gfx::ARgbColor(0, 0, 0) );
            Pt::Gfx::ImagePainter imagePainter(image);
            Pt::Gui::Painter painter = this->painter();
            painter.drawImage(_pos, image);
        }

        void refresh()
        {
            Pt::Gfx::ARgbColor white(0xffff, 0xffff, 0xffff);
            Pt::Gfx::Font font24("Vera", 24);
            Pt::Gfx::ARgbImage image(220, 50, Pt::Gfx::ARgbColor(0xffff, 0, 0) );

            Pt::Gfx::ImagePainter imagePainter(image);
            imagePainter.setFont(font24);
            imagePainter.drawText(Pt::Math::Point(32, 33), L"Hello World!", &white);

            Pt::Gui::Painter painter = this->painter();
            painter.drawImage(_pos, image);
            Pt::System::Thread::sleep(10);
        }

    private:
        Pt::Math::Point _pos;
};


int main(int argc, char** argv)
{
    Pt::Gui::Application app;

    DemoWidget widget;
    connect(widget.closed, app, &Pt::Gui::Application::exit);
    connect(widget.escape, app, &Pt::Gui::Application::exit);

    widget.show();
    return app.run();
}
