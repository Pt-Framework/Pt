/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "Pt/System/Timer.h"
#include "Pt/System/MainLoop.h"
#include "Pt/System/Clock.h"
#include "Pt/Timespan.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

class TimerTest : public Pt::Unit::TestSuite
                , public Pt::Connectable
{
    public:
        TimerTest()
        : TestSuite("TimerTest")
        , _timer(0)
        , _loop(0)
        , _count(0)
        {
            this->registerMethod("Timeout", *this, &TimerTest::Timeout);
			this->registerMethod("StopTimer", *this, &TimerTest::StopTimer);
            this->registerMethod("RemoveOnTimeout", *this, &TimerTest::RemoveOnTimeout);
            this->registerMethod("RemoveAddOnTimeout", *this, &TimerTest::RemoveAddOnTimeout);
            this->registerMethod("DestroyOnTimeout", *this, &TimerTest::DestroyOnTimeout);
#if __cplusplus >= 202002L
            this->registerMethod("CoWait", *this, &TimerTest::CoWait);
            this->registerMethod("DestroyTimer", *this, &TimerTest::DestroyTimer);
            this->registerMethod("DestroyWaitingTimer", *this, &TimerTest::DestroyWaitingTimer);
            this->registerMethod("DestroyTask", *this, &TimerTest::DestroyTask);
            this->registerMethod("PendingAwaiter", *this, &TimerTest::PendingAwaiter);
#endif
        }

        void setUp()
        {
            _count = 0;

            _loop = new Pt::System::MainLoop();

            _timer = new Pt::System::Timer();
            _timer->setActive(*_loop);
            _timer->start(100);
        }

        void tearDown()
        {
            delete _loop;
            _loop = 0;

            delete _timer;
            _timer = 0;
        }

		void StopTimer()
		{
			Pt::System::Timer timer;
			Pt::System::MainLoop loop;
			Pt::System::Timer timer2;

			timer.setActive(loop);
			timer.start(400);
			timer.stop();

			timer2.setActive(loop);
			timer2.start(400);
			timer2.stop();
		}

        void Timeout()
        {
            _timer->timeout() += Pt::slot(*this, &TimerTest::onTimeout);

            Pt::System::Clock clock;
            clock.start();
            _loop->run();
            Pt::Timespan elapsed = clock.stop();

            PT_UNIT_ASSERT(_count == 3);
            PT_UNIT_ASSERT(elapsed.toMSecs() > 280);
            PT_UNIT_ASSERT(elapsed.toMSecs() < 320);
        }

        void RemoveOnTimeout()
        {
            Pt::System::Timer exitTimer;
            exitTimer.start(500);
            exitTimer.setActive(*_loop);
            exitTimer.timeout() += Pt::slot(*_loop, &Pt::System::MainLoop::exit);

            _timer->timeout() += Pt::slot(*this, &TimerTest::onTimeout);
            _timer->timeout() += Pt::slot(*this, &TimerTest::removeTimer);

            _loop->run();
            PT_UNIT_ASSERT(_count == 1);
        }

        void RemoveAddOnTimeout()
        {
            _timer->timeout() += Pt::slot(*this, &TimerTest::onTimeout);
            _timer->timeout() += Pt::slot(*this, &TimerTest::removeAddTimer);

            _loop->run();
            PT_UNIT_ASSERT(_count == 3);
        }

        void DestroyOnTimeout()
        {
            Pt::System::Timer exitTimer;
            exitTimer.start(500);
            exitTimer.setActive(*_loop);
            exitTimer.timeout() += Pt::slot(*_loop, &Pt::System::MainLoop::exit);

            _timer->timeout() += Pt::slot(*this, &TimerTest::onTimeout );
            _timer->timeout() += Pt::slot(*this, &TimerTest::destroyTimer );

            _loop->run();
            PT_UNIT_ASSERT(_count == 1);
        }

        void onTimeout()
        {
            _count++;
            if(_count >= 3)
                _loop->exit();
        }

        void removeTimer()
        {
            _timer->detach();
        }

        void removeAddTimer()
        {
            _timer->detach();
            _timer->setActive(*_loop);
        }

        void destroyTimer()
        {
            delete _timer;
            _timer = 0;
        }

#if __cplusplus >= 202002L
        void CoWait();
        void DestroyTimer();
        void DestroyWaitingTimer();
        void DestroyTask();
        void PendingAwaiter();
        Pt::Task<> coWaitAsync();
        Pt::Task<> awaitTimerAsync(Pt::System::AsyncWait& awaiter);
#endif

    private:
        Pt::System::Timer* _timer;
        Pt::System::MainLoop* _loop;
        unsigned _count;
};

Pt::Unit::RegisterTest<TimerTest> register_TimerTest;


#if __cplusplus >= 202002L

Pt::Task<> TimerTest::coWaitAsync()
{
    Pt::System::Clock clock;
    clock.start();

    co_await _timer->waitAsync(100);

    Pt::Timespan elapsed = clock.stop();
    PT_UNIT_ASSERT(elapsed.toMSecs() >= 90);
    PT_UNIT_ASSERT(elapsed.toMSecs() < 200);

    _loop->exit();
}


void TimerTest::CoWait()
{
    Pt::Task<> task = coWaitAsync();
    task.run();
    _loop->run();
}


Pt::Task<> TimerTest::awaitTimerAsync(Pt::System::AsyncWait& awaiter)
{
    co_await awaiter;
}


void TimerTest::DestroyTimer()
{
    Pt::System::Timer* timer = new Pt::System::Timer();

    {
        Pt::System::AsyncWait awaiter = timer->waitAsync(100);
        delete timer;

        Pt::Task<> task = awaitTimerAsync(awaiter);
        task.run();

        PT_UNIT_ASSERT( task.done() );
        PT_UNIT_ASSERT_THROW( task.result(), std::logic_error );
    }
}


void TimerTest::DestroyWaitingTimer()
{
    Pt::System::MainLoop loop;
    Pt::System::Timer* timer = new Pt::System::Timer();
    timer->setActive(loop);

    Pt::System::AsyncWait awaiter = timer->waitAsync(100);
    Pt::Task<> task = awaitTimerAsync(awaiter);
    task.run();
    delete timer;

    PT_UNIT_ASSERT( ! task.done() );
    task.cancel();
    PT_UNIT_ASSERT( ! task );
}


void TimerTest::DestroyTask()
{
    {
        Pt::Task<> task = coWaitAsync();
        task.run();
        PT_UNIT_ASSERT( _timer->isStarted() );
    }

    PT_UNIT_ASSERT( ! _timer->isStarted() );
}


void TimerTest::PendingAwaiter()
{
    Pt::System::AsyncWait wait = _timer->waitAsync(100);

    PT_UNIT_ASSERT_THROW( _timer->waitAsync(100), std::logic_error );
}

#endif // __cplusplus >= 202002L
