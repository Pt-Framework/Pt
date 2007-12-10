/***************************************************************************
 *   Copyright (C) 2004-2007 Marc Boris Duerner                            *
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
#include "Pt/Facets.h"
#include "Pt/String.h"
#include "Pt/Unicode.h"

namespace std {

//
// ctype facet
//
std::locale::id ctype<Pt::Char>::id;

#if (defined _MSC_VER || defined __QNX__ || defined __xlC__)

ctype<Pt::Char>::ctype(size_t refs)
: ctype_base(refs)
{ }

#else

ctype<Pt::Char>::ctype(size_t refs)
: locale::facet(refs)
{ }

#endif


ctype<Pt::Char>::~ctype()
{
}


bool ctype<Pt::Char>::do_is(mask m, Pt::Char c) const
{
    return m & ctypeMask(c);
}


const Pt::Char*
ctype<Pt::Char>::do_is(const Pt::Char* begin, const Pt::Char* end, mask* vec) const
{
    for( ; begin < end; ++begin)
    {
        *vec = ctypeMask(*begin);
        ++vec;
    }

    return end;
}


const Pt::Char*
ctype<Pt::Char>::do_scan_is(mask m, const Pt::Char* begin, const Pt::Char* end) const
{
    while( begin != end && !is(m,*begin))
    {
        ++begin;
    }

    return begin;
}


const Pt::Char*
ctype<Pt::Char>::do_scan_not(mask m, const Pt::Char* begin, const Pt::Char* end) const
{
    while( begin != end && is(m,*begin))
    {
        ++begin;
    }

    return begin;
}


Pt::Char
ctype<Pt::Char>::do_toupper(Pt::Char ch) const
{
    return toupper(ch);
}


const Pt::Char*
ctype<Pt::Char>::do_toupper(Pt::Char* begin, const Pt::Char* end) const
{
    for(; begin < end; ++begin)
    {
        *begin = do_toupper(*begin);
    }

    return end;
}


Pt::Char
ctype<Pt::Char>::do_tolower(Pt::Char ch) const
{
    return tolower(ch);
}


const Pt::Char*
ctype<Pt::Char>::do_tolower(Pt::Char* begin, const Pt::Char* end) const
{
    for(; begin < end; ++begin) {
        *begin = do_tolower(*begin);
    }

    return end;
}


Pt::Char ctype<Pt::Char>::do_widen(char ch) const
{
    return Pt::Char(ch);
}


const char* ctype<Pt::Char>::do_widen(const char* begin, const char* end, Pt::Char* dest) const
{
    for(const char* cur = begin; cur < end; ++cur) {
        *dest = do_widen(*cur);
        ++dest;
    }

    return end;
}


char ctype<Pt::Char>::do_narrow(Pt::Char ch, char dfault) const
{
    return ch.narrow(dfault);
}


const Pt::Char*
ctype<Pt::Char>::do_narrow(const Pt::Char* begin, const Pt::Char* end, char dfault, char* dest) const
{
    for(const Pt::Char* cur = begin; cur < end; ++cur) {
        *dest = do_narrow(*cur, dfault);
        ++dest;
    }

    return end;
}


//
// numpunct facet
//
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

//
// codecvt facet
//
std::locale::id codecvt<Pt::Char, char, mbstate_t>::id;


#if defined _MSC_VER || __QNX__

codecvt<Pt::Char, char, mbstate_t>::codecvt(size_t ref)
: codecvt_base(ref)
{}

#else

codecvt<Pt::Char, char, mbstate_t>::codecvt(size_t ref)
: locale::facet(ref)
{}

#endif


codecvt<Pt::Char, char, mbstate_t>::~codecvt()
{}

} // namespace std
