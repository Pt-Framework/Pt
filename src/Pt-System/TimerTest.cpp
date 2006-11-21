#include <iostream>
#include <sstream>
using namespace std;

#include "Pt/System/Timer.h"
using namespace Pt;

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestFixture.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"


class TimerTest : public Pt::Connectable, public Pt::Unit::TestSuite
{
	public:
		TimerTest()
		: TestSuite("TimerTest")
		{
			this->registerMethod("test", *this, &TimerTest::test);
		}

		void test()
		{
			Pt::System::Timer timer;
			connect( timer.onTime, *this, &TimerTest::onTime );
			timer.start();
			Pt::System::Thread::sleep( 10000 );
			timer.stop();
			PT_UNIT_ASSERT( _count >= 98 );
		}

		void onTime( size_t count )
		{
			_count = count;
			if(_count %5 == 0)
				Pt::System::Thread::sleep( 100 );
				
		}	

private:
	Pt::uint32_t _count;

};

Pt::Unit::RegisterTest<TimerTest> register_TimerTest;