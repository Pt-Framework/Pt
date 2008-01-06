#include "Pt/Gui/Application.h"
#include "Pt/Gui/Widget.h"
#include <iostream>

int main( int argc, const char* argv[])
{
    std::cerr << "Started" << std::endl;
    Pt::Gui::Application app;

    Pt::Gui::Widget widget( Pt::Math::Point(50, 400), Pt::Math::Size(300, 200) );
    widget.show();

    app.run();
    return 0;
}
