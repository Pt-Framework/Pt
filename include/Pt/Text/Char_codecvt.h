/***************************************************************************
 *   Copyright (C) 2004 Marc Boris Duerner                                 *
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

#ifndef PTV_Text_codecvt_h
#define PTV_Text_codecvt_h

#include <Pt/Text/Api.h>
#include <Pt/Text/Char.h>
#include <Pt/Text/locale.h>

// This class' filename was suffixed with "Char_" because using "codecvt.h" as filename confuses
// the compiler/linker as there a STL-files of the same name.

namespace std {

#ifdef _MSC_VER

	class PT_TEXT_API codecvt_base;

	template<>
	class PT_TEXT_API codecvt<Pt::Char, char, mbstate_t> : public codecvt_base {

#else

	template<>
	class PT_TEXT_API codecvt<Pt::Char, char, mbstate_t> : public codecvt_base, public locale::facet {
#endif

		public:
			static locale::id id;

		public:
			explicit codecvt(size_t ref = 0);

			virtual ~codecvt();

			codecvt_base::result out(mbstate_t& state, const Pt::Char* from,
			                         const Pt::Char* from_end, const Pt::Char*& from_next,
			                         char* to, char* to_end, char*& to_next) const
			{ return this->do_out(state, from, from_end, from_next, to, to_end, to_next); }

			codecvt_base::result unshift(mbstate_t& state, char* to, char* to_end,
			                             char*& to_next) const
			{ return this->do_unshift(state, to, to_end, to_next); }

			codecvt_base::result in(mbstate_t& state, const char* from,
			                        const char* from_end, const char*& from_next,
			                        Pt::Char* to, Pt::Char* to_end, Pt::Char*& to_next) const
			{ return this->do_in(state, from, from_end, from_next, to, to_end, to_next); }

			int encoding() const
			{ return this->do_encoding(); }

			bool always_noconv() const
			{ return this->do_always_noconv(); }

			int length(mbstate_t& state, const char* from,
			           const char* end, size_t max) const
			{ return this->do_length(state, from, end, max); }

			int max_length() const
			{ return this->do_max_length(); }

		protected:
			virtual codecvt_base::result do_out(mbstate_t& state, const Pt::Char* from,
			                                    const Pt::Char* from_end, const Pt::Char*& from_next,
			                                    char* to, char* to_end, char*& to_next) const = 0;

			virtual codecvt_base::result do_unshift(mbstate_t& state, char* to,
			                                        char* to_end, char*& to_next) const = 0;

			virtual codecvt_base::result do_in(mbstate_t& state,
			                                   const char* from, const char* from_end,
			                                   const char*& from_next, Pt::Char* to, Pt::Char* to_end,
			                                   Pt::Char*& to_next) const = 0;

			virtual int do_encoding() const = 0;

			virtual bool do_always_noconv() const = 0;

			virtual int do_length(mbstate_t&, const char* from,
			                      const char* end, size_t max) const = 0;

			virtual int do_max_length() const = 0;
	};

}

#endif
