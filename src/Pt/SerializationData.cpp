#include <Pt/SerializationData.h>


namespace {

template<typename ForwardIter, typename T>
ForwardIter lowerBound(ForwardIter first, ForwardIter last, const T& val)
{
    typedef typename std::iterator_traits<ForwardIter>::value_type _ValueType;
    typedef typename std::iterator_traits<ForwardIter>::difference_type _DistanceType;

    _DistanceType len = distance(first, last);
    _DistanceType half;
    ForwardIter middle;

    while(len > 0)
    {
        half = len >> 1;
        middle = first;
        advance(middle, half);
        if (*middle < val)
        {
            first = middle;
            ++first;
            len = len - half - 1;
        }
        else
            len = half;
    }

    return first;
}

}


namespace Pt {

NoSuchEntry::NoSuchEntry(const std::string& name, const SourceInfo& si)
: std::logic_error("No entry named '" + name + "'" + si)
{}


NoSuchEntry::~NoSuchEntry() throw()
{}


SerializationEntry::SerializationEntry(SerializationData& parent, const Pt::String& name)
: SerializationNode(&parent, name)
{}


SerializationEntry::SerializationEntry(SerializationData& parent, const Pt::String& name, const Pt::Variant& value)
: SerializationNode(&parent, name)
, _value(value)
{}


SerializationData::SerializationData(SerializationData* parent)
: SerializationNode(parent)
{}


SerializationData::SerializationData(SerializationData* parent, const Pt::String& name)
: SerializationNode(parent, name)
{}


SerializationData::~SerializationData()
{
    Nodes::iterator it;
    for(it = _nodes.begin(); it != _nodes.end(); ++it)
    {
        delete *it;
    }
}


const SerializationNode* SerializationData::getNode(size_t n) const
{
    if( n >= _nodes.size() )
        return 0;

    return _nodes[n];
}


const SerializationNode* SerializationData::getNode(const Pt::String& name) const
{
    Nodes::const_iterator it = _nodes.begin();
    for(; it != _nodes.end(); ++it)
    {
        if( (*it)->name() == name)
            return (*it);
    }

    return 0;
}


SerializationNode* SerializationData::getNode(const Pt::String& name)
{
    Nodes::iterator it = _nodes.begin();
    for(; it != _nodes.end(); ++it)
    {
        if( (*it)->name() == name)
            return (*it);
    }

    return 0;
}


void SerializationData::addEntry(const Pt::String& name, const Pt::Variant& value)
{
    SerializationEntry* entry = new SerializationEntry(*this, name, value);
    _nodes.push_back(entry);
}


SerializationEntry& SerializationData::addEntry(const Pt::Variant& value)
{
    SerializationEntry* entry = new SerializationEntry(*this, Pt::String(), value);
    _nodes.push_back(entry);
    return *entry;
}


SerializationData& SerializationData::addData(const Pt::String& name)
{
    SerializationData* data = new SerializationData(this, name);
    _nodes.push_back(data);
    return *data;
}


SerializationData& SerializationData::addData()
{
    SerializationData* data = new SerializationData(this);
    _nodes.push_back(data);
    return *data;
}

const SerializationData* SerializationData::getData(const Pt::String& name) const
{
    const SerializationNode* node = this->getNode(name);
    if(node)
        return node->toData();

    return 0;
}


SerializationData* SerializationData::getData(const Pt::String& name)
{
    SerializationNode* node = this->getNode(name);
    if(node)
        return node->toData();

    return 0;
}


const Pt::Variant* SerializationData::getEntry(const Pt::String& name) const
{
    const SerializationNode* node = this->getNode(name);
    if(node)
        return &(node->toEntry()->value());

    return 0;
}


} // namespace Pt
