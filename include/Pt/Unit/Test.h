/***************************************************************************
 *   Copyright (C) 2005-2008 by Dr. Marc Boris Duerner                     *
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

#include <Pt/Unit/Api.h>
#include <Pt/Unit/Reporter.h>
#include <Pt/Unit/Assertion.h>
#include <Pt/Connectable.h>
#include <Pt/NonCopyable.h>
#include <string>

namespace Pt {

namespace Unit {

    class TestContext;

    /** @brief Test base class
        @ingroup UnitTests

        This is the base class for all types of tests that can be registered
        and run in a test application. It provides a virtual method run that
        is overriden by the derived classes and signals to inform about
        events that occur while the test is run.
    */
    class PT_UNIT_API Test : public Connectable,
                             protected NonCopyable
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

            const std::string& name() const;

            /** @brief Reports the start of a test
            */
            virtual void started(const TestContext& ctx)
            {
                std::list<Reporter*>::iterator it;
                for(it = _reporter.begin(); it != _reporter.end(); ++it)
                {
                    (*it)->started(ctx);
                }

                if(_parent)
                    _parent->started(ctx);
            }

            /** @brief Finished notification

                This signal is sent when the test finished. It does not
                indicate that the test was successful.
            */
            void finished(const TestContext& ctx)
            {
                std::list<Reporter*>::iterator it;
                for(it = _reporter.begin(); it != _reporter.end(); ++it)
                {
                    (*it)->finished(ctx);
                }

                if(_parent)
                    _parent->finished(ctx);
            }

            /** @brief Success notification

                This signal is sent when the test was successful.
            */
            void success(const TestContext& ctx)
            {
                std::list<Reporter*>::iterator it;
                for(it = _reporter.begin(); it != _reporter.end(); ++it)
                {
                    (*it)->success(ctx);
                }

                if(_parent)
                    _parent->success(ctx);
            }

            /** @brief Assertion notification

                This signal is sent when a assertion failed.
            */
            void assertion(const TestContext& ctx, const Assertion& ass)
            {
                std::list<Reporter*>::iterator it;
                for(it = _reporter.begin(); it != _reporter.end(); ++it)
                {
                    (*it)->assertion(ctx, ass);
                }

                if(_parent)
                    _parent->assertion(ctx, ass);
            }
            /** @brief Exception notification

                This signal is sent when a regular std::exception occured.
            */
            void exception(const TestContext& ctx, const std::exception& ex)
            {
                std::list<Reporter*>::iterator it;
                for(it = _reporter.begin(); it != _reporter.end(); ++it)
                {
                    (*it)->exception(ctx, ex);
                }

                if(_parent)
                    _parent->exception(ctx, ex);
            }
            /** @brief Error notification

                This signal is sent when an unknown error occured.
            */
            void error(const TestContext& ctx)
            {
                std::list<Reporter*>::iterator it;
                for(it = _reporter.begin(); it != _reporter.end(); ++it)
                {
                    (*it)->error(ctx);
                }

                if(_parent)
                    _parent->error(ctx);
            }
            /** @brief Message notification

                This signal can be sent to report informational messages.
            */
            void message(const std::string& msg)
            {
                std::list<Reporter*>::iterator it;
                for(it = _reporter.begin(); it != _reporter.end(); ++it)
                {
                    (*it)->message(msg);
                }

                if(_parent)
                    _parent->message(msg);
            }

            void setParent(Test* test)
            {
                _parent = test;
            }

            Test* parent()
            {
                return _parent;
            }

            const Test* parent() const
            {
                return _parent;
            }

            /** @brief Add reporter for test events

                Adds the reporter \a r to report test events. The caller 
                owns the reporter and must make sure it lives as long as 
                the test.
            */
            void addReporter(Reporter& r)
            {
                connect(r.destroyed, *this, &Test::removeReporter);
                _reporter.push_back(&r);
            }

            void removeReporter(Reporter& r)
            {
                _reporter.remove(&r);
            }

        protected:
            /** @brief Construct a test by name
                @param name Name of the test
            */
            Test(const std::string& name)
            : _name(name)
            , _parent(0)
            { }

        private:
            std::string _name;
            Test* _parent;
            std::list<Reporter*> _reporter;
    };

} // namespace Unit

} // namespace Pt

#endif
