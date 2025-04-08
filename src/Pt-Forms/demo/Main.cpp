#include "MainWindow.h"
#include <Pt/Forms/Application.h>
#include <Pt/Forms/Workspace.h>
#include <Pt/Gfx/Point.h>
#include <Pt/System/Logger.h>
#include <Pt/Main.h>

#include <Pt/Forms/TableLayout.h>
#include <Pt/Forms/Label.h>

int main(int argc, char* args[])
{
    try
    {
        Pt::System::Logger::setLogLevel( "Pt.Forms", Pt::System::Debug );

        Pt::Forms::Application app(argc, args);
        app.setScaleFactor(1);

#define OLDAPP 1

#ifdef OLDAPP
        Pt::Forms::Demo::MainWindow window;
        window.setTitle("Pt Forms Demo");
        window.show();
        window.activate();
#else
        Pt::Forms::Window w1;
        w1.setTitle("Window_1");
        w1.move( Pt::Gfx::PointF(50, 50) );
        w1.resize( Pt::Gfx::SizeF(200, 200) );
        w1.show();

        Pt::Forms::PushButton button;
        button.setText("Quit");
        button.setPadding(10);
        button.clicked() += Pt::slot(app, &Pt::Forms::Application::exit);

        Pt::Forms::Workspace workspace;
        workspace.addWindow(w1);

        Pt::Forms::DockingLayout layout;
        layout.addItem(button, Pt::Forms::DockingLayout::Top);
        layout.addItem(workspace, Pt::Forms::DockingLayout::Fill);

        Pt::Forms::Window main;
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
