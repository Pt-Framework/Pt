/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Drner                                *
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

#include "UtfCommons.h"
#include "Pt/Text/Utf16Codec.h"
using namespace Pt::Text;


Utf16Codec::Utf16Codec(size_t ref) : Pt::TextCodec(ref)
{}


Utf16Codec::~Utf16Codec() 
{}


//! decodes UTF-16 to UTF-32
TextCodec::result Utf16Codec::do_in(mbstate_t& s, const char* fromBegin, const char* fromEnd, const char*& fromNext,
                                                Pt::Char* toBegin, Pt::Char* toEnd, Pt::Char*& toNext) const
{
	TextCodec::result retstat = TextCodec::ok;
	
	UTF16 *fbegin = (UTF16 *) fromBegin;
	UTF16 *fend   = (UTF16 *) fromEnd;
	UTF16 *fnext  = fbegin;
	
	UTF32 *tbegin = (UTF32 *) toBegin;
	UTF32 *tend   = (UTF32 *) toEnd;
	UTF32 *tnext  = tbegin;
	
	UTF32 ch, ch2;
	
	while(fnext < fend) {
		
		ch = *fnext++;
		
		/// If we have a surrogate pair, convert to UTF32 first. 
		if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
			// If the 16 bits following the high surrogate are in the source buffer... 
			if (fnext < fend) {
				ch2 = *fnext;
				// If it's a low surrogate, convert to UTF32. 
					if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) {
						ch = ((ch - UNI_SUR_HIGH_START) << halfShift) + (ch2 - UNI_SUR_LOW_START) + halfBase;
						++fnext;
					} else {
						--fnext; // return to the illegal value itself 
						retstat = TextCodec::error;
						break;
					}
			} else { // We don't have the 16 bits following the high surrogate (source exhausted) 
				fnext--;
				retstat = TextCodec::partial;
				break;
			}
		} else {
			// UTF-16 surrogate values are illegal in UTF-32 
			if (ch >= UNI_SUR_LOW_START && ch <= UNI_SUR_LOW_END) {
				--fnext; // return to the illegal value itself 
				retstat = TextCodec::error;
				break;
			}
		}
	
			if (tnext >= tend) {
				--fnext;
				tnext = tend;
				retstat = TextCodec::partial;
				break;
			}
			
			*tnext++ = ch;
		
	} // while

	// update pointers	
	fromNext = (const char *) fnext;
	toNext = (Char *) tnext;
	
	return retstat;
}

//! encodes UTF-32 to UTF-16
TextCodec::result Utf16Codec::do_out(mbstate_t& s, const Pt::Char* fromBegin, const Pt::Char* fromEnd, const Pt::Char*& fromNext,
                                                char* toBegin, char* toEnd, char*& toNext) const
{ 
	TextCodec::result retstat = TextCodec::ok;
	
	UTF32 *fbegin = (UTF32 *) fromBegin;
	UTF32 *fend   = (UTF32 *) fromEnd;
	UTF32 *fnext  = fbegin;
	UTF32 ch;
	
	UTF16 *tbegin = (UTF16 *) toBegin;
	UTF16 *tend   = (UTF16 *) toEnd;
	UTF16 *tnext  = tbegin;
	
	while (fnext< fend) {
	
		if (tnext >= tend) {
			tnext = tend;
			retstat = TextCodec::partial;
			break;
		}
		
		if (ch <= UNI_MAX_BMP) { // Target is a character <= 0xFFFF 
			// UTF-16 surrogate values are illegal in UTF-32; 0xffff or 0xfffe are both reserved values 
			if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
				--fnext; // return to the illegal value itself 
				retstat = TextCodec::error;
				break;
			} else {
				*tnext++ = (UTF16)ch; // normal case 
			}
		}	else if (ch > UNI_MAX_LEGAL_UTF32) {
				retstat = TextCodec::error;
				*tnext++ = (UTF16)UNI_REPLACEMENT_CHAR;
		} else {
			// target is a character in range 0xFFFF - 0x10FFFF. 
			if (tnext + 1 >= tend) {
				fnext--;
				tnext = tend;
				retstat = TextCodec::partial;
			}
			ch -= halfBase;
			*tnext++ = (UTF16)((ch >> halfShift) + UNI_SUR_HIGH_START);
			*tnext++ = (UTF16)((ch & halfMask) + UNI_SUR_LOW_START);
		}
							
	} // while
	
	// update pointers	
	fromNext = (const Char *) fnext;
	toNext = (char *) tnext;
	
	return retstat;
}


int Utf16Codec::do_length(mbstate_t& s, const char* fromBegin, const char* fromEnd, size_t max) const
{
	return fromEnd-fromBegin;
}


int Utf16Codec::do_max_length() const throw()
{
	return 2; // Should be only 2 UTF-16 chars (= 4 bytes)
	          // Question: what we should actually return here ?
						// The number of UTF-16 chars? Or the bytes?
}

bool Utf16Codec::do_always_no_conv() const throw()
{
	return false;
}

