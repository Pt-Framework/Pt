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
#include <Pt/Text/codecvt.h>


namespace Pt {

namespace Text {

	//! \ingroup Ptv
	template <typename I, typename E>
	class PT_EXPORT TextCodec : public std::codecvt<I, E, mbstate_t> {
		public:
			typedef I InternT;
			typedef E ExternT;

		public:
			TextCodec(size_t ref = 0)
			: std::codecvt<InternT, ExternT, mbstate_t>(ref)
			{}

			virtual ~TextCodec()
			{}

		protected:
			std::codecvt_base::result do_unshift(mbstate_t&, ExternT*, ExternT*, ExternT*&) const
			{ return std::codecvt_base::ok; }

			int do_encoding() const throw()
			{ return 0; }

			bool do_always_noconv() const throw()
			{ return false; }
	};

} //namespace Text

using Text::TextCodec;

} //namespace Pt

#endif

