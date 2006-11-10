/***************************************************************************
 *   Copyright (C) 2005 by Dr. Marc Boris Dürner                           *
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
#include "Pt/Signal.h"
#include "Pt/Exception.h"
using namespace Pt;

#include <string>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <ctime>
using namespace std;


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


class Receiver : public Connectable {
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

		cerr << "\n------- Performance Test --------" << endl;

		Signal<> signal;
		connect(signal, receiver, &Receiver::onSignal0);
		cerr << "Signal benchmark... ";
		begin = clock();
		for(long i = 0; i < 100000000; ++i) {
			signal();
		}
		end = clock();
		cerr << "Duration: " << (end - begin) << endl;

		cerr << "mem_fun_t benchmark... ";
		begin = clock();
		mem_fun_t<void, Receiver> mf( &Receiver::onSignal0 );
		for(long i = 0; i < 100000000; ++i) {
			mf(&receiver);
		}
		end = clock();
		cerr << "Duration: " << (end - begin) << endl;
		cerr << "---------------------------------\n\n";
}
void CopyTest()
{
	Receiver* recv = new Receiver;
	Signal<> signal1;
	Signal<> signal2;

	Connection connection = connect(signal1, *recv, &Receiver::onSignal0);
	signal2 = signal1;

	connection.close();

	signal2.send();
	if(recv->called() == false)
		throw Pt::Exception("Signal not sent to connected slot.", PT_SOURCEINFO);

	delete recv;
}


void methodTest0()
{
	Receiver* recv = new Receiver;
	Signal<> signal;
	connect( signal, slot(recv, &Receiver::onSignal0) );

	// test that a deleted receiver removes itself from a sender
	delete recv;
	signal.send();
	if(signal.connections().size() != 0)
		throw Pt::Exception("Connections left after disconnect.", PT_SOURCEINFO);

	recv = new Receiver;
	Connection connection = connect(signal, slot(recv, &Receiver::onSignal0) );

	// test that a signal is really transmitted
	signal.send();
	if(recv->called() == false)
		throw Pt::Exception("Signal not sent to connected slot.", PT_SOURCEINFO);

	recv->reset();

	// test disconnecting a signal/slot
	connection.close();
	signal.send();
	if(recv->called() == true)
		throw Pt::Exception("Signal sent to disconnected slot.", PT_SOURCEINFO);
	if(signal.connections().size() != 0)
		throw Pt::Exception("Connections left after disconnect.", PT_SOURCEINFO);

	delete recv;
}


void methodTest1()
{
	Receiver* recv = new Receiver;
	Signal<int> signal;

	connect(signal, *recv, &Receiver::onSignal1);
	delete recv;
	signal.send(1);
	if(signal.connections().size() != 0)
		throw Pt::Exception("Connections left after disconnect.", PT_SOURCEINFO);

	recv = new Receiver;
	connect(signal, *recv, &Receiver::onSignal1);
	signal.send(1);
	if(recv->called() == false)
		throw Pt::Exception("Signal not sent to connected slot.", PT_SOURCEINFO);

	recv->reset();

/*
	disconnect(signal, *recv, &Receiver::onSignal1);
	signal.send(1);
	if(recv->called() == true)
		throw Pt::Exception("Signal sent to disconnected slot.", PT_SOURCEINFO);
	if(signal.connections().size() != 0)
		throw Pt::Exception("Connections left after disconnect.", PT_SOURCEINFO);
*/

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
		throw Pt::Exception("Connections left after disconnect.", PT_SOURCEINFO);

	recv = new Receiver;
	connect(signal, *recv, &Receiver::onSignal2);
	signal.send(1,2);
	if(recv->called() == false)
		throw Pt::Exception("Signal not sent to connected slot.", PT_SOURCEINFO);

	recv->reset();

/*
	disconnect(signal, *recv, &Receiver::onSignal2);
	signal.send(1,2);
	if(recv->called() == true)
		throw Pt::Exception("Signal sent to disconnected slot.", PT_SOURCEINFO);
	if(signal.connections().size() != 0)
		throw Pt::Exception("Connections left after disconnect.", PT_SOURCEINFO);
*/
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
		throw Pt::Exception("Connections left after disconnect.", PT_SOURCEINFO);

	recv = new Receiver;
	connect(signal, *recv, &Receiver::onSignal3);
	signal.send(1,2,3);
	if(recv->called() == false)
		throw Pt::Exception("Signal not sent to connected slot.", PT_SOURCEINFO);

	recv->reset();

/*
	disconnect(signal, *recv, &Receiver::onSignal3);
	signal.send(1,2,3);
	if(recv->called() == true)
		throw Pt::Exception("Signal sent to disconnected slot.", PT_SOURCEINFO);
	if(signal.connections().size() != 0)
		throw Pt::Exception("Connections left after disconnect.", PT_SOURCEINFO);
*/

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
		throw Pt::Exception("Signal not sent to connected slot.", PT_SOURCEINFO);

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
		throw Pt::Exception("Signal not sent to connected slot.", PT_SOURCEINFO);

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
		throw Pt::Exception("Signal not sent to connected slot.", PT_SOURCEINFO);

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
		throw Pt::Exception("Signal not sent to connected slot.", PT_SOURCEINFO);

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
		throw Pt::Exception("Signal not sent to connected slot.", PT_SOURCEINFO);

