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

        Pt::Hmi::Application    application;
        
        Pt::Hmi::Demo::MainWindow    mainWindow1;
        mainWindow1.setTitle("Main 1");
        mainWindow1.show();
        mainWindow1.activate();

        //Pt::Hmi::Label label1;
        //label1.setText("Label 1");
        //label1.setAlignment(Pt::Hmi::Alignment::Center);
        //label1.setAutoSize(true);

        //Pt::Hmi::Label label2;
        //label2.setText("Label 2");
        //label2.setAlignment(Pt::Hmi::Alignment::Center);
        //label2.setAutoSize(true);

        //Pt::Hmi::Label label3;
        //label3.setText("Label 3");
        //label3.setAlignment(Pt::Hmi::Alignment::Center);
        //label3.setAutoSize(true);

        //Pt::Hmi::Label label4;
        //label4.setText("Label 4");
        //label4.setAlignment(Pt::Hmi::Alignment::Center);
        //label4.setAutoSize(true);

        //Pt::Hmi::TableLayout tableLayout;
        //tableLayout.addItem(label1, 0, 0);
        //tableLayout.addItem(label2, 0, 1);
        //tableLayout.addItem(label3, 1, 0);
        //tableLayout.addItem(label4, 1, 1);

        //tableLayout.setColumn(0, Pt::Hmi::TableLayout::Fill);
        //tableLayout.setRow(0, Pt::Hmi::TableLayout::Fill);       

        //Pt::Hmi::Window tableWindow;
        //tableWindow.setMainWidget(&tableLayout);
        //tableWindow.resize( Pt::Gfx::SizeF(300, 200) );
        //tableWindow.show();

        application.run();
    }
    catch(const std::exception& ex)
    {
        std::clog << "ERROR: " << ex.what() << std::endl;
    }
}

