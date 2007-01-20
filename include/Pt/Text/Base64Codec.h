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

#ifndef Pt_Text_Base64Codec_h
#define Pt_Text_Base64Codec_h

#include <Pt/Text/Api.h>
#include <Pt/Types.h>
#include <Pt/Text/TextCodec.h>
#include <locale>


namespace Pt {

	class PT_TEXT_API Base64Codec : public TextCodec<char, char> {
		public:
			explicit Base64Codec(size_t ref = 0);

			virtual ~Base64Codec()
			{}

			virtual result do_in(mbstate_t& s, const char* fromBegin,
			                     const char* fromEnd, const char*& fromNext,
			                     char* toBegin, char* toEnd, char*& toNext) const;

			virtual result do_out(mbstate_t& s, const char* fromBegin,
			                      const char* fromEnd, const char*& fromNext,
			                      char* toBegin, char* toEnd, char*& toNext) const;

			virtual result do_unshift(mbstate_t& state, char* to,
			                         char* to_end, char*& to_next) const;

			virtual bool do_always_noconv() const throw();

			virtual int do_length(mbstate_t& s, const char* fromBegin, const char* fromEnd, size_t max) const;

			virtual int do_encoding() const throw();

			virtual int do_max_length() const throw();

		private:
			// move to state type
			mutable Pt::uint8_t _first;

			// move to state type
			mutable Pt::uint8_t _second;

			// move to state type
			mutable size_t _padSize;
	};


} //namespace Pt

#endif

