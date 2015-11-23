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
#include "Pt/Utf16Codec.h"
#include "Pt/Utf8Codec.h"
#include "Pt/Utf32Codec.h"
#include "Pt/TextStream.h"
#include "Pt/StringStream.h"
#include <Pt/System/Clock.h>
#include <string>
#include <sstream>
#include <cstring>

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
            Pt::Unit::TestSuite::registerMethod( "InvalidUTF8",
                                                 *this, &TextStreamTest::InvalidUTF8 );
            
            Pt::Unit::TestSuite::registerMethod( "UTF8ToUnicode",
                                                 *this, &TextStreamTest::UTF8ToUnicode);
            Pt::Unit::TestSuite::registerMethod( "UnicodeToUTF8",
                                                 *this, &TextStreamTest::UnicodeToUTF8);
            
            Pt::Unit::TestSuite::registerMethod( "UTF16ToUnicode",
                                                 *this, &TextStreamTest::UTF16ToUnicode);
            Pt::Unit::TestSuite::registerMethod( "UnicodeToUTF16",
                                                 *this, &TextStreamTest::UnicodeToUTF16);

            Pt::Unit::TestSuite::registerMethod( "UTF16LeToUnicode",
                                                 *this, &TextStreamTest::UTF16LeToUnicode);
            Pt::Unit::TestSuite::registerMethod( "UnicodeToUTF16Le",
                                                 *this, &TextStreamTest::UnicodeToUTF16Le);

            Pt::Unit::TestSuite::registerMethod( "UTF32BeToUnicode",
                                                 *this, &TextStreamTest::UTF32BeToUnicode);
            Pt::Unit::TestSuite::registerMethod( "UnicodeToUTF32Be",
                                                 *this, &TextStreamTest::UnicodeToUTF32Be);

            Pt::Unit::TestSuite::registerMethod( "UTF32LeToUnicode",
                                                 *this, &TextStreamTest::UTF32LeToUnicode);
            Pt::Unit::TestSuite::registerMethod( "UnicodeToUTF32Le",
                                                 *this, &TextStreamTest::UnicodeToUTF32Le);
            
            Pt::Unit::TestSuite::registerMethod( "GetChar",
                                                 *this, &TextStreamTest::GetChar );
            Pt::Unit::TestSuite::registerMethod( "GetLineBuffer",
                                                 *this, &TextStreamTest::GetLineBuffer );
            Pt::Unit::TestSuite::registerMethod( "GetLineString",
                                                 *this, &TextStreamTest::GetLineString );
            
            Pt::Unit::TestSuite::registerMethod( "NumPut",
                                                 *this, &TextStreamTest::NumPut );
            Pt::Unit::TestSuite::registerMethod( "NumGet",
                                                 *this, &TextStreamTest::NumGet );
            Pt::Unit::TestSuite::registerMethod( "NumPunct",
                                                 *this, &TextStreamTest::NumPunct );
            
            Pt::Unit::TestSuite::registerMethod( "UTF8Benchmark",
                                                 *this, &TextStreamTest::UTF8Benchmark );

        }

        void Base64Out();
        void Base64In();

        void InvalidUTF8();
        
        void UTF8ToUnicode();
        void UnicodeToUTF8();
        
        void UTF16ToUnicode();
        void UnicodeToUTF16();

        void UTF16LeToUnicode();
        void UnicodeToUTF16Le();

        void UTF32BeToUnicode();
        void UnicodeToUTF32Be();

        void UTF32LeToUnicode();
        void UnicodeToUTF32Le();
        
        void GetChar();
        void GetLineBuffer();
        void GetLineString();

        void NumGet();
        void NumPut();
        void NumPunct();
        void UTF8Benchmark();
};


Pt::Unit::RegisterTest<TextStreamTest> _registerTestStreamTest;


char TextUTF8[]        = { (char)0xce, (char)0xba, (char)0xe1, (char)0xbd, (char)0xb9, (char)0xcf, (char)0x83,
                           (char)0xce, (char)0xbc, (char)0xce, (char)0xb5, (char)0x0 };

char TextUTF16BE[]     = { (char)0x03, (char)0xba, (char)0x1F, (char)0x79, (char)0x03, (char)0xC3, (char)0x03, 
                           (char)0xBC, (char)0x03, (char)0xB5, (char)0x0 };

