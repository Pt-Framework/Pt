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


void XmlSerializer::write(const SerializationInfo& si)
{
    if (!_writer)
        throw std::logic_error("XmlSerizalizer was not yet opened." + PT_SOURCEINFO);

    if( si.category() == SerializationInfo::Value )
    {
        Attribute attr( Pt::String(L"id"), Pt::String::widen( si.id() ) ); /// NEW
        _writer->writeElement( Pt::String::widen( si.name() ), &attr, 1, si.toString() ); /// NEW
        ///OLD: _writer->writeElement( Pt::String::widen( si.name() ), si.toString() );
    }
    else if( si.category() == SerializationInfo::Object )
    {
        if( si.id().empty() == false )
        {
            Attribute attr( Pt::String(L"id"), Pt::String::widen( si.id() ) );
            _writer->writeStartElement( Pt::String::widen( si.name() ), &attr, 1 );
        }
        else
            _writer->writeStartElement( Pt::String::widen( si.name() ) );

        SerializationInfo::ConstIterator it;
        for(it = si.begin(); it != si.end(); ++it)
        {
            this->write( *it );
        }

        _writer->writeEndElement();
    }
    else if( si.category() == SerializationInfo::Reference )
    {
        Attribute attr( Pt::String(L"ref"), si.toString() );
        _writer->writeElement( Pt::String::widen( si.name() ), &attr, 1, Pt::String() );
    }
}


void XmlSerializer::flush()
{
    if (_writer)
        _writer->flush();
}

} // namespace Xml

} // namespace Pt
