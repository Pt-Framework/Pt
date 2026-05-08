#include "MainWindow.h"
#include <Pt/Forms/Application.h>
#include <Pt/Forms/Workspace.h>
#include <Pt/Gfx/Point.h>
#include <Pt/System/Logger.h>
#include <Pt/System/Timer.h>
#include <Pt/Main.h>

#include <Pt/Forms/CanvasLayout.h>
#include <Pt/Forms/Label.h>
#include <Pt/Forms/Panel.h>
#include <Pt/SmartPtr.h>

void fullApp(Pt::Forms::Application& app)
{
    // on the heap for emscripten, because side modules have separate stacks
    Pt::SmartPtr<Pt::Forms::Demo::MainWindow> window(new Pt::Forms::Demo::MainWindow);

    window->setTitle("Pt Forms Demo");
    window->show();
    window->activate();
    
    app.run();
}


class SimpleApp : public Pt::Connectable
{
    private:
        Pt::Forms::Window w1;
        Pt::Forms::Label label;
        Pt::Forms::PushButton button;
        Pt::Forms::Window main;
        Pt::System::Timer autoCenter;

        void onAutoLayout()
        {
            static int nnn = 150;
            button.setMinimumHeight(nnn);
            nnn += 20;
            if(nnn > 250)
                nnn = 150;

            w1.autoSize();
            w1.autoCenter();
            //w1.move( Pt::Gfx::PointF(50, 50) );

            main.autoSize();
        }

        void onTimer()
        {
            std::clog << "\n--- RUNNING AUTO_CENTER ---" << std::endl;
            
            main.resize( Pt::Gfx::SizeF(800, 500) );
            main.autoCenter();

            autoCenter.stop();
        }

    public:
        void run(Pt::Forms::Application& app)
        {
            label.setName("Label");
            //label.setText("Hello World!");
            label.setAlignment(Pt::Forms::Alignment::Center);
            //label.setMinimumSize(Pt::Gfx::SizeF(180, 120) );

            Pt::System::Path appdir( Pt::Forms::Application::instance().argv()[0] );
            appdir = appdir.dirName();

            Pt::Gfx::Image image;
            app.loadImage(appdir / "platinum.png", image);
            //app.loadImage(appdir / "test-64x64.png", image);

            Pt::Forms::Icon labelIcon;
            labelIcon.addImage(image);

            Pt::Gfx::SizeF size = Pt::Gfx::SizeF( static_cast<double>(image.width()), 
                                                  static_cast<double>(image.height()) );
            label.setIcon(labelIcon, size);

            w1.setTitle("Window_1");
            w1.setName("Window_1");
            w1.setContent(&label);
        
            //w1.move( Pt::Gfx::PointF(50, 50) );
            //w1.resize( Pt::Gfx::SizeF(200, 200) );
    
            w1.autoCenter(); 
            w1.autoSize();
            //w1.show();

            Pt::Gfx::Image buttonImage;
            app.loadImage(appdir / "test-64x64.png", buttonImage);

            Pt::Forms::Icon buttonIcon;
            buttonIcon.addImage(buttonImage);

            Pt::Gfx::SizeF buttonSize = Pt::Gfx::SizeF( static_cast<double>(buttonImage.width()), 
                                                        static_cast<double>(buttonImage.height()) );
            button.setIcon(buttonIcon, buttonSize);
            button.setLayout(Pt::Forms::PushButton::Top);
            button.setText("AUTO LAYOUT");
            button.setPadding(10);
            button.setMargin(10);
            button.clicked() += Pt::slot(*this, &SimpleApp::onAutoLayout);

            Pt::Forms::Workspace workspace;
            workspace.setName("Workspace");
            workspace.setMinimumSize(Pt::Gfx::SizeF(500, 400) );
            workspace.addWindow(w1);

            w1.move( Pt::Gfx::PointF(50, 50) );
            w1.autoCenter(); 
            w1.show();

            Pt::Forms::DockingLayout layout;
            layout.setName("DockingLayout");
            layout.addItem(button, Pt::Forms::DockingLayout::Bottom);
            layout.addItem(workspace, Pt::Forms::DockingLayout::Fill);

            main.setContent(&layout);
            main.setTitle("Main Window");
            main.setName("Main");
        
            main.autoSize();
            main.autoCenter();

            main.closed() += Pt::slot(app, &Pt::Forms::Application::exit);

            //main.resize( Pt::Gfx::SizeF(500, 500) );
            //main.move( Pt::Gfx::PointF(100, 100) );
    
            main.show();
            main.activate();

            autoCenter.setActive( app.loop() );
            autoCenter.timeout() += Pt::slot(*this, &SimpleApp::onTimer);
            //autoCenter.start(3000);

            std::clog << "\n--- RUNNING APPLICATION ---" << std::endl;
            app.run();
        }
};


void staticApp(Pt::Forms::Application& app)
{
    Pt::Forms::Label label;
    label.setName("Label");
    label.setContour( Pt::Gfx::Color(0, 0, 0) );
    label.setText("Hello World!");
    label.setAlignment(Pt::Forms::Alignment::Center);

    Pt::Forms::PushButton button;
    button.setName("Button");
    button.setText("Quit");
    button.clicked() += Pt::slot(app, &Pt::Forms::Application::exit);

    Pt::Forms::CanvasLayout canvas;
    canvas.setName("CanvasLayout");
    canvas.setPadding(10);
    canvas.addItem( label, Pt::Gfx::PointF(0, 0), 
                           Pt::Gfx::SizeF(320, 180) );
    canvas.addItem( button, Pt::Gfx::PointF(120, 190), 
                            Pt::Gfx::SizeF(80, 30) );

    Pt::Forms::Window main;
    main.setName("Main");
    main.setContent(&canvas);
    main.setTitle("Static Layout");
    main.autoSize();
    main.autoCenter();
    
    main.show();
    main.activate();

    app.run();
}


int main(int argc, char* args[])
{
    try
    {
        Pt::System::Logger::setLogLevel( "Pt.Forms", Pt::System::Info );

        Pt::Forms::Application app(argc, args);
        app.setScaleFactor(1);

        fullApp(app);
        
        //SimpleApp simpleApp;
        //simpleApp.run(app);
        
        //staticApp(app);
    }
    catch(const std::exception& ex)
    {
        std::clog << "ERROR: " << ex.what() << std::endl;
    }
}
