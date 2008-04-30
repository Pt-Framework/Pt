/***************************************************************************
 *   Copyright (C) 2005-2007 by Dr. Marc Boris Duerner                     *
 *   Copyright (C) 2005 Stephan Beal                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction. Specifically, if other files    *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License. This exception does not however invalidate any other         *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
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
#undef PT_API_EXPORT

#include "Pt/Signal.h"

#include <string>
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"

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
        { ++_count; }

        int count() const
        { return _count; }

        void reset()
        { _count = 0; }

    private:
        int _count;
};


class SignalTest : public Pt::Unit::TestSuite
{
    public:
        SignalTest()
        : Pt::Unit::TestSuite("SignalTest")
        , _caller(0)
        , _callee(0)
        {
            Pt::Unit::TestSuite::registerMethod( "Disconnect", *this, &SignalTest::Disconnect );
            Pt::Unit::TestSuite::registerMethod( "DeleteWhileSend", *this, &SignalTest::DeleteWhileSend );
            Pt::Unit::TestSuite::registerMethod( "CopySignal", *this, &SignalTest::CopySignal );
            Pt::Unit::TestSuite::registerMethod( "Send0", *this, &SignalTest::Send0 );
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
        void Disconnect()
        {
            Pt::Signal<> sn;
            connect(sn, *this, &SignalTest::method0);
            disconnect(sn, *this, &SignalTest::method0);;
            PT_UNIT_ASSERT(sn.connectionCount() == 0)

            connect(sn, *this, &SignalTest::constMethod0);
            disconnect(sn, *this, &SignalTest::constMethod0);
            PT_UNIT_ASSERT(sn.connectionCount() == 0)

            connect(sn, &function0);
            disconnect(sn, &function0);
            PT_UNIT_ASSERT(sn.connectionCount() == 0)
        }

        void Send0()
        {
            Callee* recv = new Callee;
            Pt::Signal<> signal;
            connect( signal, slot(*recv, &Callee::slot0) );
            PT_UNIT_ASSERT(signal.connectionCount() == 1)

            // A deleted receiver must remove itself from a signal
            delete recv;
            signal.send();
            PT_UNIT_ASSERT(signal.connectionCount() == 0)

            // A signal must call its slot when connected
            recv = new Callee;
            Pt::Connection connection = connect(signal, slot(*recv, &Callee::slot0) );
            signal.send();
            PT_UNIT_ASSERT( recv->count() == 1)

            recv->reset();

            // Closing connections must remove them
            connection.close();
            signal.send();
            PT_UNIT_ASSERT( recv->count() == 0 )
            PT_UNIT_ASSERT( signal.connectionCount() == 0)

            delete recv;
        }

        void SignalToSignal0()
        {
            Callee* recv = new Callee;
            Pt::Signal<> signal1;
            Pt::Signal<> signal2;

            connect( signal1, slot(signal2) );
            connect( signal2, slot(*recv, &Callee::slot0) );

            // Slot must be called via signal chain
            signal1.send();
            PT_UNIT_ASSERT( recv->count() == 1 )

            delete recv;
        }

        void CopySignal()
        {
            Callee callee;
            Pt::Signal<>* signal1 = new Pt::Signal<>;
            Pt::Signal<> signal2;

            Pt::Connection connection1 = connect(*signal1, callee, &Callee::slot0);
            signal2 = *signal1;

            delete signal1;

            signal2.send();
            PT_UNIT_ASSERT( callee.count() == 1 )
        }

        void DeleteWhileSend()
        {
            connect(*_caller, *this, &SignalTest::method0);
            connect(*_caller, *this, &SignalTest::deleteCallee );
            connect(*_caller, *this, &SignalTest::method0);
            connect(*_caller, *this, &SignalTest::deleteCaller);
            connect(*_caller, *this, &SignalTest::method0);

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
