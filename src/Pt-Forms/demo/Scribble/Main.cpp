#include "MainWindow.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Main.h>

int main(int argc, char* args[])
{	
	Pt::Hmi::Application		application;
	Pt::Forms::Demo::MainWindow	mainWindow;
	
	mainWindow.show();

	application.showConsole();
    application.run();
}

