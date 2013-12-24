/*
 * Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "SettingsReader.h"
#include <Pt/Convert.h>

namespace Pt {

void SettingsReader::State::syntaxError(std::size_t line)
{
    throw SettingsError("syntax error", line);
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
        std::size_t pos = name.rfind('.');
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
        throw SettingsError("too many closing braces", _line);

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
    if(_token == L"yes" || _token == L"YES" ||
       _token == L"on" || _token == L"ON" ||
       _token == L"true" || _token == L"TRUE" )
    {
        _current->setBool(true);
    }
    else if(_token == L"no" || _token == L"NO" ||
            _token == L"off" || _token == L"OFF" ||
            _token == L"false" || _token == L"FALSE" )
    {
        _current->setBool(false);
    }
    else
    {
        unsigned dot = 0;
        unsigned digits = 0;
        Pt::String::const_iterator it;
        for( it = _token.begin(); it != _token.end(); ++it )
        {
            if(*it == '.')
                dot++;
            else if(Pt::isdigit(*it))
                digits++;
        }

        if(dot == 1 && digits >= 1 && (_token.length() - 1) == digits )
        {
            double d = 0;
            Pt::parseFloat(_token.begin(), _token.end(), d);
            _current->setDouble(d);
        }
        else if(_token.length() == digits && digits >= 1)
        {
            Pt::int32_t i = 0;
            Pt::parseInt(_token.begin(), _token.end(), i);
            _current->setInt32(i);
        }
        else
        {
            throw SettingsError("invalid entry value", line());
        }
    }

    _token.clear();
}


void SettingsReader::pushString()
{
    _current->setString(_token);
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
        throw SettingsError("unexpected EOF", _line );

    switch( ch.value() )
    {
        case 'n':
            return Pt::Char(L'\n');

        case 'r':
            return Pt::Char(L'\r');
    }

    return ch;
}

} // namespace Pt
