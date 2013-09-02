#include "MainWindow.h"
#include <Pt/Hmi/Application.h>

int main(int argc, char* args[])
{	
	Pt::Hmi::Application		application;
	Pt::Hmi::Demo::MainWindow	mainWindow;
	
	mainWindow.show();

	application.showConsole();
    application.run();
}