char TextUTF16LE[]     = { (char)0xba, (char)0x03, (char)0x79, (char)0x1F,  (char)0xC3, (char)0x03, (char)0xBC,   
                           (char)0x03, (char)0xB5, (char)0x03, (char)0x0 };

char TextUTF32BE[]     = { (char)0x0, (char)0x0, (char)0x03, (char)0xba, 
                           (char)0x0, (char)0x0, (char)0x1F, (char)0x79,
                           (char)0x0, (char)0x0, (char)0x03, (char)0xC3,
                           (char)0x0, (char)0x0, (char)0x03, (char)0xBC,
                           (char)0x0, (char)0x0,  (char)0x03, (char)0xB5,
                           (char)0x0 };

char TextUTF32LE[]     = { (char)0xba,(char)0x03, (char)0x0, (char)0x0, 
                           (char)0x79,(char)0x1F, (char)0x0, (char)0x0,  
                           (char)0xC3,(char)0x03, (char)0x0, (char)0x0, 
                           (char)0xBC,(char)0x03, (char)0x0, (char)0x0,  
                           (char)0xB5,(char)0x03, (char)0x0, (char)0x0,  
                           (char)0x0 };

Pt::Char TextUnicode[] = { 954, 8057, 963, 956, 949, 0 };


void TextStreamTest::Base64Out()
{
    char to[100];
    char* nextTo;
    const char* nextFrom;
    Pt::MBState state;
    Pt::Base64Codec b64c;

    const char* from = "abcdef";
    std::memset(to, 0, sizeof(to));
    state = Pt::MBState();
    b64c.out(state, from, from+6, nextFrom, to, to+100, nextTo);
    b64c.unshift(state, nextTo, to+100, nextTo);
    PT_UNIT_ASSERT( std::strcmp("YWJjZGVm", to) == 0 );

    const char* from2 = "abcdefg";
    std::memset(to, 0, sizeof(to));
    state = Pt::MBState();
    b64c.out(state, from2, from2+7, nextFrom, to, to+100, nextTo);
    b64c.unshift(state, nextTo, to+100, nextTo);
    PT_UNIT_ASSERT( std::strcmp("YWJjZGVmZw==", to) == 0 );

    std::stringstream ss3;
    Pt::BasicTextStream<char, char> ts3(ss3, new Pt::Base64Codec);
    ts3 << "abcdefgh";
    ts3.flush();
    PT_UNIT_ASSERT( ss3.str() == "YWJjZGVmZ2g=" );
}

void TextStreamTest::Base64In()
{
    char to[100];
    char* nextTo;
    const char* nextFrom;
    Pt::MBState state;
    Pt::Base64Codec b64c;

    const char* from = "YWJj";
    std::memset(to, 0, sizeof(to));
    state = Pt::MBState();
    b64c.in(state, from, from+4, nextFrom, to, to+100, nextTo);
    PT_UNIT_ASSERT( std::strcmp("abc", to) == 0 );
    PT_UNIT_ASSERT( nextTo - to == 3 );

    const char* from2 = "YWJjZGU=";
    std::memset(to, 0, sizeof(to));
    state = Pt::MBState();
    b64c.in(state, from2, from2+8, nextFrom, to, to+100, nextTo);
    PT_UNIT_ASSERT( std::strcmp("abcde", to) == 0 );
    PT_UNIT_ASSERT( nextTo - to == 5 );

    std::stringstream ss3("YWJjZA==");
    Pt::BasicTextStream<char, char> ts3(ss3, new Pt::Base64Codec);
    std::string token;
    ts3 >> token;
    PT_UNIT_ASSERT( token == "abcd" );
}


void TextStreamTest::InvalidUTF8()
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

    ts.clear();
    ts.discard();

    std::stringstream ss2( TextUTF8 );
    ts.attach(ss2);
    PT_UNIT_ASSERT( ! ts.fail() );

    std::getline(ts, str);
    PT_UNIT_ASSERT( ! ts.fail() );
}


void TextStreamTest::UTF8ToUnicode()
{
    Pt::TextStream ts( new Pt::Utf8Codec() );
    ts.textBuffer().import(TextUTF8, std::strlen(TextUTF8));

    PT_UNIT_ASSERT(ts.get() == TextUnicode[0].value());
    PT_UNIT_ASSERT(ts.get() == TextUnicode[1].value());
    PT_UNIT_ASSERT(ts.get() == TextUnicode[2].value());
    PT_UNIT_ASSERT(ts.get() == TextUnicode[3].value());
    PT_UNIT_ASSERT(ts.get() == TextUnicode[4].value());
}


