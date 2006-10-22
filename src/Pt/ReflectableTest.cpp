#include "Pt/Reflectable.h"
#include "Pt/Property.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestFixture.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
using namespace Pt;

class ReflactableObject : public Reflectable
{
    public:
		ReflactableObject()
		: aProperty( "aProperty", me() , 20 )
		{ } 
    
		ValueProperty<int> aProperty;    

	private:
		//By using "this" in ctor produce unter VC compiler a warnning.
		ReflactableObject* me()
		{ return this; }	
};


class ReflectableTest : public Pt::Unit::TestSuite, public Connectable
{
	public:
		ReflectableTest()
		: Pt::Unit::TestSuite( "ReflectableTest" )
		, _onValueChanged(false)
		{
			Unit::TestSuite::registerTest( callable(this, &ReflectableTest::test), "test" );
		}

	protected:	
		void test()
		{
			ReflactableObject	test; 	
			int					value;
			Any					any;
       
			//Test the getter.
			value = test.aProperty.get();;
			PT_UNIT_ASSERT( value == 20 );
			
			//Test the setter.
			test.aProperty.set( 10 );
			value = test.aProperty.get();;
			PT_UNIT_ASSERT( value == 10 );

			//Test the any setter.
			any = -3;
			test.aProperty.setValue( any );
			value = test.aProperty.get();;
			PT_UNIT_ASSERT( value == -3 );

			//Test the any getter
			any = test.aProperty.value();;
			value = any_cast<int>(any);
			PT_UNIT_ASSERT( value == -3 );

			//Test the reflection getter. 
			any = test.property("aProperty");
			value = any_cast<int>(any);
			PT_UNIT_ASSERT( value == -3 );
			
			//Test the reflection setter.
			any =  5;
			test.setProperty( "aProperty", any );
			value = test.aProperty.get();;
			PT_UNIT_ASSERT( value == 5 );

			//Test the property signal "onValueChanged".
			connect( test.aProperty.onValueChanged, slot(this, &ReflectableTest::onValueChanged) );
			
			//Test the setter.
			test.aProperty.set( 2 );
			PT_UNIT_ASSERT( _onValueChanged == true );

			//Test the any setter.
			_onValueChanged = false;
			any = 3;
			test.aProperty.setValue( any );
			PT_UNIT_ASSERT( _onValueChanged == true );

			//Test the reflection setter.
			_onValueChanged = false;
			any =  1;
			test.setProperty( "aProperty", any );
			PT_UNIT_ASSERT( _onValueChanged == true );
		}
	
	private:
		void onValueChanged()
		{ _onValueChanged = true; }

		bool _onValueChanged;
};


Pt::Unit::RegisterTest<ReflectableTest> reflectableTest;