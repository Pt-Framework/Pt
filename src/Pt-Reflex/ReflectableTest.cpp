/***************************************************************************
 *   Copyright (C) 2007 by Marc Boris Drner                                *
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
#undef PT_REFLEX_API_EXPORT

#include "Pt/Reflex/Reflectable.h"
#include "Pt/SerializationInfo.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/RegisterTest.h"

using namespace Pt::Reflex;

class ClassDef;


template <typename T>
class ClassTraits
{
    public:
        static void construct(Reflectable&, T& instance )
        { }
};


class ClassDef
{
    typedef Reflectable* (*Create)(ClassDef const *);

    public:
        ClassDef(ClassDef const* base, Create c)
        : _base(base)
        , _create(c)
        { }

        Reflectable* create() const
        {
            return _create(this);
        }

        ClassDef const* base() const
        { return _base; }

    private:
        ClassDef const * const _base;
        Create _create;
};


template <typename C>
class StaticObject : public Reflectable
{
    public:
        StaticObject()
        : Reflectable("")
        {
            _object = new C;
            ClassTraits<C>::construct(*this, *_object);
        }

    private:
        C* _object;
};


class DynamicObject : public Reflectable
{
    public:
        DynamicObject(ClassDef const* def)
        : Reflectable("")
        {
            if( def->base() )
            {
                _base = def->base()->create();
                this->include(*_base);
            }
        }

    private:
        Reflectable* _base;
};


/** Static classes
*/
template <class T>
Reflectable* createStaticObject(ClassDef const* def)
{
    return new StaticObject<T>();
}


template <typename T>
ClassDef staticClass()
{
    return ClassDef(0, &createStaticObject<T>);
}


/** Meta classes
*/
template <class T>
static Reflectable* createMetaObject(ClassDef const* cdef)
{
    return new T;
}


template <typename T>
ClassDef metaClass()
{
    return ClassDef( 0, &createMetaObject<T> );
}


/** Dynamic classes
*/
Reflectable* createDynamicObject(ClassDef const* def)
{
    return new DynamicObject(def);
}


ClassDef dynamicClass(ClassDef const* base)
{
    return ClassDef(base, &createDynamicObject);
}


class MyClass
{
    public:
        MyClass()
        : _number(3)
        {}

        int number() const
        { return _number; }

        void setNumber(int n)
        {
            std::cerr << "Setting number: " << n << std::endl;
            _number = n;
        }

    private:
        int _number;
};


template <>
class ClassTraits<MyClass>
{
    public:
        static void construct(Pt::Reflex::Reflectable& refl, MyClass& mc)
        {
            refl.registerProperty("number", mc, &MyClass::number, &MyClass::setNumber);
        }
};


class SignalInfo
{
    public:
        SignalInfo(Pt::Signal<>& signal)
        : _signal(&signal)
        {}

        void send(const Pt::Any* args, size_t argCount)
        {
            _signal->send();
        }

        Pt::Connection connect(CallableInfo& ci)
        {
            if(ci.argSize() != 0)
            {
                throw std::invalid_argument("Incompatible slot" + PT_SOURCEINFO);
            }

            Pt::Slot* slot = ci.createSlot();
            return Pt::Connection(*_signal, slot);
        }

    private:
        Pt::Signal<>* _signal;
};


class TestReflectable : public Pt::Reflex::Reflectable
{
    public:
        TestReflectable()
        : Pt::Reflex::Reflectable("TestReflectable")
        {
            this->registerProperty("number", *this, _number, &TestReflectable::setNumber);
            this->registerReadProperty("count", *this, _number );

            this->registerMethod("method0", *this, &TestReflectable::method0);
            this->registerMethod("method1", *this, &TestReflectable::method1);
            this->registerMethod("method2", *this, &TestReflectable::method2);
            this->registerMethod("method3", *this, &TestReflectable::method3);
        }

