/*
   Copyright (C) 2015-2023 by Dr. Marc Boris Duerner
  
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   
   As a special exception, you may use this file as part of a free
   software library without restriction. Specifically, if other files
   instantiate templates or use macros or inline functions from this
   file, or you compile this file and link it with other files to
   produce an executable, this file does not by itself cause the
   resulting executable to be covered by the GNU General Public
   License. This exception does not however invalidate any other
   reasons why the executable file might be covered by the GNU Library
   General Public License.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
   MA 02110-1301 USA
*/

#include <Pt/Unit/TestSuite.h>
#include <Pt/Unit/TestMain.h>
#include <Pt/Unit/RegisterTest.h>

#include <Pt/Json/Document.h>
//#include <Pt/Json/StartDocument.h>
#include <Pt/Json/EndDocument.h>
#include <Pt/Json/Null.h>
#include <Pt/Json/Boolean.h>
#include <Pt/Json/String.h>
#include <Pt/Json/Integer.h>
#include <Pt/Json/Float.h>
#include <Pt/Json/StartObject.h>
#include <Pt/Json/Member.h>
#include <Pt/Json/EndObject.h>
#include <Pt/Json/StartArray.h>
#include <Pt/Json/EndArray.h>

#include <Pt/Json/JsonReader.h>

#include <Pt/Composer.h>
#include <Pt/StringStream.h>

#include <vector>
#include <cmath>

class JsonReaderTest : public Pt::Unit::TestSuite
{
  public:
    JsonReaderTest()
    : Pt::Unit::TestSuite("JsonReaderTest")
    {
      this->registerMethod("EmptyDocument", *this, &JsonReaderTest::EmptyDocument);
      this->registerMethod("Float", *this, &JsonReaderTest::Float);
      this->registerMethod("SimpleArray", *this, &JsonReaderTest::SimpleArray);
      this->registerMethod("SimpleObject", *this, &JsonReaderTest::SimpleObject);
    }

    void EmptyDocument();
    void Float();
    void SimpleArray();
    void SimpleObject();
};


Pt::Unit::RegisterTest<JsonReaderTest> register_PtJsonTest;


void JsonReaderTest::EmptyDocument()
{
  using namespace Pt;
  using namespace Pt::Json;

  Pt::String s = "{}";
  IStringStream iss(s);
  
  Document doc;
  doc.load(iss);
}


void JsonReaderTest::Float()
{
    using namespace Pt;
    using namespace Pt::Json;

    Pt::String s = "{\"a\":1.1,\"b\":[2.1, 2.2],\"c\":3.3}";
    IStringStream iss(s);

    Document doc;
    doc.load(iss);

    double a = 0;
    doc["a"].getValue(a);
    PT_UNIT_ASSERT( std::abs(a - 1.1) < 0.01 );

    std::vector<double> b;
    doc["b"].getValue(b);
    PT_UNIT_ASSERT( std::abs(b[0] - 2.1) < 0.01 );
    PT_UNIT_ASSERT( std::abs(b[1] - 2.2) < 0.01 );

    double c = 0;
    doc["c"].getValue(c);
    PT_UNIT_ASSERT( std::abs(c - 3.3) < 0.01 );
}


void JsonReaderTest::SimpleArray()
{
    using namespace Pt;
    using namespace Pt::Json;

    Pt::String s = "[\"a\",\"b\",\"c\"]";
    IStringStream iss(s);

    Document doc;
    doc.load(iss);

    std::vector<std::string> vec;
    doc.root().getValue(vec);
    PT_UNIT_ASSERT_EQUAL(vec[0], "a");
    PT_UNIT_ASSERT_EQUAL(vec[1], "b");
    PT_UNIT_ASSERT_EQUAL(vec[2], "c");
}


void JsonReaderTest::SimpleObject()
{
    using namespace Pt;
    using namespace Pt::Json;

    //Pt::String s = "{\"a\":\"abc\",\"o\":{\"x\":[\"y\", 1, 2.1], \"f\":3.14},\"i\":123, \"b\":true, \"z\":null}";

    Pt::String s = "{\"a\":1,\"b\":2,\"c\":3}";
    IStringStream iss(s);

    Document doc;
    doc.load(iss);

    int a = 0;
    doc["a"].getValue(a);
    PT_UNIT_ASSERT_EQUAL(a, 1);

    int b = 0;
    doc["b"].getValue(b);
    PT_UNIT_ASSERT_EQUAL(b, 2);

    int c = 0;
    doc["c"].getValue(c);
    PT_UNIT_ASSERT_EQUAL(c, 1);
}
