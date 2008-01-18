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

#include <Pt/Unit/Api.h>
#include <Pt/Unit/Test.h>
#include <Pt/Unit/TestProtocol.h>
#include <Pt/Unit/TestContext.h>
#include <Pt/SerializationInfo.h>
#include <map>


namespace Pt {

namespace Unit {

    template < class C,
               typename A1 = Pt::Void,
               typename A2 = Pt::Void,
               typename A3 = Pt::Void,
               typename A4 = Pt::Void,
               typename A5 = Pt::Void,
               typename A6 = Pt::Void,
               typename A7 = Pt::Void,
               typename A8 = Pt::Void >
    class TestMethod : public Pt::Method<void, C, A1, A2, A3, A4, A5, A6, A7, A8>
                     , public Pt::Unit::Test
    {
        public:
            typedef C ClassT;
            typedef void (C::*MemFuncT)(A1, A2, A3, A4, A5, A6, A7, A8);

        public:
            TestMethod(const std::string& name, C& object, MemFuncT ptr)
            : Pt::Method<void, C, A1, A2, A3, A4, A5, A6, A7, A8>(object, ptr)
            , Pt::Unit::Test(name)
            {}

            void run(const SerializationInfo* args, size_t argCount)
            {
                if(argCount != 8)
                    throw std::invalid_argument("invalid number of arguments");

                A1 a1 = A1();
                args[0] >>= a1;

                A2 a2 = A2();
                args[1] >>= a2;

                A3 a3 = A3();
                args[2] >>= a3;

                A4 a4 = A4();
                args[3] >>= a4;

                A5 a5 = A5();
                args[4] >>= a5;

                A6 a6 = A6();
                args[5] >>= a6;

                A7 a7 = A7();
                args[6] >>= a7;

                A8 a8 = A8();
                args[7] >>= a8;

                Pt::Method<void, C>::call(a1, a2, a3, a4, a5, a6, a7, a8);
            }
    };


    template < class C,
               typename A1,
               typename A2,
               typename A3,
               typename A4,
               typename A5>
    class TestMethod<C,
                     A1,
                     A2,
                     A3,
                     A4,
                     A5,
                     Pt::Void,
                     Pt::Void,
                     Pt::Void> : public Pt::Method<void, C, A1, A2, A3, A4, A5>
                               , public Pt::Unit::Test
    {
        public:
            typedef C ClassT;
            typedef void (C::*MemFuncT)(A1, A2, A3, A4, A5);

        public:
            TestMethod(const std::string& name, C& object, MemFuncT ptr)
            : Pt::Method<void, C, A1, A2, A3, A4, A5>(object, ptr)
            , Pt::Unit::Test(name)
            {}

            void run(const SerializationInfo* args, size_t argCount)
            {
                if(argCount != 5)
                    throw std::invalid_argument("invalid number of arguments");

                A1 a1 = A1();
                args[0] >>= a1;

                A2 a2 = A2();
                args[1] >>= a2;

                A3 a3 = A3();
                args[2] >>= a3;

                A4 a4 = A4();
                args[3] >>= a4;

                A5 a5 = A5();
                args[4] >>= a5;

                Pt::Method<void, C, A1, A2, A3, A4, A5>::call(a1, a2, a3, a4, a5);
            }
    };


    template < class C,
               typename A1,
               typename A2,
               typename A3,
               typename A4>
    class TestMethod<C,
                     A1,
                     A2,
                     A3,
                     A4,
                     Pt::Void,
                     Pt::Void,
                     Pt::Void,
                     Pt::Void> : public Pt::Method<void, C, A1, A2, A3, A4>
                               , public Pt::Unit::Test
    {
        public:
            typedef C ClassT;
            typedef void (C::*MemFuncT)(A1, A2, A3, A4);

        public:
            TestMethod(const std::string& name, C& object, MemFuncT ptr)
            : Pt::Method<void, C, A1, A2, A3, A4>(object, ptr)
            , Pt::Unit::Test(name)
            {}

            void run(const SerializationInfo* args, size_t argCount)
            {
                if(argCount != 4)
                    throw std::invalid_argument("invalid number of arguments");

                A1 a1 = A1();
                args[0] >>= a1;

                A2 a2 = A2();
                args[1] >>= a2;

                A3 a3 = A3();
                args[2] >>= a3;

                A4 a4 = A4();
                args[3] >>= a4;

                Pt::Method<void, C, A1, A2, A3, A4>::call(a1, a2, a3, a4);
            }
    };


    template < class C,
               typename A1,
               typename A2,
               typename A3>
    class TestMethod<C,
                     A1,
                     A2,
                     A3,
                     Pt::Void,
                     Pt::Void,
                     Pt::Void,
                     Pt::Void,
                     Pt::Void> : public Pt::Method<void, C, A1, A2, A3>
                               , public Pt::Unit::Test
    {
        public:
            typedef C ClassT;
            typedef void (C::*MemFuncT)(A1, A2, A3);

        public:
            TestMethod(const std::string& name, C& object, MemFuncT ptr)
            : Pt::Method<void, C, A1, A2, A3>(object, ptr)
            , Pt::Unit::Test(name)
            {}

            void run(const SerializationInfo* args, size_t argCount)
            {
                if(argCount != 3)
                    throw std::invalid_argument("invalid number of arguments");

                A1 a1 = A1();
                args[0] >>= a1;

                A2 a2 = A2();
                args[1] >>= a2;

                A3 a3 = A3();
                args[2] >>= a3;

                Pt::Method<void, C, A1, A2, A3>::call(a1, a2, a3);
            }
    };


