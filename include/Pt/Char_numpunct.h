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
#ifndef PTV_numpunct_h
#define PTV_numpunct_h

#include <Pt/Api.h>
#include <Pt/Char.h>
#include <Pt/String.h>
#include <Pt/locale.h>

// This class' filename was suffixed with "Char_" because using "numpunct.h" as filename confuses
// the compiler/linker as there a STL-files of the same name.

namespace std {

    template <>
    class PT_API numpunct<Pt::Char> : public locale::facet {
        public:
            typedef Pt::Char char_type;
            typedef basic_string<Pt::Char> string_type;

            // gcc 3.4.x violates the c++ standard by requiring a __numpunct_cache
            #if __GNUC__ == 3 && __GNUC_MINOR__ == 4
            typedef __numpunct_cache<Pt::Char>  __cache_type;
            #endif

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
