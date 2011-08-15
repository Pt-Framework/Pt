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
#include <cstring>
#include <cassert>

namespace {

inline void freeRefStr(const char*& str, bool& isRef)
{
    if(isRef == false)
    {
        delete [] str;
        isRef = true;
    }
    
    str = "";
}


inline void setRefStr(const char*& str, bool& isRef, const char* from)
{
    assert( from != 0 );
    freeRefStr(str, isRef);
    str = from;
}


inline void copyRefStr(const char*& str, bool& isRef, const char* from, size_t fromLen)
{
    assert( from != 0 );
    freeRefStr(str, isRef);

	if(fromLen > 0)
	{
		++fromLen;
		str = new char[fromLen];
		std::memcpy( const_cast<char*>(str), from, fromLen );
		isRef = false;
    }
}

}

namespace Pt {

SerializationInfo::Iterator SerializationInfo::beginFormat(Formatter& formatter)
{
    switch(_type)
    {
        case Boolean:
            formatter.addBool( _Name, _value.b, _id );
            break;

        case Char:
            formatter.addChar( _Name, _value.ui32, _id );
            break;

        case Int:
            formatter.addInt( _Name, _value.l, _id );
            break;

        case UInt:
            formatter.addUInt( _Name, _value.ul, _id );
            break;

        case Float:
            formatter.addFloat( _Name, _value.f, _id );
            break;

        case Blob:
            formatter.addBytes( _Name, _TypeName, _value.blob.data, _value.blob.length, _id );
            break;

        case Binary:
        {
            const char* data = reinterpret_cast<const char*>(&_value);
            const char* last = data + sizeof(Variant) - 1;
            formatter.addBytes( _Name, _TypeName, data, *last, _id );
            break;
        }
        case Str:
        {
            const Pt::String* str = reinterpret_cast<const Pt::String*>(_value.str);
            formatter.addString( _Name, _TypeName, *str, _id );
            break;
        }
        case Reference:
        {
            if( ! _context )
                throw SerializationError("context not available");

            const void* refAddr = _value.ref.address;
            const char* id = _context->getId( refAddr );
            formatter.addReference( _Name, id);
            break;
        }

        case Struct:
        {
            formatter.beginObject( _Name, this->typeName(), this->id() );
            return this->begin();
        }

        case Sequence:
        {
            formatter.beginArray( _Name, this->typeName(), this->id() );
            return this->begin();
        }

        default:
            break;
    }

    return end();
}


void SerializationInfo::endFormat(Formatter& formatter)
{
    if(_type == SerializationInfo::Struct)
    {
        formatter.finishObject();
    }
    else if(_type == Pt::SerializationInfo::Sequence)
    {
        formatter.finishArray();
    }
}


void SerializationInfo::format(Formatter& formatter)
{
    switch(_type)
    {
        case Boolean:
            formatter.addBool( _Name, _value.b, _id );
            break;

        case Char:
            formatter.addChar( _Name, _value.ui32, _id );
            break;

        case Int:
            formatter.addInt( _Name, _value.l, _id );
            break;

        case UInt:
            formatter.addUInt( _Name, _value.ul, _id );
            break;

        case Float:
            formatter.addFloat( _Name, _value.f, _id );
            break;

        case Blob:
            formatter.addBytes( _Name, _TypeName, _value.blob.data, _value.blob.length, _id );
            break;

        case Binary:
        {
            const char* data = reinterpret_cast<const char*>(&_value);
            const char* last = data + sizeof(Variant) - 1;
            formatter.addBytes( _Name, _TypeName, data, *last, _id );
            break;
        }
        case Str:
        {
            const Pt::String* str = reinterpret_cast<const Pt::String*>(_value.str);
            formatter.addString( _Name, _TypeName, *str, _id );
            break;
        }

        case Reference:
        {
            if( ! _context )
                throw SerializationError("context not available");

            const void* refAddr = _value.ref.address;
            const char* id = _context->getId( refAddr );
            formatter.addReference( _Name, id);
            break;
        }

        case Struct:
        {
            formatter.beginObject( _Name, this->typeName(), this->id() );

            SerializationInfo::Iterator it;
            SerializationInfo::Iterator end = this->end();
            for(it = this->begin(); it != end; ++it)
            {
                formatter.beginMember( it->name(), it->typeName(), it->id() );
                it->format(formatter);
                formatter.finishMember();
            }

            formatter.finishObject();
            break;
        }

        case Sequence:
        {
            formatter.beginArray( _Name, this->typeName(), this->id() );

            SerializationInfo::Iterator it;
            SerializationInfo::Iterator end = this->end();
            for(it = this->begin(); it != end; ++it)
            {
                formatter.beginElement( it->typeName(), it->id()  );
                it->format(formatter);
                formatter.finishElement();
            }

            formatter.finishArray();
            break;
        }

        default:
            break;
    }
}


SerializationInfo::~SerializationInfo()
{
    this->clearValue();

    freeRefStr(_Name, _nameRef);
    freeRefStr(_TypeName, _tnRef);
    freeRefStr(_id, _idRef);
}


void SerializationInfo::clear()
{
    this->clearValue();

    freeRefStr(_Name, _nameRef);
    freeRefStr(_TypeName, _tnRef);
    freeRefStr(_id, _idRef);

    _bound = false;
    _isCompound = false;
    _type = Void;
}


void SerializationInfo::clearValue()
{
    switch(_type)
    {
        case Struct:
        case Sequence:
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
            break;
        }

        case Str:
        {
            Pt::String* str = reinterpret_cast<Pt::String*>(_value.str);
            str->~basic_string();
            break;
        }

        case Blob:
        {
            delete [] _value.blob.data;
            break;
        }

        case Reference:
        {
			delete [] _value.ref.refId;
            break;
        }
    }
}


