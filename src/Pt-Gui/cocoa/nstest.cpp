#include "Pt/Gui/Application.h"
#include "Pt/Gui/Widget.h"
#include "Pt/Gui/Painter.h"
#include <iostream>

int main( int argc, const char* argv[])
{
    std::cerr << "Started" << std::endl;
    Pt::Gui::Application app;

    Pt::Gui::Widget widget( Pt::Math::Point(50, 400), Pt::Math::Size(300, 200) );
    widget.setTitle(L"NSTest");
    widget.show();

    widget.painter().drawLine( Pt::Math::Point(10, 10), Pt::Math::Point(100, 100) );
    app.run();
    return 0;
}
