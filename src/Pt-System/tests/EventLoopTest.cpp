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
#include "Pt/PoolAllocator.h"
#include <new>

#include "Pt/System/Selectable.h"
#include "Pt/System/Thread.h"
#include "Pt/System/Mutex.h"

#if __cplusplus >= 202002L
#include <Pt/Generator.h>
#endif

class TestAllocator : public Pt::Allocator
{
    public:
        TestAllocator(std::size_t maxAlloc)
        : _maxAlloc(maxAlloc)
        , _allocCount(0)
        {}

        virtual ~TestAllocator()
        {}

        virtual void* allocate(std::size_t size)
        {
            if(_allocCount == _maxAlloc)
                throw std::bad_alloc();

            void* m = operator new(size);
            ++_allocCount;
            return m;
        }

        virtual void deallocate(void* p, std::size_t size)
        {
            --_allocCount;
            operator delete(p);
        }

    private:
        std::size_t _maxAlloc;
        std::size_t _allocCount;
};

class E1 : public Pt::Event
{
    protected:
        const std::type_info& onTypeInfo() const
        { return typeid(E1); }

        Pt::Event& onClone(Pt::Allocator& allocator) const
        { return copyConstruct(*this, allocator); }

        void onDestroy(Pt::Allocator& allocator)
        { destruct(*this, allocator); }
};

class E2 : public Pt::Event
{
    protected:
        const std::type_info& onTypeInfo() const
        { return typeid(E2); }

        Pt::Event& onClone(Pt::Allocator& allocator) const
        { return *(new E2(*this)); }

        void onDestroy(Pt::Allocator& allocator)
        { delete this; }
};


class TestSelectable : public Pt::System::Selectable
{
    public:
        TestSelectable()
        : Pt::System::Selectable()
        , _loop(0)
        , _thread()
        {
            _thread.init( Pt::callable(*this, &TestSelectable::executeThread) );
        }

        void begin()
        {
            if( ! _loop)
                throw std::logic_error("TestSelectable not active");

            _thread.start();
        }

    protected:
        void executeThread()
        {
            Pt::System::EventLoop* loop = 0;

            Pt::System::MutexLock lock(_mtx);
            loop = _loop;
            lock.unlock();

            for(unsigned n = 0; n < 5; ++n)
            {
                Pt::System::Thread::sleep(500);
                loop->setReady(*this);
                loop->wake();
            }

            loop->exit();
        }

        virtual void onAttach(Pt::System::EventLoop& loop)
        {
            Pt::System::MutexLock lock(_mtx);
            _loop = &loop;
        }

        virtual void onDetach(Pt::System::EventLoop& loop)
        {
            Pt::System::MutexLock lock(_mtx);
            _loop = 0;
        }

        bool onRun()
        {
            std::cout << "TestSelectable::onRun called" << std::endl;
            return true;
        }

        void onCancel()
        {
            std::cout << "TestSelectable::onCancel called" << std::endl;
            _thread.join();
        }

    private:
        Pt::System::Mutex _mtx;
        Pt::System::EventLoop* _loop;
        Pt::System::AttachedThread _thread;
};


class EventLoopTest : public Pt::Unit::TestSuite
                    , public Pt::Connectable
{
    public:
        EventLoopTest()
        : Pt::Unit::TestSuite("EventLoopTest")
        {
            Pt::Unit::TestSuite::registerMethod( "SelectableTest", *this, &EventLoopTest::SelectableTest);
            Pt::Unit::TestSuite::registerMethod( "DispatchTest", *this, &EventLoopTest::DispatchTest);
            Pt::Unit::TestSuite::registerMethod( "MaxAlloc", *this, &EventLoopTest::MaxAlloc);
            //Pt::Unit::TestSuite::registerMethod( "LoopBenchmark", *this, &EventLoopTest::LoopBenchmark);
#if __cplusplus >= 202002L
            Pt::Unit::TestSuite::registerMethod("YieldAsyncTest", *this, &EventLoopTest::YieldAsyncTest);
#endif
        }

        void setUp()
        {
            _cnt = 0;
        }

    private:
        void SelectableTest()
        {
            TestSelectable ts;

            Pt::System::MainLoop el;

            ts.setActive(el);
            ts.begin();

            el.run();
        }

        void DispatchTest()
        {
            Pt::System::MainLoop el;

            el.eventReceived() += Pt::slot(*this, &EventLoopTest::onE1);
            el.eventReceived() += Pt::slot(*this, &EventLoopTest::onE2);

            Pt::System::Timer exitTimer;
            exitTimer.setActive(el);
            exitTimer.start(500);
            exitTimer.timeout() += Pt::slot(el, &Pt::System::EventLoop::exit);

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

        void MaxAlloc()
        {
            TestAllocator alloc(3);
            Pt::System::MainLoop el(alloc);

            el.commitEvent( E1() );
            el.commitEvent( E1() );
            el.commitEvent( E1() );

            PT_UNIT_ASSERT_THROW( el.commitEvent( E1() ), std::bad_alloc );
        }

#if __cplusplus >= 202002L
        Pt::Generator<int> squares(int n)
        {
            for (int i = 1; i <= n; ++i)
                co_yield i * i;
        }

        Pt::Task<void> yieldingTask(Pt::System::EventLoop& loop, int& result)
        {
            auto sq = squares(3);
            int sum = 0;
            while( co_await sq.next() )
            {
                sum += sq.value();
                co_await loop.yieldAsync();
            }
            result = sum;
            loop.exit();
        }

        void YieldAsyncTest()
        {
            Pt::System::MainLoop loop;
            int finalResult = -1;

            Pt::Task<void> task = yieldingTask(loop, finalResult);
            task.run();

            loop.run();
            PT_UNIT_ASSERT( task.done() );
            PT_UNIT_ASSERT_EQUAL(finalResult, 14); // 1^2 + 2^2 + 3^2 = 14
        }
#endif

         Pt::System::MainLoop* _loop;

         void LoopBenchmark()
         {
             Pt::PoolAllocator allocator(64, 8, 4096);
             Pt::System::MainLoop loop(allocator);

             loop.eventReceived().connect( slot(*this, &EventLoopTest::onBenchmarkEvent) );

             _loop = &loop;
             loop.commitEvent( E1() );

             Pt::System::Clock clock;
             clock.start();
             loop.run();
             Pt::Timespan elapsed = clock.stop();

             std::cerr << "\n#### LoopTime: " << ( 500000/elapsed.toMSecs() ) * 1000 << " ev/s" << std::endl;
             //std::exit(1);
         }

         void onBenchmarkEvent(const E1&)
         {
             if(++_cnt > 500000)
             {
                 _loop->exit();
                 return;
             }

             _loop->commitEvent( E1() );
         }

        private:
            int _cnt;
};

Pt::Unit::RegisterTest<EventLoopTest> register_EventLoopTest;
