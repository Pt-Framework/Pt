/***************************************************************************
 *   Copyright (C) 2005 by Dr. Marc Boris Drner                           *
 *   Copyright (C) 2005 Stephan Beal                                       *
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

/***************************************************************************
 *   Copyright (C) 2005-2006 by Marc Boris Duerner                          *
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
#undef PT_API_EXPORT

#include "Pt/Signal.h"

#include <string>
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/TestSchedule.h"
#include "Pt/Unit/RegisterTest.h"


class Callee : public Pt::Connectable
{
    public:
        Callee()
        : _count(0)
        {}

        void slot0()
        { ++_count; }

        int count() const
        { return _count; }

        void reset()
        { _count = 0; }

    private:
        int _count;
};


class SignalTest : public Pt::Unit::TestSuite, public Pt::Connectable
{
    public:
        SignalTest()
        : Pt::Unit::TestSuite("SignalTest")
        , _called0(false)
        , _caller(0)
        , _callee(0)
        {
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
        void Send0()
        {
            Callee* recv = new Callee;
            Pt::Signal<> signal;
            connect( signal, slot(recv, &Callee::slot0) );
            PT_UNIT_ASSERT(signal.connectionCount() == 1)

            // A deleted receiver must remove itself from a signal
            delete recv;
            signal.send();
            PT_UNIT_ASSERT(signal.connectionCount() == 0)

            // A signal must call its slot when connected
            recv = new Callee;
            Pt::Connection connection = connect(signal, slot(recv, &Callee::slot0) );
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
            connect( signal2, slot(recv, &Callee::slot0) );

            // Slot must be called via signal chain
            signal1.send();
            PT_UNIT_ASSERT( recv->count() == 1 )

            delete recv;
        }

        void CopySignal()
        {
            Callee callee;
            Pt::Signal<> signal1;
            Pt::Signal<> signal2;

            Pt::Connection connection1 = connect(signal1, callee, &Callee::slot0);
            signal2 = signal1;

            connection1.close();

            signal2.send();
            PT_UNIT_ASSERT( callee.count() == 1 )
        }

        void DeleteWhileSend()
        {
            connect(*_caller, *this, &SignalTest::slot0);
            connect(*_caller, *this, &SignalTest::deleteCallee );
            connect(*_caller, *this, &SignalTest::slot0);
            connect(*_caller, *this, &SignalTest::deleteCaller);
            connect(*_caller, *this, &SignalTest::slot0);

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

        void slot0()
        { _called0 = true; }

    private:
        bool _called0;
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



void methodTest1()
{
    Receiver* recv = new Receiver;
    Signal<int> signal;

    connect(signal, *recv, &Receiver::onSignal1);
    delete recv;
    signal.send(1);
    if(signal.connections().size() != 0)
        throw std::logic_error("Connections left after disconnect." + PT_SOURCEINFO);

    recv = new Receiver;
    connect(signal, *recv, &Receiver::onSignal1);
    signal.send(1);
    if(recv->called() == false)
        throw std::logic_error("Signal not sent to connected slot." + PT_SOURCEINFO);

    recv->reset();


//    disconnect(signal, *recv, &Receiver::onSignal1);
//    signal.send(1);
//    if(recv->called() == true)
//        throw std::logic_error("Signal sent to disconnected slot." + PT_SOURCEINFO);
//    if(signal.connections().size() != 0)
//        throw std::logic_error("Connections left after disconnect." + PT_SOURCEINFO);


    delete recv;
}


void methodTest2()
{
    Receiver* recv = new Receiver;
    Signal<int, int> signal;

    connect(signal, *recv, &Receiver::onSignal2);
    delete recv;
    signal.send(1,2);
    if(signal.connections().size() != 0)
        throw std::logic_error("Connections left after disconnect." + PT_SOURCEINFO);

    recv = new Receiver;
    connect(signal, *recv, &Receiver::onSignal2);
    signal.send(1,2);
    if(recv->called() == false)
        throw std::logic_error("Signal not sent to connected slot." + PT_SOURCEINFO);

    recv->reset();

//    disconnect(signal, *recv, &Receiver::onSignal2);
//    signal.send(1,2);
//    if(recv->called() == true)
//        throw std::logic_error("Signal sent to disconnected slot." + PT_SOURCEINFO);
//    if(signal.connections().size() != 0)
//        throw std::logic_error("Connections left after disconnect." + PT_SOURCEINFO);

    delete recv;
}


void methodTest3()
{
    Receiver* recv = new Receiver;
    Signal<int, int, int> signal;

    connect(signal, *recv, &Receiver::onSignal3);
    delete recv;
    signal.send(1,2,3);
    if(signal.connections().size() != 0)
        throw std::logic_error("Connections left after disconnect." + PT_SOURCEINFO);

    recv = new Receiver;
    connect(signal, *recv, &Receiver::onSignal3);
    signal.send(1,2,3);
    if(recv->called() == false)
        throw std::logic_error("Signal not sent to connected slot." + PT_SOURCEINFO);

    recv->reset();


//    disconnect(signal, *recv, &Receiver::onSignal3);
//    signal.send(1,2,3);
//    if(recv->called() == true)
//        throw std::logic_error("Signal sent to disconnected slot." + PT_SOURCEINFO);
//    if(signal.connections().size() != 0)
//        throw std::logic_error("Connections left after disconnect." + PT_SOURCEINFO);

    delete recv;
}


void signalTest0()
{
    Receiver* recv = new Receiver;
    Signal<> signal1;
    Signal<> signal2;
    connect( signal1, slot(signal2) );
    connect( signal2, slot(recv, &Receiver::onSignal0) );

    // test that a signal is really transmitted
    signal1.send();
    if(recv->called() == false)
        throw std::logic_error("Signal not sent to connected slot." + PT_SOURCEINFO);

    delete recv;
}


void signalTest1()
{
    Receiver* recv = new Receiver;
    Signal<int> signal1;
    Signal<int> signal2;
    connect( signal1, slot(signal2) );
    connect( signal2, slot(recv, &Receiver::onSignal1) );

    // test that a signal is really transmitted
    signal1.send(1);
    if(recv->called() == false)
        throw std::logic_error("Signal not sent to connected slot." + PT_SOURCEINFO);

    delete recv;
}


void signalTest2()
{
    Receiver* recv = new Receiver;
    Signal<int, int> signal1;
    Signal<int, int> signal2;
    connect( signal1, slot(signal2) );
    connect( signal2, slot(recv, &Receiver::onSignal2) );

    // test that a signal is really transmitted
    signal1.send(1,2);
    if(recv->called() == false)
        throw std::logic_error("Signal not sent to connected slot." + PT_SOURCEINFO);

    delete recv;
}


void signalTest3()
{
    Receiver* recv = new Receiver;
    Signal<int, int, int> signal1;
    Signal<int, int, int> signal2;
    connect( signal1, slot(signal2) );
    connect( signal2, slot(recv, &Receiver::onSignal3) );

    // test that a signal is really transmitted
    signal1.send(1,2,3);
    if(recv->called() == false)
        throw std::logic_error("Signal not sent to connected slot." + PT_SOURCEINFO);

    delete recv;
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


void functionTest1()
{
    testFunction1Called = false;
    Signal<int> signal;

    testFunction1Called = false;
    connect(signal, testFunction1);
    signal.send(1);
    if(testFunction1Called == false)
        throw std::logic_error("Signal not sent to connected slot." + PT_SOURCEINFO);

//    testFunction1Called = false;
//    disconnect(signal, testFunction1);
//    signal.send(1);
//    if(testFunction1Called == true)
//        throw std::logic_error("Signal sent to disconnected slot." + PT_SOURCEINFO);
//    if(signal.connections().size() != 0)
//        throw std::logic_error("Connections left after disconnect." + PT_SOURCEINFO);

}


void functionTest2()
{
    testFunction2Called = false;
    Signal<int, int> signal;

    testFunction2Called = false;
    connect(signal, testFunction2);
    signal.send(1, 2);
    if(testFunction2Called == false)
        throw std::logic_error("Signal not sent to connected slot." + PT_SOURCEINFO);

//    testFunction2Called = false;
//    disconnect(signal, testFunction2);
//    signal.send(1, 2);
//    if(testFunction2Called == true)
//        throw std::logic_error("Signal sent to disconnected slot." + PT_SOURCEINFO);
//    if(signal.connections().size() != 0)
//        throw std::logic_error("Connections left after disconnect." + PT_SOURCEINFO);
}


void functionTest3()
{
    testFunction3Called = false;
    Signal<int, int, int> signal;

    testFunction3Called = false;
    connect(signal, testFunction3);
    signal.send(1, 2, 3);
    if(testFunction3Called == false)
        throw std::logic_error("Signal not sent to connected slot." + PT_SOURCEINFO);

//    testFunction3Called = false;
//    disconnect(signal, testFunction3);
//    signal.send(1, 2, 3);
//    if(testFunction3Called == true)
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


int main(int argc, char* argv[])
{
    std::cerr << "----- SignalTest -----" << std::endl;

    try
    {
        std::cerr << "  DeleteTest:\n";
        DeleteTest delTest;
        delTest();
        std::cerr << "ok." << std::endl;

        std::cerr << "  CopyTest: ";
        CopyTest();
        std::cerr << "ok." << std::endl;

        std::cerr << "  MethodTest0: ";
        methodTest0();
        std::cerr << "ok." << std::endl;

        std::cerr << "  MethodTest1: ";
        methodTest1();
        std::cerr << "ok." << std::endl;

        std::cerr << "  MethodTest2: ";
        methodTest2();
        std::cerr << "ok." << std::endl;

        std::cerr << "  MethodTest3: ";
        methodTest3();
        std::cerr << "ok." << std::endl;

        std::cerr << "  SignalTest0: ";
        signalTest0();
        std::cerr << "ok." << std::endl;

        std::cerr << "  SignalTest1: ";
        signalTest1();

        std::cerr << "ok." << std::endl;
        std::cerr << "  SignalTest2: ";
        signalTest2();
        std::cerr << "ok." << std::endl;

        std::cerr << "  SignalTest3: ";
        signalTest3();
        std::cerr << "ok." << std::endl;

        std::cerr << "  FunctionTest0: ";
        functionTest0();
        std::cerr << "ok." << std::endl;

        std::cerr << "  FunctionTest1: ";
        functionTest1();
        std::cerr << "ok." << std::endl;

        std::cerr << "  FunctionTest2: ";
        functionTest2();
        std::cerr << "ok." << std::endl;

        std::cerr << "  FunctionTest3: ";
        functionTest3();
        std::cerr << "ok." << std::endl;
        //performanceTest();

    }
    catch(const std::exception& e) {
        std::cerr << "failed. " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
*/
