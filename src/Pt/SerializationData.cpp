#include <Pt/SerializationData.h>


namespace Pt {

NoSuchEntry::NoSuchEntry(const std::string& name, const SourceInfo& si)
: std::logic_error("No entry named '" + name + "'" + si)
{
}

NoSuchEntry::~NoSuchEntry() throw()
{
}


SerializationEntry::SerializationEntry(SerializationData& parent, const Pt::String& name)
: SerializationNode(&parent, name)
{}


SerializationEntry::SerializationEntry(SerializationData& parent, const Pt::String& name, const Pt::Variant& value)
: SerializationNode(&parent, name)
, _value(value)
{}
        

SerializationData::SerializationData(SerializationData* parent)
: SerializationNode(parent)
{
}


SerializationData::SerializationData(SerializationData* parent, const Pt::String& name)
: SerializationNode(parent, name)
{
}


SerializationData::~SerializationData()
{
    Nodes::const_iterator it;
    for(it = _nodes.begin(); it != _nodes.end(); ++it)
    {
        delete it->second;
    }
}


const SerializationNode* SerializationData::getNode(const Pt::String& name) const
{
    Nodes::const_iterator it = _nodes.find(name);
    if( it == _nodes.end() )
        return 0;

    return it->second;
}


SerializationNode* SerializationData::getNode(const Pt::String& name)
{
    Nodes::iterator it = _nodes.find(name);
    if( it == _nodes.end() )
        return 0;

    return it->second;
}


void SerializationData::addEntry(const Pt::String& name, const Pt::Variant& value)
{
    SerializationEntry* e = new SerializationEntry(*this, name, value);
    _nodes.insert( std::make_pair(name, e) );
}


SerializationData& SerializationData::addData(const Pt::String& name)
{
    SerializationData* data = new SerializationData(this, name);
    _nodes.insert( std::make_pair(name, data) );
    return *data;
}


const SerializationData* SerializationData::getData(const Pt::String& name) const
{
    const SerializationNode* node = this->getNode(name);
    if( node && node->toData() )
        return node->toData();

    return 0;
}


SerializationData* SerializationData::getData(const Pt::String& name)
{
    SerializationNode* node = this->getNode(name);
    if( node && node->toData() )
        return node->toData();

    return 0;
}


const Pt::Variant* SerializationData::getEntry(const Pt::String& name) const
{
    const SerializationNode* node = this->getNode(name);

    if( node && node->toEntry() )
        return &( node->toEntry()->value() );

    return 0;
}


} // namespace Pt


