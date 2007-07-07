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
    SerializationData::ConstIterator it;
    for(it = data.begin(); it != data.end(); ++it)
    {
        const SerializationEntry* entry = it->toEntry();
        if(entry)
        {
            _writer->writeElement( entry->name(), Pt::String::widen( entry->value().str() ) );
        }
        else
        {
            const SerializationData* subdata = it->toData();
            _writer->writeStartElement( subdata->name() );
            this->writeData( *subdata );
            _writer->writeEndElement();
        }
    }
}

} // namespace Xml

} // namespace Pt
