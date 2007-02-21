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

#include "Pt/Api.h"
#include "Pt/String.h"
#include "Pt/Char_numpunct.h"
#include <iostream>


namespace std {

    locale::id numpunct<Pt::Char>::id;


    numpunct<Pt::Char>::numpunct(size_t refs)
    : locale::facet(refs)
    { }


    numpunct<Pt::Char>::~numpunct()
    { }


    Pt::Char numpunct<Pt::Char>::decimal_point() const
    { return this->do_decimal_point(); }


    Pt::Char numpunct<Pt::Char>::thousands_sep() const
    { return this->do_thousands_sep(); }


    string numpunct<Pt::Char>::grouping() const
    { return this->do_grouping(); }


    Pt::String  numpunct<Pt::Char>::truename() const
    { return this->do_truename(); }


    Pt::String  numpunct<Pt::Char>::falsename() const
    { return this->do_falsename(); }


    Pt::Char numpunct<Pt::Char>::do_decimal_point() const
    { return '.'; }


    Pt::Char numpunct<Pt::Char>::do_thousands_sep() const
    { return ','; }


    std::string numpunct<Pt::Char>::do_grouping() const
    { return ""; }


    Pt::String numpunct<Pt::Char>::do_truename() const
    {
        static const Pt::Char truename[] = {'t', 'r', 'u', 'e', '\0'};
        return truename;
    }


    Pt::String numpunct<Pt::Char>::do_falsename() const
    {
        static const Pt::Char falsename[] = {'f', 'a', 'l', 's', 'e', '\0'};
        return falsename;
    }

} // namespace std

