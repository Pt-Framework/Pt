/***************************************************************************
 *   Copyright (C) 2005-2006 by Dr. Marc Boris Duerner                     *
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
#ifndef PT_UNIT_TESTSUITE_H
#define PT_UNIT_TESTSUITE_H

#include <map>
#include <Pt/Unit/Test.h>
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestFixture.h>


namespace Pt {

namespace Unit {

    class TestSuite;


    class PT_EXPORT TestProtocol
    {
        public:
            virtual ~TestProtocol()
            {}

            virtual void run(TestSuite& test);
    };


    class TestSuite : public Test, public TestFixture
    {
        public:
            TestSuite(const std::string& name, TestProtocol& protocol = TestSuite::defaultProtocol)
            : Test(name)
            , _protocol(&protocol)
            { }

            virtual void run()
            {
                _protocol->run(*this);
            }

            void runTest( const std::string& name, const Args& args = Args() )
            {
                // TODO: use a sentry object
                bool isUp = false;

                Test::started.send<const Test&>( *this );
                try
                {
                    this->setUp();
                    isUp = true;
                    Reflectable::call(name, args);
                    this->tearDown();
                    Test::success.send<const Test&>( *this );
                    return;
                }
                catch(const Assertion& assertion)
                {
                    Test::assertion.send<const Test&>(*this, assertion);
                }
                catch(const std::exception& ex)
                {
                    Test::exception.send<const Test&>(*this, ex);
                }
                catch(...)
                {
                    Test::error.send<const Test&>(*this);
                }

                try
                {
                    if(isUp)
                    {
                        this->tearDown();
                    }
                }
                catch(const Assertion& assertion)
                {
                    Test::assertion.send<const Test&>(*this, assertion);
                }
                catch(const std::exception& ex)
                {
                    Test::exception.send<const Test&>(*this, ex);
                }
                catch(...)
                {
                    Test::error.send<const Test&>(*this);
                }

                Test::finished.send<const Test&>( *this );
            }

        protected:
            TestProtocol* _protocol;

        public:
            static TestProtocol defaultProtocol;
    };

    TestProtocol TestSuite::defaultProtocol;


    inline void TestProtocol::run(TestSuite& test)
    {
        const MethodMap& methods = test.methods();
        MethodMap::const_iterator it;
        for(it = methods.begin(); it != methods.end(); ++it)
        {
            test.runTest( it->first, Args() );
        }
    }

    class ListedProtocol : public TestProtocol
    {
        public:
            void includeTest(const std::string& testName)
            {
                _items.insert( std::make_pair(testName, new Args()) );
            }

            template <typename A1>
            void includeTest(const std::string& testName, A1 a1)
            {
                Args* args = new Args();
                args->push_back(a1);
                _items.insert( std::make_pair(testName, args) );
            }

            void run(TestSuite& suite)
            {
                std::multimap<std::string, Args*>::iterator it;
                for(it = _items.begin(); it != _items.end(); ++it)
                {
                    suite.runTest( it->first, *it->second );
                }
            }

        private:
            std::multimap<std::string, Args*> _items;
    };

} // namespace Unit

} // namespace Pt

#endif // for header

