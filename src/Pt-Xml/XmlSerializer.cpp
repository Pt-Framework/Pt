#include "Pt/Xml/XmlSerializer.h"
#include "Pt/Xml/XmlWriter.h"
#include "Pt/Xml/StartElement.h"
#include "Pt/Xml/EndElement.h"
#include "Pt/Xml/Characters.h"
#include "Pt/SerializationData.h"
#include "Pt/Exception.h"
#include "Pt/String.h"


namespace Pt {

namespace Xml {

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
    this->detach();
}


void XmlSerializer::attach(std::ostream& os)
{
    if (_writer)
        throw std::logic_error("XmlSerizalizer is already open." + PT_SOURCEINFO);

    _deleter.reset(new XmlWriter(os));
    _writer = _deleter.get();
}


void XmlSerializer::attach(XmlWriter* writer)
{
    if (_writer)
        throw std::logic_error("XmlSerizalizer is already open." + PT_SOURCEINFO);

    _deleter.reset(0);
    _writer = writer;
}


void XmlSerializer::detach()
{
    if (!_writer)
        return;

    this->flush();
    _deleter.reset(0);
    _writer = 0;
}

void XmlSerializer::putData(const SerializationData& data)
{
    if (!_writer)
        throw std::logic_error("XmlSerizalizer was not yet opened." + PT_SOURCEINFO);

    _writer->writeStartElement( Pt::String::widen( data.name() ) );
    this->writeData(data);
    _writer->writeEndElement();
}


void XmlSerializer::writeData(const SerializationData& data)
{
    if (!_writer)
        throw std::logic_error("XmlSerizalizer was not yet opened." + PT_SOURCEINFO);

    SerializationData::ConstNodeIterator it;
    for(it = data.begin(); it != data.end(); ++it)
    {
        if(const SerializationEntry* entry = node_cast<const SerializationEntry*>(&*it) )
        {
            _writer->writeElement( Pt::String::widen( entry->name() ), entry->str() );
        }
        else if(const SerializationData* subdata = node_cast<const SerializationData*>(&*it) )
        {
            _writer->writeStartElement( Pt::String::widen( subdata->name() ) );
            this->writeData( *subdata );
            _writer->writeEndElement();
        }
    }
}


void XmlSerializer::flush()
{
    if (_writer)
        _writer->flush();
}


} // namespace Xml

} // namespace Pt
