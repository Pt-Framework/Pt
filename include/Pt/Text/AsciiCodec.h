/***************************************************************************
 *   Copyright (C) 2004 Marc Boris Dürner                                  *
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
 ***************************************************************************/

#ifndef PTV_AsciiCodec_h
#define PTV_AsciiCodec_h

#include <Pt/Text/Api.h>
#include <Pt/Exception.h>
#include <Pt/Text/TextCodec.h>
#include <Pt/Text/Char.h>

namespace Pt {

namespace Text {

	/**
	 * @brief This simple Codec class is able to convert from ASCII to UTF-32 and from UTF-32 to ASCII.
	 *
	 * The method do_in() converts an array of char containing ACSII-data into an array of
	 * Pt::Text::Char which is UTF-32-encoded, which means that the data is a direct readable
	 * 32-bit representation of the character.
	 *
	 * The method do_out() converts an array of Pt::Text::Char objects (UTF-32/Unicode) into an
	 * array of char which contains the same sequence of characters in ASCII-encoding.
	 */
	class PT_TEXT_API AsciiCodec : public TextCodec<Char, char> {
		public:
			/**
			 * @brief Constructs a new AsciiCodec object with internal type of Pt::Text::Char and
			 * external type of $char$.
			 *
			 * @param ref This parameter is passed to TextCodec. When ref == 0 the locale takes care
			 * of deleting the facet. If ref == 1 the locale does not destroy the facet.
			 */
			explicit AsciiCodec(size_t ref = 0);

			//! Empty desctructor
			virtual ~AsciiCodec();

			//! @brief Decodes ASCII to UTF-32.
			virtual result do_in(mbstate_t& s, const char* fromBegin, const char* fromEnd, const char*& fromNext,
			                                 Char* toBegin, Char* toEnd, Char*& toNext) const;

			//! @brief Encodes UTF-32 to ASCII.
			virtual result do_out(mbstate_t& s, const Char* fromBegin, const Char* fromEnd, const Char*& fromNext,
			                                 char* toBegin, char* toEnd, char*& toNext) const;

			// interhitdoc
			virtual bool do_always_no_conv() const throw();

			// interhitdoc
			virtual int do_length(mbstate_t& s, const char* fromBegin, const char* fromEnd, size_t max) const;

			// interhitdoc
			virtual int do_max_length() const throw();
	};

} // namespace Text

} //namespace Pt

#endif

