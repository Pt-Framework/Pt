/***************************************************************************
 *   Copyright (C) 2005 by Aloysius Indrayanto                             *
 *   Copyright (C) 2005 by Marc Boris Duerner                              *
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

 /**************************************************************************
 * Copyright 2001-2004 Unicode, Inc.
 *
 * Disclaimer
 *
 * This source code is provided as is by Unicode, Inc. No claims are
 * made as to fitness for any particular purpose. No warranties of any
 * kind are expressed or implied. The recipient agrees to determine
 * applicability of information provided. If this file has been
 * purchased on magnetic or optical media from Unicode, Inc., the
 * sole remedy for any claim will be exchange of defective media
 * within 90 days of receipt.
 *
 * Limitations on Rights to Redistribute This Code
 *
 * Unicode, Inc. hereby grants the right to freely use the information
 * supplied in this file in the creation of products supporting the
 * Unicode Standard, and to make copies of this file in any form
 * for internal or external distribution as long as this notice
 * remains attached.

 ---------------------------------------------------------------------

    Conversions between UTF32, UTF-16, and UTF-8. Source code file.
    Author: Mark E. Davis, 1994.
    Rev History: Rick McGowan, fixes & updates May 2001.
    Sept 2001: fixed const & error conditions per
    mods suggested by S. Parent & A. Lillich.
    June 2002: Tim Dodd added detection and handling of incomplete
               source sequences, enhanced error detection, added casts
               to eliminate compiler warnings.
    July 2003: slight mods to back out aggressive FFFE detection.
    Jan 2004: updated switches in from-UTF8 conversions.
    Oct 2004: updated to use UNI_MAX_LEGAL_UTF32 in UTF-32 conversions.

 **************************************************************************/

#ifndef PTV_Text_Utf_h
#define PTV_Text_Utf_h

#include <Pt/Text/Char.h>

namespace Pt {

namespace utf {

	const Pt::Char ReplacementChar = 0x0000FFFD;
	const Pt::Char MaxBmp = 0x0000FFFF;
	const Pt::Char MaxUtf16 = 0x0010FFFF;
	const Pt::Char MaxUtf32 = 0x7FFFFFFF;
	const Pt::Char MaxLegalUtf32 = 0x0010FFFF;
	const Pt::Char SurHighStart = 0xD800;
	const Pt::Char SurHighEnd = 0xDBFF;
	const Pt::Char SurLowStart = 0xDC00;
	const Pt::Char SurLowEnd = 0xDFFF;

#define halfShift uint32_t(10)
#define halfBase Pt::Char(0x0010000)
#define halfMask Pt::Char(0x3FF)
#define byteMask Pt::Char(0xBF)
#define byteMark Pt::Char(0x80)


/*
 * Index into the table below with the first byte of a UTF-8 sequence to
 * get the number of trailing bytes that are supposed to follow it.
 * Note that *legal* UTF-8 values can't have 4 or 5-bytes. The table is
 * left as-is for anyone who may want to do such conversion, which was
 * allowed in earlier algorithms.
 */
const char trailingBytesForUTF8[256] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};


/*
 * Magic values subtracted from a buffer value during UTF8 conversion.
 * This table contains as many values as there might be trailing bytes
 * in a UTF-8 sequence.
 */
const Char offsetsFromUTF8[6] = {
	0x00000000UL,
	0x00003080UL,
	0x000E2080UL,
	0x03C82080UL,
	0xFA082080UL,
	0x82082080UL
};


/*
 * Once the bits are split out into bytes of UTF-8, this is a mask OR-ed
 * into the first byte, depending on how many bytes follow.  There are
 * as many entries in this table as there are UTF-8 sequence types.
 * (I.e., one byte sequence, two byte... etc.). Remember that sequencs
 * for *legal* UTF-8 will be 4 or fewer bytes total.
 */
const uint8_t firstByteMark[7] = {
	0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC
};

/**
 * @brief Checks if the given character sequence is a valid UTF-8 character.
 *
 * The given array 8-bit-values is "parsed" and tried to be converted into a
 * Unicode-character using UTF-decoding. If this is not possible $false$ is returned
 * as the 8-bit-sequence is not a valid UTF-8 character. Otherwise $true$ is
 * returned. Only the first number of characters as specified in 'length' is
 * tried to converted.
 *
 * @param source An array of 8-bit values containing raw UTF-8 character data.
 * @param length Number of characters of source which are checked if they are
 * a valid UTF-8 character.
 * @return $true$ if the given sequence is a UTF-8-encoded character, $false$
 * otherwise.
 */
inline bool isLegalUTF8(const uint8_t *source, int length) {
	uint8_t a;
	const uint8_t *srcptr = source + length;

	switch (length) {
		default:
			return false;

		// Everything else falls through when "true"...
		case 4: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
		case 3: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
		case 2: if ((a = (*--srcptr)) > 0xBF) return false;
			switch (*source) {
				// no fall-through in this inner switch
				case 0xE0: if (a < 0xA0) return false; break;
				case 0xED: if (a > 0x9F) return false; break;
				case 0xF0: if (a < 0x90) return false; break;
				case 0xF4: if (a > 0x8F) return false; break;
				default:   if (a < 0x80) return false;
			}
	case 1: if (*source >= 0x80 && *source < 0xC2) return false;
	}

	if (*source > 0xF4)
		return false;

	return true;
}

} // namespace utf8

} //namespace Pt

#endif

