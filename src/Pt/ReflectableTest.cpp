/***************************************************************************
 *   Copyright (C) 2007 by Marc Boris Drner                               *
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
#include "Pt/Reflectable.h"

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"

/*#define PT_PROPERTY( name, Name, type )       \
                                                  \
class Property##name : public Pt::Property<type>       \
        {                                         \
            public:                               \
                Property##name()                  \
                : Pt::Property<type>("number")             \
                {}                                \
        };                                        \
         void set##Name(type x)            \
        { _##name.set(x); }          \
                                                  \
        type name () const             \
        { return _##name.get(); }                 \
                                                  \
        Property##name _number;            \
*/


class TestReflectable : public Pt::Reflectable
{
    public:
        TestReflectable()
        : Pt::Reflectable("TestReflectable")
        {
            this->registerProperty("number", this, _number, &TestReflectable::setNumber);
            this->registerProperty("count", this, _number );

            this->registerMethod("method1", *this, &TestReflectable::method1);
            this->registerMethod("method2", *this, &TestReflectable::method2);
            this->registerMethod("method3", *this, &TestReflectable::method3);
        }

        void method1(int) const
        {}

        void method2(int, bool)
        {}

        void method3(int, bool, char)
        {}

        void setNumber(int i)
        { _number.set(i); }

        int number() const
        { return _number.get(); }

    private:
        Pt::PropertyValue<int> _number;

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
            Pt::Unit::TestSuite::registerMethod( "Property", *this, &ReflectableTest::Property );
            Pt::Unit::TestSuite::registerMethod( "Method1", *this, &ReflectableTest::Method1 );
            Pt::Unit::TestSuite::registerMethod( "Method2", *this, &ReflectableTest::Method2 );
            Pt::Unit::TestSuite::registerMethod( "Method3", *this, &ReflectableTest::Method3 );
        }

    protected:
        void Property()
        {
            TestReflectable reflectable;

            reflectable.setProperty("number", Pt::Any( int(5) ) );
            Pt::Any number = reflectable.property("number");
            PT_UNIT_ASSERT( number == 5 )
        }

        void Method1()
        {
            TestReflectable refl;
            PT_UNIT_ASSERT( refl.method("method1").argName(0) == std::string("int") );
        }

        void Method2()
        {
            TestReflectable refl;
            PT_UNIT_ASSERT( refl.method("method2").argName(0) == std::string("int") );
            PT_UNIT_ASSERT( refl.method("method2").argName(1) == std::string("bool") );
        }

        void Method3()
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
