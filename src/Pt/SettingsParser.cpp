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

SettingsParser::BeginStatement SettingsParser::beginStatement;
SettingsParser::OnSection SettingsParser::onSection;
SettingsParser::BeginType SettingsParser::beginType;
SettingsParser::AfterName SettingsParser::afterName;
SettingsParser::OnEqual SettingsParser::onEqual;
SettingsParser::OnCurly SettingsParser::onCurly;
SettingsParser::OnCloseCurly SettingsParser::onCloseCurly;
SettingsParser::OnQoutedValue SettingsParser::onQoutedValue;
SettingsParser::AfterQoutedValue SettingsParser::afterQoutedValue;
SettingsParser::OnRValue SettingsParser::onRValue;
SettingsParser::AfterRValue SettingsParser::afterRValue;
SettingsParser::BeginTypedValue SettingsParser::beginTypedValue;
SettingsParser::OnTypedValue SettingsParser::onTypedValue;
SettingsParser::OnQoutedTypedValue SettingsParser::onQoutedTypedValue;
SettingsParser::EndTypedValue SettingsParser::endTypedValue;
SettingsParser::AfterValue SettingsParser::afterValue;

SettingsParser::SettingsParser(std::basic_istream<Pt::Char>& is)
: state(0)
, _current(0)
, _is(&is)
, _line(0)
, _depth(0)
{ }


void SettingsParser::parse(SerializationInfo& si)
{
    _current = &si;
    state = &beginStatement;
    _line  = 0;
    Pt::Char ch = 0;

    while ( _is->get(ch) )
    {
        state = state->onChar(ch, *this);

        if(ch == '\n')
            ++_line;
    }

    // if exceptions are deactivated caller must check
    // istream for failure
    if( _is->bad() )
        return;

    state->onChar( std::char_traits<char>::eof(), *this );
}

}
