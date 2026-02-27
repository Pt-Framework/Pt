/*
 * Copyright (C) 2005-2010 by Dr. Marc Boris Duerner
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

#include "Pt/Settings.h"
#include "Pt/Date.h"
#include "Pt/TextStream.h"
#include "Pt/Utf8Codec.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include <string>
#include <sstream>

class SettingsTest : public Pt::Unit::TestSuite
{
    public:
        SettingsTest()
        : Pt::Unit::TestSuite("SettingsTest")
        {
            Pt::Unit::TestSuite::registerMethod( "EscapeString", *this, &SettingsTest::EscapeString );
            Pt::Unit::TestSuite::registerMethod( "Comment", *this, &SettingsTest::Comment );

            Pt::Unit::TestSuite::registerMethod( "SimpleValue", *this, &SettingsTest::SimpleValue );
            Pt::Unit::TestSuite::registerMethod( "SimpleTypedValue", *this, &SettingsTest::SimpleTypedValue );
            Pt::Unit::TestSuite::registerMethod( "SimpleQoutedValue", *this, &SettingsTest::SimpleQoutedValue );
            
            Pt::Unit::TestSuite::registerMethod( "SimpleArray", *this, &SettingsTest::SimpleArray );
            Pt::Unit::TestSuite::registerMethod( "SimpleNamedArray", *this, &SettingsTest::SimpleNamedArray );
            Pt::Unit::TestSuite::registerMethod( "SimpleQoutedArray", *this, &SettingsTest::SimpleQoutedArray );
            Pt::Unit::TestSuite::registerMethod( "SimpleTypedArray", *this, &SettingsTest::SimpleTypedArray );
            Pt::Unit::TestSuite::registerMethod( "SimpleArrayQoutedTypedValues", *this, &SettingsTest::SimpleArrayQoutedTypedValues );

            Pt::Unit::TestSuite::registerMethod( "ComplexType", *this, &SettingsTest::ComplexType );
            Pt::Unit::TestSuite::registerMethod( "ComplexTypeNamedQoutedValues", *this, &SettingsTest::ComplexTypeNamedQoutedValues );
            Pt::Unit::TestSuite::registerMethod( "ComplexNamedType", *this, &SettingsTest::ComplexNamedType );

            Pt::Unit::TestSuite::registerMethod( "Section", *this, &SettingsTest::Section );
            Pt::Unit::TestSuite::registerMethod( "ArrayOfArrays", *this, &SettingsTest::ArrayOfArrays );
            Pt::Unit::TestSuite::registerMethod( "ArrayWithBrackets", *this, &SettingsTest::ArrayWithBrackets );
            Pt::Unit::TestSuite::registerMethod( "LoadSaveSerializable", *this, &SettingsTest::LoadSaveSerializable );
            Pt::Unit::TestSuite::registerMethod( "Entry", *this, &SettingsTest::Entry );
            Pt::Unit::TestSuite::registerMethod( "ConstEntry", *this, &SettingsTest::ConstEntry );
            Pt::Unit::TestSuite::registerMethod( "SequenceEntry", *this, &SettingsTest::SequenceEntry );

            Pt::Unit::TestSuite::registerMethod( "EmptyElements", *this, &SettingsTest::EmptyElements );
            Pt::Unit::TestSuite::registerMethod( "FlatValues", *this, &SettingsTest::FlatValues );

            //Pt::Unit::TestSuite::registerMethod( "Writer", *this, &SettingsTest::Writer );
        }

    protected:
        void EmptyElements();
        void FlatValues();
        void EscapeString();
        void Writer();
        void Comment();
        void SimpleValue();
        void SimpleTypedValue();
        void SimpleQoutedValue();
        void SimpleArray();
        void SimpleNamedArray();
        void SimpleQoutedArray();
        void SimpleTypedArray();
        void SimpleArrayQoutedTypedValues();
        void ComplexNamedType();
        void ComplexType();
        void ComplexTypeNamedQoutedValues();
        void Section();
        void ArrayOfArrays();
        void ArrayWithBrackets();
        void LoadSaveSerializable();
        void Entry();
        void ConstEntry();
        void SequenceEntry();
};

Pt::Unit::RegisterTest<SettingsTest> register_SettingsTest;


void SettingsTest::EscapeString()
{
    Pt::String str = "#h\"\\h";
    Pt::Settings settings;
    settings.root().addEntry("str").set(str);

    std::stringstream ss;
    Pt::TextOStream tos(ss, new Pt::Utf8Codec);
    settings.save(tos);
    tos.flush();

    Pt::Settings settings2;
    Pt::TextIStream tis(ss, new Pt::Utf8Codec);
    settings2.load(tis);

    Pt::String str2;
    settings2.entry("str").get(str2);
    PT_UNIT_ASSERT( str2 == str );
}

void SettingsTest::Writer()
{
    Pt::Settings settings;

    std::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);

    std::vector< std::vector<int> > vecvec;
    vecvec.push_back(vec);
    vecvec.push_back(vec);

    settings.makeEntry("s").set("abc");
    settings.makeEntry("b").set(true);
    settings.makeEntry("i").set( int(1));
    settings.makeEntry("u").set( unsigned(1) );
    settings.makeEntry("f").set( float(1) );
    settings.makeEntry("a").set(vec);
    settings.makeEntry("c").set(vecvec);
    settings.makeEntry("d").makeEntry("d1").set(vec);
    settings.makeEntry("d").makeEntry("d2").set(Pt::Date(2001, 11, 15));
    settings.makeEntry("d").makeEntry("d3").set(vec);
    settings.makeEntry("d").makeEntry("d4").set(vecvec);
    settings.makeEntry("d").makeEntry("d5").set(4);

    std::vector<Pt::Date> dates;
    dates.push_back( Pt::Date(2001, 11, 15) );
    dates.push_back( Pt::Date(2001, 11, 16) );
    settings.makeEntry("e").set(dates);

    std::ostringstream ss;
    Pt::TextOStream ts(ss, new Pt::Utf8Codec);
    settings.save(ts);
    ts.flush();

    std::clog << "\n\n"
              << ss.str();
}


void SettingsTest::Entry()
{
    Pt::Settings settings;
    PT_UNIT_ASSERT( settings.begin() == settings.end() );
    PT_UNIT_ASSERT( settings.root().begin() == settings.root().end() );

    Pt::Date date(2001, 11, 15);
    settings.root().addEntry("myEntry").set(date);
    PT_UNIT_ASSERT( settings.begin() != settings.end() );
    PT_UNIT_ASSERT( settings.root().begin() != settings.root().end() );

    Pt::Settings::Entry entry = settings.begin();
    PT_UNIT_ASSERT( 0 == std::strcmp(entry.name(),"myEntry") );

    Pt::Date date2(2000, 1, 1);
    entry.get(date2);
    PT_UNIT_ASSERT( date2.year() == 2001 );
    PT_UNIT_ASSERT( date2.month() == 11 );
    PT_UNIT_ASSERT( date2.day() == 15 );

    int n = 0;
    entry.set(42);
    PT_UNIT_ASSERT( entry.get(n) );
    PT_UNIT_ASSERT( n == 42 );

    settings.root().removeEntry("myEntry");
    PT_UNIT_ASSERT( settings.begin() == settings.end() );
    PT_UNIT_ASSERT( settings.root().begin() == settings.root().end() );

    settings.root().addEntry("aaa");
    settings.root().addEntry("bbb");
    settings.root().addEntry("ccc");
    PT_UNIT_ASSERT( settings.entry("aaa") != settings.end() );
    PT_UNIT_ASSERT( settings.entry("bbb") != settings.end() );
    PT_UNIT_ASSERT( settings.entry("ccc") != settings.end() );

    settings.root().removeEntry("ccc");
    settings.root().removeEntry("bbb");
    settings.root().removeEntry("aaa");
    PT_UNIT_ASSERT( settings.begin() == settings.end() );
    PT_UNIT_ASSERT( settings.root().begin() == settings.root().end() );

    settings.root().addEntry("aaa");
    settings.root().addEntry("bbb");
    settings.root().addEntry("ccc");
    PT_UNIT_ASSERT( settings.entry("aaa") != settings.end() );
    PT_UNIT_ASSERT( settings.entry("bbb") != settings.end() );
    PT_UNIT_ASSERT( settings.entry("ccc") != settings.end() );

    settings.root().removeEntry("aaa");
    settings.root().removeEntry("bbb");
    settings.root().removeEntry("ccc");
    PT_UNIT_ASSERT( settings.begin() == settings.end() );
    PT_UNIT_ASSERT( settings.root().begin() == settings.root().end() );
}

void SettingsTest::ConstEntry()
{
    Pt::Settings s;

    const Pt::Settings& settings = s;
    PT_UNIT_ASSERT( settings.begin() == settings.end() );
    PT_UNIT_ASSERT( settings.root().begin() == settings.root().end() );

    s.root().addEntry("number").set(42);
    PT_UNIT_ASSERT( settings.begin() != settings.end() );
    PT_UNIT_ASSERT( settings.root().begin() != settings.root().end() );

    Pt::Settings::ConstEntry entry = settings.begin();
    PT_UNIT_ASSERT( 0 == std::strcmp(entry.name(),"number") );

    int n = 0;
    PT_UNIT_ASSERT( entry.get(n) );
    PT_UNIT_ASSERT( n == 42 );
}


void SettingsTest::SequenceEntry()
{
    Pt::Settings settings;

    Pt::Settings::Entry list = settings.addEntry("numbers");
    list.addEntry().set(1);
    list.addEntry().set(2);
    list.addEntry().set(3);

    std::vector<int> numbers;
    settings["numbers"].get(numbers);
    PT_UNIT_ASSERT( numbers.size() == 3 );
    PT_UNIT_ASSERT( numbers.at(0) == 1 );
    PT_UNIT_ASSERT( numbers.at(1) == 2 );
    PT_UNIT_ASSERT( numbers.at(2) == 3 );

    std::stringstream ss;
    Pt::TextOStream tos(ss, new Pt::Utf8Codec);
    settings.save(tos);
    tos.flush();
}


void SettingsTest::LoadSaveSerializable()
{
    Pt::Date date(2001, 11, 15);
    Pt::Settings settings;
    settings.root().addEntry("myDate").set(date);

    std::ostringstream ss;
    Pt::TextOStream ts(ss, new Pt::Utf8Codec);
    settings.save(ts);

    Pt::Date date2(2000, 1, 1);
    settings.entry("myDate").get(date2);
    PT_UNIT_ASSERT( date2.year() == 2001 );
    PT_UNIT_ASSERT( date2.month() == 11 );
    PT_UNIT_ASSERT( date2.day() == 15 );
}


void SettingsTest::Comment()
{
    std::stringstream ss;
    ss << ";first comment\n";
    ss << "a = \"1#;2\"\n";
    ss << "#second comment\n";
    ss << "b = 2\n";
    Pt::TextIStream ts(ss, new Pt::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);

    Pt::String a;
    settings.entry("a").get(a);
    PT_UNIT_ASSERT("1#;2" == a );
    
    int b = 0;
    settings.entry("b").get(b);
    PT_UNIT_ASSERT(2 == b );
}

void SettingsTest::ArrayOfArrays()
{
    std::stringstream ss;
    ss << "a={array{1,2,3},array{4,5,6}}\n";
    ss << "b = { { 1 , 2 , 3 } , { 4 , 5 , 6 } }\n";
    Pt::TextIStream ts(ss, new Pt::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);

    std::vector< std::vector<int> > vecOfVecs;
    
    settings.entry("a").get(vecOfVecs);
    PT_UNIT_ASSERT(2 == vecOfVecs.size() );

    settings.entry("b").get(vecOfVecs);
    PT_UNIT_ASSERT(2 == vecOfVecs.size() );
}

void SettingsTest::ArrayWithBrackets()
{
    std::stringstream ss;
    ss << "a=[array[1,2,3],array[4,5,6]]\n";
    ss << "b = [ [ 1 , 2 , 3 ] , [ 4 , 5 , int(6) ] ]\n";
    ss << "c = [ \"dog\", \"cat\", \"mouse\" ]\n";
    Pt::TextIStream ts(ss, new Pt::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);

    std::vector< std::vector<int> > vecOfVecs;
    std::vector<Pt::String> vecOfStr;
    
    settings.entry("a").get(vecOfVecs);
    PT_UNIT_ASSERT(2 == vecOfVecs.size() );

    settings.entry("b").get(vecOfVecs);
    PT_UNIT_ASSERT(2 == vecOfVecs.size() );
    
    settings.entry("c").get(vecOfStr);
    PT_UNIT_ASSERT(3 == vecOfStr.size() );
}


void SettingsTest::SimpleValue()
{
    std::stringstream ss;
    ss << "a = 5\n";
    ss << "b=6\n";
    ss << "c =0x2a\n";
    ss << "d= 053\n";
    ss << "e= 0\n";
    Pt::TextIStream ts(ss, new Pt::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);

    int a = 0;
    settings.entry("a").get(a);
    PT_UNIT_ASSERT(5 == a);

    int b = 0;
    settings.entry("b").get(b);
    PT_UNIT_ASSERT(6 == b);

    int c = 0;
    settings.entry("c").get(c);
    PT_UNIT_ASSERT(0x2a == c);

    int d = 0;
    settings.entry("d").get(d);
    PT_UNIT_ASSERT(053 == d);

    int e = 0;
    settings.entry("e").get(e);
    PT_UNIT_ASSERT(0 == e);
}

void SettingsTest::SimpleTypedValue()
{
    std::stringstream ss;
    ss << "a = int ( 5 )\n";
    ss << "b = int(6)\n";
    Pt::TextIStream ts(ss, new Pt::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);
    
    int a = 0;
    settings.entry("a").get(a);
    PT_UNIT_ASSERT(5 == a );
    
    int b = 0;
    settings.entry("b").get(b);
    PT_UNIT_ASSERT(6 == b );
}

void SettingsTest::SimpleQoutedValue()
{
    std::stringstream ss;
    ss << "a=\"a b c\"\n";
    ss << "b = \"a b c\"\n";
    Pt::TextIStream ts(ss, new Pt::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);
    
    std::string a;
    settings.entry("a").get(a);
    PT_UNIT_ASSERT("a b c" == a );
    
    std::string b;
    settings.entry("b").get(b);
    PT_UNIT_ASSERT("a b c" == b );
}

void SettingsTest::SimpleArray()
{
    std::stringstream ss;
    ss << "a={1,2,3},";
    ss << "b = { 4 , 5 , 6 } \n";
    ss << "c.x = 5\n";
    ss << "c.y = 6\n";

    Pt::TextIStream ts(ss, new Pt::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);

    ss.clear();
    ss.str("");
    Pt::TextOStream to(ss, new Pt::Utf8Codec);
    settings.save(to);

    //std::cerr << ss.str() << std::endl;
    //std::exit(0);
}

void SettingsTest::SimpleNamedArray()
{
    std::stringstream ss;
    ss << "a=array{1,2,3}\n";
    ss << "b = array { 4 , 5 , 6 } \n";
    Pt::TextIStream ts(ss, new Pt::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);
}

void SettingsTest::SimpleQoutedArray()
{
    std::stringstream ss;
    ss << "a={\"1\",\"2\",\"3\"}\n";
    ss << "b = { \"4\" , \"5\" , \"6\" } \n";
    Pt::TextIStream ts(ss, new Pt::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);
}

void SettingsTest::SimpleTypedArray()
{
    std::stringstream ss;
    ss << "a={int(1),int(2),int(3)}\n";
    ss << "b = { int( 4 ) , int( 5 ) , int( 6 ) }\n";
    Pt::TextIStream ts(ss, new Pt::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);
}

void SettingsTest::SimpleArrayQoutedTypedValues()
{
    std::stringstream ss;
    ss << "a={int(\"1\"),int(\"2\"),int(\"3\")}\n";
    ss << "b = { int( \"4\" ) , int( \"5\" ) , int( \"6\" ) }\n";
    Pt::TextIStream ts(ss, new Pt::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);
}

void SettingsTest::ComplexType()
{
    std::stringstream ss;
    ss << "a={red=1,green=2,blue=3}\n";
    ss << "b = { red = 4 , green = 5 , blue = 6 }\n";
    Pt::TextIStream ts(ss, new Pt::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);

    int n = 0;
    PT_UNIT_ASSERT( settings.entry("a").entry("green").get(n) );
    PT_UNIT_ASSERT(2 == n);

    PT_UNIT_ASSERT( settings.entry("b").entry("blue").get(n) );
    PT_UNIT_ASSERT(6 == n);
}

void SettingsTest::ComplexTypeNamedQoutedValues()
{
    std::stringstream ss;
    ss << "a={red=\"1\",green=\"2\",blue=\"3\"}\n";
    ss << "b = { red = \"4\" , green = \"5\" , blue = \"6\" }\n";
    Pt::TextIStream ts(ss, new Pt::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);

    std::string n;
    PT_UNIT_ASSERT( settings.entry("a").entry("green").get(n) );
    PT_UNIT_ASSERT("2" == n);

    PT_UNIT_ASSERT( settings.entry("b").entry("blue").get(n) );
    PT_UNIT_ASSERT_EQUALS("6", n);
}

void SettingsTest::ComplexNamedType()
{
    std::stringstream ss;
    ss << "a=Color{red=int(1),green=int(2),blue=int(3)}\n";
    ss << "b = Color { red = int ( 4 ) , green = int ( 5 ) , blue = int ( 6 ) }";
    ss << "[section]\n";
    Pt::TextIStream ts(ss, new Pt::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);

    int n = 0;
    PT_UNIT_ASSERT( settings.entry("a").entry("green").get(n) );
    PT_UNIT_ASSERT(2 == n);

    PT_UNIT_ASSERT( settings.entry("b").entry("blue").get(n) );
    PT_UNIT_ASSERT(6 == n);
}

void SettingsTest::EmptyElements()
{
    std::stringstream ss;
    ss << "a=[\n";
    ss << "b={},\n";
    ss << "c=[]\n";
    ss << "]\n";
    ss << "x=[]\n";
    ss << "y={}\n";

    Pt::TextIStream ts(ss, new Pt::Utf8Codec);
    Pt::Settings settings;
    settings.load(ts);

    std::vector<int> v;
    PT_UNIT_ASSERT( settings.entry("a").entry("b") );
    PT_UNIT_ASSERT( settings.entry("a").entry("c").get(v) );
    PT_UNIT_ASSERT( v.empty() );

    PT_UNIT_ASSERT( settings.entry("x") );
    PT_UNIT_ASSERT( settings.entry("x").get(v) );
    PT_UNIT_ASSERT( v.empty() );

    PT_UNIT_ASSERT( settings.entry("y") );
}

void SettingsTest::FlatValues()
{
    std::stringstream ss;
    ss << "a=5 b=\"b\", c=[1] d={2}, z=1, ";
    Pt::TextIStream ts(ss, new Pt::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);

    int a = 0;
    settings.entry("a").get(a);
    PT_UNIT_ASSERT(a == 5);

    std::string b;
    settings.entry("b").get(b);
    PT_UNIT_ASSERT(b == "b");

    std::vector<int> c;
    settings.entry("c").get(c);
    PT_UNIT_ASSERT(c.size() == 1);

    std::vector<int> d;
    settings.entry("d").get(d);
    PT_UNIT_ASSERT(d.size() == 1);
}

void SettingsTest::Section()
{
    std::stringstream ss;
    ss << "[a.b.c]\n";
    ss << "d.v = 1\n";
    ss << "d.u = 2\n";
    ss << "[x.y.z]\n";
    ss << "u.v = 3\n";
    ss << "u.w = 4\n";
    ss << "[e.f.g]\n";
    ss << "u.v = 5\n";
    ss << "u.w = 6\n";
    Pt::TextIStream ts(ss, new Pt::Utf8Codec);

    Pt::Settings settings;
    settings.load(ts);

    PT_UNIT_ASSERT( settings.entry("a.b.c.d") );
    
    int v = 0;
    settings.entry("a.b.c.d")->entry("v").get(v);
    PT_UNIT_ASSERT( v == 1);
    
    int u = 0;
    settings.entry("a.b.c.d")->entry("u").get(u);
    PT_UNIT_ASSERT( u == 2);

    PT_UNIT_ASSERT( settings.entry("x.y.z.u") );
    
    settings.entry("x.y.z.u")->entry("v").get(v);
    PT_UNIT_ASSERT( v == 3);
    
    int w;
    settings.entry("x.y.z.u")->entry("w").get(w);
    PT_UNIT_ASSERT( w == 4);

    PT_UNIT_ASSERT( settings.entry("e.f.g.u") );
    
    settings.entry("e.f.g.u")->entry("v").get(v);
    PT_UNIT_ASSERT( v == 5);
    
    settings.entry("e.f.g.u")->entry("w").get(w);
    PT_UNIT_ASSERT( w == 6);
}
