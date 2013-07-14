
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/System/MainLoop.h"
#include "Pt/System/Clock.h"
#include "Pt/Timespan.h"
#include "Pt/Allocator.h"
#include <Pt/System/MouseDevice.h>

class MouseDeviceTest : public Pt::Unit::TestSuite
{
    public:
        MouseDeviceTest()
        : Pt::Unit::TestSuite("MouseDeviceTest")
        {
            Pt::Unit::TestSuite::registerMethod( "events", *this, &MouseDeviceTest::EventTest);
        }

        void setUp()
        {         
        }

    private:
        void onEvent(const Pt::System::Input2DEvent& ev)
        {            
			std::cout<<"X = "<< ev.x() << "Y = "<<ev.y()<<std::endl;
        }

        void EventTest()
        {
            Pt::System::MainLoop loop;
			Pt::System::MouseDevice device;

			char test[10];
			device.InputEvent += Pt::slot(*this, &MouseDeviceTest::onEvent);
			device.setActive(loop);
			device.beginRead(test, 10);			
		
			loop.run();
        }

};

Pt::Unit::RegisterTest<MouseDeviceTest> reg_MouseDeviceTest;
