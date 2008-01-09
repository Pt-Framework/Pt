#include "Pt/Gui/Application.h"
#include "Pt/Gui/Widget.h"
#include "Pt/Gui/Pixmap.h"
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
        wp.drawLine( Pt::Math::Point(10, 10), Pt::Math::Point(200, 200) );
        
        wp.setPen( Pt::Gfx::Pen(2) );
        wp.drawLine( Pt::Math::Point(20, 10), Pt::Math::Point(90, 150) );
    }
    
    Pt::Gui::Pixmap pm(100, 100);
    {
        Pt::Gui::Painter pp = pm.painter();
        pp.setPen( Pt::Gfx::Pen(10) );
        pp.drawLine( Pt::Math::Point(10, 10), Pt::Math::Point(300, 100) );
    }
    
    {
        Pt::Gui::Painter wp = widget.painter();
        wp.drawPixmap(Pt::Math::Point(0,0), pm);
    }  
    
    app.run();
    return 0;
}
