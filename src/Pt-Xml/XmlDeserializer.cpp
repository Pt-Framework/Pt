#include "Pt/Xml/XmlDeserializer.h"
#include "Pt/Xml/XmlReader.h"
#include "Pt/Xml/StartElement.h"
#include "Pt/Xml/EndElement.h"
#include "Pt/Xml/Characters.h"
#include "Pt/Exception.h"
#include "Pt/String.h"


namespace Pt {

namespace Xml {

XmlDeserializer::XmlDeserializer(XmlReader& reader)
: _reader(&reader)
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
}


void XmlDeserializer::getData(SerializationInfo& si)
{
    _current = &si;
    _processNode = &XmlDeserializer::beginDocument;

    size_t startDepth = _reader->depth();

    for(XmlReader::Iterator it = _reader->current(); it != _reader->end(); ++it)
    {
        (this->*_processNode)(*it);

        if((it->type() == Node::EndElement) && (_reader->depth() < startDepth))
            break;
    }

    // currently at closing tag - we have to advance to the next node.
    _reader->next();

}


void XmlDeserializer::beginDocument(const Node& node)
{
    switch( node.type() )
    {
        case Node::StartElement:
        {
            _nodeName = static_cast<const StartElement&>(node).name();
            _current->setName( _nodeName.narrow() );
            _processNode = &XmlDeserializer::onRootElement;
            break;
        }
        default:
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
                throw std::logic_error("Invalid element" + PT_SOURCEINFO);
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
                    throw std::logic_error("Invalid parent" + PT_SOURCEINFO);

                SerializationInfo& added = _current->addMember( _nodeName.narrow() );
                _current = &added;
                _processNode = &XmlDeserializer::onWhitespace;
            }

            break;
        }
        case Node::StartElement:
        {
            if(_current == 0)
                throw std::logic_error("Invalid parent" + PT_SOURCEINFO);

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
            _processNode = &XmlDeserializer::onStartElement;
            break;
        }
        case Node::EndElement:
        {
            if(_current == 0)
                throw std::logic_error("Invalid parent" + PT_SOURCEINFO);

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
