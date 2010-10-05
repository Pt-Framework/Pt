/*
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
#include "Pt/Gui/Application.h"
#include "Pt/Gui/Widget.h"
#include "Pt/Gui/MouseEvent.h"
#include "Pt/Gui/Pixmap.h"
#include "Pt/Gui/Painter.h"
#include "Pt/Gfx/Pen.h"
#include "Pt/Gfx/Brush.h"
#include <iostream>

class MyWidget : public Pt::Gui::Widget
{
    Pt::Gui::Pixmap pm;

    public:
        MyWidget( Pt::Gfx::Point at, Pt::Gfx::Size size)
        : Pt::Gui::Widget(at, size)
        , pm(100, 100)
        {

            //{
                Pt::Gui::Painter pp = pm.painter();
                pp.setBrush( Pt::Gfx::Brush( Pt::Gfx::ARgbColor(0, 0, 0xffff) ) );
                pp.fillRect( Pt::Gfx::Rect(Pt::Gfx::Point(0, 0), Pt::Gfx::Size(100, 100)) );
            //}
        }

    protected:
        void _mouseEvent(const Pt::Gui::MouseEvent& ev)
        {
            //Pt::Gui::Pixmap pm(100, 100);
            //{
            //    Pt::Gui::Painter pp = pm.painter();
            //    pp.setBrush( Pt::Gfx::Brush( Pt::Gfx::ARgbColor(0, 0, 0xffff) ) );
            //    pp.fillRect( Pt::Gfx::Rect(Pt::Gfx::Point(0, 0), Pt::Gfx::Size(100, 100)) );
            //}
            Pt::Gui::Painter wp = this->painter();
            /*wp.setBrush( Pt::Gfx::Brush( Pt::Gfx::ARgbColor(0xeeee, 0, 0) ) );
            wp.fillRect( Pt::Gfx::Rect(Pt::Gfx::Point(ev.x(), ev.y()), Pt::Gfx::Size(4, 4)) );*/
            wp.drawPixmap(Pt::Gfx::Point(0,0), pm);
        }
};

int main( int argc, const char* argv[])
{
    Pt::Gui::Application app;

    MyWidget widget( Pt::Gfx::Point(50, 400), Pt::Gfx::Size(300, 200) );
    //Pt::Gui::Widget child(widget, Pt::Gfx::Point(0, 0), Pt::Gfx::Size(80, 80) );

    widget.setTitle(L"NSTest");
    connect(widget.closed, app, &Pt::Gui::Application::exit);
    widget.show();

    app.run();
    return 0;
}