void SerializationInfo::setName(const std::string& name)
{
    copyRefStr(_Name, _nameRef, name.c_str(), name.size());
}


void SerializationInfo::setName(const char* name, bool copy)
{
	if(copy)
	{
		const std::size_t len = std::strlen(name);
		copyRefStr(_Name, _nameRef, name, len);
	}
	else
		setRefStr(_Name, _nameRef, name);
}


void SerializationInfo::setTypeName(const std::string& type)
{
    copyRefStr(_TypeName, _tnRef, type.c_str(), type.size());
}


void SerializationInfo::setTypeName(const char* type)
{
    const std::size_t len = std::strlen(type);
    copyRefStr(_TypeName, _tnRef, type, len);
}


void SerializationInfo::setTypeName(const LiteralPtr<char>& type)
{
    setRefStr( _TypeName, _tnRef, type.get() );
}


void SerializationInfo::setId(const std::string& id)
{
    copyRefStr(_id, _idRef, id.c_str(), id.size());
}


void SerializationInfo::setId(const char* id)
{
    const std::string::size_type len = std::strlen(id);
    copyRefStr(_id, _idRef, id, len);
}


void SerializationInfo::setSequence()
{
    if(_type == SerializationInfo::Context)
        return;

    if( ! _isCompound )
    {
        this->clearValue();

        _value.seq.first = 0;
        _value.seq.last = 0;
        _value.seq.size = 0;

        _isCompound = true;
    }


    _type = Sequence;
}


void SerializationInfo::setContextual()
{
    if(_type == SerializationInfo::Context)
        return;

    this->clearValue();

    _isCompound = false;
    _type = Context;
}


// called during serialization, when a reference needs to be unlinked
void SerializationInfo::setReference(const void* ref)
{
    if( _type == Context )
    {
        if( _context && this->context()->referencingEnabled() )
            this->context()->prepareId(ref);

        return;
    }

    if(_type != Reference)
    {
        this->clearValue();

		_value.ref.refId = new char[1];
		_value.ref.refId[0] = '\0';
        _type = Reference;
        _isCompound = false;
    }

    _value.ref.address = const_cast<void*>(ref) ;
}


// called during deserialization, when a reference id was parsed
void SerializationInfo::setReference(const std::string& id)
{
    if(_type != Reference)
    {
        this->clearValue();

		_value.ref.refId = new char[ id.size() + 1 ];
		std::memcpy(_value.ref.refId, id.c_str(), id.size() + 1);
        _type = Reference;
        _isCompound = false;
    }
    else
    {
		char* str = new char[ id.size() + 1 ];
		delete [] _value.ref.refId;
		_value.ref.refId = str;
		std::memcpy(_value.ref.refId, id.c_str(), id.size() + 1);
    }

    _value.ref.address = 0;
}


