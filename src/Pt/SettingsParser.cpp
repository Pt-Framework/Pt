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
#include "SettingsParser.h"

namespace Pt {

SettingsParser::OnComment SettingsParser::onComment;
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
, _beforeComment(0)
, _current(0)
, _is(&is)
, _line(1)
, _depth(0)
, _isDotted(false)
{ }


void SettingsParser::parse(SerializationInfo& si)
{
    _current = &si;
    state = &beginStatement;
    _line  = 1;
    _isDotted = false;
    Pt::Char ch = 0;

    while ( _is->get(ch) )
    {
        state = state->onChar(ch, *this);

        if(ch == '\n')
        {
            ++_line;
        }
    }

    // if exceptions are deactivated caller must check
    // istream for failure
    if( _is->bad() )
        return;

    state->onChar( std::char_traits<char>::eof(), *this );
}

void SettingsParser::enterMember()
{
    //std::cerr << std::endl;
    //for(unsigned n = 0; n < _depth; ++n)
    //    std::cerr << "   ";
    //std::cerr << "+" << _token.narrow() << std::endl;

    //
    // Consider namespace at top-level. For example a.b.c means c
    // as a child of a.b. both are only added when not present.
    // If we are not top-level, always add a node.
    //
    if( _depth == 0 )
    {
        std::string name = _token.narrow();
        if( _section.size() )
            name = _section.narrow() + '.' + name;

        //
        // Add a serialization node for the parent if not present.
        // In this example the parent is a.b
        //
        size_t pos  = name.rfind('.');
        if(pos != std::string::npos)
        {
            Pt::SerializationInfo* current = _current->findMember( name.substr( 0, pos ) );
            if(current == 0)
                current = &( _current->addMember( name.substr( 0, pos ) ) );

            _current = current;
            ++_depth;

            _isDotted = true; // remember that we have to leave twice later
            name = name.substr( ++pos );
        }

        //
        // Add a node for the actual value if not present. I this
        // example c is a parent of a.b
        //
        Pt::SerializationInfo* current = _current->findMember( name );
        if(current == 0)
            current = &( _current->addMember( name ) );

        _current = current;
        ++_depth;
    }
    else
    {
        _current = &( _current->addMember( _token.narrow() ) );
        ++_depth;
    }

    _token.clear();
}

void SettingsParser::leaveMember()
{
    //std::cerr << "@" << std::endl;

    if(0 == _current->parent() )
        throw std::runtime_error("parse error too many closing braces");

    _current = _current->parent();
    --_depth;

    if(_depth == 1 && _isDotted)
    {
        // leaving a dotted entry
        _current = _current->parent();
        _isDotted = false;
        --_depth;
    }
}

void SettingsParser::pushValue()
{
    //for(unsigned n = 0; n < _depth; ++n)
    //    std::cerr << "   ";
    //std::cerr << "- value: " << _token.narrow() << std::endl;

    _current->setValue(_token);
    _token.clear();
}

void SettingsParser::pushTypeName()
{
    //for(unsigned n = 0; n < _depth; ++n)
    //    std::cerr << "   ";
    //std::cerr << "- type: " << _token.narrow() << std::endl;

    _current->setTypeName( _token.narrow() );
    _token.clear();
}

void SettingsParser::pushName()
{
    //for(unsigned n = 0; n < _depth; ++n)
    //    std::cerr << "   ";
    //std::cerr << "- name: " << _token.narrow() << std::endl;

    _current->setName( _token.narrow() );
    _token.clear();
}


/*
bool SettingsReader::getEscaped(Pt::String& s)
{
    Pt::Char ch;
    if( ! _is->get(ch) )
        return false;

    switch( ch.value() )
    {
        case 'n':
            s += Pt::Char(L'\n');
            break;

        case 'r':
            s += Pt::Char(L'\r');
            break;

        default:
            s += ch;
    }

    return true;
}
*/

}
