/***************************************************************************
 *   Copyright (C) 2006 by Tommi Mäkitalo                                  *
 *   Copyright (C) 2006 by Marc Boris Duerner                               *
 *   Copyright (C) 2006 by Stefan Bueder                                    *
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
            Pt::Unit::TestSuite::registerMethod( "RefCounted", *this, &SmartPtrTest::RefCounted );
            Pt::Unit::TestSuite::registerMethod( "InternalRefCounted", *this, &SmartPtrTest::InternalRefCounted );
            Pt::Unit::TestSuite::registerMethod( "RefLinked", *this, &SmartPtrTest::RefLinked );
            Pt::Unit::TestSuite::registerMethod( "BaseChildAssignment", *this, &SmartPtrTest::BaseChildAssignment );
        }

    public:
        void setUp();

    protected:
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


void SmartPtrTest::RefCounted()
{
    Object* obj = new Object();

    {
        Pt::SmartPtr< Object, Pt::ExternalRefCounted<Object> > smartPtr(obj);
        PT_UNIT_ASSERT( static_cast< Pt::ExternalRefCounted<Object>* >(&smartPtr)->refs() == 1 );

        Pt::SmartPtr< Object, Pt::ExternalRefCounted<Object> > second(smartPtr);
        PT_UNIT_ASSERT(static_cast< Pt::ExternalRefCounted<Object>* >(&second)->refs() == 2);

        Pt::SmartPtr< Object, Pt::ExternalRefCounted<Object> > third;
        third = second;
        PT_UNIT_ASSERT(static_cast< Pt::ExternalRefCounted<Object>* >(&third)->refs() == 3);

        third = third;
        PT_UNIT_ASSERT(static_cast< Pt::ExternalRefCounted<Object>* >(&third)->refs() == 3);
    }

    PT_UNIT_ASSERT(Object::objectRefs == 0);
}


void SmartPtrTest::InternalRefCounted()
{
    Object* obj = new Object();

    {
        Pt::SmartPtr<Object, Pt::InternalRefCounted<Object> > smartPtr(obj);
        PT_UNIT_ASSERT(obj->refs() == 1);

        Pt::SmartPtr<Object, Pt::InternalRefCounted<Object> > second(smartPtr);
        PT_UNIT_ASSERT(obj->refs() == 2);

        Pt::SmartPtr< Object, Pt::InternalRefCounted<Object> > third;
        third = second;

        PT_UNIT_ASSERT(obj->refs() == 3);
    }

    PT_UNIT_ASSERT(Object::objectRefs == 0);
}


void SmartPtrTest::RefLinked()
{
    Object* obj = new Object();

    {
        Pt::SmartPtr<Object, Pt::RefLinked<Object> > smartPtr(obj);
        Pt::SmartPtr<Object, Pt::RefLinked<Object> > second(smartPtr);
        Pt::SmartPtr<Object, Pt::RefLinked<Object> > third;
        third = second;
    }

    PT_UNIT_ASSERT(Object::objectRefs == 0);
}


void SmartPtrTest::BaseChildAssignment()
{
    //Child* obj = new Child();
    //Pt::SmartPtr<Child> smartPtr(obj);
    //Pt::SmartPtr<Base>  baseSmartPtr;
    //baseSmartPtr = smartPtr;
}
