/***************************************************************************
 *   Copyright (C) 2006 Aloysius Indrayanto                                *
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
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
#include <iostream>
#include <sstream>
using namespace std;

#include "Pt/Exception.h"
using namespace Pt;

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestFixture.h"
#include "Pt/Unit/TestCase.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/Application.h"


template <typename ExceptionT>
class ExceptionThrower {
	public:
		ExceptionThrower(const string& msg)
		: _msg(msg)
		{}

		~ExceptionThrower()
		{}

		void throwIt() const
		{ throw ExceptionT(_msg, PT_SOURCEINFO); }

	private:
		string _msg;
};

class ExceptionTest : public Pt::Unit::TestCase
{
	public:
		ExceptionTest()
		: TestCase("ExceptionTest")
		{}

		template <typename ExceptionT>
		void testException(const string &msg) const
		{
			stringstream ss;

			ExceptionThrower<ExceptionT> et(msg);

			try {
				et.throwIt();
			}
			catch(const IllegalArgument& e) {
				ss << "Got Pt::IllegalArgument" << endl;
			}
			catch(const RangeError& e) {
				ss << "Got Pt::RangeError" << endl;
			}
			catch(const UnderflowError& e) {
				ss << "Got Pt::UnderflowError" << endl;
			}
			catch(const OverflowError& e) {
				ss << "Got Pt::OverflowError" << endl;
			}
			catch(const LogicError& e) {
				ss << "Got Pt::LogicError" << endl;
			}
			catch(const RuntimeError& e) {
				ss << "Got Pt::RuntimeError" << endl;
			}
			catch(const Exception& e) {
				ss << "Got Pt::Exception" << endl;
			}
			catch(const std::exception& e) {
				ss << "Got std::exception" << endl;
			}
			catch(...) {
				ss << "Got unknown exception" << endl;
			}

			if(!ss.str().empty()) Unit::Application::message( ss.str() );
		}

		void test()
		{
			testException<Exception>("Throwing Pt::Exception");
			testException<RuntimeError>("Throwing Pt::RuntimeError");
			testException<LogicError>("Throwing Pt::LogicError");
			testException<OverflowError>("Throwing Pt::OverflowError");
			testException<UnderflowError>("Throwing Pt::UnderflowError");
			testException<RangeError>("Throwing Pt::RangeError");
			testException<IllegalArgument>("Throwing Pt::IllegalArgument");
		}
};

Pt::Unit::RegisterTest<ExceptionTest> register_ExceptionTest;

