#include "MainWindow.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Main.h>
#include <Pt/Gfx/Point.h>
#include <Pt/System/Logger.h>
#include <Pt/Gfx/ClipPolygon.h>

int main(int argc, char* args[])
{	
	try
	{
		Pt::System::Logger::setLogLevel( "Pt.Hmi", Pt::System::Debug );

		Pt::Hmi::Application	application;
		Pt::Hmi::Demo::MainWindow	mainWindow1;
	
		mainWindow1.setTitle("Main 1");
		mainWindow1.show();
		        
		application.run();
	}
	catch(const std::exception& ex)
	{
		std::clog << "ERROR: " << ex.what() << std::endl;
	}
}

