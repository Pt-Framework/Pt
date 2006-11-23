/***************************************************************************
 *   Copyright (C) 2006 by Tobias Müller                                   *
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
 **************************************************************************/

#include <cassert>
#include <sstream>
#include "Pt/Text/Utf8Codec.h"
#include "Pt/Text/Utf32Codec.h"
#include "Pt/Text/TextStream.h"
#include "Pt/Main.h"



using namespace std;
using namespace Pt::Text;

class TextStreamTest
{

public:
	static char _textUTF8[];
	static Char _textUnicode[];

	void testTextStreamDirectFromUTF8ToUnicode();
	void testTextStreamGetLineFromUTF8ToUnicode();
	void testTextBufferFromUnicodeToUTF8();
	void testTextStreamFromUnicodeToUTF8();
	void testTextStreamFromUTF32ToUnicode();
};


char TextStreamTest::_textUTF8[]    = { (char)0xce, (char)0xba, (char)0xe1, (char)0xbd, (char)0xb9, (char)0xcf, (char)0x83,
                                        (char)0xce, (char)0xbc, (char)0xce, (char)0xb5, (char)0x0 };

Char TextStreamTest::_textUnicode[] = { 954, 8057, 963, 956, 949, 0 };


int main(int argc, char* argv[])
{
	TextStreamTest tst;

	tst.testTextStreamDirectFromUTF8ToUnicode();
	tst.testTextStreamGetLineFromUTF8ToUnicode();
	tst.testTextBufferFromUnicodeToUTF8();
	tst.testTextStreamFromUnicodeToUTF8();
	tst.testTextStreamFromUTF32ToUnicode();
}


void TextStreamTest::testTextStreamDirectFromUTF8ToUnicode()
{
	stringstream ss(_textUTF8);

	Pt::Text::TextStream textStream(ss, new Pt::Text::Utf8Codec());

	assert(textStream.get() == _textUnicode[0].value());
	assert(textStream.get() == _textUnicode[1].value());
	assert(textStream.get() == _textUnicode[2].value());
	assert(textStream.get() == _textUnicode[3].value());
	assert(textStream.get() == _textUnicode[4].value());
}


void TextStreamTest::testTextStreamGetLineFromUTF8ToUnicode()
{
	stringstream ss;
	ss << _textUTF8;
	
	Pt::Text::TextStream textStream(ss, new Pt::Text::Utf8Codec());

	Char c[6];
	textStream.getline(c, 6);
	
	assert(c[0] == _textUnicode[0]);
	assert(c[1] == _textUnicode[1]);
	assert(c[2] == _textUnicode[2]);
	assert(c[3] == _textUnicode[3]);
	assert(c[4] == _textUnicode[4]);
	assert(c[5] == _textUnicode[5]);
}



void TextStreamTest::testTextBufferFromUnicodeToUTF8()
{
	stringstream ss;

	Pt::Text::TextBuffer textBuffer(ss.rdbuf(), new Pt::Text::Utf8Codec());
	textBuffer.sputn(_textUnicode, 5);
	textBuffer.pubsync();

	string str = ss.str();
	for (unsigned int i = 0; i < str.size(); i++) {
		assert(str[i] == _textUTF8[i]);
	}
}


void TextStreamTest::testTextStreamFromUnicodeToUTF8()
{
	stringstream ss;

	Pt::Text::TextStream textStream(ss, new Pt::Text::Utf8Codec());
	textStream << _textUnicode;

	string str = ss.str();
	for (unsigned int i = 0; i < str.size(); i++) {
		assert(str[i] == _textUTF8[i]);
	}
}


void TextStreamTest::testTextStreamFromUTF32ToUnicode()
{
	stringstream ss;

	Pt::Text::TextStream textStream(ss, new Pt::Text::Utf32Codec());
	textStream << _textUnicode;

	Char c[6];
	textStream.getline(c, 6);

	cerr << c[0] << endl;

	assert(c[0] == _textUnicode[0]);
	assert(c[1] == _textUnicode[1]);
	assert(c[2] == _textUnicode[2]);
	assert(c[3] == _textUnicode[3]);
	assert(c[4] == _textUnicode[4]);
	assert(c[5] == _textUnicode[5]);
}


