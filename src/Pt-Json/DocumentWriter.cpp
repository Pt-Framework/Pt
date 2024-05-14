/*
   Copyright (C) 2015-2023 by Dr. Marc Boris Duerner
  
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   
   As a special exception, you may use this file as part of a free
   software library without restriction. Specifically, if other files
   instantiate templates or use macros or inline functions from this
   file, or you compile this file and link it with other files to
   produce an executable, this file does not by itself cause the
   resulting executable to be covered by the GNU General Public
   License. This exception does not however invalidate any other
   reasons why the executable file might be covered by the GNU Library
   General Public License.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
   MA 02110-1301 USA
*/

#include <Pt/Json/DocumentWriter.h>
#include <Pt/Json/StartObject.h>
#include <Pt/Json/Member.h>
#include <Pt/Json/EndObject.h>
#include <Pt/Json/StartArray.h>
#include <Pt/Json/EndArray.h>
#include <Pt/Json/String.h>
#include <Pt/Json/Float.h>
#include <Pt/Json/Integer.h>
#include <Pt/Json/Boolean.h>
#include <Pt/Json/Null.h>

namespace Pt {

namespace Json {

DocumenWriter::DocumenWriter()
: _doc(0)
, _spaces(4)
, _depth(0)
, _os(0)
{
    reset();
}


DocumenWriter::DocumenWriter(std::basic_ostream<Pt::Char>& os, const Document& doc)
: _doc(&doc)
, _spaces(4)
, _depth(0)
, _os(&os)
{
    
}


void DocumenWriter::reset()
{
    _depth = 0;
    _os = 0;
    _doc = 0;
}


void DocumenWriter::reset(std::basic_ostream<Pt::Char>& os, const Document& doc)
{
    _depth = 0;
    _os = &os;
    _doc = &doc;    
}


void DocumenWriter::write()
{
    if( ! _doc )
        throw JsonError("invalid document"); 

    if (!_os )
        throw JsonError("uninitialised output stream");

    output(*_doc->root().si(), _spaces);
}


void DocumenWriter::output(const Pt::SerializationInfo& info, size_t spaces)
{

    _depth++;

    (*_os) << "{";

    if (spaces != 0)
        (*_os) << std::endl;

    Pt::SerializationInfo::ConstIterator it = info.begin();

    while (it != info.end())
    {
        const Pt::SerializationInfo& childEntry = *it;
        ++it;

        const bool last = (it == info.end());

        switch (childEntry.type())
        {
            case Pt::SerializationInfo::Void:
                outNull(childEntry, last);
                break;

            case Pt::SerializationInfo::Context:
            case Pt::SerializationInfo::Reference:
            case Pt::SerializationInfo::Dict:
            case Pt::SerializationInfo::DictElement:
            case Pt::SerializationInfo::Binary:
                break;

            case Pt::SerializationInfo::Boolean:
                outBool(childEntry, last);
                break;

            case Pt::SerializationInfo::Char:
            case Pt::SerializationInfo::Str:
                outStr(childEntry, last);
            break;

            case Pt::SerializationInfo::Int8:
            case Pt::SerializationInfo::Int16:
            case Pt::SerializationInfo::Int32:
            case Pt::SerializationInfo::Int64:
            case Pt::SerializationInfo::UInt8:
            case Pt::SerializationInfo::UInt16:
            case Pt::SerializationInfo::UInt32:
            case Pt::SerializationInfo::UInt64:
            case Pt::SerializationInfo::Float:
            case Pt::SerializationInfo::Double:
            case Pt::SerializationInfo::LongDouble:
                outValue(childEntry, last);
            break;

            case Pt::SerializationInfo::Struct:
                outObject(childEntry, last);
            break;

            case Pt::SerializationInfo::Sequence:
                outArray(childEntry, last);
            break;
        }
    }

    _depth--;

    (*_os) << space().c_str() << "}";

    if (_spaces != 0)
        (*_os) << std::endl;
}


void DocumenWriter::outBool(const Pt::SerializationInfo& entry, bool last)
{
    bool value = false;

    entry >>= value;

    const Pt::String name = entry.name();

    if (name.empty())
    {
        (*_os) << (value ? "true" : "false");
    }
    else
    {
        (*_os) << space().c_str() << '"' << entry.name() << "\":" << (value ? "true" : "false");

        if (!last)
            (*_os) << ",";

        if (_spaces != 0)
            (*_os) << std::endl;
    }
}


void DocumenWriter::outStr(const Pt::SerializationInfo& entry, bool last)
{
    Pt::String value = "";

    entry >>= value;

    const Pt::String name = entry.name();

    if (name.empty())
    {
        (*_os) << "\"" << value << "\"";
    }
    else
    {
        (*_os) << space().c_str() << '"' << entry.name() << "\":\"" << value << "\"";

        if (!last)
            (*_os) << ",";

        if (_spaces != 0)
            (*_os) << std::endl;
    }
}


void DocumenWriter::outValue(const Pt::SerializationInfo& entry, bool last)
{
    double value = 0;

    entry >>= value;

    const Pt::String name = entry.name();

    if (name.empty())
    {
        (*_os) << value;
    }
    else
    {
        (*_os) << space().c_str() << '"' << entry.name() << "\":" << value;

        if (!last)
            (*_os) << ",";

        if (_spaces != 0)
            (*_os) << std::endl;
    }
}


void DocumenWriter::outNull(const Pt::SerializationInfo& entry, bool last)
{
    const Pt::String name = entry.name();

    if (name.empty())
    {
        (*_os) << "null";
    }
    else
    {
        (*_os) << space().c_str() << '"' << entry.name() << "\":" << "null";

        if (!last)
            (*_os) << ",";

        if (_spaces != 0)
            (*_os) << std::endl;
    }
}


void DocumenWriter::outArray(const Pt::SerializationInfo& entry, bool last)
{
    const Pt::String name = entry.name();

    if (!name.empty())
        (*_os) << space().c_str() << '"' << name << "\":";

    (*_os) << "[";

    Pt::SerializationInfo::ConstIterator it = entry.begin();

    while (it != entry.end())
    {
        const Pt::SerializationInfo& childEntry = *it;

        switch (childEntry.type())
        {
            case Pt::SerializationInfo::Void:
                outNull(childEntry, false);
                break;

            case Pt::SerializationInfo::Context:
            case Pt::SerializationInfo::Reference:
            case Pt::SerializationInfo::Dict:
            case Pt::SerializationInfo::DictElement:
            case Pt::SerializationInfo::Binary:
                break;

            case Pt::SerializationInfo::Boolean:
                outBool(childEntry, false);
                break;

            case Pt::SerializationInfo::Char:
            case Pt::SerializationInfo::Str:
                outStr(childEntry, false);
                break;

            case Pt::SerializationInfo::Int8:
            case Pt::SerializationInfo::Int16:
            case Pt::SerializationInfo::Int32:
            case Pt::SerializationInfo::Int64:
            case Pt::SerializationInfo::UInt8:
            case Pt::SerializationInfo::UInt16:
            case Pt::SerializationInfo::UInt32:
            case Pt::SerializationInfo::UInt64:
            case Pt::SerializationInfo::Float:
            case Pt::SerializationInfo::Double:
            case Pt::SerializationInfo::LongDouble:
                outValue(childEntry, false);
                break;

            case Pt::SerializationInfo::Struct:
                outObject(childEntry, false);
                break;

            case Pt::SerializationInfo::Sequence:
                outArray(childEntry, false);
                break;
        }

        ++it;

        if (it != entry.end())
            (*_os) << ',';
    }

    if (last)
        (*_os) << "]";
    else
        (*_os) << "],";

    if (_spaces != 0)
        (*_os) << std::endl;
}


void DocumenWriter::outObject(const Pt::SerializationInfo& entry, bool last)
{
    const Pt::String name = entry.name();

    if (!name.empty())
        (*_os) << space().c_str() << "\"" << entry.name() << "\":{";

    if (_spaces != 0)
        (*_os) << std::endl;

    Pt::SerializationInfo::ConstIterator it = entry.begin();

    _depth++;

    while (it != entry.end())
    {
        const Pt::SerializationInfo& childEntry = *it;
        ++it;

        bool childLast = (it == childEntry.end());

        switch (childEntry.type())
        {
            case Pt::SerializationInfo::Void:
                outNull(childEntry, childLast);
                break;

            case Pt::SerializationInfo::Context:
            case Pt::SerializationInfo::Reference:
            case Pt::SerializationInfo::Dict:
            case Pt::SerializationInfo::DictElement:
            case Pt::SerializationInfo::Binary:
                break;

            case Pt::SerializationInfo::Boolean:
                outBool(childEntry, childLast);
                break;

            case Pt::SerializationInfo::Char:
            case Pt::SerializationInfo::Str:
                outStr(childEntry, childLast);
                break;

            case Pt::SerializationInfo::Int8:
            case Pt::SerializationInfo::Int16:
            case Pt::SerializationInfo::Int32:
            case Pt::SerializationInfo::Int64:
            case Pt::SerializationInfo::UInt8:
            case Pt::SerializationInfo::UInt16:
            case Pt::SerializationInfo::UInt32:
            case Pt::SerializationInfo::UInt64:
            case Pt::SerializationInfo::Float:
            case Pt::SerializationInfo::Double:
            case Pt::SerializationInfo::LongDouble:
                outValue(childEntry, childLast);
                break;

            case Pt::SerializationInfo::Struct:
                outObject(childEntry, childLast);
                break;

            case Pt::SerializationInfo::Sequence:
                outArray(childEntry, childLast);
                break;
        }
    }

    _depth--;

    if (last)
        (*_os) << space().c_str() << "}";
    else
        (*_os) << space().c_str() << "},";

    if (_spaces != 0)
        (*_os) << std::endl;
}

} // namespace

} // namespace
