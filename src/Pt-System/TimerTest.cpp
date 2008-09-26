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
        , _timer(0)
        , _count(0)
        {
            this->registerMethod("Timeout", *this, &TimerTest::Timeout);
            this->registerMethod("RemoveOnTimeout", *this, &TimerTest::RemoveOnTimeout);
            this->registerMethod("RemoveAddOnTimeout", *this, &TimerTest::RemoveAddOnTimeout);
            this->registerMethod("DestroyOnTimeout", *this, &TimerTest::DestroyOnTimeout);
        }

        void setUp()
        {
            _count = 0;
            _selector = new Pt::System::Selector();
            _timer = new Pt::System::Timer();
            _timer->start(500);
            _selector->add(*_timer);
        }

        void tearDown()
        {
            delete _selector;
            _selector = 0;
            delete _timer;
            _timer = 0;
        }

        void Timeout()
        {
            connect( _timer->timeout, *this, &TimerTest::onTimeout );

            _selector->wait(1000);
            PT_UNIT_ASSERT(_count == 1);

            _selector->wait(1000);
            PT_UNIT_ASSERT(_count == 2);
        }

        void RemoveOnTimeout()
        {
            connect( _timer->timeout, *this, &TimerTest::onTimeout );
            connect( _timer->timeout, *this, &TimerTest::removeTimer );

            _selector->wait(1000);
            PT_UNIT_ASSERT(_count == 1);

            _selector->wait(1000);
            PT_UNIT_ASSERT(_count == 1);
        }

        void RemoveAddOnTimeout()
        {
            connect( _timer->timeout, *this, &TimerTest::onTimeout );
            connect( _timer->timeout, *this, &TimerTest::removeAddTimer );

            _selector->wait(1000);
            PT_UNIT_ASSERT(_count == 1);

            _selector->wait(1000);
            PT_UNIT_ASSERT(_count == 2);
        }

        void DestroyOnTimeout()
        {
            connect( _timer->timeout, *this, &TimerTest::onTimeout );
            connect( _timer->timeout, *this, &TimerTest::destroyTimer );

            _selector->wait(1000);
            PT_UNIT_ASSERT(_count == 1);

            _selector->wait(1000);
            PT_UNIT_ASSERT(_count == 1);
        }

        void onTimeout()
        {
            _count++;
        }

        void removeTimer()
        {
            _timer->setSelector(0);
        }

        void removeAddTimer()
        {
            _timer->setSelector(0);
            _timer->setSelector(_selector);
        }

        void destroyTimer()
        {
            delete _timer;
            _timer = 0;
        }

    private:
        Pt::System::Timer* _timer;
        Pt::System::Selector* _selector;
        unsigned _count;
};

Pt::Unit::RegisterTest<TimerTest> register_TimerTest;
