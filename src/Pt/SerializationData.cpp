#include <Pt/SerializationData.h>


namespace Pt {

SerializationError::SerializationError(const std::string& msg, const SourceInfo& si)
: std::logic_error(msg + "'" + si)
{}


SerializationError::~SerializationError() throw()
{}




SerializationEntry::SerializationEntry(SerializationData& parent, const std::string& name)
: SerializationNode(SerializationNode::Value, &parent, name)
{}


SerializationEntry::SerializationEntry(SerializationData& parent, const std::string& name, const Pt::Variant& value)
: SerializationNode(SerializationNode::Value, &parent, name)
, _value(value)
{}




SerializationData::SerializationData(SerializationData* parent)
: SerializationNode(SerializationNode::Object, parent)
{}


SerializationData::SerializationData(SerializationData* parent, const std::string& name)
: SerializationNode(SerializationNode::Object, parent, name)
{}


SerializationData::~SerializationData()
{
    Nodes::iterator it;
    for(it = _nodes.begin(); it != _nodes.end(); ++it)
    {
        delete *it;
    }
}


const SerializationNode& SerializationData::getNode(size_t n) const
{
    if( n >= _nodes.size() )
        throw SerializationError("Out of range", PT_SOURCEINFO);

    return *_nodes[n];
}


const SerializationNode& SerializationData::getNode(const std::string& name) const
{
    Nodes::const_iterator it = _nodes.begin();
    for(; it != _nodes.end(); ++it)
    {
        if( (*it)->name() == name )
            return **it;
    }

    throw SerializationError("Missing data for '" + name + "'", PT_SOURCEINFO);
}


SerializationNode& SerializationData::getNode(const std::string& name)
{
    Nodes::iterator it = _nodes.begin();
    for(; it != _nodes.end(); ++it)
    {
        if( (*it)->name() == name )
            return **it;
    }

    throw SerializationError("Missing data for '" + name + "'", PT_SOURCEINFO);
}


const SerializationNode* SerializationData::findNode(const std::string& name) const
{
    Nodes::const_iterator it = _nodes.begin();
    for(; it != _nodes.end(); ++it)
    {
        if( (*it)->name() == name )
            return *it;
    }

    return 0;
}


SerializationNode* SerializationData::findNode(const std::string& name)
{
    Nodes::iterator it = _nodes.begin();
    for(; it != _nodes.end(); ++it)
    {
        if( (*it)->name() == name )
            return *it;
    }

    return 0;
}


void SerializationData::addEntry(const std::string& name, const Pt::Variant& value)
{
    //std::cerr << "entry: " << name.narrow() << std::endl;
    SerializationEntry* entry = new SerializationEntry(*this, name, value);
    _nodes.push_back(entry);
}


SerializationEntry& SerializationData::addEntry(const Pt::Variant& value)
{
    //std::cerr << "entry: " << std::endl;
    SerializationEntry* entry = new SerializationEntry(*this, "", value);
    _nodes.push_back(entry);
    return *entry;
}


SerializationData& SerializationData::addData(const std::string& name)
{
    //std::cerr << "data: " << name.narrow() << std::endl;
    SerializationData* data = new SerializationData(this, name);
    _nodes.push_back(data);
    return *data;
}


SerializationData& SerializationData::addData()
{
    //std::cerr << "data: " << std::endl;
    SerializationData* data = new SerializationData(this);
    _nodes.push_back(data);
    return *data;
}

const SerializationData& SerializationData::getData(const std::string& name) const
{
    const SerializationNode& node = this->getNode(name);
    const SerializationData* data = node_cast<const SerializationData*>(&node);
    if( !data )
        throw SerializationError("Missing data for '" + name + "'", PT_SOURCEINFO);

    return *data;
}


const SerializationData* SerializationData::findData(const std::string& name) const
{
    const SerializationNode* node = this->findNode(name);
    return node_cast<const SerializationData*>(node);
}


SerializationData* SerializationData::findData(const std::string& name)
{
    SerializationNode* node = this->findNode(name);
    return node_cast<SerializationData*>(node);
}


SerializationData& SerializationData::getData(const std::string& name)
{
    SerializationNode& node = this->getNode(name);
    SerializationData* data = node_cast<SerializationData*>(&node);
    if( !data )
        throw SerializationError("Missing data for '" + name + "'", PT_SOURCEINFO);

    return *data;
}


const SerializationEntry& SerializationData::getEntry(const std::string& name) const
{
    const SerializationNode& node = this->getNode(name);
    const SerializationEntry* entry = node_cast<const SerializationEntry*>(&node);
    if( !entry )
        throw SerializationError("Missing data for '" + name + "'", PT_SOURCEINFO);

    return *entry;
}


} // namespace Pt
