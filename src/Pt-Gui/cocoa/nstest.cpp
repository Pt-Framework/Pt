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
        MyWidget( Pt::Math::Point at, Pt::Math::Size size)
        : Pt::Gui::Widget(at, size)
        , pm(100, 100)
        {

            //{
                Pt::Gui::Painter pp = pm.painter();
                pp.setBrush( Pt::Gfx::Brush( Pt::Gfx::ARgbColor(0, 0, 0xffff) ) );
                pp.fillRect( Pt::Math::Rect(Pt::Math::Point(0, 0), Pt::Math::Size(100, 100)) );
            //}
        }
    
    protected:
        void _mouseEvent(const Pt::Gui::MouseEvent& ev)
        {
            //Pt::Gui::Pixmap pm(100, 100);
            //{
            //    Pt::Gui::Painter pp = pm.painter();
            //    pp.setBrush( Pt::Gfx::Brush( Pt::Gfx::ARgbColor(0, 0, 0xffff) ) );
            //    pp.fillRect( Pt::Math::Rect(Pt::Math::Point(0, 0), Pt::Math::Size(100, 100)) );
            //}
            Pt::Gui::Painter wp = this->painter();
            /*wp.setBrush( Pt::Gfx::Brush( Pt::Gfx::ARgbColor(0xeeee, 0, 0) ) );
            wp.fillRect( Pt::Math::Rect(Pt::Math::Point(ev.x(), ev.y()), Pt::Math::Size(4, 4)) );*/
            wp.drawPixmap(Pt::Math::Point(0,0), pm);
        }
};

int main( int argc, const char* argv[])
{
    Pt::Gui::Application app;

    MyWidget widget( Pt::Math::Point(50, 400), Pt::Math::Size(300, 200) );
    //Pt::Gui::Widget child(widget, Pt::Math::Point(0, 0), Pt::Math::Size(80, 80) );
            
    widget.setTitle(L"NSTest");
    connect(widget.closed, app, &Pt::Gui::Application::exit);
    widget.show();
    
    app.run();
    return 0;
}
