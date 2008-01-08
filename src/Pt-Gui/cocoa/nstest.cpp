#include "Pt/Gui/Application.h"
#include "Pt/Gui/Widget.h"
#include "Pt/Gui/Painter.h"
#include "Pt/Gfx/Pen.h"
#include <iostream>

int main( int argc, const char* argv[])
{
    Pt::Gui::Application app;

    Pt::Gui::Widget widget( Pt::Math::Point(50, 400), Pt::Math::Size(300, 200) );
    widget.setTitle(L"NSTest");
    widget.show();

    {
        Pt::Gui::Painter wp = widget.painter();
        wp.setPen( Pt::Gfx::Pen(5) );
        wp.drawLine( Pt::Math::Point(10, 10), Pt::Math::Point(100, 100) );
        
        wp.setPen( Pt::Gfx::Pen(2) );
        wp.drawLine( Pt::Math::Point(20, 10), Pt::Math::Point(90, 110) );
    }
    
    app.run();
    return 0;
}
