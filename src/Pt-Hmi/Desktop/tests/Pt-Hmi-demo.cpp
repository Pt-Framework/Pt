
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/System/MainLoop.h"
#include "Pt/System/Clock.h"
#include "Pt/Timespan.h"
#include "Pt/Allocator.h"

#include <Pt/Hmi/Application.h>
//Desktop
#include <Pt/Hmi/Desktop/Window.h>
#include <Pt/Hmi/Desktop/Panel.h>


int main(int argc, char* args[])
{

	Pt::Hmi::Application	  app;
	
	Pt::Hmi::Desktop::Window	window;	

	window.show();
	app.run();		
}


