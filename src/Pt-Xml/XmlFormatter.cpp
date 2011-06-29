/*
 * Copyright (C) 2008 by Marc Boris Duerner
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
#include "Pt/Xml/XmlFormatter.h"
#include "Pt/Xml/XmlWriter.h"
#include "Pt/Xml/StartElement.h"
#include "Pt/Xml/EndElement.h"
#include "Pt/Xml/Characters.h"
#include "Pt/Convert.h"
#include "Pt/String.h"
#include "Pt/Date.h"
#include "Pt/SourceInfo.h"
#include <stdexcept>

namespace Pt {

namespace Xml {

class DateFormatter : public XmlFormatter::Surrogate
{
    public:
        virtual ~DateFormatter() {}

        virtual void beginObject(Formatter& formatter, const std::string& name,
                                 const std::string& type, const std::string& id)
        {
            _name = name;
            _id = id;
        }

        virtual void addInt(Formatter& formatter, const std::string& name,
                            long long value, const std::string& id)
        {
            if(name == "year")
                _year = value;
            else if(name == "month")
                _month = value;
            else if(name == "day")
                _day = value;
        }

        virtual void addUInt(Formatter& formatter, const std::string& name,
                             unsigned long long value, const std::string& id)
        {
            if(name == "year")
                _year = value;
            else if(name == "month")
                _month = value;
            else if(name == "day")
                _day = value;
        }

        virtual void finishObject(Formatter& formatter)
        {
            Pt::Date dt(_year, _month, _day);
            std::string s = dt.toIsoString();
            formatter.addValue(_name, "Pt::Date", Pt::String::widen(s), _id);
        }

    private:
        std::string _name;
        std::string _id;
        int _year;
        unsigned _month;
        unsigned _day;
};


XmlFormatter::XmlFormatter()
: _writer(0)
, _deleter(0)
, _surr(0)
{
    _surrogates["Pt::Date"] = new DateFormatter();
}


XmlFormatter::XmlFormatter(std::ostream& os)
: _writer( 0 )
, _deleter( new XmlWriter(os) )
, _surr(0)
{
    _surrogates["Pt::Date"] = new DateFormatter();
    _writer = _deleter.get();
}


XmlFormatter::XmlFormatter(XmlWriter* writer)
: _writer(writer)
, _deleter(0)
, _surr(0)
{
    _surrogates["Pt::Date"] = new DateFormatter();
}


XmlFormatter::~XmlFormatter()
{
    this->detach();

    std::map<std::string, Surrogate*>::iterator it = _surrogates.begin();
    for(; it != _surrogates.end(); ++it)
    {
        delete it->second;
    }
}


void XmlFormatter::attach(std::ostream& os)
{
    if (_writer)
        throw std::logic_error("XmlSerizalizer is already open." + PT_SOURCEINFO);

    _deleter.reset(new XmlWriter(os));
    _writer = _deleter.get();
}


void XmlFormatter::attach(XmlWriter& writer)
{
    if (_writer)
        throw std::logic_error("XmlSerizalizer is already open." + PT_SOURCEINFO);

    _deleter.reset(0);
    _writer = &writer;
}


void XmlFormatter::detach()
{
    if (_writer)
    {
        this->flush();
        _deleter.reset(0);
        _writer = 0;
    }
}


void XmlFormatter::flush()
{
    if (_writer)
        _writer->flush();
}


void XmlFormatter::addValue(const std::string& name, const std::string& type,
                             const Pt::String& value, const std::string& id)
{
    if( ! id.empty() )
    {
        Xml::Attribute attr[2];
        attr[0] = Xml::Attribute( String(L"type"), String::widen( type ) );
        attr[1] = Xml::Attribute( String(L"id"), String::widen( id ) );

        if( ! name.empty() )
            _writer->writeElement( String::widen( name ), attr, 2, value );
        else
            _writer->writeElement( String::widen( type ), attr, 2, value );
    }
    else
    {
        Xml::Attribute typeAttr( String(L"type"), String::widen( type ) );

        if( ! name.empty() )
            _writer->writeElement( String::widen( name ), &typeAttr, 1, value );
        else
            _writer->writeElement( String::widen( type ), &typeAttr, 1, value );
    }
}


void XmlFormatter::addBytes(const std::string& name, const std::string& type,
                            const char* value, size_t length, const std::string& id)
{
    convert(_value, std::string(value, length));
    this->addValue(name, type, _value, id);
}


void XmlFormatter::addBool(const std::string& name, bool value,
                           const std::string& id)
{
	convert(_value, value);
	this->addValue(name, "bool", _value, id);
}


void XmlFormatter::addInt(const std::string& name, long long value,
                          const std::string& id)
{
    if(_surr)
    {
        _surr->addInt(*this, name, value, id);
        return;
    }

    convert(_value, value);
	this->addValue(name, "int", _value, id);
}


void XmlFormatter::addUInt(const std::string& name, unsigned long long value,
                           const std::string& id)
{
    if(_surr)
    {
        _surr->addInt(*this, name, value, id);
        return;
    }

    convert(_value, value);
	this->addValue(name, "unsigned", _value, id);
}


void XmlFormatter::addFloat(const std::string& name, double value,
                            const std::string& id)
{
	convert(_value, value);
	this->addValue(name, "double", _value, id);
}


void XmlFormatter::addReference(const std::string& name, const std::string& id)
{
    Attribute attr( Pt::String(L"ref"), Pt::String::widen( id ) );
    _writer->writeElement( Pt::String::widen( name ), &attr, 1, Pt::String() );
}


void XmlFormatter::beginArray(const std::string& name, const std::string& type,
                              const std::string& id)
{
    if( ! id.empty() )
    {
        Attribute attr( Pt::String(L"id"), Pt::String::widen( id ) );

        if( ! name.empty() )
            _writer->writeStartElement( Pt::String::widen( name ), &attr, 1 );
        else
            _writer->writeStartElement( String::widen( type ), &attr, 1 );
    }
    else
    {
        if( ! name.empty() )
            _writer->writeStartElement( Pt::String::widen( name ) );
        else
            _writer->writeStartElement( String::widen( type ) );
    }
}


void XmlFormatter::beginElement(const std::string& type, const std::string& id)
{
}


void XmlFormatter::finishElement()
{
}


void XmlFormatter::finishArray()
{
    _writer->writeEndElement();
}


void XmlFormatter::beginObject(const std::string& name, const std::string& type,
                               const std::string& id)
{
    std::map<std::string, Surrogate*>::iterator it = _surrogates.find( type );
    if( it != _surrogates.end() )
    {
        _surr = it->second;
        _surr->beginObject(*this, name, type, id);
        return;
    }

    this->onBeginObject(name, type, id);
}


void XmlFormatter::onBeginObject(const std::string& name, const std::string& type,
                                 const std::string& id)
{
    if( ! id.empty() )
    {
        Xml::Attribute attr( String(L"id"), String::widen( id ) );

        if( ! name.empty() )
            _writer->writeStartElement( String::widen( name ), &attr, 1 );
        else
            _writer->writeStartElement( String::widen( type ), &attr, 1 );
    }
    else
    {
        if( ! name.empty() )
            _writer->writeStartElement( String::widen( name ) );
        else
            _writer->writeStartElement( String::widen( type ) );
    }
}


void XmlFormatter::beginMember(const std::string& name, const std::string& type,
                               const std::string& id)
{
}


void XmlFormatter::finishMember()
{
}


void XmlFormatter::finishObject()
{
    if(_surr)
    {
        _surr->finishObject(*this);
        _surr = 0;
        return;
    }

    this->onFinishObject();
}


void XmlFormatter::onFinishObject()
{
    _writer->writeEndElement();
}

} // namespace Xml

} // namespace Pt
