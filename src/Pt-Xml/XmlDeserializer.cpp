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
#include "Pt/SourceInfo.h"
#include <stdexcept>

namespace Pt {

namespace Xml {

XmlDeserializer::XmlDeserializer(XmlReader& reader)
: _context(0)
, _reader(&reader)
, _deser(0)
{
	_context = &_xmlcontext;
}

XmlDeserializer::XmlDeserializer(std::istream& is)
: _context(0)
, _reader( 0 )
, _deleter(new XmlReader(is))
, _deser(0)
{
    _context = &_xmlcontext;
    _reader = _deleter.get();
}


XmlDeserializer::~XmlDeserializer()
{
    this->finish();
}


void XmlDeserializer::get(IComposer* deser)
{
    //std::cerr << "-> BEGIN"<< std::endl;
    _deser = deser;

    if(_reader->get().type() != Node::StartElement)
        _reader->nextElement();

    _processNode = &XmlDeserializer::beginDocument;

    _startDepth = _reader->depth();
    for(XmlReader::Iterator it = _reader->current(); it != _reader->end(); ++it)
    {
        (this->*_processNode)(*it);

        if((it->type() == Node::EndElement) && (_reader->depth() < _startDepth))
        {
            break;
        }
    }

    //std::cerr << "-> DONE"<< std::endl;
}


IComposer* XmlDeserializer::advance(IComposer* deser)
{
    _deser = deser;
    _processNode = &XmlDeserializer::beginDocument;

    while( _reader->advance() )
    {
        const Pt::Xml::Node& node = _reader->get();
        (this->*_processNode)(node);
    }

    return _deser;
}


void XmlDeserializer::beginDocument(const Node& node)
{
    switch( node.type() )
    {
        case Node::StartElement:
        {
            _nodeName = static_cast<const StartElement&>(node).name();
            //std::cerr << "-> beginDocument::StartElement " << _nodeName.narrow() << std::endl;
            _deser->setName( _nodeName.narrow() );

            _nodeId = static_cast<const StartElement&>(node).attribute(L"id");
            if( ! _nodeId.empty() )
            {
                _deser->setId( _nodeId.narrow() );
            }

            String refId = static_cast<const StartElement&>(node).attribute(L"ref");
            if( ! refId.empty() )
            {
                _deser->setReference( refId.narrow() );
            }

            _processNode = &XmlDeserializer::onRootElement;
            break;
        }
        default:
            throw std::logic_error("Expected start element" + PT_SOURCEINFO);
    };
}


void XmlDeserializer::onRootElement(const Node& node)
{
    //std::cerr << "XmlDeserializer::onRootElement(const Node& node)"<< std::endl;
    switch( node.type() )
    {
        case Node::Characters:
        {
            const Characters& chars = static_cast<const Characters&>(node);
            if(Pt::String::npos != chars.content().find_first_not_of(L" \t\n\r") )
            {
                /// OLD: throw std::logic_error("Invalid element" + PT_SOURCEINFO);
                _deser->setValue( chars.content() ); /// NEW
                _processNode = &XmlDeserializer::onContent;
                //std::cerr << "-> onRootElement::onContent" << chars.content().narrow() << std::endl;
            }
            else
            {
                _processNode = &XmlDeserializer::onWhitespace;
                //std::cerr << "-> onRootElement::onWhitespace"<< std::endl;
            }

            break;
        }
        case Node::StartElement:
        {
            //std::cerr << "-> StartElement" << std::endl;
            _nodeName = static_cast<const StartElement&>(node).name();
            //std::cerr << "-> onRootElement::StartElement " << _nodeName.narrow() << std::endl;

            _processNode = &XmlDeserializer::onStartElement;
            break;
        }
        case Node::EndElement:
        {
            _processNode = &XmlDeserializer::onEndElement;
            //std::cerr << "-> onRootElement::EndElement " << std::endl;
            _deser = _deser->finish();
            break;
        }

        default:
            throw std::logic_error("Invalid element" + PT_SOURCEINFO);
    };
}


void XmlDeserializer::onStartElement(const Node& node)
{
    //std::cerr << "XmlDeserializer::onStartElement(const Node& node)"<< std::endl;

    switch( node.type() )
    {
        case Node::Characters:
        {
            const Characters& chars = static_cast<const Characters&>(node);
            if(Pt::String::npos != chars.content().find_first_not_of(L" \t\n\r") )
            {
                _deser = _deser->beginMember(_nodeName.narrow() );
                _deser->setValue( chars.content() );
                _deser->setId( _nodeId.narrow() );
                _nodeId.clear();
                _deser = _deser->finish();
                //_current->addValue( _nodeName.narrow(), chars.content() );

                _processNode = &XmlDeserializer::onContent;
                //std::cerr << "-> onStartElement::onContent 1 " << chars.content().narrow() << std::endl;
            }
            else
            {
                if(_deser == 0)
                    throw std::logic_error("Element outside document tree" + PT_SOURCEINFO);

                _deser = _deser->beginMember(_nodeName.narrow() );
                //SerializationInfo& added = _current->addMember( _nodeName.narrow() );
                //_current = &added;

                _processNode = &XmlDeserializer::onWhitespace;
                //std::cerr << "-> onStartElement::onWhitespace" << std::endl;
            }

            break;
        }
        case Node::StartElement:
        {
            if(_deser == 0)
                throw std::logic_error("Element outside document tree" + PT_SOURCEINFO);

            _deser = _deser->beginMember(_nodeName.narrow() );
            //SerializationInfo& added = _current->addMember( _nodeName.narrow() );
            //_current = &added;

            _nodeName = static_cast<const StartElement&>(node).name();
            //std::cerr << "-> onStartElement::StartElement " << _nodeName.narrow() << std::endl;
            //std::cerr << "-> onStartElement"<< std::endl;
            break;
        }
        case Node::EndElement:
        {
            if( _nodeName != static_cast<const EndElement&>(node).name() )
                throw std::logic_error("Invalid element" + PT_SOURCEINFO);

            //std::cerr << "added member " << _nodeName.narrow() << std::endl;
            //std::cerr << "added member id " <<  _nodeId.narrow() << std::endl;
            _deser = _deser->beginMember(_nodeName.narrow() );
            _deser->setValue( Pt::String() );
            _deser = _deser->finish();
            //_current->addValue( _nodeName.narrow(), Pt::String() );

            _processNode = &XmlDeserializer::onEndElement;
            //std::cerr << "-> onStartElement::onEndElement"<< std::endl;
            break;
        }
        default:
            throw std::logic_error("Invalid element" + PT_SOURCEINFO);
    };
}


void XmlDeserializer::onWhitespace(const Node& node)
{
    //std::cerr << "XmlDeserializer::onWhitespace(const Node& node)"<< std::endl;

    switch( node.type() )
    {
        case Node::StartElement:
        {
            _nodeName = static_cast<const StartElement&>(node).name();

            String refId = static_cast<const StartElement&>(node).attribute(L"ref");
            if( ! refId.empty() )
            {
                _deser = _deser->beginMember(_nodeName.narrow() );
                _deser->setReference( refId.narrow() );
                _deser = _deser->finish();
                //SerializationInfo& ref = _current->addValue( _nodeName.narrow(), refId );
                //ref.setCategory(SerializationInfo::Reference);

                //ref.setId( refId.narrow() );

                _processNode = &XmlDeserializer::onContent;
                //std::cerr << "-> onWhitespace::onContent"<< std::endl;
            }

            _nodeId = static_cast<const StartElement&>(node).attribute(L"id");

            _processNode = &XmlDeserializer::onStartElement;
            //std::cerr << "-> onStartElement " << _nodeName.narrow() << std::endl;
            break;
        }
        case Node::EndElement:
        {
            if(_deser == 0)
            {
                 //throw std::logic_error("Element outside document tree" + PT_SOURCEINFO);
                _processNode = &XmlDeserializer::onEndElement;
                break;
            }

            _nodeName = static_cast<const EndElement&>(node).name();

            if(_reader->depth() >= _startDepth)
            {
                //std::cerr << "onWhitespace::finish" << std::endl;
                _deser = _deser->finish();
            }

            _processNode = &XmlDeserializer::onEndElement;
            //std::cerr << "-> onEndElement"<< std::endl;
            break;
        }
        default:
            throw std::logic_error("Expected start element" + PT_SOURCEINFO);
    };
}


void XmlDeserializer::onContent(const Node& node)
{
    switch( node.type() )
    {
        case Node::EndElement:
        {
            _processNode = &XmlDeserializer::onEndElement;
            //std::cerr << "-> onContent::onEndElement"<< std::endl;
            _deser = _deser->finish();
            break;
        }
        default:
            throw std::logic_error("Expected end element" + PT_SOURCEINFO);
    };
}


void XmlDeserializer::onEndElement(const Node& node)
{
    switch( node.type() )
    {
        case Node::Characters:
        {
            //std::cerr << "-> onEndElement::Characters"<< std::endl;
            _processNode = &XmlDeserializer::onWhitespace;
            break;
        }
        case Node::StartElement:
        {
            //std::cerr << "-> onEndElement::StartElement"<< std::endl;
            _nodeName = static_cast<const StartElement&>(node).name();
            _processNode = &XmlDeserializer::onStartElement;
            break;
        }
        case Node::EndElement:
        {
            //std::cerr << "-> onEndElement::EndElement"<< std::endl;
            if(_deser == 0)
                throw std::logic_error("Invalid parent" + PT_SOURCEINFO);

            _nodeName = static_cast<const EndElement&>(node).name();

            if(_reader->depth() >= _startDepth)
            {
                //std::cerr << "onEndElement::finish" << std::endl;
                _deser = _deser->finish();
            }

            break;
        }
        case Node::EndDocument:
        {
            //std::cerr << "-> onEndElement::EndDocument"<< std::endl;
            break;
        }
        default:
        {
            throw std::logic_error("Expected start element" + PT_SOURCEINFO);
        }
    };
}

} // namespace Xml

} // namespace Pt
