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
#include <Pt/StringStream.h>

class JsonReaderTest : public Pt::Unit::TestSuite
{
  public:
    JsonReaderTest()
    : Pt::Unit::TestSuite("JsonReaderTest")
    {
      this->registerMethod("EmptyDocument", *this, &JsonReaderTest::EmptyDocument);
    }

    void EmptyDocument();
};


Pt::Unit::RegisterTest<JsonReaderTest> register_PtJsonTest;


void JsonReaderTest::EmptyDocument()
{
  using namespace Pt;
  using namespace Pt::Json;

  Pt::String s = "{\"a\":\"b\",\"o\":{\"x\":\"y\", \"f\":3.14},\"i\":123, \"b\":true, \"z\":null}";
  //Pt::String s = "{}";
  //Pt::String s = " { } ";

  //Pt::String s = "[\"a\",\"b\"]";
  //Pt::String s = "[]";
  //Pt::String s = " [ ] ";
  
  //Pt::String s = " \"Hello\" ";

  //Pt::String s = "123";

  //Pt::String s = "null";

  //Pt::String s = "true";

  //Pt::String s = "false";

  IStringStream iss(s);
  JsonReader reader(iss);

  std::clog << std::endl;

  while( reader.next().type() != Node::EndDocument )
  {
      Node& node = reader.get();

      if(node.type() == Node::StartObject)
      {
          std::clog << "OBJECT START" << std::endl;
      }
      if(node.type() == Node::Member)
      {
          std::clog << "MEMBER: " << toMember(node).name().narrow() << std::endl;
      }
      if(node.type() == Node::EndObject)
      {
          std::clog << "OBJECT END" << std::endl;
      }
      if(node.type() == Node::Integer)
      {
          std::clog << "INTEGER: " << toInteger(node).value() << std::endl;
      }
      if(node.type() == Node::Float)
      {
          std::clog << "FLOAT: " << toFloat(node).value() << std::endl;
      }
      if(node.type() == Node::String)
      {
          std::clog << "STRING: "  << toString(node).value().narrow() << std::endl;
      }
      if(node.type() == Node::Boolean)
      {
          std::clog << "BOOL: "  << std::boolalpha << toBoolean(node).value() << std::endl;
      }
      if(node.type() == Node::Null)
      {
          std::clog << "NULL" << std::endl;
      }
  }

  Node& node = reader.get();
  EndDocument& endDoc = toEndDocument(node);
  std::clog << "DOCUMENT END" << std::endl;
}
