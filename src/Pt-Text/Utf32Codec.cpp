/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Drner                               *
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
 **************************************************************************/

#include "Pt/Text/Utf32Codec.h"
using namespace Pt::Text;


Utf32Codec::Utf32Codec(size_t ref)
: Pt::TextCodec<Char, char>(ref)
{
}


Utf32Codec::~Utf32Codec() 
{
}


Utf32Codec::result Utf32Codec::do_in(mbstate_t& s, const char* fromBegin,
                                    const char* fromEnd, const char*& fromNext,
                                    Char* toBegin, Char* toEnd, Char*& toNext) const
{
	return Utf32Codec::noconv;
}


Utf32Codec::result Utf32Codec::do_out(mbstate_t& s, const Char* fromBegin,
                                     const Char* fromEnd, const Char*& fromNext, 
                                     char* toBegin, char* toEnd, char*& toNext) const
{
	return Utf32Codec::noconv;
}


int Utf32Codec::do_length(mbstate_t& s, const char* fromBegin, const char* fromEnd, size_t max) const
{
	return (fromEnd - fromBegin)/4;
}


int Utf32Codec::do_max_length() const throw()
{
	return 4;
}


bool Utf32Codec::do_always_no_conv() const throw()
{
	return true;
}
