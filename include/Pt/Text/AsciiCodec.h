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

#ifndef Pt_AsciiCodec_h
#define Pt_AsciiCodec_h

#include <Pt/Api.h>
#include <Pt/Exception.h>
#include <Pt/Text/TextCodec.h>
#include <Pt/Text/Char.h>

namespace Pt {

namespace Text {

	//! A very lame Codec to decode ascii to UTF-32 and encode UTF-32 to ASCII
	class PT_EXPORT AsciiCodec : public TextCodec<Char, char> {
		public:
			explicit AsciiCodec(size_t ref = 0);

			virtual ~AsciiCodec();

			//! ASCII to UTF-32
			virtual result do_in(mbstate_t& s, const char* fromBegin, const char* fromEnd, const char*& fromNext,
			                                 Char* toBegin, Char* toEnd, Char*& toNext) const;

			//! UTF-32 to ASCII
			virtual result do_out(mbstate_t& s, const Char* fromBegin, const Char* fromEnd, const Char*& fromNext,
			                                 char* toBegin, char* toEnd, char*& toNext) const;

			virtual bool do_always_no_conv() const throw();

			virtual int do_length(mbstate_t& s, const char* fromBegin, const char* fromEnd, size_t max) const;

			virtual int do_max_length() const throw();
	};

} // namespace Text

} //namespace Pt

#endif

