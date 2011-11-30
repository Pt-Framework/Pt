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
#include <Pt/Main.h>
#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Rect.h>
#include <Pt/Gui/Application.h>
#include <Pt/Gui/Widget.h>
#include <Pt/Gui/Button.h>
#include <Pt/Gui/Pixmap.h>
#include <Pt/Gui/MouseEvent.h>
#include <Pt/Gui/MouseMoveEvent.h>
#include <Pt/Gui/ResizeEvent.h>
#include <Pt/Gui/Painter.h>
#include <Pt/Gui/PaintEvent.h>
#include <Pt/Gui/KeyEvent.h>
#include <Pt/Gui/Label.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <string>
#include <iostream>


int main(int argc, char* argv[])
{
    try
    {
        Pt::Gui::Application app;

        Pt::Gui::Widget widget;
        connect(widget.closed, app, &Pt::Gui::Application::exit);

        widget.show();

        Pt::Gui::Pixmap pm(100, 100);
        {
            Pt::Gui::Painter painter = pm.painter();
            painter.setBrush(Pt::Gfx::Brush( Pt::Gfx::ARgbColor(0, 0, 0) ) );
            painter.fillRect( Pt::Gfx::Rect( Pt::Gfx::Point(0, 0), Pt::Gfx::Size(100,100) ) );
            painter.setBrush(Pt::Gfx::Brush( Pt::Gfx::ARgbColor(0xffff, 0, 0) ) );
            painter.fillRect( Pt::Gfx::Rect( Pt::Gfx::Point(20, 20), Pt::Gfx::Size(100,100) ) );
            painter.setPen( Pt::Gfx::Pen(Pt::Gfx::ARgbColor(0, 0, 0xffff)) );
            painter.drawLine( Pt::Gfx::Point(10,20), Pt::Gfx::Point(100,120) );
        }

        {
            Pt::Gui::Painter painter = widget.painter();
            painter.setBrush(Pt::Gfx::Brush( Pt::Gfx::ARgbColor(0, 0, 0) ) );
            painter.fillRect( Pt::Gfx::Rect( Pt::Gfx::Point(0, 0), Pt::Gfx::Size(widget.width(),widget.height()) ) );
            painter.drawPixmap( Pt::Gfx::Point(10, 10), pm );
            painter.setPen( Pt::Gfx::Pen(Pt::Gfx::ARgbColor(0, 0, 0xffff)) );
            painter.drawLine( Pt::Gfx::Point(10,10), Pt::Gfx::Point(200,100) );
        }

        app.run();
        return 0;
    }
    catch(const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
