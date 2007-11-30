#include <Pt/SerializationInfo.h>


namespace Pt {


SerializationError::SerializationError(const std::string& msg, const SourceInfo& si)
: std::logic_error(msg + "'" + si)
{ }

SerializationError::~SerializationError() throw()
{}


SerializationInfo::SerializationInfo()
: _parent(0)
, _category(Value)
{
    _nodes.reserve(150);
}

SerializationInfo::SerializationInfo(const SerializationInfo& si)
: _parent(0)
, _category(si._category)
, _name(si._name)
, _id(si._id)
, _type(si._type)
, _value(si._value)
, _nodes(si._nodes)
{
}

SerializationInfo& SerializationInfo::operator =(const SerializationInfo& si)
{
    // don't assign parent on purpose !!!
    _category = si._category;
    _name = si._name;
    _id = si._id;
    _type = si._type;
    _value = si._value;
    _nodes = si._nodes;

    return *this;
}



SerializationInfo::~SerializationInfo()
{
    Nodes::iterator it;
    for(it = _nodes.begin(); it != _nodes.end(); ++it)
    {
        delete *it;
    }
}


SerializationInfo::Category SerializationInfo::category() const
{
    return _category;
}


void SerializationInfo::setCategory(Category cat)
{
    _category = cat;
}


SerializationInfo* SerializationInfo::parent()
{
    return _parent;
}


const SerializationInfo* SerializationInfo::parent() const
{
    return _parent;
}


const std::string& SerializationInfo::typeName() const
{
    return _type;
}


void SerializationInfo::setTypeName(const std::string& type)
{
    _type = type;
}


const std::string& SerializationInfo::name() const
{
    return _name;
}


void SerializationInfo::setName(const std::string& name)
{
    _name = name;
}


void SerializationInfo::setValue(const Pt::Variant& value)
{
    _value = value;
    _category = Value;
}


void SerializationInfo::setId(const std::string& id)
{
    _id = id;
}


const std::string& SerializationInfo::id() const
{
    return _id;
}


void SerializationInfo::fixdown(void* ref)
{
    _value = ref;
    _category = Reference;
}


void SerializationInfo::fixup(void*& type) const
{
    _value = &type;
    type = 0;
}


const Pt::String& SerializationInfo::toString() const
{
    return _value.str();
}


SerializationInfo& SerializationInfo::addMember(const std::string& name)
{
    SerializationInfo* info = new SerializationInfo();
    info->setParent(*this);
    info->setName(name);
    _nodes.push_back( info );

    _category = Object;
    return *info;
}


SerializationInfo::Iterator SerializationInfo::begin()
{
    if(_nodes.size() == 0)
        return 0;

    return &( _nodes[0] );
}


SerializationInfo::Iterator SerializationInfo::end()
{
    if(_nodes.size() == 0)
        return 0;

    return &_nodes[0] + _nodes.size();
}


SerializationInfo::ConstIterator SerializationInfo::begin() const
{
    if(_nodes.size() == 0)
        return 0;

    return &( _nodes[0] );
}


SerializationInfo::ConstIterator SerializationInfo::end() const
{
    if(_nodes.size() == 0)
        return 0;

    return &_nodes[0] + _nodes.size();
}


const SerializationInfo& SerializationInfo::getMember(const std::string& name) const
{
    Nodes::const_iterator it = _nodes.begin();
    for(; it != _nodes.end(); ++it)
    {
        if( (*it)->name() == name )
            return **it;
    }

    throw SerializationError("Missing info for '" + name + "'", PT_SOURCEINFO);
}


const SerializationInfo* SerializationInfo::findMember(const std::string& name) const
{
    Nodes::const_iterator it = _nodes.begin();
    for(; it != _nodes.end(); ++it)
    {
        if( (*it)->name() == name )
            return *it;
    }

    return 0;
}


SerializationInfo* SerializationInfo::findMember(const std::string& name)
{
    Nodes::iterator it = _nodes.begin();
    for(; it != _nodes.end(); ++it)
    {
        if( (*it)->name() == name )
            return *it;
    }

    return 0;
}


size_t SerializationInfo::memberCount() const
{
    return _nodes.size();
}


SerializationInfo::Iterator::Iterator()
: _info(0)
{}


SerializationInfo::Iterator::Iterator(const Iterator& other)
: _info(other._info)
{}


SerializationInfo::Iterator::Iterator(SerializationInfo** info)
: _info(info)
{}


SerializationInfo::Iterator& SerializationInfo::Iterator::operator=(const Iterator& other)
{
    _info = other._info;
    return *this;
}


SerializationInfo::Iterator& SerializationInfo::Iterator::operator++()
{
    ++_info;
    return *this;
}


SerializationInfo& SerializationInfo::Iterator::operator*()
{
    return **_info;
}


SerializationInfo* SerializationInfo::Iterator::operator->()
{
    return *_info;
}


bool SerializationInfo::Iterator::operator!=(const Iterator& other) const
{
    return _info != other._info;
}


SerializationInfo::ConstIterator::ConstIterator()
: _info(0)
{}


SerializationInfo::ConstIterator::ConstIterator(const ConstIterator& other)
: _info(other._info)
{}


SerializationInfo::ConstIterator::ConstIterator(SerializationInfo* const* info)
: _info(info)
{}


SerializationInfo::ConstIterator& SerializationInfo::ConstIterator::operator=(const ConstIterator& other)
{
    _info = other._info;
    return *this;
}


SerializationInfo::ConstIterator& SerializationInfo::ConstIterator::operator++()
{
    ++_info;
    return *this;
}


const SerializationInfo& SerializationInfo::ConstIterator::operator*() const
{
    return **_info;
}


const SerializationInfo* SerializationInfo::ConstIterator::operator->() const
{ return *_info; }


bool SerializationInfo::ConstIterator::operator!=(const ConstIterator& other) const
{ return _info != other._info;
}

} // namespace Pt
