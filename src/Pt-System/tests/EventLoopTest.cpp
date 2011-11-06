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
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/System/MainLoop.h"
#include "Pt/System/Clock.h"
#include "Pt/Timespan.h"
#include "Pt/Allocator.h"

class E1: public Pt::Event
{
    const std::type_info& typeInfo() const
    {
        return typeid(E1);
    }

    Pt::Event& clone(Pt::Allocator& allocator) const
    {
        void* pEvent= allocator.allocate(sizeof(E1));
        return *(new (pEvent)E1(*this));
    }

    void destroy(Pt::Allocator& allocator)
    {
        allocator.deallocate(this, sizeof(E1));
    }
};

class E2: public Pt::Event
{
    public:
    const std::type_info& typeInfo() const
    {
        return typeid(E2);
    }

    Pt::Event& clone(Pt::Allocator& allocator) const
    {
/*		void* pEvent= allocator.allocate(sizeof(E2));
        return *(new (pEvent)E2(*this));*/
        return *(new E2(*this));
    }

    void destroy(Pt::Allocator& allocator)
    {
//		allocator.deallocate(this, sizeof(E2));
        delete this;
    }
};

class EventLoopTest : public Pt::Unit::TestSuite
{
    public:
        EventLoopTest()
        : Pt::Unit::TestSuite("EventLoopTest")
        {
            Pt::Unit::TestSuite::registerMethod( "DispatchTest", *this, &EventLoopTest::DispatchTest);
            //Pt::Unit::TestSuite::registerMethod( "LoopBenchmark", *this, &EventLoopTest::LoopBenchmark);
            Pt::Unit::TestSuite::registerMethod( "IdleTimeout", *this, &EventLoopTest::IdleTimeout);
        }

        void setUp()
        {
            _cnt = 0;
        }

    private:
        void DispatchTest()
        {
            Pt::System::MainLoop el;
            el.timeout() += Pt::slot(el, &Pt::System::MainLoop::exit);
            el.setIdleTimeout(500);

            el.event() += Pt::slot(*this, &EventLoopTest::onE1);
            el.event() += Pt::slot(*this, &EventLoopTest::onE2);

            el.commitEvent( E1() );
            el.commitEvent( E2() );
            el.commitEvent( E1() );
            el.run();

            PT_UNIT_ASSERT(_cnt == 3);
        }

        void onE1(const E1&)
        {
            ++_cnt;
        }

        void onE2(const E2&)
        {
            ++_cnt;
            PT_UNIT_ASSERT(2 == _cnt);
        }

        void IdleTimeout()
        {
            Pt::System::MainLoop el;
            el.setIdleTimeout(300);
            el.timeout() += Pt::slot(el, &Pt::System::MainLoop::exit);

            Pt::System::Clock clock;
            clock.start();
            el.run();
            Pt::Timespan elapsed = clock.stop();

            PT_UNIT_ASSERT(elapsed.totalMSecs() > 280);
            PT_UNIT_ASSERT(elapsed.totalMSecs() < 320);
        }

        // Pt::System::MainLoop* _loop;

        // void LoopBenchmark()
        // {
        //     Pt::System::MainLoop el;
        //     connect(el.timeout, el, &Pt::System::MainLoop::exit);
        //     el.setIdleTimeout(500);

        //     el.event.subscribe( slot(*this, &MainLoopTest::onBenchmarkEvent) );

        //     _loop = &el;
        //     el.commitEvent( E1() );

        //     Pt::System::Clock clock;
        //     clock.start();
        //     el.run();
        //     Pt::Timespan elapsed = clock.stop();

        //     std::cerr << "\n#### LoopTime: " << ( 5000000/elapsed.totalMSecs() ) * 1000 << " ev/s" << std::endl;
        //     std::exit(1);
        // }

        // void onBenchmarkEvent(const E1&)
        // {
        //     if(++_cnt > 5000000)
        //     {
        //         _loop->exit();
        //         return;
        //     }

        //     _loop->commitEvent( E1() );
        // }

        private:
            int _cnt;
};

Pt::Unit::RegisterTest<EventLoopTest> register_EventLoopTest;
