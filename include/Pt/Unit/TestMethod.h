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
#include <Pt/TypeTraits.h>
#include <Pt/Void.h>
#include <Pt/Method.h>
#include <stdexcept>
#include <cstddef>

namespace Pt {

namespace Unit {

    class TestMethod : public Pt::Unit::Test
    {
        public:
            TestMethod(const std::string& name)
            : Pt::Unit::Test(name)
            , _args(0)
            , _argCount(0)
            {}

            virtual ~TestMethod()
            {}

            void setArgs(const SerializationInfo* si, std::size_t argCount)
            {
                _args = si;
                _argCount = argCount;
            }

            const SerializationInfo* args() const
            { return _args; }

            std::size_t argCount() const
            { return _argCount; }

        private:
            const SerializationInfo* _args;
            std::size_t _argCount;
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

            typedef typename TypeTraits<A1>::Value V1;
            typedef typename TypeTraits<A2>::Value V2;
            typedef typename TypeTraits<A3>::Value V3;
            typedef typename TypeTraits<A4>::Value V4;
            typedef typename TypeTraits<A5>::Value V5;
            typedef typename TypeTraits<A6>::Value V6;
            typedef typename TypeTraits<A7>::Value V7;
            typedef typename TypeTraits<A8>::Value V8;

        public:
            BasicTestMethod(const std::string& name, C& object, MemFuncT ptr)
            : Pt::Method<void, C, A1, A2, A3, A4, A5, A6, A7, A8>(object, ptr)
            , TestMethod(name)
            {}

            void run()
            {
                const SerializationInfo* args = this->args();
                std::size_t argCount = this->argCount();

                if(argCount != 8)
                    throw std::invalid_argument("invalid number of arguments");

                V1 v1 = V1();
                args[0] >>= v1;

                V2 v2 = V2();
                args[1] >>= v2;

                V3 v3 = V3();
                args[2] >>= v3;

                V4 v4 = V4();
                args[3] >>= v4;

                V5 v5 = V5();
                args[4] >>= v5;

                V6 v6 = V6();
                args[5] >>= v6;

                V7 v7 = V7();
                args[6] >>= v7;

                V8 v8 = V8();
                args[7] >>= v8;

                Pt::Method<void, C>::call(v1, v2, v3, v4, v5, v6, v7, v8);
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

            typedef typename TypeTraits<A1>::Value V1;
            typedef typename TypeTraits<A2>::Value V2;
            typedef typename TypeTraits<A3>::Value V3;
            typedef typename TypeTraits<A4>::Value V4;
            typedef typename TypeTraits<A5>::Value V5;

        public:
            BasicTestMethod(const std::string& name, C& object, MemFuncT ptr)
            : Pt::Method<void, C, A1, A2, A3, A4, A5>(object, ptr)
            , TestMethod(name)
            {}

            void run()
            {
                const SerializationInfo* args = this->args();
                std::size_t argCount = this->argCount();

                if(argCount != 5)
                    throw std::invalid_argument("invalid number of arguments");

                V1 v1 = V1();
                args[0] >>= v1;

                V2 v2 = V2();
                args[1] >>= v2;

                V3 v3 = V3();
                args[2] >>= v3;

                V4 v4 = V4();
                args[3] >>= v4;

                V5 v5 = V5();
                args[4] >>= v5;

                Pt::Method<void, C, A1, A2, A3, A4, A5>::call(v1, v2, v3, v4, v5);
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

            typedef typename TypeTraits<A1>::Value V1;
            typedef typename TypeTraits<A2>::Value V2;
            typedef typename TypeTraits<A3>::Value V3;
            typedef typename TypeTraits<A4>::Value V4;

        public:
            BasicTestMethod(const std::string& name, C& object, MemFuncT ptr)
            : Pt::Method<void, C, A1, A2, A3, A4>(object, ptr)
            , TestMethod(name)
            {}

            void run()
            {
                const SerializationInfo* args = this->args();
                std::size_t argCount = this->argCount();

                if(argCount != 4)
                    throw std::invalid_argument("invalid number of arguments");

                V1 v1 = V1();
                args[0] >>= v1;

                V2 v2 = V2();
                args[1] >>= v2;

                V3 v3 = V3();
                args[2] >>= v3;

                V4 v4 = V4();
                args[3] >>= v4;

                Pt::Method<void, C, A1, A2, A3, A4>::call(v1, v2, v3, v4);
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

            typedef typename TypeTraits<A1>::Value V1;
            typedef typename TypeTraits<A2>::Value V2;
            typedef typename TypeTraits<A3>::Value V3;

        public:
            BasicTestMethod(const std::string& name, C& object, MemFuncT ptr)
            : Pt::Method<void, C, A1, A2, A3>(object, ptr)
            , TestMethod(name)
            {}

            void run()
            {
                const SerializationInfo* args = this->args();
                std::size_t argCount = this->argCount();

                if(argCount != 3)
                    throw std::invalid_argument("invalid number of arguments");

                V1 v1 = V1();
                args[0] >>= v1;

                V2 v2 = V2();
                args[1] >>= v2;

                V3 v3 = V3();
                args[2] >>= v3;

                Pt::Method<void, C, A1, A2, A3>::call(v1, v2, v3);
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

            typedef typename TypeTraits<A1>::Value V1;
            typedef typename TypeTraits<A2>::Value V2;

        public:
            BasicTestMethod(const std::string& name, C& object, MemFuncT ptr)
            : Pt::Method<void, C, A1, A2>(object, ptr)
            , TestMethod(name)
            {}

            void run()
            {
                const SerializationInfo* args = this->args();
                std::size_t argCount = this->argCount();

                if(argCount != 2)
                    throw std::invalid_argument("invalid number of arguments");

                V1 v1 = V1();
                args[0] >>= v1;

                V2 v2 = V2();
                args[1] >>= v2;

                Pt::Method<void, C, A1, A2>::call(v1, v2);
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

            typedef typename TypeTraits<A1>::Value V1;

        public:
            BasicTestMethod(const std::string& name, C& object, MemFuncT ptr)
            : Pt::Method<void, C, A1>(object, ptr)
            , TestMethod(name)
            {}

            void run()
            {
                const SerializationInfo* args = this->args();
                std::size_t argCount = this->argCount();

                if(argCount != 1)
                    throw std::invalid_argument("invalid number of arguments");

                V1 v1 = V1();
                args[0] >>= v1;
                Pt::Method<void, C, A1>::call(v1);
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

            void run()
            {
                Pt::Method<void, C>::call();
            }
    };

} // namespace Unit

} // namespace Pt

#endif // for header

