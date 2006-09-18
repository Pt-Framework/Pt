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
#ifndef Pt_Text_numpunct_h
#define Pt_Text_numpunct_h

#include <Pt/Api.h>
#include <Pt/Text/Char.h>
#include <Pt/Text/String.h>

#include <locale>


namespace std {

	template <>
	class PT_EXPORT numpunct<Pt::Text::Char> : public locale::facet {
		public:
			typedef Pt::Char char_type;
			typedef basic_string<Pt::Text::Char> string_type;

			static locale::id id;

		public:
			explicit numpunct(size_t refs = 0);

			virtual ~numpunct()
			{ }

			char_type decimal_point() const
			{ return this->do_decimal_point(); }

			char_type thousands_sep() const
			{ return this->do_thousands_sep(); }

			string grouping() const
			{ return this->do_grouping(); }

			string_type truename() const
			{ return this->do_truename(); }

			string_type falsename() const
			{ return this->do_falsename(); }

		protected:
			virtual char_type do_decimal_point() const
			{ return '.'; }

			virtual char_type do_thousands_sep() const
			{ return ','; }

			virtual string do_grouping() const
			{ return ""; }

			virtual string_type do_truename() const
			{
				static const Pt::Char truename[] = {'t', 'r', 'u', 'e', '\0'};
				return truename;
			}

			virtual string_type do_falsename() const
			{
				static const Pt::Char falsename[] = {'f', 'a', 'l', 's', 'e', '\0'};
				return falsename;
			}
	};


} // namespace std


#endif