        int method0()
        { std::cerr << "XXX method 0 called." << std::endl;
            return 0;
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
        Pt::Reflex::PropertyValue<int> _number;
};


class ReflectableTest : public Pt::Unit::TestSuite, public Pt::Connectable
{
    public:
        ReflectableTest()
        : Pt::Unit::TestSuite( "ReflectableTest" )
        {
            Pt::Unit::TestSuite::registerMethod( "Property", *this, &ReflectableTest::Property );
            /*Pt::Unit::TestSuite::registerMethod( "SerializeProperty", *this, &ReflectableTest::SerializeProperty );
            Pt::Unit::TestSuite::registerMethod( "PropertyIterator", *this, &ReflectableTest::PropertyIterator );
            Pt::Unit::TestSuite::registerMethod( "ConstPropertyIterator", *this, &ReflectableTest::ConstPropertyIterator );
            Pt::Unit::TestSuite::registerMethod( "Method1", *this, &ReflectableTest::Method1 );
            Pt::Unit::TestSuite::registerMethod( "Method2", *this, &ReflectableTest::Method2 );
            Pt::Unit::TestSuite::registerMethod( "Method3", *this, &ReflectableTest::Method3 );*/
        }

    protected:
        void Property()
        {
            TestReflectable reflectable;

            std::cerr << "\n######################"<< std::endl;
            /*MyClass mc;
            ClassInfo ci(mc);
            ci.setProperty("number", 5);

            ClassInfo ci2(reflectable);*/

            ClassDef meta = metaClass<TestReflectable>();
            ClassDef stat = staticClass<MyClass>();
            ClassDef dyna = dynamicClass(&stat);

            Reflectable* rd = dyna.create();
            Pt::Any number = rd->property("number");
            std::cerr << "Number is: " <<  Pt::any_cast<int>(number) << std::endl;

            Pt::Signal<> sig;
            SignalInfo si(sig);

            Pt::Reflex::Reflectable* rm = meta.create();
            CallableInfo& ci = rm->methodInfo("method0");
            si.connect(ci);

            si.send(0,0);
            std::cerr << "\n######################"<< std::endl;

            /*
            TestReflectable reflectable;

            reflectable.setProperty("number", Pt::Any( int(5) ) );
            Pt::Any number = reflectable.property("count");
            PT_UNIT_ASSERT( number == 5 )
            */
        }

        void SerializeProperty()
        {
            TestReflectable reflectable;

            reflectable.setProperty("number", Pt::Any( int(5) ) );

            Pt::SerializationInfo si;
            si <<= static_cast<Pt::Reflex::Reflectable&>(reflectable);

            TestReflectable reflectable2;

            si >>= static_cast<Pt::Reflex::Reflectable&>(reflectable2);

            PT_UNIT_ASSERT( reflectable2.property("count") == 5 );
            PT_UNIT_ASSERT( reflectable2.property("number") == 5 );
        }

        void PropertyIterator()
        {
            TestReflectable reflectable;

            Reflectable::PropertyIterator it = reflectable.propertiesBegin();
            PT_UNIT_ASSERT( it != reflectable.propertiesEnd() )
            PT_UNIT_ASSERT( it->name() == std::string("number") )

            ++it;
            PT_UNIT_ASSERT( it != reflectable.propertiesEnd() )
            PT_UNIT_ASSERT( it->name() == std::string("count") )

            ++it;
            PT_UNIT_ASSERT( it == reflectable.propertiesEnd() );
        }

        void ConstPropertyIterator()
        {
            const TestReflectable reflectable;

            Reflectable::ConstPropertyIterator it = reflectable.propertiesBegin();
            PT_UNIT_ASSERT( it != reflectable.propertiesEnd() )
            PT_UNIT_ASSERT( it->name() == std::string("number") )

            ++it;
            PT_UNIT_ASSERT( it != reflectable.propertiesEnd() )
            PT_UNIT_ASSERT( it->name() == std::string("count") )

            ++it;
            PT_UNIT_ASSERT( it == reflectable.propertiesEnd() );
        }

        void Method1()
        {
            TestReflectable refl;
            PT_UNIT_ASSERT( refl.methodInfo("method1").argName(0) == std::string("int") );
        }

        void Method2()
        {
            TestReflectable refl;
            PT_UNIT_ASSERT( refl.methodInfo("method2").argName(0) == std::string("int") );
            PT_UNIT_ASSERT( refl.methodInfo("method2").argName(1) == std::string("bool") );
        }

        void Method3()
        {
            TestReflectable refl;
            PT_UNIT_ASSERT( refl.methodInfo("method3").argName(0) == std::string("int") );
            PT_UNIT_ASSERT( refl.methodInfo("method3").argName(1) == std::string("bool") );
            PT_UNIT_ASSERT( refl.methodInfo("method3").argName(2) == std::string("char") );
        }
};


Pt::Unit::RegisterTest<ReflectableTest> reflectableTest;
