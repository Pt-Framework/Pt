/***************************************************************************
 *   Copyright (C) 2005-2007 by Marc Boris Duerner                         *
 *   Copyright (C) 2005-2007 by Aloysius Indrayanto                        *
 *   Copyright (C) 2005-2007 by Sebastian Pieck                            *
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

#include <gf/gf.h>
#include <stdio.h>


class DemoWidget : public Pt::Gui::Widget
{
    public:
        DemoWidget()
        : _pos(0, 0)
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

public:
        void erase()
        {
            // erase old "sprite"
            Pt::Gfx::ARgbImage image(800, 480, Pt::Gfx::ARgbColor(0, 0, 0) );
            Pt::Gfx::ImagePainter imagePainter(image);
            Pt::Gui::Painter painter = this->painter();
            painter.drawImage(_pos, image);
        }

        void refresh()
        {
            Pt::Gfx::ARgbColor white(0xffff, 0xffff, 0xffff);
            Pt::Gfx::Font font24("Vera", 24);
            Pt::Gfx::ARgbImage image(800 , 480, Pt::Gfx::ARgbColor(0xffff, 0, 0) );

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
