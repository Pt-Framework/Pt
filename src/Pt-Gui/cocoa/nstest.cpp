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
    public:
        MyWidget( Pt::Math::Point at, Pt::Math::Size size)
        : Pt::Gui::Widget(at, size)
        {}
    
    protected:
        void _mouseEvent(const Pt::Gui::MouseEvent& ev)
        {
            Pt::Gui::Painter wp = this->painter();
            wp.setBrush( Pt::Gfx::Brush( Pt::Gfx::ARgbColor(0xeeee, 0, 0) ) );
            wp.fillRect( Pt::Math::Rect(Pt::Math::Point(ev.x(), ev.y()), Pt::Math::Size(4, 4)) );
        }
};

int main( int argc, const char* argv[])
{
    Pt::Gui::Application app;

    MyWidget widget( Pt::Math::Point(50, 400), Pt::Math::Size(300, 200) );
    widget.setTitle(L"NSTest");
    connect(widget.closed, app, &Pt::Gui::Application::exit);
    widget.show();
    
    app.run();
    return 0;
}
