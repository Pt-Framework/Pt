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
#include "Pt/Xml/XmlSerializer.h"
#include "Pt/Xml/XmlWriter.h"
#include "Pt/Xml/StartElement.h"
#include "Pt/Xml/EndElement.h"
#include "Pt/Xml/Characters.h"
#include "Pt/String.h"
#include "Pt/SourceInfo.h"
#include <stdexcept>

namespace Pt {

namespace Xml {

class XmlFormatter : public Formatter
{
    public:
        XmlFormatter(XmlWriter& writer)
        : _writer(&writer)
        {}

        ~XmlFormatter()
        {}

        void addValue(const std::string& name, const std::string& type, const Pt::String& value, const std::string& id)
        {
            if( ! id.empty() )
            {
                Attribute attr( Pt::String(L"id"), Pt::String::widen( id ) );
                _writer->writeElement( Pt::String::widen( name ), &attr, 1, value );
            }
            else
                _writer->writeElement( Pt::String::widen( name ), value );
        }

        void addReference(const std::string& name, const Pt::String& value)
        {
            Attribute attr( Pt::String(L"ref"), value );
            _writer->writeElement( Pt::String::widen( name ), &attr, 1, Pt::String() );
        }

        void beginArray()
        {

        }

        void finishArray()
        {

        }

        void beginObject(const std::string& name, const std::string& id)
        {
            if( ! id.empty() )
            {
                Attribute attr( Pt::String(L"id"), Pt::String::widen( id ) );
                _writer->writeStartElement( Pt::String::widen( name ), &attr, 1 );
            }
            else
                _writer->writeStartElement( Pt::String::widen( name ) );
        }

        void beginMember(const std::string& name)
        {

        }

        void finishMember()
        {

        }

        void finishObject()
        {
            _writer->writeEndElement();
        }

        void finish()
        {

        }

    private:
        XmlWriter* _writer;
};


XmlSerializer::XmlSerializer()
: _writer(0)
, _deleter(0)
{
}

XmlSerializer::XmlSerializer(std::ostream& os)
: _writer( 0 )
, _deleter( new XmlWriter(os) )
{
    _writer = _deleter.get();
}


XmlSerializer::XmlSerializer(XmlWriter* writer)
: _writer(writer)
, _deleter(0)
{
}


XmlSerializer::~XmlSerializer()
{
    this->finish();
    this->detach();
}


void XmlSerializer::attach(std::ostream& os)
{
    if (_writer)
        throw std::logic_error("XmlSerizalizer is already open." + PT_SOURCEINFO);

    _deleter.reset(new XmlWriter(os));
    _writer = _deleter.get();
}


void XmlSerializer::attach(XmlWriter& writer)
{
    if (_writer)
        throw std::logic_error("XmlSerizalizer is already open." + PT_SOURCEINFO);

    _deleter.reset(0);
    _writer = &writer;
}


void XmlSerializer::detach()
{
    if (_writer)
    {
        this->flush();
        _deleter.reset(0);
        _writer = 0;
    }
}


void XmlSerializer::flush()
{
    if (_writer)
        _writer->flush();
}


void XmlSerializer::push(const void* type, const std::string& name, ISerializer* serializer)
{
    serializer->setName(name);
    _omap[type] = serializer;
    _stack.push_back(serializer);
}


void XmlSerializer::finish()
{
    if( ! _writer )
        throw std::logic_error("XmlSerizalizer was not yet opened." + PT_SOURCEINFO);

    std::list<ISerializer*>::iterator it;
    for(it = _stack.begin(); it != _stack.end(); ++it)
    {
        ISerializer* serializer = *it;
        serializer->fixdown( _omap );
    }

    _omap.clear();

    XmlFormatter formatter(*_writer);
    for(it = _stack.begin(); it != _stack.end(); ++it)
    {
        (*it)->decompose(formatter);
        delete *it;
    }
    _stack.clear();
}

} // namespace Xml

} // namespace Pt
