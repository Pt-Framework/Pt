/***************************************************************************
 *   Copyright (C) 2004 Marc Boris Drner                                  *
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
#ifndef Pt_ctype_h
#define Pt_ctype_h

#include <Pt/Text/Char.h>

#include <locale>


namespace std {

#ifdef _MSC_VER

	template <>
	class PT_EXPORT ctype<Pt::Char> : public ctype_base {

#else

	template <>
	class PT_EXPORT ctype<Pt::Char> : public ctype_base, public locale::facet {

#endif

		public:
			typedef ctype_base::mask mask;

			static locale::id id;

		public:
			explicit ctype(size_t refs = 0);

			virtual ~ctype();

			bool is(mask m, Pt::Char c) const
			{ return this->do_is(m, c); }

			const Pt::Char* is(const Pt::Char *lo, const Pt::Char *hi, mask *vec) const
			{ return this->do_is(lo, hi, vec); }

			const Pt::Char* scan_is(mask m, const Pt::Char* lo, const Pt::Char* hi) const
			{ return this->do_scan_is(m, lo, hi); }

			const Pt::Char* scan_not(mask m, const Pt::Char* lo, const Pt::Char* hi) const
			{ return this->do_scan_not(m, lo, hi); }

			Pt::Char toupper(Pt::Char c) const
			{ return this->do_toupper(c); }

			const Pt::Char* toupper(Pt::Char *lo, const Pt::Char* hi) const
			{ return this->do_toupper(lo, hi); }

			Pt::Char tolower(Pt::Char c) const
			{ return this->do_tolower(c); }

			const Pt::Char* tolower(Pt::Char* lo, const Pt::Char* hi) const
			{ return this->do_tolower(lo, hi); }

			Pt::Char widen(char c) const
			{ return this->do_widen(c); }

			const char* widen(const char* lo, const char* hi, Pt::Char* to) const
			{ return this->do_widen(lo, hi, to); }

			char narrow(Pt::Char c, char dfault) const
			{ return this->do_narrow(c, dfault); }

			const Pt::Char* narrow(const Pt::Char* lo, const Pt::Char* hi,
			                       char dfault, char *to) const
			{ return this->do_narrow(lo, hi, dfault, to); }

		protected:
			ctype_base::mask lookup(Pt::Char c) const;

			virtual bool do_is(mask m, Pt::Char c) const;

			virtual const Pt::Char* do_is(const Pt::Char* lo, const Pt::Char* hi,
			                               mask* vec) const;

			virtual const Pt::Char* do_scan_is(mask m, const Pt::Char* lo,
			                                    const Pt::Char* hi) const;

			virtual const Pt::Char* do_scan_not(mask m, const Pt::Char* lo,
			                                     const Pt::Char* hi) const;

			virtual Pt::Char do_toupper(Pt::Char) const;

			virtual const Pt::Char* do_toupper(Pt::Char* lo, const Pt::Char* hi) const;

			virtual Pt::Char do_tolower(Pt::Char) const;

			virtual const Pt::Char* do_tolower(Pt::Char* lo, const Pt::Char* hi) const;

			virtual Pt::Char do_widen(char) const;

			virtual const char* do_widen(const char* lo, const char* hi,
			                              Pt::Char* dest) const;

			virtual char do_narrow(Pt::Char, char dfault) const;

			virtual const Pt::Char* do_narrow(const Pt::Char* lo, const Pt::Char* hi,
			                                   char dfault, char* dest) const;
	};

} // namespace std


#endif
