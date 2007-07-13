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


void SerializationData::addEntry(const Pt::String& name, const Pt::Variant& value)
{
    SerializationEntry entry(*this, name, value);
    Entries::iterator it = std::upper_bound( _entries.begin(), _entries.end(), entry);
    _entries.insert( it, entry );
}


SerializationData& SerializationData::addData(const Pt::String& name)
{
    SerializationData data = SerializationData(this, name);
    _subdata.push_back(data);
    return _subdata.back();
}


const SerializationData* SerializationData::getData(const Pt::String& name) const
{
    SubData::const_iterator it = _subdata.begin();
    for(; it != _subdata.end(); ++it)
    {
        if(it->name() == name)
            return &(*it);
    }

    return 0;
}


SerializationData* SerializationData::getData(const Pt::String& name)
{
    SubData::iterator it = _subdata.begin();
    for(; it != _subdata.end(); ++it)
    {
        if(it->name() == name)
            return &(*it);
    }

    return 0;
}


const Pt::Variant* SerializationData::getEntry(const Pt::String& name) const
{
/*    Entries::const_iterator it = lowerBound( _entries.begin(), _entries.end(), name);
    if( it == _entries.end() )
        return 0;

    return &( it->value() );*/

    Entries::const_iterator it = _entries.begin();
    for(; it != _entries.end(); ++it)
    {
        if(it->name() == name)
            return &( it->value() );
    }

    return 0;
}


} // namespace Pt


