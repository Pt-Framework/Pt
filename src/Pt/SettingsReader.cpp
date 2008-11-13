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
#include "SettingsReader.h"

namespace Pt {

void SettingsReader::State::syntaxError(unsigned line, const SourceInfo& si)
{
	std::string msg("syntax error");
	throw SettingsError(msg, line, si );
}


void SettingsReader::parse(SerializationInfo& si)
{
    _current = &si;
    state = BeginStatement::instance();
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


void SettingsReader::enterMember()
{
    //
    // Consider namespace at top-level. For example a.b.c means c
    // as a child of a.b. both are only added when not present.
    // If we are not top-level, always add a node.
    //
    if( _depth == 0 )
    {
        std::string name;
        if( _section.size() )
        {
            name += _section.narrow();
            name += '.';
            name += _token.narrow();
        }
        else
        {
            name = _token.narrow();
        }

        //
        // Add a serialization node for the parent if not present.
        // In this example the parent is a.b
        //
        size_t pos = name.rfind('.');
        if(pos != std::string::npos)
        {
            std::string root = name.substr( 0, pos );
            Pt::SerializationInfo* current = _current->findMember( root );
            if(current == 0)
                current = &( _current->addMember( root ) );

            _current = current;
            ++_depth;

            _isDotted = true; // remember that we have to leave twice later
            name = name.substr( ++pos ); // TODO: use remove or erase
        }

        //
        // Add a node for the actual value if not present. In this
        // example c is a parent of a.b
        //
        Pt::SerializationInfo* current = _current->findMember( name );
        if(current == 0)
            current = &( _current->addMember( name ) );

        _current = current;
    }
    else
    {
        _current = &( _current->addMember( _token.narrow() ) );
    }

    ++_depth;
    _token.clear();
}


void SettingsReader::leaveMember()
{
    //std::cerr << "@" << std::endl;

    if(0 == _current->parent() )
        throw SettingsError("too many closing braces", _line, PT_SOURCEINFO);

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


void SettingsReader::pushValue()
{
    _current->setValue(_token);
    _token.clear();
}


void SettingsReader::pushTypeName()
{
    _current->setTypeName( _token.narrow() );
    _token.clear();
}


void SettingsReader::pushName()
{
    _current->setName( _token.narrow() );
    _token.clear();
}


Pt::Char SettingsReader::getEscaped()
{
    Pt::Char ch;
    if( ! _is->get(ch) )
        throw SettingsError("unexpected EOF", _line, PT_SOURCEINFO);

    switch( ch.value() )
    {
        case 'n':
            return Pt::Char(L'\n');

        case 'r':
            return Pt::Char(L'\r');
    }

    return ch;
}

}
