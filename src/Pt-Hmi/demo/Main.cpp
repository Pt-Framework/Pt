#include "MainWindow.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Main.h>

int main(int argc, char* args[])
{	
	try
	{
		Pt::Hmi::Application	application;
		Pt::Hmi::WindowModel _model;
		Pt::Hmi::Demo::MainWindow	mainWindow(&_model);
	
		mainWindow.show();

		application.showConsole();
		application.run();
	}
	catch(const std::exception& ex)
	{
		std::clog << "ERROR: " << ex.what() << std::endl;
	}
}

