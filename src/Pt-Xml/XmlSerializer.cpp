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

XmlSerializer::XmlSerializer(std::ostream& os)
: _writer( 0 )
, _deleter( new XmlWriter(os) )
{
    _writer = _deleter.get();
}


XmlSerializer::~XmlSerializer()
{
}


void XmlSerializer::putData(const SerializationData& data)
{
    _writer->writeStartElement( data.name() );
    this->writeData(data);
    _writer->writeEndElement();
    _writer->flush();   
}


void XmlSerializer::writeData(const SerializationData& data)
{
    const SerializationData::Entries& entries = data.entries();
    SerializationData::Entries::const_iterator it;
    for(it = entries.begin(); it != entries.end(); ++it)
    {
        const SerializationEntry& entry = *it;
        _writer->writeElement( entry.name(), Pt::String::widen( entry.value().str() ) );
    }

    const SerializationData::SubData& sub = data.subData();
    SerializationData::SubData::const_iterator iter;
    for(iter = sub.begin(); iter != sub.end(); ++iter)
    {
        const SerializationData& subdata = *iter;
        _writer->writeStartElement( subdata.name() );
        this->writeData( subdata );
        _writer->writeEndElement();
    }
}

} // namespace Xml

} // namespace Pt
