/***************************************************************************
 *   Copyright (C) 2007 by Marc Boris Dürner                               *
 *   Copyright (C) 2007 by Laurentiu-Gheorghe Crisan                       *
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

#include "Pt/Pt.h"
#include "Pt/Property.h"
#include "Pt/Reflectable.h"

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"


class TestReflectable : public Pt::Reflectable
{
    public:
        TestReflectable()
        : Pt::Reflectable("TestReflectable")
        , intProperty( "intProperty", self() , 20 )
        {
            this->registerProperty("value", this, &TestReflectable::value, &TestReflectable::setValue);

            this->registerMethod("method1", *this, &TestReflectable::method1);
            this->registerMethod("method2", *this, &TestReflectable::method2);
            this->registerMethod("method3", *this, &TestReflectable::method3);
        }

        Pt::Property<int> intProperty;

        void method1(int)
        {}

        void method2(int, bool)
        {}

        void method3(int, bool, char)
        {}

        void setValue(int i)
        { _value = i; }

        int value()
        { return _value; }

    private:
        int _value;

        TestReflectable* self()
        { return this; }
};


class ReflectableTest : public Pt::Unit::TestSuite, public Pt::Connectable
{
    public:
        ReflectableTest()
        : Pt::Unit::TestSuite( "ReflectableTest" )
        , _onValueChanged(false)
        {
            Pt::Unit::TestSuite::registerMethod( "PropertyTest", *this, &ReflectableTest::PropertyTest );
            Pt::Unit::TestSuite::registerMethod( "Method1Test", *this, &ReflectableTest::Method1Test );
            Pt::Unit::TestSuite::registerMethod( "Method2Test", *this, &ReflectableTest::Method2Test );
            Pt::Unit::TestSuite::registerMethod( "Method3Test", *this, &ReflectableTest::Method3Test );
        }

    protected:
        void PropertyTest()
        {
            TestReflectable    refl;
            int    value = 0;
            Pt::Any    any;

            //Test the getter.
            value = refl.intProperty.get();;
            PT_UNIT_ASSERT( value == 20 );

            //Test the setter.
            refl.intProperty.set( 10 );
            value = refl.intProperty.get();;
            PT_UNIT_ASSERT( value == 10 );

            //Test the any setter.
            any = -3;
            refl.intProperty.setValue( any );
            value = refl.intProperty.get();;
            PT_UNIT_ASSERT( value == -3 );

            //Test the any getter
            any = refl.intProperty.value();;
            value = Pt::any_cast<int>(any);
            PT_UNIT_ASSERT( value == -3 );

            //Test the reflection getter.
            any = refl.property("intProperty");
            value = Pt::any_cast<int>(any);
            PT_UNIT_ASSERT( value == -3 );

            //Test the reflection setter.
            any =  5;
            refl.setProperty( "intProperty", any );
            value = refl.intProperty.get();;
            PT_UNIT_ASSERT( value == 5 );

            //Test the property signal "onValueChanged".
            connect( refl.intProperty.onValueChanged, *this, &ReflectableTest::onValueChanged );

            //Test the setter.
            refl.intProperty.set( 2 );
            PT_UNIT_ASSERT( _onValueChanged == true );

            //Test the any setter.
            _onValueChanged = false;
            any = 3;
            refl.intProperty.setValue( any );
            PT_UNIT_ASSERT( _onValueChanged == true);

            //Test the reflection setter.
            _onValueChanged = false;
            any =  1;
            refl.setProperty( "intProperty", any );
            PT_UNIT_ASSERT( _onValueChanged == true );

            std::stringstream ss("123");
            any = refl.property( "intProperty" );
            ss >> any;
            refl.setProperty("intProperty", any);
            PT_UNIT_ASSERT( any == 123 );
        }

        void Method1Test()
        {
            TestReflectable refl;
            PT_UNIT_ASSERT( refl.method("method1").argName(0) == std::string("int") );
        }

        void Method2Test()
        {
            TestReflectable refl;
            PT_UNIT_ASSERT( refl.method("method2").argName(0) == std::string("int") );
            PT_UNIT_ASSERT( refl.method("method2").argName(1) == std::string("bool") );
        }

        void Method3Test()
        {
            TestReflectable refl;
            PT_UNIT_ASSERT( refl.method("method3").argName(0) == std::string("int") );
            PT_UNIT_ASSERT( refl.method("method3").argName(1) == std::string("bool") );
            PT_UNIT_ASSERT( refl.method("method3").argName(2) == std::string("char") );
        }

    private:
        void onValueChanged()
        { _onValueChanged = true; }

        bool _onValueChanged;
};


Pt::Unit::RegisterTest<ReflectableTest> reflectableTest;
