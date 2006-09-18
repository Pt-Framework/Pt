/***************************************************************************
 *   Copyright (C) 2005 Marc Boris Dürner                                  *
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
#define UEOF ((Pt::uint32_t)-1)

#include "Pt/Text/Char.h"
#include "UnicodeTable.h"
//#include "unicodedata.h"

#include <iostream>


namespace Pt {

namespace Text {

const Char Char::Null = Char(0);
const Char Char::Replacement = Char(0xfffd);
const Char Char::ByteOrderMark = Char(0xfeff);
const Char Char::ByteOrderSwapped = Char(0xfffe);


int Char::category() const
{
	return Unicode::category(_value);
}


int Char::bidiClass() const
{
	return 0;
}


int Char::decomposition() const
{
	return 0;
}


int Char::combining() const
{
	return 0;
}


int Char::isPunctuation() const
{
	int ret = 0;

	switch( this->category() )
	{
		case Char::PunctConnector:
		case Char::PunctDash:
		case Char::PunctOpen:
		case Char::PunctClose:
		case Char::PunctInitial:
		case Char::PunctFinal:
		case Char::PunctOther:
			ret = 1;
		default:
			break;
	}

	return ret;
}


int Char::isAlnum() const {
	int ret = 0;

	switch( this->category() )
	{
		case Char::LetterUpper:
		case Char::LetterLower:
		case Char::LetterTitle:
		case Char::LetterModifier:
		case Char::LetterOther:
		case Char::NumberDecimal:
		case Char::NumberLetter:
		case Char::NumberOther:
			ret = 1;
		default:
			break;
	}

	return ret;
}


int Char::isAlpha() const {
	int ret = 0;

	switch( this->category() )
	{
		case Char::LetterUpper:
		case Char::LetterLower:
		case Char::LetterTitle:
		case Char::LetterModifier:
		case Char::LetterOther:
			ret = 1;
		default:
			break;
	}

	return ret;
}


int Char::isControl() const {
	int ret = 0;

	switch(this->category())
	{
		case Char::MarkNonSpacing:
		case Char::MarkSpacingCombining:
		case Char::MarkEnclosing:
		case Char::OtherControl:
		case Char::OtherFormat:
		case Char::OtherSurrogate:
		case Char::OtherPrivate:
		case Char::OtherNotAssigned:
			ret = 1;
		default:
			break;
	}

	return ret;
}


int Char::isDigit() const {
	int ret = 0;

	switch(this->category())
	{
		case Char::NumberDecimal:
		case Char::NumberLetter:
		case Char::NumberOther:
			ret = 1;
		default:
			break;
	}

	return ret;
}


int Char::isGraph() const {
	int ret = 0;

	switch(this->category())
	{
		case Char::SymbolMath:
		case Char::SymbolCurrency:
		case Char::SymbolModifier:
		case Char::SymbolOther:
			ret = 1;
		default:
			break;
	}

	return ret;
}


int Char::isLower() const
{
	return this->category() == Char::LetterLower ? 1 : 0;
}


int Char::isPrint() const
{
	int ret = 1;

	switch( this->category() )
	{
		case Char::MarkNonSpacing:
		case Char::MarkSpacingCombining:
		case Char::MarkEnclosing:
		case Char::OtherControl:
		case Char::OtherFormat:
		case Char::OtherSurrogate:
		case Char::OtherPrivate:
		case Char::OtherNotAssigned:
			ret = 0;
		default:
			break;
	}

	return ret;
}


int Char::isUpper() const
{
	return this->category() == Char::LetterUpper ? 1 : 0;
}


int Char::isSpace() const
{
	int ret = 0;

	switch( this->category() )
	{
		case Char::SeparatorSpace:
		case Char::SeparatorLine:
		case Char::SeparatorParagraph:
			ret = 1;
		default:
			break;
	}

	return ret;
}


Char Char::toLower() const
{
	return Unicode::toLower(_value);
}


Char Char::toUpper() const
{
	return Unicode::toUpper(_value);
}


char Char::narrow(char def) const
{
	if( _value == std::char_traits<Char>::eof() ) {
		return std::char_traits<char>::eof();
	}

	if( _value <= 0xff ) {
		return (char)_value;
	}

	return def;
}


} // namespace Text

} // namespace Pt


