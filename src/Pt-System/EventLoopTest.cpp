#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/System/EventLoop.h"
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
        }

        void setUp()
        {
            _cnt= 0;
        }

    private:
        void DispatchTest()
        {
            Pt::System::EventLoop el;
            connect(el.timeout, el, &Pt::System::EventLoop::exit);

            el.addHandler( slot(*this, &EventLoopTest::onE1) );
            el.addHandler( slot(*this, &EventLoopTest::onE2) );

            el.setIdleTimeout(1000);

            el.queueEvent( E1() );
            el.queueEvent( E2()/*, Pt::IEventLoop::PRIO_HIGH*/ );
            el.queueEvent( E1() );
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
        }

            int _cnt;
};

Pt::Unit::RegisterTest<EventLoopTest> register_EventLoopTest;
