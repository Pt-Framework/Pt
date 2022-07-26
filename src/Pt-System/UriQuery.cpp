/*
 * Copyright (C) 2022 Marc Duerner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
 * MA 02110-1301 USA
 */

#include <Pt/System/UriQuery.h>
#include <Pt/System/Uri.h>

namespace Pt {

namespace System {

UriQuery::UriQuery()
{
}


UriQuery::UriQuery(const std::string& s)
{
  set(s);
}


UriQuery::~UriQuery()
{
}


bool UriQuery::isEmpty() const
{
    return _fields.empty();
}


void UriQuery::clear()
{
    _fields.clear();
}


void UriQuery::set(const std::string& s)
{
    clear();

    enum State { Key, Value } state = Key;

    std::string key, value;

    for(std::string::const_iterator it = s.begin(); it != s.end(); ++it)
    {
        char ch = *it;

        switch(state)
        {
            case Key:
            {
                if(ch == '&')
                {
                    _fields[ Uri::decode(key) ] = Pt::String();
                    key.clear();
                }
                else if(ch == '=')
                    state = Value;
                else if(ch == '+')
                    key += ' ';
                else
                    key += ch;
                
                break;
            }
            
            case Value:
            {
                if(ch == '&')
                {
                    state = Key;

                    _fields[ Uri::decode(key) ] = Uri::decode(value);

                    key.clear();
                    value.clear();
                }
                else if(ch == '+')
                    value += ' ';
                else
                    value += ch;
                
                break;
            }
        }
    }

    if( ! key.empty() )
    {       
        if( value.empty() )
            _fields[ Uri::decode(key) ] = Pt::String();
        else
            _fields[ Uri::decode(key) ] = Uri::decode(value);
    }
}


bool UriQuery::hasField(const Pt::String& key) const
{
  return _fields.find(key) != _fields.end();
}


const Pt::String& UriQuery::field(const Pt::String& key, const Pt::String& def) const
{
    const Pt::String* field = findField(key);
    if( ! field )
      return def;

    return *field;
}


const Pt::String* UriQuery::findField(const Pt::String& key) const
{
    std::map<Pt::String, Pt::String>::const_iterator it = _fields.find(key);
    if( it == _fields.end() )
        return 0;

    return &it->second;
}


void UriQuery::setField(const Pt::String& key, const Pt::String& value)
{
    _fields[key] = value;
}


void UriQuery::removeField(const Pt::String& key)
{
    _fields.erase(key);
}


std::string UriQuery::toEncoded() const
{ 
  std::string encoded;

  std::map<Pt::String, Pt::String>::const_iterator it;
  for(it = _fields.begin(); it != _fields.end(); ++it)
  {
      if( ! encoded.empty() )
        encoded += '&';
      
      encoded += Uri::encode(it->first);

      if( ! it->second.empty() )
      {
          encoded += '=';
          encoded += Uri::encode(it->second);
      }
  }

  return encoded;
}


bool UriQuery::equals(const UriQuery& query) const
{
    return _fields == query._fields;
}

} // namespace System

} // namespace Pt
