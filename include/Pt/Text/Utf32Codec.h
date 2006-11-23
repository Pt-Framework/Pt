/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Drner                               *
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

#ifndef _Pt_Utf32Codec_h
#define _Pt_Utf32Codec_h

#include <Pt/Api.h>
#include <Pt/Exception.h>
#include <Pt/Text/Char.h>
#include <Pt/Text/TextCodec.h>

namespace Pt {

namespace Text {

	class PT_EXPORT Utf32Codec : public TextCodec<Char, char> {
		public:
			/**
			 * @brief Constructs a new Utf32Codec object.
			 *
			 * The internal type is ptv::text::Char and external type is $char$
			 * 
			 * @param ref This optional parameter is passed to std::codecvt. When ref == 0 the locale takes
			 * care of deleting the facet. If ref == 1 the locale does not destroy the facet. Default value is 0.
			 */
			Utf32Codec(size_t ref = 0);

			//! Empty destructor
			virtual ~Utf32Codec();

			virtual Utf32Codec::result do_in(mbstate_t& s, const char* fromBegin,
			                                 const char* fromEnd, const char*& fromNext,
			                                 Char* toBegin, Char* toEnd, Char*& toNext) const;

			virtual Utf32Codec::result do_out(mbstate_t& s, const Char* fromBegin, const Char* fromEnd, const Char*& fromNext,
			                                 char* toBegin, char* toEnd, char*& toNext) const;

			// inheritdoc
			virtual int do_length(mbstate_t& s, const char* fromBegin, const char* fromEnd, size_t max) const;

			// inheritdoc
			virtual int do_max_length() const throw();

			// inheritdoc
			virtual bool do_always_no_conv() const throw();
	};

} //namespace Text

} //namespace Pt

#endif

