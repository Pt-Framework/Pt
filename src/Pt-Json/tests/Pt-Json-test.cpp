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
#include <Pt/Json/JsonWriter.h>
#include <Pt/Json/JsonFormatter.h>

#include <Pt/StringStream.h>
#include <Pt/String.h>
#include <Pt/IOStream.h>
#include <Pt/TextStream.h>
#include <Pt/Utf8Codec.h>
#include <Pt/Settings.h>

#include <vector>
#include <cmath>

class SerDemo
{
    public:
        class SubItem
        {
            public:
            SubItem()
            : _intValue(7411)
            , _boolValue(true)
            , _floatValue(7411.7411f)
            , _doubelValue(3.123123)
            , _array(4)
            {
                _array[0] = 1;
                _array[1] = 2;
                _array[2] = 3;
                _array[3] = 4;
            }

            ~SubItem()
            {

            }

            void clear()
            {
                _intValue = 0;
                _boolValue = 0;
                _floatValue = 0;
                _doubelValue = 0;
                _array.clear();
            }


            bool operator!=(const SubItem& obj)
            {
                return !operator==(obj);
            }

            bool operator==(const SubItem& obj)
            {
                if (_intValue != obj._intValue)
                    return false;

                if (_floatValue != obj._floatValue)
                    return false;

                if (_doubelValue != obj._doubelValue)
                    return false;

                if (_array.size() != obj._array.size())
                    return false;

                for (size_t i = 0; i < _array.size(); ++i)
                {
                    if (_array[i] != obj._array[i])
                        return false;
                }

                return true;
            }

            int _intValue;
            bool _boolValue;
            float _floatValue;
            double _doubelValue;
            std::vector<int> _array;
        };

        SerDemo()
        : _intValue(12)
        , _boolValue(false)
        , _floatValue(23.33f)
        , _doubelValue(123.1234567689)
        , _items(3)
        , _structValue()
        {

        }

        ~SerDemo()
        {

        }


        void clear()
        {
            _intValue = 0;
            _boolValue = 0;
            _floatValue = 0;
            _doubelValue = 0;
            _items.clear();
            _structValue.clear();
        }


        bool operator!=(const SerDemo& obj)
        {
            return !operator==(obj);
        }

        bool operator==(const SerDemo& obj)
        {
            if (_intValue != obj._intValue)
                return false;

            if (_boolValue != obj._boolValue)
                return false;


            if (_floatValue != obj._floatValue)
                return false;


            if (_doubelValue != obj._doubelValue)
                return false;
            
            
            if (_items.size() != obj._items.size())
                return false;

            for (size_t i = 0; i < _items.size(); ++i)
            {
                if (_items[i] != obj._items[i])
                    return false;
            }

            if (_structValue != obj._structValue)
                return false;

            return true;
        }

        int _intValue;
        bool _boolValue;
        float _floatValue;
        double _doubelValue;
        std::vector<SubItem> _items;
        SubItem _structValue;
};

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
      this->registerMethod("Settings", *this, &JsonReaderTest::Settings);
    }

    void EmptyDocument();
    void Float();
    void SimpleArray();
    void SimpleObject();
    void Settings();

};

void operator<<=(Pt::SerializationInfo& si, const SerDemo::SubItem& obj)
{
    si.addMember("intValue") <<= obj._intValue;
    si.addMember("boolValue") <<= obj._boolValue;
    si.addMember("floatValue") <<= obj._floatValue;
    si.addMember("doubelValue") <<= obj._doubelValue;
    si.addMember("array") <<= obj._array;
}


void operator>>=(const Pt::SerializationInfo& si, SerDemo::SubItem& obj)
{
    si.getMember("intValue") >>= obj._intValue;
    si.getMember("boolValue") >>= obj._boolValue;
    si.getMember("floatValue") >>= obj._floatValue;
    si.getMember("doubelValue") >>= obj._doubelValue;
    si.getMember("array") >>= obj._array;
}

void operator<<=(Pt::SerializationInfo& si, const SerDemo& obj)
{
    si.addMember("intValue") <<= obj._intValue;
    si.addMember("boolValue") <<= obj._boolValue;
    si.addMember("floatValue") <<= obj._floatValue;
    si.addMember("doubelValue") <<= obj._doubelValue;
    si.addMember("items") <<= obj._items;
    si.addMember("structValue") <<= obj._structValue;
}

void operator>>=(const Pt::SerializationInfo& si, SerDemo& obj)
{
    si.getMember("intValue") >>= obj._intValue;
    si.getMember("boolValue") >>= obj._boolValue;
    si.getMember("floatValue") >>= obj._floatValue;
    si.getMember("doubelValue") >>= obj._doubelValue;
    si.getMember("items") >>= obj._items;
    si.getMember("structValue") >>= obj._structValue;
}

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
    PT_UNIT_ASSERT_EQUAL(c, 3);

    //OStringStream oss(s);
    //JsonWriter writer(oss);

    //writer.writeObject();

    //writer.writeMember("person1");
    //writer.writeObject();
    //writer.writeMember("name");
    //writer.writeString("lorenzo");
    //writer.writeMember("age");
    //writer.writeString("42");
    //writer.writeObjectEnd();

    //writer.writeMember("person2");
    //writer.writeObject();
    //
    //writer.writeMember("numbers");
    //writer.writeArray();

    //writer.writeArray();
    //writer.writeString("1");
    //writer.writeString("2");
    //writer.writeString("3");
    //writer.writeArrayEnd();

    //writer.writeArray();
    //writer.writeString("a");
    //writer.writeString("b");
    //writer.writeString("c");
    //writer.writeArrayEnd();

    //writer.writeArrayEnd();
    //
    //writer.writeObjectEnd();

    //writer.writeObjectEnd();

    //std::cout << "\n" << oss.str().narrow() << std::endl;
}


void JsonReaderTest::Settings()
{
    SerDemo serObj;
    std::string str;

    {//Serialize
        std::stringstream       ss;
        Pt::TextOStream         os(ss, new Pt::Utf8Codec());
        Pt::Json::JsonWriter    writer(os);
        Pt::Json::JsonFormatter formater(writer);

        Pt::Settings settings;

        settings.addEntry("SerDemo").set(serObj);

        settings.save(formater);
        os.flush();
        ss.flush();

        str = ss.str();
    }

    SerDemo desObj;

    desObj.clear();

    {//Deserialize
        std::stringstream       ss(str);
        Pt::TextIStream         is(ss, new Pt::Utf8Codec());
        Pt::Json::JsonReader    reader(is);
        Pt::Json::JsonFormatter formater(reader);

        Pt::Settings settings;

        settings.load(formater);

        settings["SerDemo"].get(desObj);
    }

    PT_UNIT_ASSERT(desObj == serObj);
}
