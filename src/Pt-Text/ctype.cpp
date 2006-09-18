/***************************************************************************
 *   Copyright (C) 2005 Marc Boris Drner                                  *
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

#include "Pt/Text/ctype.h"
using namespace Pt;

#include <iostream>


namespace std {

std::locale::id ctype<Pt::Char>::id;


#ifdef _MSC_VER

ctype<Pt::Char>::ctype(size_t refs)
: ctype_base(refs)
{}

#else

ctype<Pt::Char>::ctype(size_t refs)
: locale::facet(refs)
{}

#endif


ctype<Pt::Char>::~ctype() 
{
}


ctype<Pt::Char>::mask ctype<Pt::Char>::lookup(Pt::Char c) const
{
	ctype_base::mask m = 0;

	switch( c.category() ) {
		case Char::MarkNonSpacing:
			m |= ctype_base::print;
			break;

		case Char::MarkSpacingCombining:
			m |= ctype_base::print;
			break;

		case Char::MarkEnclosing:
			m |= ctype_base::print;
			break;

		case Char::NumberDecimal:
			m |= ctype_base::print;
			m |= ctype_base::digit;
			m |= ctype_base::alnum;
			m |= ctype_base::graph;
			break;

		case Char::NumberLetter:
			m |= ctype_base::print;
			//m |= ctype_base::alpha;
			//m |= ctype_base::alnum;
			//m |= ctype_base::graph;
			break;

		case Char::NumberOther:
			m |= ctype_base::print;
			break;

		case Char::SeparatorSpace:
			m |= ctype_base::space;
			break;

		case Char::SeparatorLine:
			m |= ctype_base::space;
			break;

		case Char::SeparatorParagraph:
			m |= ctype_base::space;
			break;

		case Char::OtherControl:
			m |= ctype_base::cntrl;
			break;

		case Char::OtherFormat:
			m |= ctype_base::cntrl;
			break;

		case Char::OtherSurrogate:
			m |= ctype_base::cntrl;
			break;

		case Char::OtherPrivate:
			m |= ctype_base::cntrl;
			break;

		case Char::OtherNotAssigned:
			m |= ctype_base::cntrl;
			break;

		case Char::LetterUpper:
			m |= ctype_base::print;
			m |= ctype_base::upper;
			m |= ctype_base::alpha;
			m |= ctype_base::alnum;
			m |= ctype_base::graph;
			break;

		case Char::LetterLower:
			m |= ctype_base::print;
			m |= ctype_base::lower;
			m |= ctype_base::alpha;
			m |= ctype_base::alnum;
			m |= ctype_base::graph;
			break;

		case Char::LetterTitle:
			m |= ctype_base::print;
			//m |= ctype_base::upper;
			m |= ctype_base::alpha;
			m |= ctype_base::alnum;
			m |= ctype_base::graph;
			break;

		case Char::LetterModifier:
			m |= ctype_base::print;
			break;

		case Char::LetterOther:
			m |= ctype_base::print;
			break;

		case Char::PunctConnector:
			m |= ctype_base::print;
			m |= ctype_base::punct;
			m |= ctype_base::graph;
			break;

		case Char::PunctDash:
			m |= ctype_base::print;
			m |= ctype_base::punct;
			m |= ctype_base::graph;
			break;

		case Char::PunctOpen:
			m |= ctype_base::print;
			m |= ctype_base::punct;
			m |= ctype_base::graph;
			break;

		case Char::PunctClose:
			m |= ctype_base::print;
			m |= ctype_base::punct;
			m |= ctype_base::graph;
			break;

		case Char::PunctInitial:
			m |= ctype_base::print;
			m |= ctype_base::punct;
			m |= ctype_base::graph;
			break;

		case Char::PunctFinal:
			m |= ctype_base::print;
			m |= ctype_base::punct;
			m |= ctype_base::graph;
			break;

		case Char::PunctOther:
			m |= ctype_base::print;
			m |= ctype_base::punct;
			m |= ctype_base::graph;
			break;

		case Char::SymbolMath:
			m |= ctype_base::print;
			break;

		case Char::SymbolCurrency:
			m |= ctype_base::print;
			break;

		case Char::SymbolModifier:
			m |= ctype_base::print;
			break;

		case Char::SymbolOther:
			m |= ctype_base::print;
			break;
	}

	return m;
}


bool ctype<Pt::Char>::do_is(mask m, Pt::Char c) const 
{
	return m == this->lookup(c);
}


const Pt::Char* ctype<Pt::Char>::do_is(const Pt::Char* begin, const Pt::Char* end, mask* vec) const 
{
	for( ; begin < end; ++begin) {
		*vec = this->lookup(*begin);
		++vec;
	}

	return end;
}


const Pt::Char* ctype<Pt::Char>::do_scan_is(mask m, const Pt::Char* begin, const Pt::Char* end) const {
	while( begin != end && !is(m,*begin)) {
		++begin;
	}

	return begin;
}


const Pt::Char* ctype<Pt::Char>::do_scan_not(mask m, const Pt::Char* begin, const Pt::Char* end) const {
	while( begin != end && is(m,*begin)) {
		++begin;
	}

	return begin;
}


Pt::Char ctype<Pt::Char>::do_toupper(Pt::Char ch) const {
	return ch.toUpper();
}


const Pt::Char* ctype<Pt::Char>::do_toupper(Pt::Char* begin, const Pt::Char* end) const {
	for(; begin < end; ++begin) {
		*begin = do_toupper(*begin);
	}

	return end;
}


Pt::Char ctype<Pt::Char>::do_tolower(Pt::Char ch) const {
	return ch.toLower();
}


const Pt::Char* ctype<Pt::Char>::do_tolower(Pt::Char* begin, const Pt::Char* end) const {
	for(; begin < end; ++begin) {
		*begin = do_tolower(*begin);
	}

	return end;
}


Pt::Char ctype<Pt::Char>::do_widen(char ch) const {
	return Pt::Char(ch);
}


const char* ctype<Pt::Char>::do_widen(const char* begin, const char* end, Pt::Char* dest) const {
	for(const char* cur = begin; cur < end; ++cur) {
		*dest = do_widen(*cur);
		++dest;
	}

	return end;
}


char ctype<Pt::Char>::do_narrow(Pt::Char ch, char dfault) const {
	return ch.narrow(dfault);
}


const Pt::Char* ctype<Pt::Char>::do_narrow(const Pt::Char* begin, const Pt::Char* end, char dfault, char* dest) const {
	for(const Pt::Char* cur = begin; cur < end; ++cur) {
		*dest = do_narrow(*cur, dfault);
		++dest;
	}

	return end;
}

} // namespace std


