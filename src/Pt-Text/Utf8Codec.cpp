/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris DÃ¼rner                               *
 *                         Aloysius Indrayanto                             *
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

#include "Pt/Text/Utf8Codec.h"
#include "Utf.h"

#include <iostream>
using namespace std;


namespace Pt {

Utf8Codec::Utf8Codec(size_t ref)
: Pt::TextCodec<Char, char>(ref)
{}


Utf8Codec::result Utf8Codec::do_in(mbstate_t& s, const char* fromBegin, const char* fromEnd, const char*& fromNext,
                                   Pt::Char* toBegin, Pt::Char* toEnd, Pt::Char*& toNext) const
{
	Utf8Codec::result retstat = ok;
	fromNext = fromBegin;
	toNext = toBegin;

	while(fromNext < fromEnd) {
		uint8_t* fnext = (uint8_t *)(fromNext);

		if(toNext >= toEnd) {
			retstat = partial;
			break;
		}

		const size_t extraBytesToRead = Utf::trailingBytesForUTF8[*fnext];
		if(fromNext + extraBytesToRead >= fromEnd) {
			retstat = partial;
			break;
		}

		if( !Utf::isLegalUTF8( (const uint8_t*)fnext, extraBytesToRead + 1 ) ) {
			retstat = error;
			break;
		}

		*toNext = 0;
		switch (extraBytesToRead) {
			case 5: *toNext += *fnext++; *toNext <<= 6; // We should never get this for legal UTF-8
			case 4: *toNext += *fnext++; *toNext <<= 6; // We should never get this for legal UTF-8
			case 3: *toNext += *fnext++; *toNext <<= 6;
			case 2: *toNext += *fnext++; *toNext <<= 6;
			case 1: *toNext += *fnext++; *toNext <<= 6;
			case 0: *toNext += *fnext++;
		}
		*toNext -= Utf::offsetsFromUTF8[extraBytesToRead];

		// UTF-16 surrogate values are illegal in UTF-32, and anything
		// over Plane 17 (> 0x10FFFF) is illegal.
		if(*toNext > Utf::MaxLegalUtf32) {
			*toNext = Utf::ReplacementChar;
		}
		else if(*toNext >= Utf::SurHighStart && *toNext <= Utf::SurLowEnd) {
			*toNext = Utf::ReplacementChar;
		}

		++toNext;
		fromNext += (extraBytesToRead + 1);
	}

	return retstat;
}


Utf8Codec::result Utf8Codec::do_out(mbstate_t& s, const Pt::Char* fromBegin, const Pt::Char* fromEnd, const Pt::Char*& fromNext,
                                                  char* toBegin, char* toEnd, char*& toNext) const
{
	result retstat = ok;
	fromNext  = fromBegin;
	toNext = toBegin;
	Pt::Char ch;

	size_t bytesToWrite;

	while(fromNext < fromEnd) {
		ch = *fromNext;
		if (ch >= Utf::SurHighStart && ch <= Utf::SurLowEnd) {
			retstat = error;
			break;
		}

		// Figure out how many bytes the result will require. Turn any
		// illegally large UTF32 things (> Plane 17) into replacement chars.
		if (ch < Pt::Char(0x80)) {
			bytesToWrite = 1;
		}
		else if (ch < Pt::Char(0x800)) {
			bytesToWrite = 2;
		}
		else if (ch < Pt::Char(0x10000)) {
			bytesToWrite = 3;
		}
		else if (ch <= Utf::MaxLegalUtf32) {
			bytesToWrite = 4;
		}
		else {
			bytesToWrite = 3;
			ch = Utf::ReplacementChar;
		}

		uint8_t* current = (uint8_t*)(toNext + bytesToWrite);
		if( current >= (uint8_t*)(toEnd) ) {
			retstat = partial;
			break;
		}

		switch(bytesToWrite) { // note: everything falls through...
			case 4: *--current = (uint8_t)((ch | byteMark) & byteMask).value(); ch >>= 6; 
			case 3: *--current = (uint8_t)((ch | byteMark) & byteMask).value(); ch >>= 6; 
			case 2: *--current = (uint8_t)((ch | byteMark) & byteMask).value(); ch >>= 6; 
			case 1: *--current = (uint8_t) (ch.value() | Utf::firstByteMark[bytesToWrite]); 
		}

		toNext += bytesToWrite;
		++fromNext;
	}

	return retstat;
}


int Utf8Codec::do_length(mbstate_t& s, const char* fromBegin, const char* fromEnd, size_t max) const
{
	const char* fromNext = fromBegin;
	size_t counter = 0;

	while(fromNext < fromEnd && counter <= max) {
		int extraBytesToRead = Utf::trailingBytesForUTF8[ (unsigned char)*fromNext ]; // NOTE: check again...

		if(fromNext + extraBytesToRead >= fromEnd) {
			break;
		}

		if(!Utf::isLegalUTF8( (const uint8_t*) fromNext, extraBytesToRead + 1 ) ) {
			break;
		}

		fromNext += extraBytesToRead + 1;
		counter += extraBytesToRead + 1;
	}

	return fromNext - fromBegin;
}


int Utf8Codec::do_max_length() const throw()
{
	return 4;
}


bool Utf8Codec::do_always_noconv() const throw()
{
	return false;
}

} // namespace Pt
