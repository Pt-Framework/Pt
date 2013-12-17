/*
 * Copyright (C) 2010 Marc Duerner
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

#include "Pt/Regex.h"
#include "regexp.h"
#include <stdexcept>
#include <cctype>
#include <cassert>
#include <cstdlib>

namespace Pt {

InvalidRegex::InvalidRegex(const char* msg)
: std::runtime_error(msg)
{}


Regex::Regex()
: _expr(0)
{
}


Regex::Regex(const Pt::Char* ex)
: _expr(0)
{
    _expr = ::regcomp(ex);

    if( ! _expr )
        throw InvalidRegex("invalid regex");

    assert(_expr->regrefs == 0);
    ++_expr->regrefs;
}


Regex::Regex(const Pt::String& ex)
: _expr(0)
{
    _expr = ::regcomp( ex.c_str() );

    if( ! _expr )
        throw InvalidRegex("invalid regex");

    assert(_expr->regrefs == 0);
    ++_expr->regrefs;
}


Regex::Regex(const Regex& other)
: _expr(0)
{
    if( other._expr )
    {
        _expr = other._expr;
        ++_expr->regrefs;
    }
}


Regex::~Regex()
{
    if(_expr && 0 == --_expr->regrefs)
    {
        std::free(_expr);
        _expr = 0;
    }
}


Regex& Regex::operator=(const Regex& other)
{
    if(_expr && 0 == --_expr->regrefs)
    {
        std::free(_expr);
        _expr = 0;
    }

    if( other._expr )
    {
        _expr = other._expr;
        ++_expr->regrefs;
    }

    return *this;
}


bool Regex::match(const Pt::String& str) const
{
    RegexSMatch smatch;
    return match(str, smatch);
}


bool Regex::match(const Pt::String& str, RegexSMatch& smatch) const
{
    return match(str.c_str(), smatch);
}


bool Regex::match(const Char* str) const
{
    RegexSMatch smatch;
    return match(str, smatch);
}


bool Regex::match(const Char* str, RegexSMatch& smatch) const
{
    smatch._size = 0;
    smatch._str = str;

    if( ! _expr )
        return false;

    int ret = regexec( _expr, smatch._match, smatch._str );

    if(ret == 0)
    {
        smatch._size = 0;
        smatch._match->startp[0] = 0;
        smatch._match->endp[0] = 0;
        return false;
    }

    unsigned n = 0;
    for(n = 0; n < 10 && smatch._match->startp[n] ; ++n)
    { }

    smatch._size = n;
    return true;
}


RegexSMatch::RegexSMatch()
: _str(0)
, _size(0)
, _match(0)
{
    _match = new pt_regmatch_t;
}


RegexSMatch::RegexSMatch(const RegexSMatch& other)
: _str(0)
, _size(0)
, _match(0)
{
    _match = new pt_regmatch_t( *other._match );
}


RegexSMatch::~RegexSMatch()
{
    delete _match;
}


RegexSMatch& RegexSMatch::operator=(const RegexSMatch& other)
{
    _str = other._str;
    _size = other._size;
    *_match = *other._match;
    return *this;
}


bool RegexSMatch::empty() const
{
    return _size > 0;
}


std::size_t RegexSMatch::size() const
{
    return _size;
}


std::size_t RegexSMatch::maxSize() const
{
    return NSUBEXP;
}


std::size_t RegexSMatch::position(std::size_t n) const
{
    if(n >= _size)
        return 0;

    return _match->startp[n] - _str;
}


std::size_t RegexSMatch::length(std::size_t n) const
{
    if(n >= _size)
        return 0;

    return _match->endp[n] - _match->startp[n];
}


Pt::String RegexSMatch::str(std::size_t n) const
{
    if(n >= _size)
        return String();

    return Pt::String( _match->startp[n], _match->endp[n] );
}


Pt::String RegexSMatch::format(const Pt::String& str) const
{
  enum state_type
  {
    state_0,
    state_esc,
    state_var0,
    state_var1,
    state_1
  } state;

  state = state_0;
  Pt::String ret;

  for (Pt::String::const_iterator it = str.begin(); it != str.end(); ++it)
  {
    Char ch = *it;

    switch (state)
    {
      case state_0:
        if (ch == '$')
          state = state_var0;
        else if (ch == '\\')
          state = state_esc;
        break;

      case state_esc:
        ret += ch;
        state = state_1;
        break;

      case state_var0:
        if( isdigit(ch) )
        {
          ret = Pt::String(str.begin(), it - 1);
            
          unsigned n = ch.value() - '0';
          if(n < _size)
          {
            const Pt::Char* s = _match->startp[n];
            const Pt::Char* e = _match->endp[n];
            assert(s && e);

            ret.append(s, e-s);
          }

            state = state_1;
        }
        else
          state = state_0;
        break;

      case state_1:
        if (ch == '$')
          state = state_var1;
        else if (ch == '\\')
          state = state_esc;
        else
          ret += ch;
        break;

      case state_var1:
        if( isdigit(ch) )
        {
          unsigned n = ch.value() - '0';

          if(n < _size)
          {
            const Pt::Char* s = _match->startp[n];
            const Pt::Char* e = _match->endp[n];
            assert(s && e);

            ret.append(s, e-s);
          }

          state = state_1;
        }
        else if (ch == '$')
          ret += '$';
        else
        {
          ret += '$';
          ret += ch;
        }
        break;
    }
  }

  switch (state)
  {
    case state_0:
    case state_var0:
      return str;

    case state_esc:
      return ret + '\\';

    case state_var1:
      return ret + '$';

    case state_1:
      return ret;
  }

  return ret;
}

} // namespace Pt
