/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Duerner                              *
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
#include "Pt/Utf16Codec.h"

namespace Pt {

Utf16Codec::Utf16Codec(size_t ref)
: Pt::TextCodec<Char, char>(ref)
{}


Utf16Codec::~Utf16Codec()
{}


Utf16Codec::result Utf16Codec::do_in(std::mbstate_t& s, const char* fromBegin, const char* fromEnd, const char*& fromNext,
                                     Pt::Char* toBegin, Pt::Char* toEnd, Pt::Char*& toNext) const
{
    result retstat = ok;

    fromNext = fromBegin;
    uint16_t* fNext = (uint16_t*)fromBegin;
    uint16_t* fEnd  = (uint16_t*)fromEnd;
    uint16_t ch;
    uint16_t ch2;

    while(fNext < fEnd) {

        ch = *fNext++;

        /// If we have a surrogate pair, convert to UTF32 first.
        if (ch >= 0xD800 && ch <= 0xDBFF) {
            // If the 16 bits following the high surrogate are in the source buffer...
            if (fNext < fEnd) {
                ch2 = *fNext;
                // If it's a low surrogate, convert to UTF32.
                    if (ch2 >= 0xDC00 && ch2 <= 0xDFFF) {
                        ch = ((ch - 0xD800) << 10) + (ch2 - 0xDC00) + 0x0010000U;
                        ++fNext;
                    } else {
                        --fNext; // return to the illegal value itself
                        retstat = error;
                        break;
                    }
            } else { // We don't have the 16 bits following the high surrogate (source exhausted)
                fNext--;
                retstat = partial;
                break;
            }
        } else {
            // UTF-16 surrogate values are illegal in UTF-32
            if (ch >= 0xDC00 && ch <= 0xDFFF) {
                --fNext; // return to the illegal value itself
                retstat = error;
                break;
            }
        }

        if (toNext >= toEnd) {
            --fNext;
            toNext = toEnd;
            retstat = partial;
            break;
        }

        *toNext++ = ch;

    } // while

    // update pointers
    fromNext = (const char*)fNext;

    return retstat;
}


Utf16Codec::result Utf16Codec::do_out(std::mbstate_t& s, const Pt::Char* fromBegin, const Pt::Char* fromEnd, const Pt::Char*& fromNext,
                                      char* toBegin, char* toEnd, char*& toNext) const
{
    result retstat = ok;

    fromNext = fromBegin;
    uint16_t* tNext = (uint16_t*)toBegin;
    uint16_t* tEnd  = (uint16_t*)toEnd;
    Pt::Char ch;

    while (fromNext < fromEnd) {

        if (tNext >= tEnd) {
            tNext = tEnd;
            retstat = partial;
            break;
        }

        ch = *fromNext++;

        if (ch <= Pt::Char(0xFFFF))
        { // Target is a character <= 0xFFFF
            // UTF-16 surrogate values are illegal in UTF-32; 0xffff or 0xfffe are both reserved values
            if (ch >= Pt::Char(0xD800) && ch <= Pt::Char(0xDFFF))
            {
                --fromNext; // return to the illegal value itself
                retstat = error;
                break;
            }
            else
            {
                *tNext++ = ch; // normal case
            }
        }
        else if (ch > Pt::Char(0x0010FFFF))
        {
                retstat = error;
                *tNext++ = 0xFFFD;
        }
        else
        {
            // target is a character in range 0xFFFF - 0x10FFFF.
            if (tNext + 1 >= tEnd)
            {
                fromNext--;
                tNext = tEnd;
                retstat = partial;
            }
            ch -= 0x0010000UL;
            *toNext++ = (((uint32_t)ch >> 10) + 0xD800);
            *toNext++ = (((uint32_t)ch & 0x3FFU) + 0xDC00);
        }

    } // while

    // update pointers
    toNext = (char*)tNext;

    return retstat;
}


int Utf16Codec::do_length(std::mbstate_t& s, const char* fromBegin, const char* fromEnd, size_t max) const
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

}