// called during deserialization, when a reference needs to be fixed up
void SerializationInfo::load(void* type, FixupInfo::FixupHandler fh, unsigned m) const
{
    if( _type != Reference)
        throw SerializationError("not a reference");

    const char* refId = _value.ref.refId;
    _value.ref.address = type;

    if(_context)
    {
        _context->prepareFixup(type, refId, fh, m);
    }
}


const char* SerializationInfo::getBinary(size_t& length) const
{
    const char* ret = 0;

    if(_type == Binary)
    {
        ret = reinterpret_cast<char*>(&_value);
        const char* last = ret + sizeof(Variant) - 1;
        length = *last;
    }
    else if(_type == Blob)
    {
        length = _value.blob.length;
        ret = _value.blob.data;
    }
    else
    {
        throw SerializationError("not a binary value");
    }

    return ret;
}


void SerializationInfo::setBinary(const char* data, size_t length)
{
    if( _type == Context )
        return;

    if(_type != Void)
        this->clearValue();

    if( length < sizeof(Variant) )
    {
        char* first = reinterpret_cast<char*>(&_value);
        std::memcpy(first, data, length);
        char* last = first + sizeof(Variant) - 1;
        *last = static_cast<Pt::uint8_t>(length);
        _type = Binary;
    }
    else
    {
        _value.blob.data = new char[length];
        std::memcpy(_value.blob.data, data, length);
        _value.blob.length = length;
        _type = Blob;
    }

    _isCompound = false;
}


void SerializationInfo::getString(Pt::String& s) const
{
    if(_type == Str)
    {
        const Pt::String* str = reinterpret_cast<const Pt::String*>(_value.str);
        s = *str;
    }
    else if(_type ==  Boolean)
    {
        convert(s, _value.b);
    }
    else if(_type ==  Char)
    {
        s += Pt::Char(_value.ui32);
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
    else
        throw SerializationError("not a string value");
}


void SerializationInfo::setString(const Pt::String& value)
{
    if( _type == Context )
        return;

    if(_type == Void)
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

    _isCompound = false;
    _type = Str;
}


void SerializationInfo::getValue(char& c) const
{
    Pt::Char ch;
    this->getValue(ch);
	c = static_cast<int>(ch);
}


void SerializationInfo::setValue(char c)
{
	Pt::Char ch(c);
	this->setValue(ch);
}


void SerializationInfo::getValue(Pt::Char& c) const
{
    switch(_type)
    {
        case Char:
            c = _value.ui32;
            break;

        case Str:
        {
            const Pt::String* str = reinterpret_cast<const Pt::String*>(_value.str);
            if( str->size() != 1 )
				throw SerializationError("expected character value");
				
			c = (*str)[0];
            break;
        }

        default:
            throw SerializationError("expected character value");
    }
}


void SerializationInfo::setValue(const Pt::Char& c)
{
    if( _type == Context )
        return;

    this->clearValue();

    _isCompound = false;
    _value.ui32 = c;
    _type = Char;
}


void SerializationInfo::getBool(bool& value) const
{
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
            throw SerializationError("expected integer value");
    }
}


void SerializationInfo::setBool(bool value)
{
    if( _type == Context )
        return;

    this->clearValue();

    _isCompound = false;
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
            throw SerializationError("expected scalar value");
    }
}


void SerializationInfo::setValue(long long l)
{
    if( _type == Context )
        return;

    if(_type != Void)
        this->clearValue();

    _isCompound = false;
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
            throw SerializationError("expected integer value");
    }
}


void SerializationInfo::setValue(unsigned long long l)
{
    if( _type == Context )
        return;

    this->clearValue();

    _isCompound = false;
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
            throw SerializationError("expected integer value");
    }
}


void SerializationInfo::setValue(double value)
{
    if( _type == Context )
        return;

    this->clearValue();

    _isCompound = false;
    _value.f = value;
    _type = Float;
}


