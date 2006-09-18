/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
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

#include "Pt/Text/Base64Codec.h"

#include <iostream>
using namespace std;


namespace Pt {


char base64(uint8_t n)
{
	static char b64Table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	return b64Table[n];
}


uint8_t uint8(char b64)
{
	switch(b64) {
		case 'A': return 0; case 'B': return 1; case 'C': return 2;
		case 'D': return 3; case 'E': return 4; case 'F': return 5;
		case 'G': return 6; case 'H': return 7; case 'I': return 8;
		case 'J': return 9; case 'K': return 10; case 'L': return 11;
		case 'M': return 12; case 'N': return 13; case 'O': return 14;
		case 'P': return 15; case 'Q': return 16; case 'R': return 17;
		case 'S': return 18; case 'T': return 19; case 'U': return 20;
		case 'V': return 21; case 'W': return 22; case 'X': return 23;
		case 'Y': return 24; case 'Z': return 25; case 'a': return 26;
		case 'b': return 27; case 'c': return 28; case 'd': return 29;
		case 'e': return 30; case 'f': return 31; case 'g': return 32;
		case 'h': return 33; case 'i': return 34; case 'j': return 35;
		case 'k': return 36; case 'l': return 37; case 'm': return 38;
		case 'n': return 39; case 'o': return 40; case 'p': return 41;
		case 'q': return 42; case 'r': return 43; case 's': return 44;
		case 't': return 45; case 'u': return 46; case 'v': return 47;
		case 'w': return 48; case 'x': return 49; case 'y': return 50;
		case 'z': return 51; case '0': return 52; case '1': return 53;
		case '2': return 54; case '3': return 55; case '4': return 56;
		case '5': return 57; case '6': return 58; case '7': return 59;
		case '8': return 60; case '9': return 61; case '+': return 62;
		case '/': return 63; case '=': return 64;

		default:
			return 0;
	}
}


Base64Codec::Base64Codec(size_t ref)
: TextCodec<char, char>(ref),
  _padSize(0)
{
}


// from base-64 to 8bit
std::codecvt<char, char, mbstate_t>::result
Base64Codec::do_in(mbstate_t& s, const char* fromBegin, const char* fromEnd, const char*& fromNext,
                                   char* toBegin, char* toEnd, char*& toNext) const
{
	fromNext = fromBegin;
	toNext = toBegin;

	if(fromBegin == fromEnd) {
		return ok;
	}

	if( (toEnd - toNext) < 3 ||
	    (fromEnd - fromNext) < 4 ) {
		return error;
	}

	while( (fromEnd - fromNext) >= 4 &&
	       (toEnd - toNext) >= 3 ) {
		Pt::uint8_t first  = uint8( *fromNext );
		Pt::uint8_t second = uint8( *(++fromNext) );
		Pt::uint8_t third  = uint8( *(++fromNext) );
		Pt::uint8_t fourth = uint8( *(++fromNext) );

		*toNext = (first << 2) + (second >> 4);
		*(++toNext) = (second << 4) + (third >> 2);

		if(fourth != 64) {
			*(++toNext) = (third << 6) + (fourth);
		}

		++toNext;
		++fromNext;

		if( fromEnd == fromNext ) {
			return ok;
		}
	}
	cerr << "IM : " << (fromEnd - fromNext) << endl;
	cerr << "OUT: " << (toEnd - toNext) << endl;
	return partial;
}


std::codecvt<char, char, mbstate_t>::result
Base64Codec::do_out(mbstate_t& s, const char* fromBegin, const char* fromEnd, const char*& fromNext,
                    char* toBegin, char* toEnd, char*& toNext) const
{
	fromNext = fromBegin;
	toNext = toBegin;

	if(fromBegin == fromEnd) {
		return ok;
	}

	if(toEnd - toNext < 4) {
		return error;
	}

	// process until two or less characters are left
	while( (fromNext + 2) < fromEnd ) {
		if(toEnd - toNext < 4) {
			return partial;
		}

		Pt::uint8_t* first = (Pt::uint8_t*)(fromNext);
		Pt::uint8_t* second = (Pt::uint8_t*)(first+1);
		Pt::uint8_t* third = (Pt::uint8_t*)(first+2);

		*toNext     = base64( (*first >> 2) & 0x3f );
		*(++toNext) = base64( ((*first << 4) + ((*second) >> 4)) & 0x3f );
		*(++toNext) = base64( ((*second << 2) + ((*third) >> 6)) & 0x3f );
		*(++toNext) = base64( *third & 0x3f );

		++toNext;
		fromNext = fromNext + 3;
	}

	switch( fromEnd - fromNext ) {
		case 2:
			_first  = (Pt::uint8_t)( *fromNext );
			_second = (Pt::uint8_t)( *(fromNext+1) );
			_padSize = 1;
			return partial;

		case 1:
			_first = (Pt::uint8_t)(*fromNext);
			_padSize = 2;
			return partial;

		case 0:
			return ok;

		default:
			return error;
	}

	return error;
}


std::codecvt_base::result Base64Codec::do_unshift(mbstate_t& state, char* toBegin, char* toEnd, char*& toNext) const
{
	toNext = toBegin;

	if(toEnd - toBegin < 4) {
		return error;
	}

	switch(_padSize) {
		case 2:
			*toNext     = base64( (_first >> 2) & 0x3f );
			*(++toNext) = base64( (_first << 4) & 0x3f );
			*(++toNext) = '=';
			*(++toNext) = '=';
			break;

		case 1:
			*toNext     = base64( (_first >> 2) & 0x3f );
			*(++toNext) = base64( ((_first << 4) + ((_second) >> 4)) & 0x3f );
			*(++toNext) = base64( (_second << 2) &  0x3f );
			*(++toNext) = '=';
			break;

		case 0:
			return noconv;

		default:
			return error;
	}

	_padSize = 0;
	++toNext;
	return ok;
}


int Base64Codec::do_length(mbstate_t& s, const char* fromBegin, const char* fromEnd, size_t max) const
{
	return 0;
}


int Base64Codec::do_max_length() const throw()
{
	// one base64 character can give max 2 8bit char.
	// (actually four bas64's give three 8bit's)
	return 2;
}


int Base64Codec::do_encoding() const throw()
{
	// variable due to terminating pads
	return 0;
}


bool Base64Codec::do_always_noconv() const throw()
{
	return false;
}

} // namespace Pt
