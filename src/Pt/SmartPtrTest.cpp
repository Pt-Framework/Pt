/*
 * Copyright (C) 2006 by Tommi Maekitalo
 * Copyright (C) 2006 by Marc Boris Duerner
 * Copyright (C) 2006 by Stefan Bueder
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
#undef PT_API_EXPORT

#include "Pt/SmartPtr.h"
#include "Pt/RefCounted.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"


class Object : public Pt::RefCounted
{
    public:
        Object()
        { ++objectRefs; }

        ~Object()
        { --objectRefs; }

        static std::size_t objectRefs;
};

std::size_t Object::objectRefs = 0;


class Base
{
};


class Child : public Base
{
};


class SmartPtrTest : public Pt::Unit::TestSuite
{
    public:
        SmartPtrTest()
        : Pt::Unit::TestSuite( "SmartPtrTest" )
        {
            Pt::Unit::TestSuite::registerMethod( "AutpPtr", *this, &SmartPtrTest::AutoPtr );
            Pt::Unit::TestSuite::registerMethod( "RefCounted", *this, &SmartPtrTest::RefCounted );
            Pt::Unit::TestSuite::registerMethod( "InternalRefCounted", *this, &SmartPtrTest::InternalRefCounted );
            Pt::Unit::TestSuite::registerMethod( "RefLinked", *this, &SmartPtrTest::RefLinked );
            Pt::Unit::TestSuite::registerMethod( "BaseChildAssignment", *this, &SmartPtrTest::BaseChildAssignment );
        }

    public:
        void setUp();

    protected:
        void AutoPtr();
        void RefCounted();
        void InternalRefCounted();
        void RefLinked();
        void BaseChildAssignment();
};

Pt::Unit::RegisterTest<SmartPtrTest> register_SmartPtrTest;


void SmartPtrTest::setUp()
{
    Object::objectRefs = 0;
}


void SmartPtrTest::AutoPtr()
{
    Object* obj = new Object();

    typedef Pt::AutoPtr<Object> Ptr;

    {
        Ptr ap(obj);
        PT_UNIT_ASSERT_EQUALS(ap.get(), obj);

        Ptr ap2(ap);
        PT_UNIT_ASSERT_EQUALS(ap.get(), 0);
        PT_UNIT_ASSERT_EQUALS(ap2.get(), obj);

        PT_UNIT_ASSERT_EQUALS(Object::objectRefs, 1);
    }

    PT_UNIT_ASSERT_EQUALS(Object::objectRefs, 0);
}


void SmartPtrTest::RefCounted()
{
    Object* obj = new Object();

    typedef Pt::SmartPtr< Object, Pt::ExternalRefCounted> Ptr;

    {
        Ptr smartPtr(obj);
        PT_UNIT_ASSERT_EQUALS( smartPtr.refs(), 1 );

        Ptr second(smartPtr);
        PT_UNIT_ASSERT_EQUALS( second.refs(), 2);

        Ptr third;
        third = second;
        PT_UNIT_ASSERT_EQUALS( third.refs(), 3);

        third = third;
        PT_UNIT_ASSERT_EQUALS( third.refs(), 3);
    }

    PT_UNIT_ASSERT_EQUALS(Object::objectRefs, 0);
}


void SmartPtrTest::InternalRefCounted()
{
    Object* obj = new Object();

    typedef Pt::SmartPtr<Object, Pt::InternalRefCounted> Ptr;

    {
        Ptr smartPtr(obj);
        PT_UNIT_ASSERT_EQUALS(obj->refs(), 1);

        Ptr second(smartPtr);
        PT_UNIT_ASSERT_EQUALS(obj->refs(), 2);

        Ptr third;
        third = second;

        PT_UNIT_ASSERT_EQUALS(obj->refs(), 3);
    }

    PT_UNIT_ASSERT_EQUALS(Object::objectRefs, 0);
}


void SmartPtrTest::RefLinked()
{
    Object* obj = new Object();

    typedef Pt::SmartPtr<Object, Pt::RefLinked> Ptr;

    {
        Ptr smartPtr(obj);
        Ptr second(smartPtr);
        Ptr third;
        Ptr fourth(third);

        third = second;
    }

    PT_UNIT_ASSERT_EQUALS(Object::objectRefs, 0);
}


void SmartPtrTest::BaseChildAssignment()
{
    //Child* obj = new Child();
    //Pt::SmartPtr<Child> smartPtr(obj);
    //Pt::SmartPtr<Base>  baseSmartPtr;
    //baseSmartPtr = smartPtr;
}
