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

#include <Pt/Singleton.h>
#include <Pt/Unit/Api.h>
#include <Pt/Unit/Test.h>
#include <Pt/Unit/Assertion.h>
#include <Pt/Unit/TestProtocol.h>
#include <Pt/Unit/TestContext.h>

namespace Pt {

namespace Unit {

    /** @brief Protocol and data driven testing

        The TestSuite is used to implement protocol and data driven tests.
        It inherits its ability to register methods and properties from
        %Reflectable. The implementor is supposed to write and register the
        required test methods on construction.

        @code
            class MyTest : public TestSuite
            {
                public:
                    MyTest()
                    : TestSuite("MyTest")
                    {
                        this->registerMethod("test1", *this, &MyTest::test1);
                    }

                    void test1();
            };
        @endcode

        Once the test is written it can be registered to an application by
        using the RegisterTest class template.

        The default protocol will run each registered test method when the
        test is run. Before each test method setUp is called and tearDown
        after each test. The TestProtocol can be replaced with a customised
        one and reflection can be used to call any method multiple times with
        the required data.
    */
    class PT_UNIT_API TestSuite : public Reflex::Reflectable, public Test
    {
        public:
            class Context : public TestContext
            {
                public:
                    Context(TestSuite& suite, const std::string& name,
                            const Any* args = 0, size_t argCount = 0)
                    : TestContext(suite)
                    , _suite(suite)
                    , _methodName( name )
                    , _args(args)
                    , _argCount(argCount)
                    , _testName( _suite.name() + "::" + name )
                    , _setUp(false)
                    { }

                    virtual ~Context()
                    {
                        try
                        {
                            if( _setUp )
                                _suite.tearDown();
                        }
                        catch(...)
                        {}

                        _suite.finished.send(*this);
                    }

                    const std::string& testName() const
                    { return _testName; }

                protected:
                    void _run()
                    {
                        _suite.setUp();
                        _setUp = true;
                        _suite.call(_methodName, _args, _argCount);
                    }

                private:
                    TestSuite& _suite;
                    std::string _methodName;
                    const Any* _args ;
                    size_t _argCount;
                    std::string _testName;
                    bool _setUp;
            };

        public:
            /** @brief Construct by name and protocol

                Constructs a %TestCase with the passed name and optionally
                a custom protocol. The protocol is not owned by the TestSuite,
                but can be owned by the derived class.

                @param name Name of the test
                @param protocol Protocol for the test.
            */
            TestSuite(const std::string& name, TestProtocol& protocol = TestSuite::defaultProtocol)
            : Reflectable("Pt.Unit.TestSuite")
            , Test(name)
            , _protocol(&protocol)
            { }

            /** @brief Sets the protocol.
                @param protocol Protocol for the test
            */
            void setProtocol(TestProtocol* protocol);

            /** \brief Set up conText before running a test.

                This function is called before each registered tester function
                is invoked. It is meant to initialize any required resources.
            */
            virtual void setUp();

            /** \brief Clean up after the test run.

                This function is called after each registered tester function
                is invoked. It is meant to remove any resources previously
                initialized in TestSuite::setUp.
            */
            virtual void tearDown();

            /** @brief Runs the test suite

                The TestProtocol assosiated with the test will be executed.
                The default protocol will simply call all registered tests.
            */
            virtual void run();

            /** @brief Runs a registered test

                A test method will be called by name and the given arguments
                are passe to it just like when the reflection API is used.
                The method 'setUp' will be called before, and the method
                tearDown after the test. Signals inherited from Unit::Test
                are sent appropriatly.

                @param name Name of the method to be run
                @param args Arguments to invoke the method
            */
            void runTest( const std::string& name, const Any* args, size_t argCount );

            void runTest( const std::string& name, const SerializationInfo* args, size_t argCount);

            void runTest( const std::string& name)
            {
                const Any* a = 0;
                this->runTest(name, a, 0);
            }

        protected:
            /** @brief The assoziated test protocol
            */
            TestProtocol* _protocol;

            typedef void (*Deserialize)(const SerializationInfo&, Any&);

            std::map<std::string, Deserialize> _deserializers;

            template <typename T>
            void deserialize(const Pt::SerializationInfo& si, Any& any)
            {
                T value = T();
                si >>= value;
                any = value;
            }

            template <class ParentT>
            void registerTest(const std::string& name, ParentT& parent, void (ParentT::*method)() )
            {
                this->registerMethod(name, parent, method);
            }

            template <class ParentT, typename A1>
            void registerTest(const std::string& name, ParentT& parent, void (ParentT::*method)(A1) )
            {
                _deserializers[ TypeTraits<A1>::typeName() ] =  &deserialize<A1>;
                this->registerMethod(name, parent, method);
            }

            template <class ParentT, typename A1, typename A2>
            void registerTest(const std::string& name, ParentT& parent, void (ParentT::*method)(A1, A2) )
            {
                _deserializers[ TypeTraits<A1>::typeName() ] =  &deserialize<A1>;
                _deserializers[ TypeTraits<A2>::typeName() ] =  &deserialize<A2>;
                this->registerMethod(name, parent, method);
            }

            template <class ParentT, typename A1, typename A2, typename A3>
            void registerTest(const std::string& name, ParentT& parent, void (ParentT::*method)(A1, A2, A3) )
            {
                _deserializers[ TypeTraits<A1>::typeName() ] =  &deserialize<A1>;
                _deserializers[ TypeTraits<A2>::typeName() ] =  &deserialize<A2>;
                _deserializers[ TypeTraits<A3>::typeName() ] =  &deserialize<A3>;
                this->registerMethod(name, parent, method);
            }

            template <class ParentT, typename A1, typename A2, typename A3, typename A4>
            void registerTest(const std::string& name, ParentT& parent, void (ParentT::*method)(A1, A2, A3, A4) )
            {
                _deserializers[ TypeTraits<A1>::typeName() ] =  &deserialize<A1>;
                _deserializers[ TypeTraits<A2>::typeName() ] =  &deserialize<A2>;
                _deserializers[ TypeTraits<A3>::typeName() ] =  &deserialize<A3>;
                _deserializers[ TypeTraits<A4>::typeName() ] =  &deserialize<A4>;
                this->registerMethod(name, parent, method);
            }

            template <class ParentT, typename A1, typename A2, typename A3, typename A4, typename A5>
            void registerTest(const std::string& name, ParentT& parent, void (ParentT::*method)(A1, A2, A3, A4, A5) )
            {
                _deserializers[ TypeTraits<A1>::typeName() ] =  &deserialize<A1>;
                _deserializers[ TypeTraits<A2>::typeName() ] =  &deserialize<A2>;
                _deserializers[ TypeTraits<A3>::typeName() ] =  &deserialize<A3>;
                _deserializers[ TypeTraits<A4>::typeName() ] =  &deserialize<A4>;
                _deserializers[ TypeTraits<A5>::typeName() ] =  &deserialize<A5>;
                this->registerMethod(name, parent, method);
            }

        public:
            static TestProtocol defaultProtocol;
    };

} // namespace Unit

} // namespace Pt

#endif // for header

