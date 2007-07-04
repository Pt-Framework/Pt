#include <Pt/SerializationData.h>


namespace Pt {

NoSuchEntry::NoSuchEntry(const std::string& name, const SourceInfo& si)
: std::logic_error("No entry named '" + name + "'" + si)
{
}

NoSuchEntry::~NoSuchEntry() throw()
{
}


ObjectEntry::ObjectEntry(SerializationData& parent, const Pt::String& name)
: _parentData(&parent)
, _objectName(name)
{
}


ObjectEntry::ObjectEntry(SerializationData& parent, const Pt::String& name, const Pt::Variant& value)
: _parentData(&parent)
, _objectName(name)
, _objectValue(value)
{
}


SerializationData* ObjectEntry::_parent()
{
    return _parentData;
}


const SerializationData* ObjectEntry::_parent() const
{
    return _parentData;
}


const Pt::String& ObjectEntry::_name() const
{
    return _objectName;
}


const Pt::Variant& ObjectEntry::_value() const
{
    return _objectValue;
}


ObjectData::ObjectData(SerializationData* parent)
: _parentData(parent)
{
}


ObjectData::ObjectData(SerializationData* parent, const Pt::String& name)
: _parentData(parent)
, _objectName(name)
{
}


ObjectData::~ObjectData()
{
    Nodes::const_iterator it;
    for(it = _nodes.begin(); it != _nodes.end(); ++it)
    {
        delete it->second;
    }
}


SerializationData* ObjectData::_parent()
{
    return _parentData;
}


const SerializationData* ObjectData::_parent() const
{
    return _parentData;
}


const Pt::String& ObjectData::_name() const
{
    return _objectName;
}


const SerializationNode* ObjectData::_getNode(const Pt::String& name) const
{
    Nodes::const_iterator it = _nodes.find(name);
    if( it == _nodes.end() )
        return 0;

    return it->second;
}


SerializationNode* ObjectData::_getNode(const Pt::String& name)
{
    Nodes::iterator it = _nodes.find(name);
    if( it == _nodes.end() )
        return 0;

    return it->second;
}


void ObjectData::_addEntry(const Pt::String& name, const Pt::Variant& value)
{
    ObjectEntry* e = new ObjectEntry(*this, name, value);
    _nodes.insert( std::make_pair(name, e) );
}


ObjectData& ObjectData::_addData(const Pt::String& name)
{
    ObjectData* data = new ObjectData(this, name);
    _nodes.insert( std::make_pair(name, data) );
    return *data;
}



} // namespace Pt


