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

        Pt::Hmi::Application app;
        app.screen().setScaleFactor(1.4);

        Pt::Hmi::Demo::MainWindow    mainWindow1;
        mainWindow1.setTitle("Main 1");
        mainWindow1.show();
        mainWindow1.activate();

        app.run();
    }
    catch(const std::exception& ex)
    {
        std::clog << "ERROR: " << ex.what() << std::endl;
    }
}

