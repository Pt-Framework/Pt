
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/System/MainLoop.h"
#include "Pt/System/Clock.h"
#include "Pt/Timespan.h"
#include "Pt/Allocator.h"

#include <Pt/Hmi/Application.h>
//Devices
#include <Pt/Hmi/Desktop/Window.h>


int main(int argc, char* args[])
{

	Pt::Hmi::Application	  app;
	
	app.setResolution(1/96.0);

	
	Pt::Hmi::Desktop::Window	window;	

	window.show();
	app.run();		
}


