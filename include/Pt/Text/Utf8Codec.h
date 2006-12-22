/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
 *   Copyright (C) 2005 by Aloysius Indrayanto                             *
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

/*************************************************************************
  !!! The algorithm used here was extracted from UTF* converter codes !!!
  !!! (from unicode.org) which carrying these notices:                !!!
 
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

#ifndef Pt_Text_Utf8Codec_h
#define Pt_Text_Utf8Codec_h

#include <Pt/Api.h>
#include <Pt/Types.h>
#include <Pt/Text/TextCodec.h>
#include <Pt/Text/Char.h>


namespace Pt {

namespace Text {

	/**
	 * @brief This Codec class is able to convert from UTF-8 to UTF-32 and from UTF-32 to UTF-8.
	 *
	 * The method do_in() converts an array of char containing UTF-8-encoded data into an array
	 * of ptv::text::Char which is UTF-32-encoded, which means that the data is a direct readable
	 * 32-bit representation of the character.
	 *
	 * The method do_out() converts an array of ptv::text::Char objects (UTF-32/Unicode) into an
	 * array of char which contains the same sequence of characters in UTF-8-encoding.
	 */
	class PT_API Utf8Codec : public TextCodec<Char, char> {
		public:
			/**
			 * @brief Constructs a new Utf8Codec object which converts UTF-8 to UTF-32 and UTF-32 to UTF-8.
			 *
			 * The internal type is ptv::text::Char and external type is $char$
			 * 
			 * @param ref This optional parameter is passed to std::codecvt. When ref == 0 the locale takes
			 * care of deleting the facet. If ref == 1 the locale does not destroy the facet. Default value is 0.
			 */
			explicit Utf8Codec(size_t ref = 0);

			//! Empty destructor
			virtual ~Utf8Codec()
			{}

			//! decode UTF-8 to UTF-32
			virtual result do_in(mbstate_t& s, const char* fromBegin,
			                                const char* fromEnd, const char*& fromNext, 
			                                Char* toBegin, Char* toEnd, Char*& toNext) const;

			//! >encode UTF-32 to UTF-8
			virtual result do_out(mbstate_t& s, const Char* fromBegin,
			                                 const Char* fromEnd, const Char*& fromNext, 
			                                 char* toBegin, char* toEnd, char*& toNext) const;

			virtual bool do_always_noconv() const throw();

			virtual int do_length(mbstate_t& s, const char* fromBegin, const char* fromEnd, size_t max) const;

			virtual int do_max_length() const throw();
	};

} //namespace Text

} //namespace Pt

#endif

