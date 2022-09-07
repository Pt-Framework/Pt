#include "MainWindow.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/Shell.h>
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
        //app.setScaleFactor(1.25);

#define OLDAPP 1

#ifdef OLDAPP
        Pt::Hmi::Demo::MainWindow window;
        window.setTitle("Main_1");
        window.show();
        window.activate();
#else
        Pt::Hmi::Window w1;
        w1.setTitle("Window_1");
        w1.move( Pt::Gfx::PointF(50, 50) );
        w1.resize( Pt::Gfx::SizeF(200, 200) );
        w1.show();

        Pt::Hmi::PushButton button;
        button.setText("Quit");
        button.setPadding(10);
        button.clicked() += Pt::slot(app, &Pt::Hmi::Application::exit);

        Pt::Hmi::Shell shell;
        shell.addWindow(w1);

        Pt::Hmi::DockingLayout layout;
        layout.addItem(button, Pt::Hmi::DockingLayout::Top);
        layout.addItem(shell, Pt::Hmi::DockingLayout::Fill);

        Pt::Hmi::Window main;
        main.setContent(&layout);
        main.setTitle("Main Window");
        main.resize( Pt::Gfx::SizeF(500, 500) );
        main.move( Pt::Gfx::PointF(100, 50) );
        main.show();
        main.activate();
#endif        

        app.run();
    }
    catch(const std::exception& ex)
    {
        std::clog << "ERROR: " << ex.what() << std::endl;
    }
}