/*
	testFunction0Called = false;
	disconnect(signal, testFunction0);
	signal.send();
	if(testFunction0Called == true)
		throw Pt::Exception("Signal sent to disconnected slot.", PT_SOURCEINFO);
	if(signal.connections().size() != 0)
		throw Pt::Exception("Connections left after disconnect.", PT_SOURCEINFO);
*/
}


void functionTest1()
{
	testFunction1Called = false;
	Signal<int> signal;

	testFunction1Called = false;
	connect(signal, testFunction1);
	signal.send(1);
	if(testFunction1Called == false)
		throw Pt::Exception("Signal not sent to connected slot.", PT_SOURCEINFO);
/*
	testFunction1Called = false;
	disconnect(signal, testFunction1);
	signal.send(1);
	if(testFunction1Called == true)
		throw Pt::Exception("Signal sent to disconnected slot.", PT_SOURCEINFO);
	if(signal.connections().size() != 0)
		throw Pt::Exception("Connections left after disconnect.", PT_SOURCEINFO);
*/
}


void functionTest2()
{
	testFunction2Called = false;
	Signal<int, int> signal;

	testFunction2Called = false;
	connect(signal, testFunction2);
	signal.send(1, 2);
	if(testFunction2Called == false)
		throw Pt::Exception("Signal not sent to connected slot.", PT_SOURCEINFO);
/*
	testFunction2Called = false;
	disconnect(signal, testFunction2);
	signal.send(1, 2);
	if(testFunction2Called == true)
		throw Pt::Exception("Signal sent to disconnected slot.", PT_SOURCEINFO);
	if(signal.connections().size() != 0)
		throw Pt::Exception("Connections left after disconnect.", PT_SOURCEINFO);
*/
}


void functionTest3()
{
	testFunction3Called = false;
	Signal<int, int, int> signal;

	testFunction3Called = false;
	connect(signal, testFunction3);
	signal.send(1, 2, 3);
	if(testFunction3Called == false)
		throw Pt::Exception("Signal not sent to connected slot.", PT_SOURCEINFO);
/*
	testFunction3Called = false;
	disconnect(signal, testFunction3);
	signal.send(1, 2, 3);
	if(testFunction3Called == true)
		throw Pt::Exception("Signal sent to disconnected slot.", PT_SOURCEINFO);
	if(signal.connections().size() != 0)
		throw Pt::Exception("Connections left after disconnect.", PT_SOURCEINFO);
*/
}


class DeleteTest : public Pt::Connectable
{
public:
	Signal<> signal;

	DeleteTest()
	: _caller(0), _callee(0)
	{}

	~DeleteTest()
	{
		//cerr << "SelfDisconnectTest::~SelfDisconnectTest" << endl;
	}

	void deleteCaller()
	{
		//cerr << "SelfDisconnectTest::deleteCaller" << endl;
		delete _caller;
		_caller = 0;
	}


	void deleteCallee()
	{
		//cerr << "SelfDisconnectTest::deleteCallee" << endl;
		delete _callee;
		_callee = 0;
	}

	void operator()()
	{
		_caller = new Signal<>;
		_callee = new DeleteTest;

		connect(*_caller, *this, &DeleteTest::deleteCallee );
		_caller->send();

		connect(*_caller, *this, &DeleteTest::deleteCaller);
		_caller->send();
	}

	Signal<>* _caller;
	DeleteTest* _callee;
};



int main()
{
	cerr << "----- SignalTest -----" << endl;

	try
	{
		cerr << "  DeleteTest: ";
		DeleteTest delTest;
		delTest();
		cerr << "ok." << endl;

		cerr << "  CopyTest: ";
		CopyTest();
		cerr << "ok." << endl;

		cerr << "  MethodTest0: ";
		methodTest0();
		cerr << "ok." << endl;

		cerr << "  MethodTest1: ";
		methodTest1();
		cerr << "ok." << endl;

		cerr << "  MethodTest2: ";
		methodTest2();
		cerr << "ok." << endl;

		cerr << "  MethodTest3: ";
		methodTest3();
		cerr << "ok." << endl;

		cerr << "  SignalTest0: ";
		signalTest0();
		cerr << "ok." << endl;

		cerr << "  SignalTest1: ";
		signalTest1();

		cerr << "ok." << endl;
		cerr << "  SignalTest2: ";
		signalTest2();
		cerr << "ok." << endl;

		cerr << "  SignalTest3: ";
		signalTest3();
		cerr << "ok." << endl;

		cerr << "  FunctionTest0: ";
		functionTest0();
		cerr << "ok." << endl;

		cerr << "  FunctionTest1: ";
		functionTest1();
		cerr << "ok." << endl;

		cerr << "  FunctionTest2: ";
		functionTest2();
		cerr << "ok." << endl;

		cerr << "  FunctionTest3: ";
		functionTest3();
		cerr << "ok." << endl;

		performanceTest();

	}
	catch(const std::exception& e) {
		cerr << "failed. " << e.what() << endl;
		return 1;
	}

	return 0;
}

