/*
 * Copyright (C) 2008 by Marc Duerner
 * 
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
    Pt::System::AttachedThread thread( Pt::callable(*this, &ThreadTest::thread1) );
    thread.start();
    thread.join();
    PT_UNIT_ASSERT( flag() );
}


inline void ThreadTest::Detached()
{
    TestThread* thread = new TestThread(*this);

    _mutex.lock();

    thread->start();

    _cond.wait(_mutex);

    PT_UNIT_ASSERT( _flag );
    _mutex.unlock();
}


Pt::Unit::RegisterTest<ThreadTest> register_ThreadTest;
