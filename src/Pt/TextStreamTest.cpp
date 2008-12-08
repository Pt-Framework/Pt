/***************************************************************************
 *   Copyright (C) 2004 Marc Boris Duerner                                 *
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

#include "Pt/Api.h"
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Utf8Codec.h"
#include "Pt/Utf16Codec.h"
#include "Pt/Utf32Codec.h"
#include "Pt/TextStream.h"
#include <string>
#include <sstream>

class Utf8Converter
{
    public:
        Utf8Converter()
        : _ts( _in, new Pt::Utf8Codec() )
        {}

        void convert(const char* from, std::basic_string<Pt::Char>& to)
        {
            static const Pt::Char _eof = std::char_traits<Pt::Char>::eof();
            _ts.clear();
            _in.clear();
            _in.str(from);
            std::getline( _ts, to, _eof );
        }

    private:
        std::istringstream    _in;
        Pt::TextIStream _ts;
};


class TextStreamTest : public Pt::Unit::TestSuite
{
    public:
        TextStreamTest()
        : Pt::Unit::TestSuite("TextStreamTest")
        {
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

		void InvalidUTF8String();
        void testTextStreamDirectFromUTF8ToUnicode();
        void testTextStreamGetLineFromUTF8ToUnicode();
        void testTextBufferFromUnicodeToUTF8();
        void testTextStreamFromUnicodeToUTF8();
        void testTextStreamFromUTF32ToUnicode();
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

    Pt::TextBuffer TextBuffer(ss.rdbuf(), new Pt::Utf8Codec());
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


void TextStreamTest::testTextStreamFromUTF32ToUnicode()
{
    std::stringstream ss;

    Pt::TextStream TextStream(ss, new Pt::Utf32Codec());
    TextStream << _TextUnicode;

    Pt::Char c[6];
    TextStream.getline(c, 6);

    //std::cerr << c[0] << std::endl;

    //PT_UNIT_ASSERT(c[0] == _TextUnicode[0]);
    //PT_UNIT_ASSERT(c[1] == _TextUnicode[1]);
    //PT_UNIT_ASSERT(c[2] == _TextUnicode[2]);
    //PT_UNIT_ASSERT(c[3] == _TextUnicode[3]);
    //PT_UNIT_ASSERT(c[4] == _TextUnicode[4]);
    //PT_UNIT_ASSERT(c[5] == _TextUnicode[5]);
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
