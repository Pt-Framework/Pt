/*
 * Copyright (C) 2005-2006 by Dr. Marc Boris Duerner
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
#ifndef PT_UNIT_TESTMETHOD_H
#define PT_UNIT_TESTMETHOD_H

#include <Pt/Unit/Api.h>
#include <Pt/Unit/Test.h>
#include <Pt/SerializationInfo.h>
#include <stdexcept>
#include <cstddef>

namespace Pt {

namespace Unit {

    class TestMethod : public Pt::Unit::Test
    {
        public:
            TestMethod(const std::string& name)
            : Pt::Unit::Test(name)
            {}

            virtual ~TestMethod()
            {}

            virtual void run()
            {}

            virtual void exec(const SerializationInfo* si, std::size_t argCount) = 0;
    };


    template < class C,
               typename A1 = Pt::Void,
               typename A2 = Pt::Void,
               typename A3 = Pt::Void,
               typename A4 = Pt::Void,
               typename A5 = Pt::Void,
               typename A6 = Pt::Void,
               typename A7 = Pt::Void,
               typename A8 = Pt::Void >
    class BasicTestMethod : public Pt::Method<void, C, A1, A2, A3, A4, A5, A6, A7, A8>
                          , public TestMethod
    {
        public:
            typedef C ClassT;
            typedef void (C::*MemFuncT)(A1, A2, A3, A4, A5, A6, A7, A8);

        public:
            BasicTestMethod(const std::string& name, C& object, MemFuncT ptr)
            : Pt::Method<void, C, A1, A2, A3, A4, A5, A6, A7, A8>(object, ptr)
            , TestMethod(name)
            {}

            void exec(const SerializationInfo* args, std::size_t argCount)
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
    class BasicTestMethod<C,
                          A1,
                          A2,
                          A3,
                          A4,
                          A5,
                          Pt::Void,
                          Pt::Void,
                          Pt::Void> : public Pt::Method<void, C, A1, A2, A3, A4, A5>
                                    , public TestMethod
    {
        public:
            typedef C ClassT;
            typedef void (C::*MemFuncT)(A1, A2, A3, A4, A5);

        public:
            BasicTestMethod(const std::string& name, C& object, MemFuncT ptr)
            : Pt::Method<void, C, A1, A2, A3, A4, A5>(object, ptr)
            , TestMethod(name)
            {}

            void exec(const SerializationInfo* args, std::size_t argCount)
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
    class BasicTestMethod<C,
                          A1,
                          A2,
                          A3,
                          A4,
                          Pt::Void,
                          Pt::Void,
                          Pt::Void,
                          Pt::Void> : public Pt::Method<void, C, A1, A2, A3, A4>
                                    , public TestMethod
    {
        public:
            typedef C ClassT;
            typedef void (C::*MemFuncT)(A1, A2, A3, A4);

        public:
            BasicTestMethod(const std::string& name, C& object, MemFuncT ptr)
            : Pt::Method<void, C, A1, A2, A3, A4>(object, ptr)
            , TestMethod(name)
            {}

            void exec(const SerializationInfo* args, std::size_t argCount)
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
    class BasicTestMethod<C,
                          A1,
                          A2,
                          A3,
                          Pt::Void,
                          Pt::Void,
                          Pt::Void,
                          Pt::Void,
                          Pt::Void> : public Pt::Method<void, C, A1, A2, A3>
                                    , public TestMethod
    {
        public:
            typedef C ClassT;
            typedef void (C::*MemFuncT)(A1, A2, A3);

        public:
            BasicTestMethod(const std::string& name, C& object, MemFuncT ptr)
            : Pt::Method<void, C, A1, A2, A3>(object, ptr)
            , TestMethod(name)
            {}

            void exec(const SerializationInfo* args, std::size_t argCount)
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
    class BasicTestMethod<C,
                          A1,
                          A2,
                          Pt::Void,
                          Pt::Void,
                          Pt::Void,
                          Pt::Void,
                          Pt::Void,
                          Pt::Void> : public Pt::Method<void, C, A1, A2>
                                    , public TestMethod
    {
        public:
            typedef C ClassT;
            typedef void (C::*MemFuncT)(A1, A2);

        public:
            BasicTestMethod(const std::string& name, C& object, MemFuncT ptr)
            : Pt::Method<void, C, A1, A2>(object, ptr)
            , TestMethod(name)
            {}

            void exec(const SerializationInfo* args, std::size_t argCount)
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
    class BasicTestMethod<C,
                          A1,
                          Pt::Void,
                          Pt::Void,
                          Pt::Void,
                          Pt::Void,
                          Pt::Void,
                          Pt::Void,
                          Pt::Void> : public Pt::Method<void, C, A1>
                                    , public TestMethod
    {
        public:
            typedef C ClassT;
            typedef void (C::*MemFuncT)(A1);

        public:
            BasicTestMethod(const std::string& name, C& object, MemFuncT ptr)
            : Pt::Method<void, C, A1>(object, ptr)
            , TestMethod(name)
            {}

            void exec(const SerializationInfo* args, std::size_t argCount)
            {
                if(argCount != 1)
                    throw std::invalid_argument("invalid number of arguments");

                A1 a1 = A1();
                args[0] >>= a1;
                Pt::Method<void, C, A1>::call(a1);
            }
    };


    template < class C >
    class BasicTestMethod<C,
                          Pt::Void,
                          Pt::Void,
                          Pt::Void,
                          Pt::Void,
                          Pt::Void,
                          Pt::Void,
                          Pt::Void,
                          Pt::Void> : public Pt::Method<void, C>
                                    , public TestMethod
    {
        public:
            typedef C ClassT;
            typedef void (C::*MemFuncT)();

        public:
            BasicTestMethod(const std::string& name, C& object, MemFuncT ptr)
            : Pt::Method<void, C>(object, ptr)
            , TestMethod(name)
            {}

            void exec(const SerializationInfo* si, std::size_t argCount)
            {
                Pt::Method<void, C>::call();
            }
    };

} // namespace Unit

} // namespace Pt

#endif // for header

