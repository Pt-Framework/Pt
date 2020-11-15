#include "MainWindow.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Gfx/Point.h>
#include <Pt/System/Logger.h>
#include <Pt/Main.h>

#include <Pt/Hmi/TableLayout.h>
#include <Pt/Hmi/Label.h>

int main(int argc, char* args[])
{
    try
    {
        Pt::System::Logger::setLogLevel( "Pt.Hmi", Pt::System::Debug );

        Pt::Hmi::Application app(argc, args);
        app.setScaleFactor(1.25);

        Pt::Hmi::Demo::MainWindow window;
        window.setTitle("Main 1");
        window.show();
        window.activate();

        app.run();
    }
    catch(const std::exception& ex)
    {
        std::clog << "ERROR: " << ex.what() << std::endl;
    }
}
