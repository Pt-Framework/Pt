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
, _peeking(false)
{
}


XmlDeserializer::XmlDeserializer(std::istream& is)
: _reader( 0 )
, _deleter(new XmlReader(is))
{
    _reader = _deleter.get();
}


XmlDeserializer::~XmlDeserializer()
{
    this->finish();
}


void XmlDeserializer::read(SerializationInfo& si)
{
    if(_reader->get().type() != Node::StartElement)
        _reader->nextElement();

    _current = &si;
    _processNode = &XmlDeserializer::beginDocument;

    size_t startDepth = _reader->depth();
    for(XmlReader::Iterator it = _reader->current(); it != _reader->end(); ++it)
    {
        (this->*_processNode)(*it);

        if((it->type() == Node::EndElement) && (_reader->depth() < startDepth))
        {
            break;
        }
    }
}


SerializationInfo& XmlDeserializer::peek()
{
    if( ! _peeking )
    {
        _stack.push_back( SerializationInfo() );
        Pt::SerializationInfo& si =_stack.back();
        this->read( si );
        _peeking = true;
    }

    _peeking = true;
    return _stack.back();
}


void XmlDeserializer::finish()
{
/*
    std::list<Pt::SerializationInfo>::iterator it;
    for(it = _stack.begin(); it != _stack.end(); ++it)
    {
        this->fixup(*it);
    }
*/

    std::map<void*, std::string>::iterator it;
    for(it = _pointers.begin(); it != _pointers.end(); ++it)
    {
        void* fixme = it->first;
        std::string id = it->second;
        void* obj = _objects[id];
        //std::cerr << "FIXING: " << fixme << " to " << obj << std::endl;

        void** vp =(void**)(fixme);
        *vp = obj;
    }

    _peeking = false;
    _objects.clear();
    _stack.clear();
    _pointers.clear();
}


Pt::SerializationInfo& XmlDeserializer::get()
{
    if( ! _peeking )
    {
        _stack.push_back( SerializationInfo() );
        Pt::SerializationInfo& si =_stack.back();
        this->read( si );
    }

    _peeking = false;
    return _stack.back();
}


void XmlDeserializer::markFixup(Pt::SerializationInfo& si, void* type, Fixup fixup)
{
    if( ! si.id().empty() )
    {
        _objects[ si.id() ] = type;
        _fixups[ si.id() ] = fixup;
    }

    Pt::SerializationInfo::Iterator it;
    for(it = si.begin(); it != si.end(); ++it)
    {
         if(it->category() == Pt::SerializationInfo::Reference)
        {
            //std::cerr << "UNFIXED: " << it->fixupAddr() << " needs " << it->toValue<std::string>() << std::endl;

            _pointers[ it->fixupAddr() ] = it->toValue<std::string>();
        }
    }
}


void XmlDeserializer::fixup(const Pt::SerializationInfo& si)
{
    Pt::SerializationInfo::ConstIterator it;
    for(it = si.begin(); it != si.end(); ++it)
    {
        if(it->category() == Pt::SerializationInfo::Reference)
        {
            void* obj = _objects[ it->toValue<std::string>() ]; //TODO check that it exists
            void* fixme = it->fixupAddr();
            Fixup fixupHandler = _fixups[ it->toValue<std::string>() ];
            fixupHandler( (void**)(&fixme), it->fixupInfo(), obj);
        }

        if(it->category() == Pt::SerializationInfo::Object)
        {
            this->fixup(*it);
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
            _current->setName( _nodeName.narrow() );

            _nodeId = static_cast<const StartElement&>(node).attribute(L"id");
            if( ! _nodeId.empty() )
            {
                _current->setId( _nodeId.narrow() );
            }
            _processNode = &XmlDeserializer::onRootElement;
            break;
        }
        default:
            std::cerr << "NODE: " << node.type() << std::endl;
            throw std::logic_error("Expected start element" + PT_SOURCEINFO);
    };
}


void XmlDeserializer::onRootElement(const Node& node)
{
    switch( node.type() )
    {
        case Node::Characters:
        {
            const Characters& chars = static_cast<const Characters&>(node);
            if(Pt::String::npos != chars.content().find_first_not_of(L" \t\n\r") )
            {
                /// OLD: throw std::logic_error("Invalid element" + PT_SOURCEINFO);
                _current->setValue( chars.content() ); /// NEW
                _processNode = &XmlDeserializer::onContent;
            }
            else
            {
                _processNode = &XmlDeserializer::onWhitespace;
            }

            break;
        }
        case Node::StartElement:
        {
            _nodeName = static_cast<const StartElement&>(node).name();
            _processNode = &XmlDeserializer::onStartElement;
            break;
        }

        default:
            throw std::logic_error("Invalid element" + PT_SOURCEINFO);
    };
}


void XmlDeserializer::onStartElement(const Node& node)
{
    switch( node.type() )
    {
        case Node::Characters:
        {
            const Characters& chars = static_cast<const Characters&>(node);
            if(Pt::String::npos != chars.content().find_first_not_of(L" \t\n\r") )
            {
                _current->addValue( _nodeName.narrow(), chars.content() );
                _processNode = &XmlDeserializer::onContent;
            }
            else
            {
                if(_current == 0)
                    throw std::logic_error("Element outside document tree" + PT_SOURCEINFO);

                SerializationInfo& added = _current->addMember( _nodeName.narrow() );
                _current = &added;
                _processNode = &XmlDeserializer::onWhitespace;
            }

            break;
        }
        case Node::StartElement:
        {
            if(_current == 0)
                throw std::logic_error("Element outside document tree" + PT_SOURCEINFO);

             SerializationInfo& added = _current->addMember( _nodeName.narrow() );
            _current = &added;

            _nodeName = static_cast<const StartElement&>(node).name();
            break;
        }
        case Node::EndElement:
        {
            if( _nodeName != static_cast<const EndElement&>(node).name() )
                throw std::logic_error("Invalid element" + PT_SOURCEINFO);

            _current->addValue( _nodeName.narrow(), Pt::String() );
            _processNode = &XmlDeserializer::onEndElement;
            break;
        }
        default:
            throw std::logic_error("Invalid element" + PT_SOURCEINFO);
    };
}


void XmlDeserializer::onWhitespace(const Node& node)
{
    switch( node.type() )
    {
        case Node::StartElement:
        {
            _nodeName = static_cast<const StartElement&>(node).name();

            String refId = static_cast<const StartElement&>(node).attribute(L"ref");
            if( ! refId.empty() )
            {
                SerializationInfo& ref = _current->addValue( _nodeName.narrow(), refId );
                ref.setCategory(SerializationInfo::Reference);
                //ref.setId( refId.narrow() );

                _processNode = &XmlDeserializer::onContent;
            }

            _processNode = &XmlDeserializer::onStartElement;
            break;
        }
        case Node::EndElement:
        {
            if(_current == 0)
                throw std::logic_error("Element outside document tree" + PT_SOURCEINFO);

            _nodeName = static_cast<const EndElement&>(node).name();
            _current = _current->parent();

            _processNode = &XmlDeserializer::onEndElement;
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
            if(_current == 0)
                throw std::logic_error("Invalid parent" + PT_SOURCEINFO);

            _nodeName = static_cast<const EndElement&>(node).name();
            _current = _current->parent();

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

} // namespace Xml

} // namespace Pt
