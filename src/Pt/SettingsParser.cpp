/***************************************************************************
 *   Copyright (C) 2005-2007 by Dr. Marc Boris Duerner                     *
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
#include "Pt/SettingsParser.h"

namespace Pt {

SettingsParser::SettingsParser(std::basic_istream<Pt::Char>& is)
: state(0)
, _is(&is)
{ }

void SettingsParser::parse(SerializationInfo& si)
{
    //state = &Parser::beforeExpression;

    Pt::Char ch;
    while ( _is->get(ch) )
    {
        state = state->onChar(ch, *this);
    }

    // if exceptions are deactivated caller must check
    // istream for failure
    if( _is->bad() )
        return;

    state->onChar( std::char_traits<char>::eof(), *this );
}

}
