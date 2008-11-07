/***************************************************************************
 *   Copyright (C) 2008 by Marc Duerner                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "Pt/System/Thread.h"
#include "Pt/System/Mutex.h"
#include "Pt/System/Condition.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"

class ThreadTest;

class TestThread : public Pt::System::DetachedThread
{
    public:
        TestThread(ThreadTest& test)
        : _test(test)
        {}

        virtual void run();

    private:
        ThreadTest& _test;
};

class ThreadTest : public Pt::Unit::TestSuite
{
    public:
        ThreadTest()
        : Pt::Unit::TestSuite("ThreadTest")
        , _mutex(Pt::System::Mutex::Normal)
        , _flag(false)
        {
            Pt::Unit::TestSuite::registerMethod( "Attached", *this, &ThreadTest::Attached );
            Pt::Unit::TestSuite::registerMethod( "Detached", *this, &ThreadTest::Detached );
        }

        void setUp()
        {
            setFlag(false);
        }

        bool flag() const
        {
            Pt::System::MutexLock lock(_mutex);
            return _flag;
        }

        void setFlag(bool f)
        {
            Pt::System::MutexLock lock(_mutex);
            _flag = f;
            _cond.signal();
        }

    protected:
        void Attached();
        void Detached();

        void thread1()
        {
            setFlag(true);
        }

    private:
        mutable Pt::System::Mutex _mutex;
        mutable Pt::System::Condition _cond;
        bool _flag;
};


inline void TestThread::run()
{
    _test.setFlag(true);
}


inline void ThreadTest::Attached()
{
    Pt::System::AttachedThread thread( callable(*this, &ThreadTest::thread1) );
    thread.start();
    thread.join();
    PT_UNIT_ASSERT( flag() );
}


inline void ThreadTest::Detached()
{
    TestThread* thread = new TestThread(*this);
    thread->start();

    _mutex.lock();
    _cond.wait(_mutex);

    PT_UNIT_ASSERT( _flag );
    _mutex.unlock();
}


Pt::Unit::RegisterTest<ThreadTest> register_ThreadTest;
