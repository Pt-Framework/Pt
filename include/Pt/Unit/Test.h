/***************************************************************************
 *   Copyright (C) 2005-2006 by Marc Boris Dürner                          *
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
#ifndef PT_UNIT_TEST_H
#define PT_UNIT_TEST_H

#include <string>
#include <Pt/Signal.h>
#include <Pt/NonCopyable.h>
#include <Pt/Reflectable.h>


namespace Pt {

namespace Unit {	
	
	class TestContext;
	
    /** @brief Test base class

        This is the base class for all types of tests that can be registered
        and run in a test application. It provides a virtual method run that
        is overriden by the derived classes and signals to inform about
        events that occur while the test is run.
    */
    class Test : protected NonCopyable
    {
        public:
            /** @brief Destructor
            */
            virtual ~Test()
            { }

            /** @brief Runs the test

                Derived test classes are supposed to implement this method
                to run the test procedure. A derived class should send the 
                'started' signal at the begin of the test and send the 
                'finished' signal at the end of the test. If the test was 
                successful, the 'success' signal is sent, otheriwse one of the
                signals indicating a failrue. In case of a failed assertion,
                the signal 'assertion' is sent, if a regular std::exception was
                the cause of the error the signal 'exception' is sent and and
                the signal 'error' indicates an unknown exception or error. 
                This method should not propagate any exceptions
            */
            virtual void run() = 0;

			const std::string& name() const
			{ return _name; }
			
            /** @brief Start notification

                This signal is sent when the test has started.
                TODO: use TestContext as paramater
            */
            Signal<const TestContext&> started;

            /** @brief Finished notification

                This signal is sent when the test finished. It does not
                indicate that the test was successful.
            */
            Signal<const Test&> finished;

            /** @brief Success notification

                This signal is sent when the test was successful.
            */
            Signal<const Test&> success;

            /** @brief Assertion notification

                This signal is sent when a assertion failed.
            */
            Signal<const Test&, const Assertion&> assertion;

            /** @brief Exception notification

                This signal is sent when a regular std::exception occured.
            */
            Signal<const Test&, const std::exception&> exception;

            /** @brief Error notification

                This signal is sent when an unknown error occured.
            */
            Signal<const Test&> error;

            /** @brief Message notification

                This signal can be sent to report informational messages.
            */
            Signal<const std::string&> message;

        protected:
            /** @brief Construct a test by name

                This ctor is almost never called by the user directly, but 
                rather from derived classes' initialization lists, which 
                pass the name of the test.

                @param name Name of the test
            */
            Test(const std::string& name)
            : _name(name)
            { }
            
        private:
			std::string _name;
    };


	class PT_API TestContext
	{
		public:
			virtual ~TestContext()
			{
				_test.finished.send<const Test&>( _test ); 
			}
			
			virtual const std::string& testName() const
			{ 
				static const std::string unknown = "unknown";
				return unknown;
			}
 
			void run()
			{
				try
				{
					_test.started.send<const TestContext&>( *this );
					this->_run();
					_test.success.send<const Test&>( _test );
				}
				catch(const Assertion& assertion)
				{
					_test.assertion.send<const Test&>(_test, assertion);
				}
				catch(const std::exception& ex)
				{
					_test.exception.send<const Test&>(_test, ex);
				}
				catch(...)
				{
					_test.error.send<const Test&>(_test);
				}
			}
			
		protected:
			virtual void _run()
			{}

			TestContext(Test& test)
			: _test(test)
			{}
			
		private:
			Test& _test;
	};
	
} // namespace Unit

} // namespace Pt

#endif
