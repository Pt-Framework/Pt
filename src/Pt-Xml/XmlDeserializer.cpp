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
#include "Pt/Xml/XmlDeserializer.h"
#include "Pt/Xml/XmlReader.h"
#include "Pt/Xml/StartElement.h"
#include "Pt/Xml/EndElement.h"
#include "Pt/Xml/Characters.h"
#include "Pt/String.h"
#include "Pt/Date.h"
#include "Pt/SourceInfo.h"
#include <stdexcept>

namespace Pt {

namespace Xml {

XmlDeserializer::XmlDeserializer(XmlReader& reader)
: _reader(&reader)
, _composer(0)
{
	this->reset( &_xmlcontext );
}

XmlDeserializer::XmlDeserializer(std::istream& is)
: _reader( 0 )
, _deleter(new XmlReader(is))
, _composer(0)
{
	this->reset( &_xmlcontext );
    _reader = _deleter.get();
}


XmlDeserializer::~XmlDeserializer()
{
}


void XmlDeserializer::get(IComposer& comp)
{
    //std::cerr << "-> GET"<< std::endl;
    _composer = &comp;

    _processNode = &XmlDeserializer::OnBegin;

    XmlReader::Iterator it = _reader->current();
    if(it->type() == Node::EndElement)
        ++it;

    for( ; it != _reader->end(); ++it)
    {
        //std::cerr << "-> GOT NODE: " << it->type() << std::endl;
        (this->*_processNode)(*it);

        if(_composer == 0)
            break;
    }

    //std::cerr << "-> DONE"<< std::endl;
}


void XmlDeserializer::onBegin(IComposer& comp)
{
    _composer = &comp;
    _processNode = &XmlDeserializer::OnBegin;
}


bool XmlDeserializer::onAdvance(IComposer& comp)
{
    _composer = &comp;

    while( _reader->advance() )
    {
        const Pt::Xml::Node& node = _reader->get();
        //std::cerr << "-> GOT NODE: " << node.type() << std::endl;

        if(node.type() == Node::EndDocument)
            throw SerializationError("incomplete type");

        (this->*_processNode)(node);

        if( _composer == 0 )
        {
            //std::cerr << "-> COMPOSER END" << std::endl;
            return true;
        }
    }

    //std::cerr << "-> COMPOSER MORE" << std::endl;
    return false;
}


void XmlDeserializer::OnBegin(const Node& node)
{
    //std::cerr << "-> OnBegin" << std::endl;

    switch( node.type() )
    {
        case Node::StartElement:
        {
            const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);

            //std::cerr << "BEGIN MEMBER: " << se.name().narrow() << std::endl;
            this->setName( se.name().narrow() );

            String nodeId = se.attribute(L"id");
            if( ! nodeId.empty() )
            {
                //std::cerr << "ID: " << nodeId.narrow() << std::endl;
                this->setId( nodeId.narrow() );
            }

            String type = se.attribute(L"type");
            if( ! type.empty() )
            {
                //std::cerr << "TYPE: " << type.narrow() << std::endl;
                this->setTypeName( type.narrow() );
            }

            String refId = se.attribute(L"ref");
            if( ! refId.empty() )
            {
                //std::cerr << "REF: " << refId.narrow() << std::endl;
                this->setReference( refId.narrow() );
                _processNode = &XmlDeserializer::OnReferenceBegin;
                break;
            }

            _processNode = &XmlDeserializer::OnMemberBegin;
            break;
        }

        case Node::EndElement:
        {
            throw SerializationError("expected start element");
        }

        default:
            break;
    }
}


void XmlDeserializer::OnReferenceBegin(const Node& node)
{
    //std::cerr << "-> OnReferenceBegin" << std::endl;

    switch( node.type() )
    {
        case Node::StartElement:
        {
            throw SerializationError("invalid reference");
        }

        case Node::EndElement:
        {
            const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
            this->finishXmlMember(ee);
            break;
        }

        default:
            break;
    }
}


void XmlDeserializer::OnMemberBegin(const Node& node)
{
    //std::cerr << "-> OnMemberBegin" << std::endl;

    switch( node.type() )
    {
        case Node::StartElement:
        {
            const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
            this->beginXmlMember(se);
            break;
        }

        case Node::Characters:
        {
            const Xml::Characters& chars = static_cast<const Xml::Characters&>(node);
            _value = chars.content();
            _processNode = &XmlDeserializer::OnValue;
            break;
        }

        case Node::EndElement:
        {
            //std::cerr << "VALUE: <empty>" << std::endl;
            this->setValue( Pt::String() );

            const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
            this->finishXmlMember(ee);
            break;
        }

        default:
            break;
    }
}


void XmlDeserializer::OnValue(const Node& node)
{
    //std::cerr << "-> OnValue" << std::endl;

    switch( node.type() )
    {
        case Node::StartElement:
        {
            const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
            this->beginXmlMember(se);
            break;
        }

        case Node::EndElement:
        {
            //std::cerr << "VALUE: " << _value.narrow() << std::endl;

            this->setValue(_value);

            const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
            this->finishXmlMember(ee);
            break;
        }

        default:
            break;
    }
}


void XmlDeserializer::OnMemberEnd(const Node& node)
{
    //std::cerr << "-> OnMemberEnd" << std::endl;

    switch( node.type() )
    {
        case Node::StartElement:
        {
            const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
            this->beginXmlMember(se);
            break;
        }

        case Node::EndElement:
        {
            const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
            this->finishXmlMember(ee);
            break;
        }

        default:
            break;
    }
}


void XmlDeserializer::beginXmlMember(const Xml::StartElement& se)
{
    //std::cerr << "BEGIN MEMBER: " << se.name().narrow() << std::endl;
    this->beginMember( se.name().narrow() );

    String nodeId = se.attribute(L"id");
    if( ! nodeId.empty() )
    {
        //std::cerr << "ID: " << nodeId.narrow() << std::endl;
        this->setId( nodeId.narrow() );
    }

    String type = se.attribute(L"type");
    if( ! type.empty() )
    {
        //std::cerr << "BTYPE: " << type.narrow() << std::endl;
        this->setTypeName( type.narrow() );
    }

    String refId = se.attribute(L"ref");
    if( ! refId.empty() )
    {
        //std::cerr << "REF: " << refId.narrow() << std::endl;
        this->setReference( refId.narrow() );
        _processNode = &XmlDeserializer::OnReferenceBegin;
        return;
    }

    _processNode = &XmlDeserializer::OnMemberBegin;
}


void XmlDeserializer::finishXmlMember(const Xml::EndElement& )
{
    //const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
    //std::cerr << "END MEMBER: " << ee.name().narrow() << std::endl;
    this->finishMember();
    _processNode = &XmlDeserializer::OnMemberEnd;
}


void XmlDeserializer::setName(const std::string& name)
{
    _composer->setName( name );
}


void XmlDeserializer::setTypeName(const std::string& type)
{
    _composer->setTypeName(type);
    _typeName = type;
}


void XmlDeserializer::setReference(const std::string& refid)
{
    _composer->setReference( refid );
}


void XmlDeserializer::setId(const std::string& id)
{
    _composer->setId( id );
}


void XmlDeserializer::setValue(const Pt::String& value)
{
    _composer->setValue(value);
}


void XmlDeserializer::beginMember(const std::string& name)
{
    _composer = _composer->beginMember(name);
}


void XmlDeserializer::finishMember()
{
    _composer = _composer->finish();
}

} // namespace Xml

} // namespace Pt
