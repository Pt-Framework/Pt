/***************************************************************************
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 Tobias Mller                                 *
 *   Copyright (C) 2006-2007 PTV AG                                        *
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
#ifndef PT_UNICODE_H
#define PT_UNICODE_H

#include <Pt/Api.h>
#include <Pt/Types.h>
#include <Pt/Char.h>
#include <Pt/Locale.h>

namespace Pt {

PT_API std::ctype_base::mask ctypeMask(const Char& ch);


inline int isalpha(const Pt::Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::alpha;
    /*int cat = category(ch);
    return ( cat >= Unicode::LetterUpper ) &&
           ( cat <= Unicode::LetterOther);*/
}


inline int isalnum(const Pt::Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::alnum;
    /*int cat = category(ch);
    return ( cat >= Unicode::LetterUpper ) &&
           ( cat <= Unicode::NumberOther);*/
}


inline int ispunct(const Pt::Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::punct;
    /*int cat = category(ch);
    return ( cat >= Unicode::PunctConnector ) &&
           ( cat <= Unicode::PunctOther );*/
}


inline int iscntrl(const Pt::Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::cntrl;
    //int cat = category(ch);
    //return ( cat <= Unicode::MarkEnclosing );
}


inline int isdigit(const Pt::Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::digit;
}


inline int isxdigit(const Pt::Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::xdigit;
}

inline int isgraph(const Pt::Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::graph;
    //int cat = category(ch);
    //return ( cat >= Unicode::LetterUpper ) &&
    //       ( cat <= Unicode::SymbolOther );
}


inline int islower(const Pt::Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::lower;
    //return category(ch) == Unicode::LetterLower;
}


inline int isupper(const Pt::Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::upper;
    //return category(ch) == Unicode::LetterUpper;
}


inline int isprint(const Pt::Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::print;
    /*int cat = category(ch);
    return ( cat >= Unicode::MarkNonSpacing ) &&
           ( cat <= Unicode::SymbolOther );*/
}


inline int isspace(const Pt::Char& ch)
{
    return ctypeMask(ch) & std::ctype_base::space;
    /*int cat = category(ch);
    return ( cat >= Unicode::MarkNonSpacing ) &&
           ( cat <= Unicode::SeparatorParagraph ) ||
           ( ch.value() >= 9 && ch.value() <= 13);*/
}


PT_API Pt::Char tolower(const Pt::Char& ch);


PT_API Pt::Char toupper(const Pt::Char& ch);

} // namespace Pt

#endif