void TextStreamTest::UnicodeToUTF8()
{
    std::stringstream ss;

    Pt::TextBuffer TextBuffer(ss, new Pt::Utf8Codec());
    TextBuffer.sputn(TextUnicode, 5);
    TextBuffer.pubsync();

    std::string str = ss.str();
    PT_UNIT_ASSERT( str.size() == std::strlen(TextUTF8) );
    PT_UNIT_ASSERT( str == TextUTF8 );
}


void TextStreamTest::UTF16ToUnicode()
{
    Pt::TextStream ts( new Pt::Utf16BECodec() );
    ts.textBuffer().import(TextUTF16BE, std::strlen(TextUTF16BE));

    PT_UNIT_ASSERT(ts.get() == TextUnicode[0].value());
    PT_UNIT_ASSERT(ts.get() == TextUnicode[1].value());
    PT_UNIT_ASSERT(ts.get() == TextUnicode[2].value());
    PT_UNIT_ASSERT(ts.get() == TextUnicode[3].value());
    PT_UNIT_ASSERT(ts.get() == TextUnicode[4].value());
}


void TextStreamTest::UnicodeToUTF16()
{
    std::stringstream ss;

    Pt::TextBuffer tbuf(ss, new Pt::Utf16BECodec());
    tbuf.sputn(TextUnicode, 5);
    tbuf.pubsync();

    std::string str = ss.str();
    PT_UNIT_ASSERT( str.size() == std::strlen(TextUTF16BE) );
    PT_UNIT_ASSERT( str == TextUTF16BE );
}

void TextStreamTest::UTF16LeToUnicode()
{
    Pt::TextStream ts( new Pt::Utf16LECodec() );
    ts.textBuffer().import(TextUTF16LE, std::strlen(TextUTF16LE));

    PT_UNIT_ASSERT(ts.get() == TextUnicode[0].value());
    PT_UNIT_ASSERT(ts.get() == TextUnicode[1].value());
    PT_UNIT_ASSERT(ts.get() == TextUnicode[2].value());
    PT_UNIT_ASSERT(ts.get() == TextUnicode[3].value());
    PT_UNIT_ASSERT(ts.get() == TextUnicode[4].value());
}


void TextStreamTest::UnicodeToUTF16Le()
{
    std::stringstream ss;

    Pt::TextBuffer tbuf(ss, new Pt::Utf16LECodec());
    tbuf.sputn(TextUnicode, 5);
    tbuf.pubsync();

    std::string str = ss.str();
    PT_UNIT_ASSERT( str.size() == std::strlen(TextUTF16LE) );
    PT_UNIT_ASSERT( str == TextUTF16LE );
}


void TextStreamTest::UTF32BeToUnicode()
{
    Pt::TextStream ts( new Pt::Utf32BECodec() );
    ts.textBuffer().import(TextUTF32BE, sizeof(TextUTF32BE) - 1);

    PT_UNIT_ASSERT(ts.get() == TextUnicode[0].value());
    PT_UNIT_ASSERT(ts.get() == TextUnicode[1].value());
    PT_UNIT_ASSERT(ts.get() == TextUnicode[2].value());
    PT_UNIT_ASSERT(ts.get() == TextUnicode[3].value());
    PT_UNIT_ASSERT(ts.get() == TextUnicode[4].value());
}


void TextStreamTest::UnicodeToUTF32Be()
{
    std::stringstream ss;

    Pt::TextBuffer tbuf(ss, new Pt::Utf32BECodec());
    tbuf.sputn(TextUnicode, 5);
    tbuf.pubsync();

    std::string str = ss.str();
    PT_UNIT_ASSERT( str.size() == sizeof(TextUTF32BE) - 1 );
    PT_UNIT_ASSERT( 0 == std::memcmp(str.c_str(), TextUTF32BE, sizeof(TextUTF32BE) - 1) );
}


void TextStreamTest::UTF32LeToUnicode()
{
    Pt::TextStream ts( new Pt::Utf32LECodec() );
    ts.textBuffer().import(TextUTF32LE, sizeof(TextUTF32LE) - 1);

    PT_UNIT_ASSERT(ts.get() == TextUnicode[0].value());
    PT_UNIT_ASSERT(ts.get() == TextUnicode[1].value());
    PT_UNIT_ASSERT(ts.get() == TextUnicode[2].value());
    PT_UNIT_ASSERT(ts.get() == TextUnicode[3].value());
    PT_UNIT_ASSERT(ts.get() == TextUnicode[4].value());
}


