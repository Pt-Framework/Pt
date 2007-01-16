/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#include <iostream>

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"

#include "Pt/SmartPtr.h"
#include "Pt/RefCounted.h"

using namespace Pt;


class Object : public RefCounted
{
	public:
		Object(){ ++objectRefs; }
		~Object(){ --objectRefs; }

		static size_t objectRefs;
};

class Base
{
};

class Child : public Base
{
};

size_t Object::objectRefs = 0;


class SmartPtrTest : public Pt::Unit::TestSuite
{
    public:
        SmartPtrTest()
        : Pt::Unit::TestSuite( "SmartPtrTest" )		
		{
			Pt::Unit::TestSuite::registerMethod( "testRefCounted", *this, &SmartPtrTest::testRefCounted );
			Pt::Unit::TestSuite::registerMethod( "testInternalRefCounted", *this, &SmartPtrTest::testInternalRefCounted );
			Pt::Unit::TestSuite::registerMethod( "testRefLinked", *this, &SmartPtrTest::testRefLinked );
			Pt::Unit::TestSuite::registerMethod( "testBaseChildAssignment", *this, &SmartPtrTest::testBaseChildAssignment );
		}

	public:
		void setUp();

	protected:
		void testRefCounted();
		void testInternalRefCounted();
		void testRefLinked();
        void testBaseChildAssignment();
};

Pt::Unit::RegisterTest<SmartPtrTest> register_SmartPtrTest;


void SmartPtrTest::setUp()
{
	Object::objectRefs = 0;
}


void SmartPtrTest::testRefCounted()
{
	Object* obj = new Object();

	{
		SmartPtr< Object, ExternalRefCounted<Object> > smartPtr(obj);
		PT_UNIT_ASSERT( static_cast< ExternalRefCounted<Object>* >(&smartPtr)->refs() == 1 );

		SmartPtr< Object, ExternalRefCounted<Object> > second(smartPtr);
		PT_UNIT_ASSERT(static_cast< ExternalRefCounted<Object>* >(&second)->refs() == 2);

		SmartPtr< Object, ExternalRefCounted<Object> > third;
		third = second;
		PT_UNIT_ASSERT(static_cast< ExternalRefCounted<Object>* >(&third)->refs() == 3);

		third = third;
		PT_UNIT_ASSERT(static_cast< ExternalRefCounted<Object>* >(&third)->refs() == 3);
	}

	PT_UNIT_ASSERT(Object::objectRefs == 0);
}


void SmartPtrTest::testInternalRefCounted()
{
	Object* obj = new Object();

	{
		SmartPtr<Object, InternalRefCounted<Object> > smartPtr(obj);
		PT_UNIT_ASSERT(obj->refs() == 1);

		SmartPtr<Object, InternalRefCounted<Object> > second(smartPtr);
		PT_UNIT_ASSERT(obj->refs() == 2);

		SmartPtr< Object, InternalRefCounted<Object> > third;
		third = second;
		PT_UNIT_ASSERT(obj->refs() == 3);
	}

	PT_UNIT_ASSERT(Object::objectRefs == 0);
}


void SmartPtrTest::testRefLinked()
{
	Object* obj = new Object();

	{
		SmartPtr<Object, RefLinked<Object> > smartPtr(obj);
		SmartPtr<Object, RefLinked<Object> > second(smartPtr);
		SmartPtr<Object, RefLinked<Object> > third;
		third = second;
	}

	PT_UNIT_ASSERT(Object::objectRefs == 0);
}

void SmartPtrTest::testBaseChildAssignment()
{
    Child* obj = new Child();

    SmartPtr<Child> smartPtr(obj);

   
    SmartPtr<Base>  baseSmartPtr;

    baseSmartPtr = smartPtr;  
}
