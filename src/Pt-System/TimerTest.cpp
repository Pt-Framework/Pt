#include "Pt/System/Timer.h"
#include "Pt/System/Selector.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include <iostream>
#include <sstream>

class TimerTest : public Pt::Unit::TestSuite
{
    public:
        TimerTest()
        : TestSuite("TimerTest")
        , _count(0)
        {
            this->registerMethod("Timeout", *this, &TimerTest::timeout);
        }

        void timeout()
        {
            Pt::System::Timer timer;
            connect( timer.timeout, *this, &TimerTest::onTimeout );
            timer.start(1000);

            Pt::System::Selector selector;
            selector.add(timer);
            selector.wait(50000);
            PT_UNIT_ASSERT(_count == 1);

            if(_count == 1)
            {
                std::cerr << "SUCCESS" << std::endl;
                PT_UNIT_ASSERT(_count == 0);
            }
        }

        void onTimeout()
        {
            _count++;
        }

    private:
        unsigned _count;
};

Pt::Unit::RegisterTest<TimerTest> register_TimerTest;
