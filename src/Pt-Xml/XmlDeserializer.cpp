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
: _reader(&reader)
, _deser(0)
{
}


XmlDeserializer::XmlDeserializer(std::istream& is)
: _reader( 0 )
, _deleter(new XmlReader(is))
, _deser(0)
{
    _reader = _deleter.get();
}


XmlDeserializer::~XmlDeserializer()
{
}


void XmlDeserializer::get(IDeserializer* deser)
{
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
}


void XmlDeserializer::beginDocument(const Node& node)
{
    switch( node.type() )
    {
        case Node::StartElement:
        {
            _nodeName = static_cast<const StartElement&>(node).name();
            //std::cerr << "-> StartElement " << _nodeName.narrow() << std::endl;
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
                //std::cerr << "-> onContent root " << chars.content().narrow() << std::endl;
            }
            else
            {
                _processNode = &XmlDeserializer::onWhitespace;
                //std::cerr << "-> onWhitespace"<< std::endl;
            }

            break;
        }
        case Node::StartElement:
        {
            //std::cerr << "-> StartElement" << std::endl;
            _nodeName = static_cast<const StartElement&>(node).name();
            //std::cerr << "-> StartElement " << _nodeName.narrow() << std::endl;

            _processNode = &XmlDeserializer::onStartElement;
            break;
        }
        case Node::EndElement:
        {
            _processNode = &XmlDeserializer::onEndElement;
            //std::cerr << "-> EndElement " << std::endl;
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
                _deser = _deser->leaveMember();
                //_current->addValue( _nodeName.narrow(), chars.content() );

                _processNode = &XmlDeserializer::onContent;
                //std::cerr << "-> onContent 1 " << chars.content().narrow() << std::endl;
            }
            else
            {
                if(_deser == 0)
                    throw std::logic_error("Element outside document tree" + PT_SOURCEINFO);

                _deser = _deser->beginMember(_nodeName.narrow() );
                //SerializationInfo& added = _current->addMember( _nodeName.narrow() );
                //_current = &added;

                _processNode = &XmlDeserializer::onWhitespace;
                //std::cerr << "-> onWhitespace" << std::endl;
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
            //std::cerr << "-> StartElement " << _nodeName.narrow() << std::endl;
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
            _deser = _deser->leaveMember();
            //_current->addValue( _nodeName.narrow(), Pt::String() );

            _processNode = &XmlDeserializer::onEndElement;
            //std::cerr << "-> onEndElement"<< std::endl;
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
                _deser = _deser->leaveMember();
                //SerializationInfo& ref = _current->addValue( _nodeName.narrow(), refId );
                //ref.setCategory(SerializationInfo::Reference);

                //ref.setId( refId.narrow() );

                _processNode = &XmlDeserializer::onContent;
                //std::cerr << "-> onContent"<< std::endl;
            }

            _nodeId = static_cast<const StartElement&>(node).attribute(L"id");

            _processNode = &XmlDeserializer::onStartElement;
            //std::cerr << "-> onStartElement " << _nodeName.narrow() << std::endl;
            break;
        }
        case Node::EndElement:
        {
            if(_deser == 0)
                throw std::logic_error("Element outside document tree" + PT_SOURCEINFO);

            _nodeName = static_cast<const EndElement&>(node).name();

            if(_reader->depth() >= _startDepth)
                _deser = _deser->leaveMember();

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
            //std::cerr << "-> onEndElement"<< std::endl;
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
            _processNode = &XmlDeserializer::onWhitespace;
            break;
        }
        case Node::StartElement:
        {
            _nodeName = static_cast<const StartElement&>(node).name();
            _processNode = &XmlDeserializer::onStartElement;
            break;
        }
        case Node::EndElement:
        {
            if(_deser == 0)
                throw std::logic_error("Invalid parent" + PT_SOURCEINFO);

            _nodeName = static_cast<const EndElement&>(node).name();

            if(_reader->depth() >= _startDepth)
                _deser = _deser->leaveMember();

            break;
        }
        case Node::EndDocument:
        {
            break;
        }
        default:
        {
            throw std::logic_error("Expected start element" + PT_SOURCEINFO);
        }
    };
}


void XmlDeserializer::beginLinkTarget(const std::string& name, const std::string& id,
                                      void* obj, const std::type_info& fixupInfo)
{
    if( id.empty() )
        return;

    //std::cerr << "beginLinkTarget: "  << obj << " " << fixupInfo.name() << " id: " << id << std::endl;
    FixupInfo fi;
    fi.address = obj;
    fi.type = &fixupInfo;
    fi.fixup = 0;
    _targets[id] = fi;
}


void XmlDeserializer::finishLinkTarget()
{
}


void XmlDeserializer::prepareLink(const std::string& id, void* obj, FixupHandler fh)
{
    //std::cerr << "prepareLink: " << obj << " " << fixupInfo.name() << " id " << id << std::endl;
    FixupInfo fi;
    fi.address = obj;
    fi.fixup = fh;
    _pointers.insert( std::pair<std::string, FixupInfo>(id, fi) );
}


void XmlDeserializer::link()
{
    std::multimap<std::string, FixupInfo>::iterator it;
    for(it = _pointers.begin(); it != _pointers.end(); ++it)
    {
        void* fixme = it->second.address;

        std::string id = it->first;

        if( _targets.find(id) == _targets.end() )
            throw SerializationError("reference target not found");

        void* target = _targets[id].address;
        const std::type_info* targetType = _targets[id].type ;

        //std::cerr << "FIXING: " << fixme << " to " << target  << " by id " << id << std::endl;
        it->second.fixup(fixme, target, *targetType);
    }

    _targets.clear();
    _pointers.clear();
}


void XmlDeserializer::reset()
{
    _targets.clear();
    _pointers.clear();
}

} // namespace Xml

} // namespace Pt
