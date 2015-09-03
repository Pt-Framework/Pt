#include "MainWindow.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Main.h>
#include <Pt/Ui/Point.h>
#include <Pt/System/Logger.h>
#include <Pt/Ui/ClipPolygon.h>

int main(int argc, char* args[])
{	
	try
	{
		Pt::System::Logger::setLogLevel( "Pt.Hmi", Pt::System::Debug );

		Pt::Hmi::Application	application;
		Pt::Hmi::Demo::MainWindow	mainWindow1;
		Pt::Hmi::Demo::MainWindow	mainWindow2;
	
		mainWindow1.Caption = "Main 1";
		mainWindow2.Caption = "Main 2";

		mainWindow1.show();
		mainWindow2.show();

		application.run();
	}
	catch(const std::exception& ex)
	{
		std::clog << "ERROR: " << ex.what() << std::endl;
	}
}