    template < class C,
               typename A1,
               typename A2>
    class TestMethod<C,
                     A1,
                     A2,
                     Pt::Void,
                     Pt::Void,
                     Pt::Void,
                     Pt::Void,
                     Pt::Void,
                     Pt::Void> : public Pt::Method<void, C, A1, A2>
                               , public Pt::Unit::Test
    {
        public:
            typedef C ClassT;
            typedef void (C::*MemFuncT)(A1, A2);

        public:
            TestMethod(const std::string& name, C& object, MemFuncT ptr)
            : Pt::Method<void, C, A1, A2>(object, ptr)
            , Pt::Unit::Test(name)
            {}

            void run(const SerializationInfo* args, size_t argCount)
            {
                if(argCount != 2)
                    throw std::invalid_argument("invalid number of arguments");

                A1 a1 = A1();
                args[0] >>= a1;

                A2 a2 = A2();
                args[1] >>= a2;

                Pt::Method<void, C, A1, A2>::call(a1, a2);
            }
    };


    template < class C,
               typename A1>
    class TestMethod<C,
                     A1,
                     Pt::Void,
                     Pt::Void,
                     Pt::Void,
                     Pt::Void,
                     Pt::Void,
                     Pt::Void,
                     Pt::Void> : public Pt::Method<void, C, A1>
                               , public Pt::Unit::Test
    {
        public:
            typedef C ClassT;
            typedef void (C::*MemFuncT)(A1);

        public:
            TestMethod(const std::string& name, C& object, MemFuncT ptr)
            : Pt::Method<void, C, A1>(object, ptr)
            , Pt::Unit::Test(name)
            {}

            void run(const SerializationInfo* args, size_t argCount)
            {
                if(argCount != 1)
                    throw std::invalid_argument("invalid number of arguments");

                A1 a1 = A1();
                args[0] >>= a1;
                Pt::Method<void, C, A1>::call(a1);
            }
    };

    template < class C >
    class TestMethod<C,
                     Pt::Void,
                     Pt::Void,
                     Pt::Void,
                     Pt::Void,
                     Pt::Void,
                     Pt::Void,
                     Pt::Void,
                     Pt::Void> : public Pt::Method<void, C>
                               , public Pt::Unit::Test
    {
        public:
            typedef C ClassT;
            typedef void (C::*MemFuncT)();

        public:
            TestMethod(const std::string& name, C& object, MemFuncT ptr)
            : Pt::Method<void, C>(object, ptr)
            , Pt::Unit::Test(name)
            {}

            void run(const SerializationInfo* si, size_t argCount)
            {
                Pt::Method<void, C>::call();
            }
    };

    /** @brief Protocol and data driven testing
        @ingroup UnitTests

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
    class PT_UNIT_API TestSuite : public Test
                                , public TestFixture
    {
        public:
            /** @brief Construct by name and protocol

                Constructs a %TestCase with the passed name and optionally
                a custom protocol. The protocol is not owned by the TestSuite,
                but can be owned by the derived class.

                @param name Name of the test
                @param protocol Protocol for the test.
            */
            TestSuite(const std::string& name, TestProtocol& protocol = TestSuite::defaultProtocol);

            ~TestSuite();

            //! @brief Deprecated
            virtual void setProperty(const std::string& name, const Pt::SerializationInfo& value);

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
            virtual void run(const SerializationInfo* si, size_t argCount);

            /** @brief Runs a registered test

                A test method will be called by name and the given arguments
                are passe to it just like when the reflection API is used.
                The method 'setUp' will be called before, and the method
                tearDown after the test. Signals inherited from Unit::Test
                are sent appropriatly.

                @param name Name of the method to be run
                @param args Arguments to invoke the method
            */
            void runTest( const std::string& name, const SerializationInfo* args = 0, size_t argCount = 0);

            void runAll();

        protected:
            Test* findTest(const std::string& name);

            template <class ParentT>
            void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*method)() )
            {
                Pt::Unit::Test* test = new TestMethod<ParentT>(this->name() + "::" + name, parent, method);
                this->registerTest(test);
            }

            template <class ParentT, typename A1>
            void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*method)(A1) )
            {
                Pt::Unit::Test* test = new TestMethod<ParentT, A1>(this->name() + "::" + name, parent, method);
                this->registerTest(test);
            }

            template <class ParentT, typename A1, typename A2>
            void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*method)(A1, A2) )
            {
                Pt::Unit::Test* test = new TestMethod<ParentT, A1, A2>(this->name() + "::" + name, parent, method);
                this->registerTest(test);
            }

            template <class ParentT, typename A1, typename A2, typename A3>
            void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*method)(A1, A2, A3) )
            {
                Pt::Unit::Test* test = new TestMethod<ParentT, A1, A2, A3>(this->name() + "::" + name, parent, method);
                this->registerTest(test);
            }

            template <class ParentT, typename A1, typename A2, typename A3, typename A4>
            void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*method)(A1, A2, A3, A4) )
            {
                Pt::Unit::Test* test = new TestMethod<ParentT, A1, A2, A3, A4>(this->name() + "::" + name, parent, method);
                this->registerTest(test);
            }

            template <class ParentT, typename A1, typename A2, typename A3, typename A4, typename A5>
            void registerMethod(const std::string& name, ParentT& parent, void (ParentT::*method)(A1, A2, A3, A4, A5) )
            {
                Pt::Unit::Test* test = new TestMethod<ParentT, A1, A2, A3, A4, A5>(this->name() + "::" + name, parent, method);
                this->registerTest(test);
            }

            void registerTest(Test* test);

        private:
            /** @brief The assoziated test protocol
            */
            TestProtocol* _protocol;

            std::multimap<std::string, Test*> _tests;

        public:
            static TestProtocol defaultProtocol;
    };

} // namespace Unit

} // namespace Pt

#endif // for header