bool SerializationInfo::beginSave(const void* p)
{
    if( ! this->context() || ! this->context()->referencingEnabled() )
        return true;

    if( _type == Context )
    {
        return this->context()->beginSave(p, _Name);
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
            // we point to a string owned by the context. The lifetime of this
            // SerializationInfo is coupled to the lifetime of the context.
            // The id can be "" or a null-terminated string which means,
            // in either case, the type was saved for the first time.      
            setRefStr(_id, _idRef, id);
        }
        else
        {
            // the id can be 0 if the type has already been saved
            first = false;
        }

        _bound = true;
    }

    return first;
}


void SerializationInfo::finishSave()
{
    if( _type == Context && this->context() && this->context()->referencingEnabled() )
    {
        this->context()->finishSave();
        return;
    }
}


void SerializationInfo::rebind(void* obj) const
{
    _bound = true;

    if( ! _context )
        return;

    _context->rebindTarget(_id, obj);

    // NOTE: all fixup addresses and child addresses are invalid too
}


void SerializationInfo::rebindFixup(void* obj) const
{
    if( _type != Reference )
        throw SerializationError("not a reference");

    const char* refId = _value.ref.refId;
    void* addr = _value.ref.address;

    if(_context)
        _context->rebindFixup( refId, obj, addr );
}


void SerializationInfo::beginLoad(void* p, const std::type_info& ti) const
{
    if(_context && _context->referencingEnabled() && (_parent == 0 || _parent->_bound) )
    {
        _bound = true;
        _context->beginLoad(p, ti, _Name, _id);
    }
}


void SerializationInfo::finishLoad() const
{
    if(_context && _context->referencingEnabled() && _bound)
    {
        _context->finishLoad();
    }
}


SerializationInfo& SerializationInfo::addMember(const char* name, bool copy)
{
    if( _type == Context )
    {
        this->setName(name, copy);
        return *this;
    }

    if( ! _isCompound )
    {
        this->clearValue();

        _value.seq.size = 0;
        _value.seq.first = 0;
        _value.seq.last = 0;
        _isCompound = true;
    }

    _type = Struct;

    SerializationInfo& si = this->addChild();
    si.setName(name, copy);
    return si;
}


void SerializationInfo::removeMember(const char* name)
{
    if( _isCompound )
    {
        SerializationInfo* si = 0;
        SerializationInfo* prev = 0;

        for(SerializationInfo* it = _value.seq.first; it != 0; it = it->sibling())
        {
            if( 0 == std::strcmp(name, it->name()) )
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
    if( _type == Context )
    {
        freeRefStr(_Name, _nameRef);
        return *this;
    }

   if( ! _isCompound )
    {
        this->clearValue();

        _value.seq.size = 0;
        _value.seq.first = 0;
        _value.seq.last = 0;
        _isCompound = true;
    }

    _type = Sequence;

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
    if( ! _isCompound )
    {
        return SerializationInfo::Iterator(0);
    }

    return SerializationInfo::Iterator(_value.seq.first);
}


SerializationInfo::ConstIterator SerializationInfo::begin() const
{
    if( ! _isCompound )
    {
        return SerializationInfo::ConstIterator(0);
    }

    return SerializationInfo::ConstIterator(_value.seq.first);
}


const SerializationInfo& SerializationInfo::getMember(const char* name) const
{
    if( _isCompound )
    {
        ConstIterator it( _value.seq.first );
        for(; it != ConstIterator( 0 ); ++it)
        {
            if( 0 == std::strcmp(name, it->name()) )
                return *it;
        }
    }

    throw SerializationError("Missing info for '" + std::string(name) + "'", PT_SOURCEINFO);
}


const SerializationInfo* SerializationInfo::findMember(const char* name) const
{
    if( _isCompound )
    {
        ConstIterator it(_value.seq.first);
        for(; it != ConstIterator( 0 ); ++it)
        {
            if( 0 == std::strcmp(name, it->name()) )
                return &(*it);
        }
    }

    return 0;
}


SerializationInfo* SerializationInfo::findMember(const char* name)
{
    if( _isCompound )
    {
        Iterator it ( _value.seq.first);
        for(; it != Iterator( 0); ++it)
        {
            if( 0 == std::strcmp(name, it->name()) )
                return &(*it);
        }
    }

    return 0;
}


size_t SerializationInfo::memberCount() const
{
    if( _isCompound )
    {
        return _value.seq.size;
    }

    return 0;
}

} // namespace Pt
