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
#undef PT_TEXT_API_EXPORT

#include "Pt/Api.h"
#include <string>
#include <sstream>
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/TestMain.h"
#include "Pt/Unit/TestSchedule.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Text/Utf8Codec.h"
#include "Pt/Text/Utf16Codec.h"
#include "Pt/Text/Utf32Codec.h"
#include "Pt/Text/TextStream.h"


class TextStreamTest : public Pt::Unit::TestSuite
{
	public:
        TextStreamTest()
        : Pt::Unit::TestSuite("TextStreamTest")
        {
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
		static Pt::Text::Char _TextUnicode[];
};


Pt::Unit::RegisterTest<TextStreamTest> _registerTestStreamTest;


char TextStreamTest::_TextUTF8[]    = { (char)0xce, (char)0xba, (char)0xe1, (char)0xbd, (char)0xb9, (char)0xcf, (char)0x83,
                                        (char)0xce, (char)0xbc, (char)0xce, (char)0xb5, (char)0x0 };

Pt::Text::Char TextStreamTest::_TextUnicode[] = { 954, 8057, 963, 956, 949, 0 };


void TextStreamTest::testTextStreamDirectFromUTF8ToUnicode()
{
	std::stringstream ss(_TextUTF8);

	Pt::Text::TextStream TextStream(ss, new Pt::Text::Utf8Codec());

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

	Pt::Text::TextStream TextStream(ss, new Pt::Text::Utf8Codec());

	Pt::Text::Char c[6];
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

	Pt::Text::TextBuffer TextBuffer(ss.rdbuf(), new Pt::Text::Utf8Codec());
	TextBuffer.sputn(_TextUnicode, 5);
	TextBuffer.pubsync();

	std::string str = ss.str();
	for (unsigned int i = 0; i < str.size(); i++) {
		//CPPUNIT_ASSERT(str[i] == _TextUTF8[i]);
	}
}


void TextStreamTest::testTextStreamFromUnicodeToUTF8()
{
	std::stringstream ss;

	Pt::Text::TextStream TextStream(ss, new Pt::Text::Utf8Codec());
	TextStream << _TextUnicode;

	std::string str = ss.str();
	for (unsigned int i = 0; i < str.size(); i++) {
		//CPPUNIT_ASSERT(str[i] == _TextUTF8[i]);
	}
}


void TextStreamTest::testTextStreamFromUTF32ToUnicode()
{
	std::stringstream ss;

	Pt::Text::TextStream TextStream(ss, new Pt::Text::Utf32Codec());
	TextStream << _TextUnicode;

	Pt::Text::Char c[6];
	TextStream.getline(c, 6);

	std::cerr << c[0] << std::endl;

	//CPPUNIT_ASSERT(c[0] == _TextUnicode[0]);
	//CPPUNIT_ASSERT(c[1] == _TextUnicode[1]);
	//CPPUNIT_ASSERT(c[2] == _TextUnicode[2]);
	//CPPUNIT_ASSERT(c[3] == _TextUnicode[3]);
	//CPPUNIT_ASSERT(c[4] == _TextUnicode[4]);
	//CPPUNIT_ASSERT(c[5] == _TextUnicode[5]);
}



void TextStreamTest::testGetline()
{
	stringstream ss("Hello world");

	Pt::Text::TextStream TextStream(ss, new Pt::Text::Utf8Codec());

	Pt::Text::String s;
	getline(TextStream, s);

	PT_UNIT_ASSERT(s.narrow() == "Hello world");
}


void TextStreamTest::testNum_get()
{
	stringstream ss("3.1415");

	Pt::Text::TextStream TextStream(ss, new Pt::Text::Utf8Codec());

	float f;
	TextStream >> f;

	PT_UNIT_ASSERT(f == 3.1415f);
}


void TextStreamTest::testNum_put()
{
	stringstream ss;

	Pt::Text::TextStream TextStream(ss, new Pt::Text::Utf8Codec());

	TextStream << 3.1415f;
	TextStream.flush();

	PT_UNIT_ASSERT(ss.str() == "3.1415");
}


void TextStreamTest::testNumpunct()
{
	stringstream ss;

	Pt::Text::TextStream TextStream(ss, new Pt::Text::Utf8Codec());

	TextStream << 123456789L;
	TextStream.flush();

	PT_UNIT_ASSERT(ss.str() == "123456789");
}
