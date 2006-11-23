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

#ifndef Pt_TextCodec_h
#define Pt_TextCodec_h

#include <Pt/Api.h>
#include <Pt/Exception.h>
#include <Pt/Text/Char_codecvt.h>


namespace Pt {

namespace Text {

	/**
	 * @brief Generic TextCodec class/facet which may be subclassed by specific Codec classes.
	 *
	 * This class contains default implementations for the methods do_unshift(), do_encoding()
	 * and do_always_noconv() so sub-classes do not have to implement this default behaviour.
	 *
	 * Codecs are used to convert one text-encoding into another text-encoding. The internal
	 * and external data type can be specified using the template parameter 'I' (internal) and
	 * 'E' (external).
	 *
	 * When used on a platform which supports locales and facets the conversion may use
	 * locale-specific conversion of the text.
	 *
	 * This class derives from facet std::codecvt. Further documentation can be found there.
	 *
	 * @param I The character type associated with the internal code set.
	 * @param E The character type associated with the external code set.
	 *
	 * @see Utf8Codec
	 * @see Utf16Codec
	 * @see Utf32Codec
	 */
	template <typename I, typename E>
	class PT_EXPORT TextCodec : public std::codecvt<I, E, mbstate_t> {
		public:
			typedef I InternT;
			typedef E ExternT;

		public:
			/**
			 * @brief Constructs a new TextCodec object.
			 *
			 * The internal and external type are specified by the template parameters of the class.
			 * 
			 * @param ref This parameter is passed to std::codecvt. When ref == 0 the locale takes care
			 * of deleting the facet. If ref == 1 the locale does not destroy the facet.
			 */
			TextCodec(size_t ref = 0)
			: std::codecvt<InternT, ExternT, mbstate_t>(ref)
			{}

			//! Empty desctructor
			virtual ~TextCodec()
			{}

		protected:
			// inheritdoc
			std::codecvt_base::result do_unshift(mbstate_t&, ExternT*, ExternT*, ExternT*&) const
			{ return std::codecvt_base::ok; }

			// inheritdoc
			int do_encoding() const throw()
			{ return 0; }

			// inheritdoc
			bool do_always_noconv() const throw()
			{ return false; }
	};

} //namespace Text

using Text::TextCodec;

} //namespace Pt

#endif

