/*
 * Copyright (C) 2005-2007 by Dr. Marc Boris Duerner
 * Copyright (C) 2005 Stephan Beal
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
#include "Pt/Signal.h"
#include "Pt/Lambda.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include <string>
#include <cassert>

namespace {

void function0()
{}

class Callee : public Pt::Connectable
{
    public:
        Callee()
        : _count(0)
        {}

        int destroySelf()
        {
            delete this;
            return 0;
        }

        void slot0()
        {
            ++_count;
        }

        void slot1(int n)
        {
            ++_count;
        }

        void slot2(int, int)
        {
            ++_count;
        }

        int count() const
        { return _count; }

        void reset()
        { _count = 0; }

    private:
        int _count;
};

}

class SignalTest : public Pt::Unit::TestSuite
                 , public Pt::Connectable
{
    public:
        SignalTest()
        : Pt::Unit::TestSuite("SignalTest")
        , _caller(0)
        , _callee(0)
        {
            Pt::Unit::TestSuite::registerMethod( "Bind1", *this, &SignalTest::Bind1 );
            Pt::Unit::TestSuite::registerMethod( "Bind2", *this, &SignalTest::Bind2 );

            Pt::Unit::TestSuite::registerMethod( "Disconnect", *this, &SignalTest::Disconnect );
            Pt::Unit::TestSuite::registerMethod( "DisconnectWhileSend", *this, &SignalTest::DisconnectWhileSend );
            Pt::Unit::TestSuite::registerMethod( "DeleteWhileSend", *this, &SignalTest::DeleteWhileSend );
            Pt::Unit::TestSuite::registerMethod( "CopySignal", *this, &SignalTest::CopySignal );
            Pt::Unit::TestSuite::registerMethod( "Lambda", *this, &SignalTest::Lambda );
            Pt::Unit::TestSuite::registerMethod( "LambdaGeneric", *this, &SignalTest::LambdaGeneric );
            Pt::Unit::TestSuite::registerMethod( "LambdaReturn", *this, &SignalTest::LambdaReturn );
            Pt::Unit::TestSuite::registerMethod( "LambdaClose", *this, &SignalTest::LambdaClose );
            Pt::Unit::TestSuite::registerMethod( "LambdaContext", *this, &SignalTest::LambdaContext );
            Pt::Unit::TestSuite::registerMethod( "Send0", *this, &SignalTest::Send0 );
            Pt::Unit::TestSuite::registerMethod( "Send2", *this, &SignalTest::Send2 );
            Pt::Unit::TestSuite::registerMethod( "SignalToSignal0", *this, &SignalTest::SignalToSignal0 );
        }

        virtual void setUp()
        {
            _caller = new Pt::Signal<>;
            _callee = new Callee;
        }

        virtual void tearDown()
        {
            delete _caller;
            _caller = 0;
            delete _callee;
            _callee = 0;
        }

    protected:
        void Bind1()
        {
            Callee* recv = new Callee;
            Pt::Signal<> signal;

            signal += Pt::slot( Pt::slot(*recv, &Callee::slot1), 42 );
            PT_UNIT_ASSERT(signal.connectionCount() == 1);

            // A deleted receiver must remove itself from a signal
            delete recv;
            signal.send();
            PT_UNIT_ASSERT(signal.connectionCount() == 0);

            // A signal must call its slot when connected
            recv = new Callee;
            Pt::Connection connection =
                signal += Pt::slot( Pt::slot(*recv, &Callee::slot1), 42 );
            signal.send();
            PT_UNIT_ASSERT( recv->count() == 1);

            recv->reset();

            // Closing connections must remove them
            connection.close();
            signal.send();
            PT_UNIT_ASSERT( recv->count() == 0 );
            PT_UNIT_ASSERT( signal.connectionCount() == 0);

            delete recv;
        }

        void Bind2()
        {
            Callee* recv = new Callee;
            Pt::Signal<int> signal;

            signal += Pt::slot( Pt::slot(*recv, &Callee::slot2), 42 );
            PT_UNIT_ASSERT(signal.connectionCount() == 1);

            // A deleted receiver must remove itself from a signal
            delete recv;
            signal.send(5);
            PT_UNIT_ASSERT(signal.connectionCount() == 0);

            // A signal must call its slot when connected
            recv = new Callee;
            Pt::Connection connection =
                signal += Pt::slot( Pt::slot(*recv, &Callee::slot2), 42 );
            signal.send(5);
            PT_UNIT_ASSERT( recv->count() == 1);

            recv->reset();

            // Closing connections must remove them
            connection.close();
            signal.send(5);
            PT_UNIT_ASSERT( recv->count() == 0 );
            PT_UNIT_ASSERT( signal.connectionCount() == 0);

            delete recv;
        }

        void Disconnect()
        {
            Pt::Signal<> sn;
            sn += Pt::slot(*this, &SignalTest::method0);
            sn -= Pt::slot(*this, &SignalTest::method0);
            PT_UNIT_ASSERT(sn.connectionCount() == 0);

            sn += Pt::slot(*this, &SignalTest::constMethod0);
            sn -= Pt::slot(*this, &SignalTest::constMethod0);
            PT_UNIT_ASSERT(sn.connectionCount() == 0);

            sn += Pt::slot(&function0);
            sn -= Pt::slot(&function0);
            PT_UNIT_ASSERT(sn.connectionCount() == 0);
        }

        void DisconnectWhileSend()
        {
            *_caller += Pt::slot(*this, &SignalTest::method0);
            *_caller += Pt::slot(*this, &SignalTest::disconnectCaller);
            *_caller += Pt::slot(*this, &SignalTest::method0);

            _caller->send();
            PT_UNIT_ASSERT(_caller->connectionCount() == 0);
        }

        void disconnectCaller()
        {
            _caller->disconnect();
        }

        void Send0()
        {
            Callee* recv = new Callee;
            Pt::Signal<> signal;
            signal += Pt::slot(*recv, &Callee::slot0);
            PT_UNIT_ASSERT(signal.connectionCount() == 1);

            // A deleted receiver must remove itself from a signal
            delete recv;
            signal.send();
            PT_UNIT_ASSERT(signal.connectionCount() == 0);

            // A signal must call its slot when connected
            recv = new Callee;
            Pt::Connection connection = signal += Pt::slot(*recv, &Callee::slot0);
            signal.send();
            PT_UNIT_ASSERT( recv->count() == 1);

            recv->reset();

            // Closing connections must remove them
            connection.close();
            signal.send();
            PT_UNIT_ASSERT( recv->count() == 0 );
            PT_UNIT_ASSERT( signal.connectionCount() == 0);

            delete recv;
        }

        void Send2()
        {
            Callee* recv = new Callee;
            Pt::Signal<int, int> signal;
            signal += slot(*recv, &Callee::slot2);
            PT_UNIT_ASSERT(signal.connectionCount() == 1);

            // A deleted receiver must remove itself from a signal
            delete recv;
            signal.send(1, 2);
            PT_UNIT_ASSERT(signal.connectionCount() == 0);

            // A signal must call its slot when connected
            recv = new Callee;
            Pt::Connection connection = signal += slot(*recv, &Callee::slot2);
            signal.send(1, 2);
            PT_UNIT_ASSERT( recv->count() == 1);

            recv->reset();

            // Closing connections must remove them
            connection.close();
            signal.send(1, 2);
            PT_UNIT_ASSERT( recv->count() == 0 );
            PT_UNIT_ASSERT( signal.connectionCount() == 0);

            delete recv;
        }

        void SignalToSignal0()
        {
            Callee* recv = new Callee;
            Pt::Signal<> signal1;
            Pt::Signal<> signal2;

            signal1 += Pt::slot(signal2);
            signal2 += Pt::slot(*recv, &Callee::slot0);

            // Slot must be called via signal chain
            signal1.send();
            PT_UNIT_ASSERT( recv->count() == 1 );

            delete recv;
        }

        void CopySignal()
        {
            Callee callee;
            Pt::Signal<> signal1;
            Pt::Signal<>* signal2 = new Pt::Signal<>;
            Pt::Signal<> signal3;

            Pt::Connection connection1 = signal1 += Pt::slot(*signal2);
            Pt::Connection connection2 = *signal2 += Pt::slot(callee, &Callee::slot0);

            signal3 = *signal2;
            delete signal2;

            signal3.send();
            PT_UNIT_ASSERT( callee.count() == 1 );
        }

        void Lambda()
        {
            int count = 0;
            Pt::Signal<> signal;
            signal += Pt::slot([&count]() { ++count; });

            signal.send();

            PT_UNIT_ASSERT(count == 1);
        }

        void LambdaGeneric()
        {
            int value = 0;
            Pt::Signal<int> signal;
            signal += Pt::slot<void, int>([&value](auto) { value = 42; });

            signal.send(7);

            PT_UNIT_ASSERT(value == 42);
        }

        void LambdaReturn()
        {
            int value = 0;
            Pt::Signal<int> signal;
            signal += Pt::slot([&value](int argument) {
                value = argument;
                return argument;
            });

            signal.send(7);

            PT_UNIT_ASSERT(value == 7);
        }

        void LambdaClose()
        {
            int count = 0;
            Pt::Signal<> signal;
            Pt::Connection connection = signal += Pt::slot([&count]() { ++count; });

            connection.close();
            signal.send();

            PT_UNIT_ASSERT(count == 0);
            PT_UNIT_ASSERT(signal.connectionCount() == 0);
        }

        void LambdaContext()
        {
            int count = 0;
            Pt::Signal<> signal;

            {
                Pt::Connectable context;
                signal += Pt::slot(context, [&count]() { ++count; });
                PT_UNIT_ASSERT(context.connectionCount() == 1);
            }

            signal.send();

            PT_UNIT_ASSERT(count == 0);
            PT_UNIT_ASSERT(signal.connectionCount() == 0);
        }

        void DeleteWhileSend()
        {
            *_caller += Pt::slot(*this, &SignalTest::method0);
            *_caller += Pt::slot(*this, &SignalTest::deleteCallee );
            *_caller += Pt::slot(*this, &SignalTest::method0);
            *_caller += Pt::slot(*this, &SignalTest::deleteCaller);
            *_caller += Pt::slot(*this, &SignalTest::method0);

            _caller->send();
        }

        void deleteCallee()
        {
            //std::cerr << "SignalTest::deleteCallee" << std::endl;
            delete _callee;
            _callee = 0;
        }

        void deleteCaller()
        {
            //std::cerr << "SignalTest::deleteCaller" << std::endl;
            delete _caller;
            _caller = 0;
        }

        void method0()
        { }

        void constMethod0() const
        { }

    private:
        Pt::Signal<>* _caller;
        Callee* _callee;
};

Pt::Unit::RegisterTest<SignalTest> register_SignalTest;



/*
#undef PT_API_EXPORT

#include "Pt/Signal.h"
#include "Pt/Exception.h"
#include "Pt/Main.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <ctime>

using namespace Pt;


bool testFunction0Called = false;
bool testFunction1Called = false;
bool testFunction2Called = false;
bool testFunction3Called = false;

void testFunction0()
{ testFunction0Called = true; }

void testFunction1(int)
{ testFunction1Called = true; }

void testFunction2(int, int)
{ testFunction2Called = true; }

void testFunction3(int, int, int)
{ testFunction3Called = true; }


class Receiver : public Pt::Connectable {
    public:
        Receiver()
        : _called(false)
        {}

        void onSignal0()
        { _called = true; }

        void onSignal1(int)
        { _called = true; }

        void onSignal2(int, int)
        { _called = true; }

        void onSignal3(int, int, int)
        { _called = true; }

        bool called() const
        { return _called; }

        void reset()
        { _called = false; }

    private:
        bool _called;
};


void performanceTest()
{
        clock_t begin, end;
        Receiver receiver;

        std::cerr << "\n------- Performance Test --------" << std::endl;

        Signal<> signal;
        connect(signal, receiver, &Receiver::onSignal0);
        std::cerr << "Signal benchmark... ";
        begin = clock();
        for(long i = 0; i < 100000000; ++i) {
            signal();
        }
        end = clock();
        std::cerr << "Duration: " << (end - begin) << std::endl;

        cerr << "mem_fun_t benchmark... ";
        begin = clock();
        mem_fun_t<void, Receiver> mf( &Receiver::onSignal0 );
        for(long i = 0; i < 100000000; ++i) {
            mf(&receiver);
        }
        end = clock();
        std::cerr << "Duration: " << (end - begin) << std::endl;
        std::cerr << "---------------------------------\n\n";
}


void functionTest0()
{
    testFunction0Called = false;
    Signal<> signal;

    testFunction0Called = false;
    connect(signal, testFunction0);
    signal.send();
    if(testFunction0Called == false)
        throw std::logic_error("Signal not sent to connected slot." + PT_SOURCEINFO);


//    testFunction0Called = false;
//    disconnect(signal, testFunction0);
//    signal.send();
//    if(testFunction0Called == true)
//        throw std::logic_error("Signal sent to disconnected slot." + PT_SOURCEINFO);
//    if(signal.connections().size() != 0)
//        throw std::logic_error("Connections left after disconnect." + PT_SOURCEINFO);

}


void slotMethod()
{ std::cerr << "slotMethod called." << std::endl;}

class DeleteTest : public Pt::Connectable
{
public:
    //Signal<> signal;

    DeleteTest()
    : _caller(0)
    {}

    ~DeleteTest()
    {
        //std::cerr << "DeleteTest::~DeleteTest" << std::endl;
    }

    void deleteCallee()
    {
        std::cerr << "SelfDisconnectTest::deleteCallee" << std::endl;
        delete _callee;
        _callee = 0;
    }

    void deleteCaller()
    {
        std::cerr << "\nSelfDisconnectTest::deleteCaller" << std::endl;
        delete _caller;
        _caller = 0;
    }

    void operator()()
    {
        _caller = new Signal<>;
        _callee = new DeleteTest;

        connect(*_caller, &slotMethod);
        connect(*_caller, *_callee, &DeleteTest::deleteCallee );
        connect(*_caller, &slotMethod);
        connect(*_caller, *this, &DeleteTest::deleteCaller);
        connect(*_caller, &slotMethod);
        connect(*_caller, &slotMethod);
        connect(*_caller, &slotMethod);
        connect(*_caller, &slotMethod);
        _caller->send();
    }

    Signal<>* _caller;
    static DeleteTest* _callee;
};

DeleteTest* DeleteTest::_callee = 0;

*/
