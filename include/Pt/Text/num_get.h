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
#ifndef Pt_Text_num_get_h
#define Pt_Text_num_get_h

#include <Pt/Text/Char.h>

#include <locale>


namespace std {

	template<typename InIterT>
	class num_get<Pt::Char, InIterT> : public locale::facet {
		public:
			typedef Pt::Char char_type;
			typedef InIterT iter_type;

			static locale::id id;

			explicit num_get(size_t refs = 0)
			: facet(refs)
			{ }

			virtual ~num_get()
			{ }

			iter_type get(iter_type in, iter_type end, ios_base& io, ios_base::iostate& err, bool& v) const
			{ return this->do_get(in, end, io, err, v); }

			iter_type get(iter_type in, iter_type end, ios_base& io, ios_base::iostate& err, long& v) const
			{ return this->do_get(in, end, io, err, v); }

			iter_type
			get(iter_type in, iter_type end, ios_base& io, ios_base::iostate& err, unsigned short& v) const
			{ return this->do_get(in, end, io, err, v); }

			iter_type
			get(iter_type in, iter_type end, ios_base& io, ios_base::iostate& err, unsigned int& v)   const
			{ return this->do_get(in, end, io, err, v); }

			iter_type
			get(iter_type in, iter_type end, ios_base& io, ios_base::iostate& err, unsigned long& v)  const
			{ return this->do_get(in, end, io, err, v); }

			iter_type
			get(iter_type in, iter_type end, ios_base& io, ios_base::iostate& err, float& v) const
			{ return this->do_get(in, end, io, err, v); }

			iter_type
			get(iter_type in, iter_type end, ios_base& io, ios_base::iostate& err, double& v) const
			{ return this->do_get(in, end, io, err, v); }

			iter_type
			get(iter_type in, iter_type end, ios_base& io, ios_base::iostate& err, long double& v) const
			{ return this->do_get(in, end, io, err, v); }

			iter_type
			get(iter_type in, iter_type end, ios_base& io, ios_base::iostate& err, void*& v) const
			{ return this->do_get(in, end, io, err, v); }

		protected:
			virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate&, bool&) const
			{}

			virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate&, long&) const
			{}

			virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate& err, unsigned short&) const
			{}

			virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate& err, unsigned int&) const
			{}

			virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate& err, unsigned long&) const
			{}

			virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate& err, float&) const
			{}

			virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate& err, double&) const
			{}

			virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate& err, long double&) const
			{}

			virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate& err, void*&) const
			{}
	};

	template<typename InIterT>
	locale::id num_get<Pt::Char, InIterT>::id;

} // namespace std


#endif
