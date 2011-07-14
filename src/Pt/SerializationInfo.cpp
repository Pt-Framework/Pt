/*
 * Copyright (C) 2005-2008 by Dr. Marc Boris Duerner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <Pt/Formatter.h>
#include <Pt/SerializationInfo.h>
#include <Pt/SerializationContext.h>
#include <Pt/SerializationSurrogate.h>

namespace Pt {

bool SerializationInfo::beginFormat(Formatter& formatter)
{
    if(this->category() == SerializationInfo::Scalar)
    {
        switch(_type)
        {
            case Boolean:
                formatter.addBool( _name, _value.b, _id );
                break;

            case Int:
                formatter.addInt( _name, _value.l, _id );
                break;

            case UInt:
                formatter.addUInt( _name, _value.ul, _id );
                break;

            case Float:
                formatter.addFloat( _name, _value.f, _id );
                break;

            case Str:
            {
                const Pt::String* str = reinterpret_cast<const Pt::String*>(_value.str);
                formatter.addValue( _name, _typeName, *str, _id );
                break;
            }

            default:
                break;
        }

        return false;
    }
    else if(this->category() == Pt::SerializationInfo::Reference)
    {
        if( ! _context )
            throw SerializationError("context not available");

        const void* refAddr = _value.ref.address;
        const char* id = _context->getId( refAddr );
        formatter.addReference( this->name(), id);
        return false;
    }
    else if(this->category() == SerializationInfo::Struct)
    {
        formatter.beginObject( this->name(), this->typeName(), this->id() );
        return true;
    }
    else if(this->category() == Pt::SerializationInfo::Sequence)
    {
        formatter.beginArray( this->name(), this->typeName(), this->id() );
        return true;
    }

    return false;
}


void SerializationInfo::endFormat(Formatter& formatter)
{
    if(this->category() == SerializationInfo::Struct)
    {
        formatter.finishObject();
    }
    else if(this->category() == Pt::SerializationInfo::Sequence)
    {
        formatter.finishArray();
    }
}


void SerializationInfo::format(Formatter& formatter)
{
    if(this->category() == SerializationInfo::Scalar)
    {
        switch(_type)
        {
            case Boolean:
                formatter.addBool( _name, _value.b, _id );
                break;

            case Int:
                formatter.addInt( _name, _value.l, _id );
                break;

            case UInt:
                formatter.addUInt( _name, _value.ul, _id );
                break;

            case Float:
                formatter.addFloat( _name, _value.f, _id );
                break;

            case Str:
            {
                const Pt::String* str = reinterpret_cast<const Pt::String*>(_value.str);
                formatter.addValue( _name, _typeName, *str, _id );
                break;
            }

            default:
                break;
        }
    }
    else if(this->category() == Pt::SerializationInfo::Reference)
    {
        if( ! _context )
            throw SerializationError("context not available");

        const void* refAddr = _value.ref.address;
        const char* id = _context->getId( refAddr );
        formatter.addReference( this->name(), id);
    }
    else if(this->category() == SerializationInfo::Struct)
    {
        formatter.beginObject( this->name(), this->typeName(), this->id() );

        SerializationInfo::Iterator it;
        for(it = this->begin(); it != this->end(); ++it)
        {
            formatter.beginMember( it->name(), it->typeName(), it->id() );
            it->format(formatter);
            formatter.finishMember();
        }

        formatter.finishObject();
    }
    else if(this->category() == Pt::SerializationInfo::Sequence)
    {
        formatter.beginArray( this->name(), this->typeName(), this->id() );

        SerializationInfo::Iterator it;
        for(it = this->begin(); it != this->end(); ++it)
        {
            formatter.beginElement( it->typeName(), it->id()  );
            it->format(formatter);
            formatter.finishElement();
        }

        formatter.finishArray();
    }
}


SerializationInfo::Category SerializationInfo::category() const
{
    return _category;
}


SerializationInfo::~SerializationInfo()
{
    this->clearValue();
}


void SerializationInfo::clear()
{
    this->clearValue();

    if( _name.size() )
        _name.clear();

    if( _typeName.size() )
        _typeName.clear();

    if( _id.size() )
        _id.clear();

    _bound = 0;
    _category = Void;
    _type = Unknown;
}


void SerializationInfo::clear(SerializationContext* context)
{
    this->clear();
    _context = context;
}


void SerializationInfo::clearValue()
{
    if(_category == Struct || _category == Sequence)
    {
        for(SerializationInfo* it = _value.seq.first; it != 0; )
        {
            if(_context)
            {
                SerializationInfo* tmp = it;
                it = it->sibling();
                tmp->setSibling(0);

                _context->push(tmp);
            }
            else
            {
                SerializationInfo* tmp = it;
                it = it->sibling();
                delete tmp;
            }
        }
    }
    else if(_type == Str)
    {
        Pt::String* str = reinterpret_cast<Pt::String*>(_value.str);
        str->~basic_string();
    }
    else if(_category == Reference)
    {
        std::string* str = reinterpret_cast<std::string*>(_value.ref.refid);
        str->~basic_string();
    }
}


void SerializationInfo::setSequence()
{
    if(this->category() == SerializationInfo::Context)
        return;

    if( this->category() != SerializationInfo::Sequence &&
        this->category() != SerializationInfo::Struct )
    {
        this->clearValue();

        _value.seq.first = 0;
        _value.seq.last = 0;
        _value.seq.size = 0;

        _category = Sequence;
        _type = Unknown;
    }

    _category = Sequence;
}


void SerializationInfo::setContextual()
{
    if(this->category() == SerializationInfo::Context)
        return;

    this->clearValue();

    _category = Context;
    _type = Unknown;
}


// called during serialization, when a reference needs to be unlinked
void SerializationInfo::setReference(const void* ref)
{
    if( category() == Context )
    {
        if( _context && this->context()->referencingEnabled() )
            this->context()->prepareId(ref);

        return;
    }

    if(category() != Reference)
    {
        this->clearValue();

        new(_value.ref.refid) std::string;
        _type = Unknown;
        _category = Reference;
    }

    _value.ref.address = const_cast<void*>(ref) ;
}


// called during deserialization, when a reference id was parsed
void SerializationInfo::setReference(const std::string& id)
{
    if(category() != Reference)
    {
        this->clearValue();

        new(_value.ref.refid) std::string(id);
        _type = Unknown;
        _category = Reference;
    }
    else
    {
        std::string* str = reinterpret_cast<std::string*>(_value.ref.refid);
        *str = id;
    }

    _value.ref.address = 0;
}


// called during deserialization, when a reference needs to be fixed up
void SerializationInfo::load(void* type, FixupInfo::FixupHandler fh, unsigned m) const
{
    if( this->category() != Reference)
        throw SerializationError("not a reference");

    const std::string* refId = reinterpret_cast<const std::string*>(_value.ref.refid);
    _value.ref.address = type;

    if(_context)
    {
        _context->prepareFixup(type, *refId, fh, m);
    }
}


const char* SerializationInfo::getBinary(size_t& length) const
{
    length = 0;
    return 0;
}


void SerializationInfo::setBinary(const char* data, size_t length)
{
}


void SerializationInfo::getValue(Pt::String& s) const
{
    if( this->category() != Scalar )
        throw SerializationError("not a value");

    if(_type == Str)
    {
        const Pt::String* str = reinterpret_cast<const Pt::String*>(_value.str);
        s = *str;
    }
    else if(_type ==  Boolean)
    {
        convert(s, _value.b);
    }
    else if(_type ==  Int)
    {
        convert(s, _value.l);
    }
    else if(_type == UInt)
    {
        convert(s, _value.ul);
    }
    else if(_type ==  Float)
    {
        convert(s, _value.f);
    }
}


void SerializationInfo::setValue(const Pt::String& value)
{
    if( category() == Context )
        return;

    if(_category == Void)
    {
        new(_value.str) Pt::String(value);
    }
    else if(_type != Str)
    {
        this->clearValue();
        new(_value.str) Pt::String(value);
    }
    else
    {
        Pt::String* str = reinterpret_cast<Pt::String*>(_value.str);
        *str = value;
    }

    _category = Scalar;
    _type = Str;
}


void SerializationInfo::getValue(char& c) const
{
    Pt::String s;
    this->getValue(s);
    if( s.empty() )
        throw SerializationError("not a char value");

    c = s[0];
}


void SerializationInfo::setValue(char c)
{
    if( category() == Context )
        return;

    Pt::String s;
    s.push_back( Pt::Char(c));
    this->setValue(s);
}


void SerializationInfo::getValue( bool& value) const
{
    if( this->category() != Scalar )
        throw SerializationError("expected integer value");

    switch(_type)
    {
        case Boolean:
            value = _value.b;
            break;

        case Int:
            value = 0 != _value.l;
            break;

        case UInt:
            value = 0 != _value.ul;
            break;

        case Float:
            value = 0 != _value.f;
            break;

        case Str:
        {
            const Pt::String* str = reinterpret_cast<const Pt::String*>(_value.str);
            convert(value, *str);
            break;
        }

        default:
            break;
    }
}


void SerializationInfo::setValue(bool value)
{
    if( category() == Context )
        return;

    this->clearValue();

    _category = Scalar;
    _value.b = value;
    _type = Boolean;
}


void SerializationInfo::getValue(short& s) const
{
    long long l = 0;
    this->getValue(l);
    // TODO: consider SerializationError on overflow
    s = static_cast<short>(l);
}


void SerializationInfo::getValue(long& i) const
{
    long long l = 0;
    this->getValue(l);
    // TODO: consider SerializationError on overflow
    i = static_cast<int>(l);
}


void SerializationInfo::getValue(long long & l) const
{
    if( this->category() != Scalar )
        throw SerializationError("expected scalar value");

    switch(_type)
    {
        case Boolean:
            l =  static_cast<long long>(_value.b);
            break;

        case Int:
            l =  static_cast<long long>(_value.l);
            break;

        case UInt:
            l =  static_cast<long long>(_value.ul);
            break;

        case Float:
            l =  static_cast<long long>(_value.f);
            break;

        case Str:
        {
            const Pt::String* str = reinterpret_cast<const Pt::String*>(_value.str);
            convert(l, *str);
            break;
        }

        default:
            break;
    }
}


void SerializationInfo::setValue(long long l)
{
    if( category() == Context )
        return;

    if(_category != Void && _type != Int)
        this->clearValue();

    _category = Scalar;
    _value.l = l;
    _type = Int;
}


void SerializationInfo::getValue(unsigned short& us) const
{
    unsigned long long ul = 0;
    this->getValue(ul);
    // TODO: consider SerializationError on overflow
    us = static_cast<unsigned short>(ul);
}


void SerializationInfo::getValue(unsigned int& ui) const
{
    unsigned long long ul = 0;
    this->getValue(ul);
    // TODO: consider SerializationError on overflow
    ui = static_cast<unsigned int>(ul);
}


void SerializationInfo::getValue(unsigned long& ui) const
{
    unsigned long long ul = 0;
    this->getValue(ul);
    // TODO: consider SerializationError on overflow
    ui = static_cast<unsigned long>(ul);
}


void SerializationInfo::getValue(unsigned long long & l) const
{
    if( this->category() != Scalar )
        throw SerializationError("expected integer value");

    switch(_type)
    {
        case Boolean:
            l =  static_cast<unsigned long long>(_value.b);
            break;

        case Int:
            l =  static_cast<unsigned long long>(_value.l);
            break;

        case UInt:
            l =  static_cast<unsigned long long>(_value.ul);
            break;

        case Float:
            l =  static_cast<unsigned long long>(_value.f);
            break;

        case Str:
        {
            const Pt::String* str = reinterpret_cast<const Pt::String*>(_value.str);
            convert(l, *str);
            break;
        }

        default:
            break;
    }
}


void SerializationInfo::setValue(unsigned long long l)
{
    if( category() == Context )
        return;

    this->clearValue();

    _category = Scalar;
    _value.ul = l;
    _type = UInt;
}


void SerializationInfo::getValue(float& f) const
{
    double d = 0.0;
    this->getValue(d);
    // TODO: consider SerializationError on overflow
    f = static_cast<float>(d);
}


void SerializationInfo::getValue( double& value) const
{
    if( this->category() != Scalar )
        throw SerializationError("expected integer value");

    switch(_type)
    {
        case Boolean:
            value = static_cast<double>(_value.b);
            break;

        case Int:
            value = static_cast<double>(_value.l);
            break;

        case UInt:
            value = static_cast<double>(_value.ul);
            break;

        case Float:
            value = _value.f;
            break;

        case Str:
        {
            const Pt::String* str = reinterpret_cast<const Pt::String*>(_value.str);
            convert(value, *str);
            break;
        }

        default:
            break;
    }
}


void SerializationInfo::setValue(double value)
{
    if( category() == Context )
        return;

    this->clearValue();

    _category = Scalar;
    _value.f = value;
    _type = Float;
}


bool SerializationInfo::beginSave(const void* p)
{
    if( ! this->context() || ! this->context()->referencingEnabled() )
        return true;

    if( category() == Context )
    {
        return this->context()->beginSave(p, _name);
    }

    bool first = true;

    if(_parent == 0 || _parent->_bound)
    {
        // all referenced objects are known by the context
        // if this object is referenced, make an id and set _id
        // _id can be written when this type is formatted
        const char* id = _context->makeId(p);
        if(id)
        {
            // the id can be "" or a null-terminated string which means,
            // in either case, the type was saved for the first time.
            _id = id;
        }
        else
        {
            // the id can be 0 if the type has already been saved
            first = false;
        }

        _bound = p;
    }

    return first;
}


void SerializationInfo::finishSave()
{
    if( category() == Context && this->context() && this->context()->referencingEnabled() )
    {
        this->context()->finishSave();
        return;
    }
}


void SerializationInfo::rebind(void* obj) const
{
    _bound = obj;

    if( ! _context )
        return;

    _context->rebindTarget(_id, obj);

    // NOTE: all fixup addresses and child addresses are invalid too
}


void SerializationInfo::rebindFixup(void* obj) const
{
    if( this->category() != Reference )
        throw SerializationError("not a reference");

    const std::string* refId = reinterpret_cast<const std::string*>(_value.ref.refid);
    void* addr = _value.ref.address;

    if(_context)
        _context->rebindFixup( *refId, obj, addr );
}


void SerializationInfo::beginLoad(void* p, const std::type_info& ti) const
{
    if(_context && _context->referencingEnabled() && (_parent == 0 || _parent->_bound) )
    {
        _bound = p;
        _context->beginLoad(p, ti, _name, _id);
    }
}


void SerializationInfo::finishLoad() const
{
    if(_context && _context->referencingEnabled() && _bound)
    {
        _context->finishLoad();
    }
}


SerializationInfo& SerializationInfo::addMember(const std::string& name)
{
    if( category() == Context )
    {
        _name = name;
        return *this;
    }

    if( this->category() != Sequence && this->category() != Struct)
    {
        this->clearValue();

        _value.seq.size = 0;
        _value.seq.first = 0;
        _value.seq.last = 0;
        _type = Unknown;
    }

    _category = Struct;

    SerializationInfo& si = this->addChild();
    si.setName(name);
    return si;
}


void SerializationInfo::removeMember(const std::string& name)
{
    if(this->category() == Struct || this->category() == Sequence)
    {
        SerializationInfo* si = 0;
        SerializationInfo* prev = 0;

        for(SerializationInfo* it = _value.seq.first; it != 0; it = it->sibling())
        {
            if(it->name() == name)
            {
                SerializationInfo* next = it->sibling();
                if( prev )
                {
                    prev->setSibling( next );
                }

                if(it == _value.seq.first)
                {
                    _value.seq.first = next;
                }

                if(it == _value.seq.last)
                {
                    _value.seq.last = prev;
                }

                --_value.seq.size;
                it->setSibling(0);
                si = it;

                if(_context)
                    _context->push(si);
                else
                    delete si;

               break;
            }

            prev = it;
        }
    }
}


SerializationInfo& SerializationInfo::addElement()
{
    if( category() == Context )
    {
        if( _name.size() )
            _name.clear();
        return *this;
    }

   if( this->category() != Sequence && this->category() != Struct)
    {
        this->clearValue();

        _value.seq.size = 0;
        _value.seq.first = 0;
        _value.seq.last = 0;
        _type = Unknown;
    }

    _category = Sequence;

    return this->addChild();
}


SerializationInfo& SerializationInfo::addChild()
{
    SerializationInfo* si = 0;
    if( _context )
    {
        si = _context->get();
    }
    else
    {
        si = new SerializationInfo();
    }

    si->_parent = this;
    si->_next = 0;

    ++_value.seq.size;
    if(_value.seq.first)
    {
        _value.seq.last->setSibling(si);
        _value.seq.last = si;
    }
    else
    {
        _value.seq.first = si;
        _value.seq.last = si;
    }

    return *si;
}

SerializationInfo::Iterator SerializationInfo::begin()
{
    if(this->category() != Struct && this->category() != Sequence)
    {
        return SerializationInfo::Iterator(0);
    }

    return SerializationInfo::Iterator(_value.seq.first);
}


SerializationInfo::ConstIterator SerializationInfo::begin() const
{
    if(this->category() != Struct && this->category() != Sequence)
    {
        return SerializationInfo::ConstIterator(0);
    }

    return SerializationInfo::ConstIterator(_value.seq.first);
}


const SerializationInfo& SerializationInfo::getMember(const std::string& name) const
{
    if(this->category() == Struct || this->category() == Sequence)
    {
        ConstIterator it( _value.seq.first );
        for(; it != ConstIterator( 0 ); ++it)
        {
            if( it->name() == name )
                return *it;
        }
    }

    throw SerializationError("Missing info for '" + name + "'", PT_SOURCEINFO);
}


const SerializationInfo* SerializationInfo::findMember(const std::string& name) const
{
    if(this->category() == Struct || this->category() == Sequence)
    {
        ConstIterator it(_value.seq.first);
        for(; it != ConstIterator( 0 ); ++it)
        {
            if( it->name() == name )
                return &(*it);
        }
    }

    return 0;
}


SerializationInfo* SerializationInfo::findMember(const std::string& name)
{
    if(this->category() == Struct || this->category() == Sequence)
    {
        Iterator it ( _value.seq.first);
        for(; it != Iterator( 0); ++it)
        {
            if( it->name() == name )
                return &(*it);
        }
    }

    return 0;
}


size_t SerializationInfo::memberCount() const
{
    if(this->category() == Struct || this->category() == Sequence)
    {
        return _value.seq.size;
    }

    return 0;
}

} // namespace Pt
