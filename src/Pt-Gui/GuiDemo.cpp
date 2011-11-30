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
#include <Pt/Gui/Panel.h>
#include <Pt/Gui/SimpleGridLayout.h>
#include <Pt/Gui/VerticalLayout.h>
#include <Pt/Gui/ProgressBar.h>
#include <Pt/Gfx/Pen.h>
#include <Pt/Gfx/Brush.h>
#include <string>
#include <iostream>

using namespace Pt;
using namespace Pt::Gui;
using namespace Pt::Gfx;

class MyApplication : public Pt::Gui::Application {
    public:
        MyApplication(ProgressBar& pbar)
        : _pbar(pbar)
        {}

        void dec()
        {
            _pbar.setValue(_pbar.value() - 1);
            _pbar.update();
        }

        void inc()
        {
            _pbar.setValue(_pbar.value() + 1);
            _pbar.update();
        }

    private:
        ProgressBar& _pbar;
};

int main(int argc, char* argv[])
{
    try
    {
        Widget mainWidget(Point(100, 100), Size(400, 300));
        Panel  mainPanel(mainWidget, Point(0, 0), Size(400, 300));

            Widget pbarWidget(mainPanel, Point(0, 0), Size(400, 100));
                ProgressBar pbar(pbarWidget,  Point(0, 0), Size(400, 100));
                pbar.setMinimum(0);
                pbar.setMaximum(10);

            Widget ltbnWidget(mainPanel, Point(0, 0), Size(400, 200));
                Label  label1 (ltbnWidget, Point(0, 0), Size(1, 1), Pt::String::widen("Label 1" ));
                Label  label2 (ltbnWidget, Point(0, 0), Size(1, 1), Pt::String::widen("Label 2" ));
                Button button1(ltbnWidget, Point(0, 0), Size(1, 1), Pt::String::widen("Button 1"));
                Button button2(ltbnWidget, Point(0, 0), Size(1, 1), Pt::String::widen("Button 2"));

        VerticalLayout vertLayout = VerticalLayout::create(mainPanel);
            vertLayout.set(ltbnWidget, VerticalLayout::Grab, Margin(0, 0, 0, 0));
            vertLayout.set(pbarWidget, VerticalLayout::Grab, Margin(10, 10, 10, 10));
        vertLayout.update();

        SimpleGridLayout gridLayout = SimpleGridLayout::create(ltbnWidget, 2, 2, 0, 0);
            gridLayout.setLayoutData(label1,  SimpleGridLayoutData(0, 0));
            gridLayout.setLayoutData(label2,  SimpleGridLayoutData(0, 1));
            gridLayout.setLayoutData(button1, SimpleGridLayoutData(1, 0));
            gridLayout.setLayoutData(button2, SimpleGridLayoutData(1, 1));
        gridLayout.update();

        MyApplication app(pbar);
        connect(button1.clicked,   app, &MyApplication::dec);
        connect(button2.clicked,   app, &MyApplication::inc);
        connect(mainWidget.closed, app, &MyApplication::exit);
        mainWidget.show();
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
