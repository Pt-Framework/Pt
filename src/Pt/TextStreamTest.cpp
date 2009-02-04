/*
 * Copyright (C) 2004 Marc Boris Duerner
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

#include "Pt/Api.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Base64Codec.h"
#include "Pt/Utf8Codec.h"
#include "Pt/TextStream.h"
#include <string>
#include <sstream>
 
#include "Pt/System/Thread.h"
#include "Pt/System/Mutex.h"

const Pt::Char abc[] = {L'a', L'b', L'c', 0};
const Pt::Char def[] = {L'd', L'e', L'f', 0};
Pt::String str = abc;

bool exitThreads = false;
Pt::System::Mutex exitMtx;

void str_modify()
{
    while( ! exitThreads )
    {
        Pt::String s;
        s = str;
        s += def;

        unsigned idx = s.find('f');
        if(idx == Pt::String::npos || s.length() != 6)
        { 
            std::cerr << "f not found" << std::endl; 
            std::exit(1);
        }
    }
}

void str_read()
{
    while( ! exitThreads )
    {
        Pt::String s = str;
        unsigned idx = s.find('c');
        if(idx == Pt::String::npos || s.length() != 3)
        { 
            std::cerr << "c not found" << std::endl; 
            std::exit(1);}
        }
}

void test_string()
{
    for(unsigned n = 0; n < 500 ; ++n)
    {
        Pt::System::AttachedThread th1( Pt::callable(str_modify) );
        Pt::System::AttachedThread th2( Pt::callable(str_read) );
        Pt::System::AttachedThread th3( Pt::callable(str_modify) );
        Pt::System::AttachedThread th4( Pt::callable(str_read) );
        
        std::cerr << "starting threads" << std::endl; 
        exitMtx.lock();
        exitThreads = false;
        exitMtx.unlock();

        th1.start();
        th2.start();
        th3.start();
        th4.start();
        Pt::System::Thread::sleep(5000);

        std::cerr << "joining threads" << std::endl;
        exitMtx.lock();
        exitThreads = true;
        exitMtx.unlock();

        th1.join();
        th2.join();
        th3.join();
        th4.join();
        
        std::cerr << "=> refcount: " << str.sdata().refs() << " content: " << str.narrow() << std::endl;
        if(str.sdata().refs() != 1)
        {
            std::exit(1);
        }
    }

    std::cerr << "all OK." << std::endl;
    std::exit(0);
}


class TextStreamTest : public Pt::Unit::TestSuite
{
    public:
        TextStreamTest()
        : Pt::Unit::TestSuite("TextStreamTest")
        {
            Pt::Unit::TestSuite::registerMethod( "Base64In",
                                                 *this, &TextStreamTest::Base64In );
            Pt::Unit::TestSuite::registerMethod( "Base64Out",
                                                 *this, &TextStreamTest::Base64Out );
            Pt::Unit::TestSuite::registerMethod( "InvalidUTF8String",
                                                 *this, &TextStreamTest::InvalidUTF8String );
            Pt::Unit::TestSuite::registerMethod( "testTextStreamDirectFromUTF8ToUnicode",
                                                 *this, &TextStreamTest::testTextStreamDirectFromUTF8ToUnicode );
            Pt::Unit::TestSuite::registerMethod( "testTextStreamGetLineFromUTF8ToUnicode",
                                                 *this, &TextStreamTest::testTextStreamGetLineFromUTF8ToUnicode );
            Pt::Unit::TestSuite::registerMethod( "testGetline",
                                                 *this, &TextStreamTest::testGetline );
            Pt::Unit::TestSuite::registerMethod( "testNum_put",
                                                 *this, &TextStreamTest::testNum_put );
            Pt::Unit::TestSuite::registerMethod( "testNum_get",
                                                 *this, &TextStreamTest::testNum_get );
            Pt::Unit::TestSuite::registerMethod( "testNumpunct",
                                                 *this, &TextStreamTest::testNumpunct );
		}

        void Base64Out();
        void Base64In();
		void InvalidUTF8String();
        void testTextStreamDirectFromUTF8ToUnicode();
        void testTextStreamGetLineFromUTF8ToUnicode();
        void testTextBufferFromUnicodeToUTF8();
        void testTextStreamFromUnicodeToUTF8();
        void testGetline();
        void testNum_get();
        void testNum_put();
        void testNumpunct();

    public:
        static char _TextUTF8[];
        static Pt::Char _TextUnicode[];
};


Pt::Unit::RegisterTest<TextStreamTest> _registerTestStreamTest;


char TextStreamTest::_TextUTF8[]    = { (char)0xce, (char)0xba, (char)0xe1, (char)0xbd, (char)0xb9, (char)0xcf, (char)0x83,
                                        (char)0xce, (char)0xbc, (char)0xce, (char)0xb5, (char)0x0 };

Pt::Char TextStreamTest::_TextUnicode[] = { 954, 8057, 963, 956, 949, 0 };


void TextStreamTest::Base64Out()
{
    char to[100];
    char* nextTo;
    const char* nextFrom;
    Pt::MBState state;
    Pt::Base64Codec b64c;

    const char* from = "abc";
    memset(to, 0, sizeof(to));
    state = Pt::MBState();
    b64c.out(state, from, from+3, nextFrom, to, to+100, nextTo);
    b64c.unshift(state, nextTo, to+100, nextTo);
    PT_UNIT_ASSERT( strcmp("YWJj", to) == 0 );

    const char* from2 = "abcd";
    memset(to, 0, sizeof(to));
    state = Pt::MBState();
    b64c.out(state, from2, from2+4, nextFrom, to, to+100, nextTo);
    b64c.unshift(state, nextTo, to+100, nextTo);
    PT_UNIT_ASSERT( strcmp("YWJjZA==", to) == 0 );

    std::stringstream ss3;
    Pt::BasicTextStream<char, char> ts3(ss3, new Pt::Base64Codec);
    ts3 << "abcde";
    ts3.terminate();
    PT_UNIT_ASSERT( ss3.str() == "YWJjZGU=" );
}

void TextStreamTest::Base64In()
{
    char to[100];
    char* nextTo;
    const char* nextFrom;
    Pt::MBState state;
    Pt::Base64Codec b64c;

    const char* from = "YWJj";
    memset(to, 0, sizeof(to));
    state = Pt::MBState();
    b64c.in(state, from, from+4, nextFrom, to, to+100, nextTo);
    PT_UNIT_ASSERT( strcmp("abc", to) == 0 );
    PT_UNIT_ASSERT( nextTo - to == 3 );

    const char* from2 = "YWJjZGU=";
    memset(to, 0, sizeof(to));
    state = Pt::MBState();
    b64c.in(state, from2, from2+8, nextFrom, to, to+100, nextTo);
    PT_UNIT_ASSERT( strcmp("abcde", to) == 0 );
    PT_UNIT_ASSERT( nextTo - to == 5 );

    std::stringstream ss3("YWJjZA==");
    Pt::BasicTextStream<char, char> ts3(ss3, new Pt::Base64Codec);
    std::string token;
    ts3 >> token;
    PT_UNIT_ASSERT( token == "abcd" );
}

void TextStreamTest::InvalidUTF8String()
{
    std::stringstream ss;
    Pt::TextStream ts( ss, new Pt::Utf8Codec() );
 
	std::string invalid = "Xevil";
	invalid[0] = std::char_traits<char>::to_char_type(159); //"�evil"
    ss.str(invalid);
    PT_UNIT_ASSERT( !ss.fail() );
    
    Pt::String str;
    std::getline(ts, str);
    PT_UNIT_ASSERT( ts.fail() );

	// attaching the TextStream also clears state
	std::stringstream ss2( _TextUTF8 );
	ts.attach(ss2);
    PT_UNIT_ASSERT( !ts.fail() );

	std::getline(ts, str);
    PT_UNIT_ASSERT( !ts.fail() );
}


void TextStreamTest::testTextStreamDirectFromUTF8ToUnicode()
{
    std::stringstream ss(_TextUTF8);

    Pt::TextStream TextStream(ss, new Pt::Utf8Codec());

    PT_UNIT_ASSERT(TextStream.get() == _TextUnicode[0].value());
    PT_UNIT_ASSERT(TextStream.get() == _TextUnicode[1].value());
    PT_UNIT_ASSERT(TextStream.get() == _TextUnicode[2].value());
    PT_UNIT_ASSERT(TextStream.get() == _TextUnicode[3].value());
    PT_UNIT_ASSERT(TextStream.get() == _TextUnicode[4].value());
}


void TextStreamTest::testTextStreamGetLineFromUTF8ToUnicode()
{
    std::stringstream ss;
    ss << _TextUTF8;

    Pt::TextStream TextStream(ss, new Pt::Utf8Codec());

    Pt::Char c[6];
    TextStream.getline(c, 6);

    PT_UNIT_ASSERT(c[0] == _TextUnicode[0]);
    PT_UNIT_ASSERT(c[1] == _TextUnicode[1]);
    PT_UNIT_ASSERT(c[2] == _TextUnicode[2]);
    PT_UNIT_ASSERT(c[3] == _TextUnicode[3]);
    PT_UNIT_ASSERT(c[4] == _TextUnicode[4]);
    PT_UNIT_ASSERT(c[5] == _TextUnicode[5]);
}


void TextStreamTest::testTextBufferFromUnicodeToUTF8()
{
    std::stringstream ss;

    Pt::TextBuffer TextBuffer(&ss, new Pt::Utf8Codec());
    TextBuffer.sputn(_TextUnicode, 5);
    TextBuffer.pubsync();

    std::string str = ss.str();
    for (unsigned int i = 0; i < str.size(); i++) {
        //PT_UNIT_ASSERT(str[i] == _TextUTF8[i]);
    }
}


void TextStreamTest::testTextStreamFromUnicodeToUTF8()
{
    std::stringstream ss;

    Pt::TextStream TextStream(ss, new Pt::Utf8Codec());
    TextStream << _TextUnicode;

    std::string str = ss.str();
    for (unsigned int i = 0; i < str.size(); i++) {
        //PT_UNIT_ASSERT(str[i] == _TextUTF8[i]);
    }
}


void TextStreamTest::testGetline()
{
    std::stringstream ss("Hello world");

    Pt::TextStream TextStream(ss, new Pt::Utf8Codec());

    Pt::String s;
    getline(TextStream, s);

    PT_UNIT_ASSERT(s.narrow() == "Hello world");
}


void TextStreamTest::testNum_get()
{
    std::stringstream ss("3.1415");

    Pt::TextStream TextStream(ss, new Pt::Utf8Codec());

    float f;
    TextStream >> f;

    PT_UNIT_ASSERT(f == 3.1415f);
}


void TextStreamTest::testNum_put()
{
    std::stringstream ss;

    Pt::TextStream TextStream(ss, new Pt::Utf8Codec());

    TextStream << 3.1415f;
    TextStream.flush();

    PT_UNIT_ASSERT(ss.str() == "3.1415");
}


void TextStreamTest::testNumpunct()
{
    std::stringstream ss;

    Pt::TextStream TextStream(ss, new Pt::Utf8Codec());

    TextStream << 123456789L;
    TextStream.flush();

    PT_UNIT_ASSERT(ss.str() == "123456789");
}