void TextStreamTest::UnicodeToUTF32Le()
{
    std::stringstream ss;

    Pt::TextBuffer tbuf(ss, new Pt::Utf32LECodec());
    tbuf.sputn(TextUnicode, 5);
    tbuf.pubsync();

    std::string str = ss.str();
    PT_UNIT_ASSERT( str.size() == sizeof(TextUTF32LE) - 1 );
    PT_UNIT_ASSERT( 0 == std::memcmp(str.c_str(), TextUTF32LE, sizeof(TextUTF32LE) - 1) );
}


void TextStreamTest::GetChar()
{
    std::stringstream ss(TextUTF8);

    Pt::TextStream TextStream(ss, new Pt::Utf8Codec());

    PT_UNIT_ASSERT(TextStream.get() == TextUnicode[0].value());
    PT_UNIT_ASSERT(TextStream.get() == TextUnicode[1].value());
    PT_UNIT_ASSERT(TextStream.get() == TextUnicode[2].value());
    PT_UNIT_ASSERT(TextStream.get() == TextUnicode[3].value());
    PT_UNIT_ASSERT(TextStream.get() == TextUnicode[4].value());
}


void TextStreamTest::GetLineBuffer()
{
    std::stringstream ss;
    ss << TextUTF8;

    Pt::TextStream ts(ss, new Pt::Utf8Codec());

    Pt::Char c[6];
    ts.getline(c, 6);

    PT_UNIT_ASSERT(c[0] == TextUnicode[0]);
    PT_UNIT_ASSERT(c[1] == TextUnicode[1]);
    PT_UNIT_ASSERT(c[2] == TextUnicode[2]);
    PT_UNIT_ASSERT(c[3] == TextUnicode[3]);
    PT_UNIT_ASSERT(c[4] == TextUnicode[4]);
    PT_UNIT_ASSERT(c[5] == TextUnicode[5]);
}


void TextStreamTest::GetLineString()
{
    std::stringstream ss("Hello world");

    Pt::TextStream TextStream(ss, new Pt::Utf8Codec());

    Pt::String s;
    getline(TextStream, s);

    PT_UNIT_ASSERT(s.narrow() == "Hello world");
}


void TextStreamTest::NumGet()
{
    std::stringstream ss("3.1415");

    Pt::TextStream TextStream(ss, new Pt::Utf8Codec());

    float f;
    TextStream >> f;

    PT_UNIT_ASSERT(f == 3.1415f);
}


void TextStreamTest::NumPut()
{
    std::stringstream ss;

    Pt::TextStream TextStream(ss, new Pt::Utf8Codec());

    TextStream << 3.1415f;
    TextStream.flush();

    PT_UNIT_ASSERT(ss.str().substr(0, 5) == "3.141");
}


void TextStreamTest::NumPunct()
{
    std::stringstream ss;

    Pt::TextStream TextStream(ss, new Pt::Utf8Codec());

    TextStream << 123456789L;
    TextStream.flush();

    PT_UNIT_ASSERT(ss.str() == "123456789");
}


void TextStreamTest::UTF8Benchmark()
{
  Pt::System::Clock c;
  c.start();

  unsigned size = 1024;
  std::vector<char> output(size+10, 'a');
  std::vector<Pt::Char> input(size, Pt::Char('a'));

  for(unsigned n = 0; n < 5000*3; ++n)
  {
    Pt::MBState mb ;
    Pt::Utf8Codec codec;

    const Pt::Char* fromNext = &input[0];
    char* toNext = &output[0];

    Pt::Utf8Codec::result r = codec.out(mb, &input[0], &input[0] + size, fromNext, 
      &output[0], &output[0] + size+10, toNext); 

    if(r != Pt::Utf8Codec::ok)
      throw std::runtime_error("conversion failed");

    //int ss = codec.length(mb, &output[0], &output[0] + size, size ); 

    //if(ss != size)
    //  throw std::runtime_error("length failed");
  }

  Pt::Timespan ts = c.stop();
  std::cerr << "time:  " << ts.toMSecs()  << " msecs." << std::endl;

}
