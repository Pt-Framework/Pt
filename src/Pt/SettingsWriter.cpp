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

#include "SettingsWriter.h"
#include <Pt/Convert.h>

namespace {

// TODO: use a formatter
Pt::String toStr(const Pt::SerializationInfo& si)
{
    Pt::String s;

    switch( si.type() )
    {
        case Pt::SerializationInfo::Str:
            si.getString(s);
            break;
        
        case Pt::SerializationInfo::Boolean:
            bool b;
            si.getBool(b);
            s = b ? "true" : "false" ;
            break;
    
        case Pt::SerializationInfo::Char:
        {
            Pt::Char c;
            si.getChar(c);
            s += c;
            break;
        }
    
        case Pt::SerializationInfo::Int8:
        case Pt::SerializationInfo::Int16:
        case Pt::SerializationInfo::Int32:
        case Pt::SerializationInfo::Int64:
            Pt::int64_t i;
            si.getInt64(i);
            Pt::formatInt(std::back_inserter(s), i);
            break;
    
        case Pt::SerializationInfo::UInt8:
        case Pt::SerializationInfo::UInt16:
        case Pt::SerializationInfo::UInt32:
        case Pt::SerializationInfo::UInt64:
            Pt::uint64_t u;
            si.getUInt64(u);
            Pt::formatInt(std::back_inserter(s), u);
            break;
    
        case Pt::SerializationInfo::Float:
        case Pt::SerializationInfo::Double:
        case Pt::SerializationInfo::LongDouble:
            long double d;
            si.getLongDouble(d);
            Pt::formatFloat(std::back_inserter(s), d);
            break;
        
        default:
            throw std::logic_error("conversion to string failed");
    }

    return s;
}

} // namespace

namespace Pt {

void SettingsWriter::write(const SerializationInfo& si)
{	
    Pt::String value;
    SerializationInfo::ConstIterator it;
    for(it = si.begin(); it != si.end(); ++it)
    {
        if( it->isScalar() )
        {
            value = toStr(*it);
            this->writeEntry( it->name(), value, it->typeName() );
            *_os << std::endl;
        }
        else if( it->isStruct() || it->isSequence() )
        {
            
            // Array types may have no instance-names
            if( it->findMember("") )
            {
                *_os << Pt::String::widen( it->name() ) << Char(' ') << Char('=') << Char(' ');
                *_os << Char('{') << Char(' ');
                this->writeParent( *it, "");
                *_os << Char(' ') << Char('}') << std::endl;
                continue;
            }

            //this->writeSection( subdata->name() );
            this->writeParent( *it, it->name() );
        }
    }
}


void SettingsWriter::writeParent(const SerializationInfo& sd, const std::string& prefix)
{
    Pt::String value;
    bool separate = false;

    SerializationInfo::ConstIterator it;
    for(it = sd.begin(); it != sd.end(); ++it)
    {
        if( it->isScalar() )
        {
            std::string name = it->name();

            // only comma separate array members (which have no name)
            if( separate && name.empty() )
                *_os << Char(',') << Char(' ');

             value = toStr(*it);
             if( ! prefix.empty() )
                *_os << Pt::String::widen( prefix ) << '.';

            this->writeEntry( name, value, it->typeName() );

            if(! name.empty() )
                *_os << std::endl;
        }
        else if( it->isStruct() || it->isSequence() )
        {
            *_os << Pt::String::widen( prefix ) << Char('.') << Pt::String::widen( it->name() ) << Char(' ') << Char('=') << Char(' ');

            if( ! it->isSequence() )
                *_os << Pt::String::widen( it->typeName() );
                
            *_os << Char('{') << Char(' ');
            this->writeChild(*it);
            *_os << Char(' ') << Char('}') << std::endl;
        }

        separate = true;
    }
}


void SettingsWriter::writeChild(const SerializationInfo& sd)
{
    Pt::String value;
    bool separate = false;

    SerializationInfo::ConstIterator it;
    for(it = sd.begin(); it != sd.end(); ++it)
    {
        if(separate)
            *_os << Char(',') << Char(' ');

        if( it->isScalar() )
        {
            value = toStr(*it);
            this->writeEntry( it->name(), value, it->typeName() );
        }
        else if( it->isStruct() || it->isSequence() )
        {
            if( it->name()[0] != '\0' && ! sd.isSequence() )
                *_os << Pt::String::widen( it->name() ) << Char(' ') << Char('=') << Char(' ');

            *_os << Pt::String::widen( it->typeName() ) << Char('{') << Char(' ') ;
            this->writeChild(*it);
            *_os << Char(' ') << Char('}');
        }

        separate = true;
    }
}


void writeEscapedValue(std::basic_ostream<Pt::Char>& os, const Pt::String& value)
{
    for(size_t n = 0; n < value.size(); ++n)
    {
        switch( value[n].value() )
        {
            case '\\':
                os << Pt::Char('\\');

            default:
                os << value[n];
        }
    }
}


void SettingsWriter::writeEntry(const std::string& name, const Pt::String& value, const std::string& type)
{
    if( type.empty() )
    {
        if( name.empty() == false)
            *_os << Pt::String::widen(name) << Char('=');

        *_os  << Char('\"');
        writeEscapedValue(*_os, value);
        *_os << Char('\"');

        return;
    }

    if( name.empty() == false)
        *_os << Pt::String::widen(name) << Char(' ') << Char('=') << Char(' ');

    *_os << Pt::String::widen(type) << Char('(') << Char('\"');
    writeEscapedValue(*_os, value);
    *_os << Char('\"') << Char(')');
}


void SettingsWriter::writeSection(const Pt::String& prefix)
{
    *_os << Char('[') << prefix << Char(']') << std::endl;
}

} // namespace Pt
