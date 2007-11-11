#include <Pt/Main.h>
#include <Pt/Math/Point.h>
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
        
        {
        	Pt::Gui::Painter painter = widget.painter();
        	painter.drawLine( Pt::Math::Point(10,10), Pt::Math::Point(100,100) );
        }
        return app.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
