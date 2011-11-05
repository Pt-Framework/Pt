/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#include <iostream>
#include <fstream>
#include <math.h>
#include <Pt/Xml/XmlSerializer.h>
#include <Pt/Xml/XmlDeserializer.h>
#include <Pt/Arg.h>
#include <Pt/System/Clock.h>
#include <Pt/Convert.h>

namespace
{
    struct TestObject
    {
        int intValue;
        std::string stringValue;
        double doubleValue;
    };

    void operator>>= (const Pt::SerializationInfo& si, TestObject& obj)
    {
        si.getMember("intValue") >>= obj.intValue;
        si.getMember("stringValue") >>= obj.stringValue;
        si.getMember("doubleValue") >>= obj.doubleValue;
    }

    void operator<<= (Pt::SerializationInfo& si, const TestObject& obj)
    {
        si.addMember("intValue") <<= obj.intValue;
        si.addMember("stringValue") <<= obj.stringValue;
        si.addMember("doubleValue") <<= obj.doubleValue;
        si.setTypeName("TestObject");
    }
}

template <typename T, typename Serializer, typename Deserializer>
void benchSerialization(const T& d, const char* fname = 0)
{
    std::stringstream data;

    Pt::SerializationContext ctx;

    Serializer serializer(data);
    serializer.context()->enableReferencing(false);

    Deserializer deserializer(data);
    deserializer.context()->enableReferencing(false);

    Pt::System::Clock clock;
    clock.start();
    serializer.serialize(d, "d");
    serializer.finish();
    serializer.flush();
    Pt::Timespan ts = clock.stop();

    if (fname)
    {
        std::ofstream f(fname);
        f << data.str();
    }

    T v2;
    clock.start();
    deserializer.deserialize(v2);
    Pt::Timespan td = clock.stop();

    std::cout << "\tserialization: " << ts.toUSecs() / 1e6 << " sec\n"
                 "\tdeserialization: " << td.toUSecs() / 1e6 << " sec\n"
                 "\tsize: " << data.str().size() << " bytes" << std::endl;
}

template <typename T>
void benchXmlSerialization(const T& d, const char* fname = 0)
{
    benchSerialization<T, Pt::Xml::XmlSerializer, Pt::Xml::XmlDeserializer>(d, fname);
}

template <typename T>
void benchVector(const char* typeName, unsigned N, bool fileoutput)
{
    std::cout << "vector of " << typeName << " values:" << std::endl;

    std::vector<T> v;
    for (unsigned n = 0; n < N; ++n)
        v.push_back(n);

    std::cout << "Xml:" << std::endl;
    benchXmlSerialization(v, fileoutput ? (std::string("vector-") + typeName + ".xml").c_str() : 0);
}

int main(int argc, char* argv[])
{
    try
    {
        Pt::Arg<unsigned> nn(argc, argv, 'n', 100000);
        Pt::Arg<unsigned> I(argc, argv, 'I', nn);
        Pt::Arg<unsigned> D(argc, argv, 'D', nn);
        Pt::Arg<unsigned> C(argc, argv, 'C', nn);
        Pt::Arg<bool> fileoutput(argc, argv, 'f');

        std::cout << "benchmark xml serializer with " << I.getValue() << " int vector " << D.getValue() << " double vector and " << C.getValue() << " custom vector iterations\n\n"
                     "options:\n"
                     "   -n <number>       specify number of default iterations\n"
                     "   -I <number>       specify number of iterations for int vector\n"
                     "   -D <number>       specify number of iterations for double vector\n"
                     "   -C <number>       specify number of iterations for custom object\n"
                     "   -f                write serialized output to files\n" << std::endl;

        benchVector<int>("int", I, fileoutput);
        benchVector<double>("double", D, fileoutput);

        {
            std::cout << "vector of custom objects:" << std::endl;

            TestObject obj;
            std::vector<TestObject> v;
            for (unsigned n = 0; n < C; ++n)
            {
                obj.intValue = n;
                obj.stringValue = Pt::convert<std::string>(n);
                obj.doubleValue = sqrt(static_cast<double>(n));
                v.push_back(obj);
            }

            std::cout << "Xml:" << std::endl;
            benchXmlSerialization(v, fileoutput ? "custobject.xml" : 0);
        }

    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

