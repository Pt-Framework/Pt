/*
 * Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
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

#include <Pt/SerializationInfo.h>
#include <Pt/Convert.h>
#include <Pt/Utf8Codec.h>
#include <Pt/Formatter.h>
#include <Pt/SerializationContext.h>
#include <Pt/SerializationSurrogate.h>
#include <cstring>
#include <cstddef>
#include <cassert>

namespace {

static const unsigned char TYPENAME_REF_BIT = 32;
static const unsigned char NAME_REF_BIT     = 64;
static const unsigned char ID_REF_BIT       = 128;

inline void freeRefStr2(const char*& str, unsigned char& flags, unsigned char mask)
{
    if( (flags & mask) == mask )
    {
        delete [] str;
        flags &= ~mask;
    }
    
    str = "";
}

inline void setRefStr2(const char*& str, unsigned char& flags, unsigned char mask, const char* from)
{
    assert( from != 0 );
    freeRefStr2(str, flags, mask);
    str = from;
}

inline void copyRefStr2(const char*& str, unsigned char& flags, unsigned char mask, const char* from, std::size_t fromLen)
{
    assert( from != 0 );
    freeRefStr2(str, flags, mask);

    if(fromLen > 0)
    {
        ++fromLen;
        str = new char[fromLen];
        std::memcpy( const_cast<char*>(str), from, fromLen );
        flags |= mask;
    }
}

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


inline void copyRefStr(const char*& str, bool& isRef, const char* from, std::size_t fromLen)
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
    if( this->parent() && this->parent()->type() == Struct )
    {
        formatter.beginMember(_Name);
    }
    else if( this->parent() && this->parent()->type() == DictElement )
    {
        if( this->sibling() )
            formatter.beginDictKey();
        else
            formatter.beginDictValue();
    }
    else if( this->parent() && this->parent()->type() == Sequence )
    {
        formatter.beginElement();
    }

    switch(_type)
    {
        case Boolean:
            formatter.addBool( _Name, _value.b, _id );
            break;

        case Char:
            formatter.addChar( _Name, _value.ui32, _id );
            break;

        case Int8:
            formatter.addInt8( _Name, static_cast<Pt::int8_t>(_value.l), _id );
            break;

        case Int16:
            formatter.addInt16( _Name, static_cast<Pt::int16_t>(_value.l), _id );
            break;

        case Int32:
            formatter.addInt32( _Name, static_cast<Pt::int32_t>(_value.l), _id );
            break;

        case Int64:
            formatter.addInt64( _Name, _value.l, _id );
            break;

        case UInt8:
            formatter.addUInt8( _Name, static_cast<Pt::uint8_t>(_value.ul), _id );
            break;

        case UInt16:
            formatter.addUInt16( _Name, static_cast<Pt::uint16_t>(_value.ul), _id );
            break;

        case UInt32:
            formatter.addUInt32( _Name, static_cast<Pt::uint32_t>(_value.ul), _id );
            break;

        case UInt64:
            formatter.addUInt64( _Name, _value.ul, _id );
            break;

        case Float:
            formatter.addFloat( _Name, static_cast<float>(_value.f), _id );
            break;

        case Double:
            formatter.addDouble( _Name, _value.f, _id );
            break;

        case LongDouble:
            formatter.addLongDouble( _Name, _value.f, _id );
            break;

        case Binary:
        {
            const char* data = 0;
            std::size_t len = 0;

            if(_isAlloc)
            {
                data = _value.blob.data;
                len = _value.blob.length;
            }
            else
            {
                data = reinterpret_cast<const char*>(&_value);
                const char* last = data + sizeof(Variant) - 1;
                len = *last;
            }

            formatter.addBinary( _Name, _TypeName, data, len, _id );
             break;
        }

        case Str:
        {
            formatter.addString( _Name, _TypeName, _value.ustr.str, _id );
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
            formatter.beginStruct( _Name, this->typeName(), this->id() );
            return this->begin();
        }

        case Dict:
        {
            formatter.beginDict( _Name, this->typeName(), this->id() );
            return this->begin();
        }

        case DictElement:
        {
            formatter.beginDictElement();
            return this->begin();
        }

        case Sequence:
        {
            formatter.beginSequence( _Name, this->typeName(), this->id() );
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
        formatter.finishStruct();
    }
    else if(_type == SerializationInfo::Dict)
    {
        formatter.finishDict();
    }
    else if(_type == SerializationInfo::DictElement)
    {
        formatter.finishDictElement();
    }
    else if(_type == Pt::SerializationInfo::Sequence)
    {
        formatter.finishSequence();
    }

    if( this->parent() && this->parent()->type() == Struct )
    {
        formatter.finishMember();
    }
    else if( this->parent() && this->parent()->type() == DictElement )
    {
        if( this->sibling() )
            formatter.finishDictKey();
        else
            formatter.finishDictValue();
    }
    else if( this->parent() && this->parent()->type() == Sequence )
    {
        formatter.finishElement();
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

        case Int8:
            formatter.addInt8( _Name, static_cast<Pt::int8_t>(_value.l), _id );
            break;

        case Int16:
            formatter.addInt16( _Name, static_cast<Pt::int16_t>(_value.l), _id );
            break;

        case Int32:
            formatter.addInt32( _Name, static_cast<Pt::int32_t>(_value.l), _id );
            break;

        case Int64:
            formatter.addInt64( _Name, _value.l, _id );
            break;

        case UInt8:
            formatter.addUInt8( _Name, static_cast<Pt::uint8_t>(_value.ul), _id );
            break;

        case UInt16:
            formatter.addUInt16( _Name, static_cast<Pt::uint16_t>(_value.ul), _id );
            break;

        case UInt32:
            formatter.addUInt32( _Name, static_cast<Pt::uint32_t>(_value.ul), _id );
            break;

        case UInt64:
            formatter.addUInt64( _Name, _value.ul, _id );
            break;

        case Float:
            formatter.addFloat( _Name, static_cast<float>(_value.f), _id );
            break;

        case Double:
            formatter.addDouble( _Name, _value.f, _id );
            break;

        case LongDouble:
            formatter.addLongDouble( _Name, _value.f, _id );
            break;

        case Binary:
        {
            const char* data = 0;
            std::size_t len = 0;

            if(_isAlloc)
            {
                data = _value.blob.data;
                len = _value.blob.length;
            }
            else
            {
                data = reinterpret_cast<const char*>(&_value);
                const char* last = data + sizeof(Variant) - 1;
                len = *last;
            }

            formatter.addBinary( _Name, _TypeName, data, len, _id );
            break;
        }

        case Str:
        {
            formatter.addString( _Name, _TypeName, _value.ustr.str, _id );
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
            formatter.beginStruct( _Name, this->typeName(), this->id() );

            SerializationInfo::Iterator it;
            SerializationInfo::Iterator end = this->end();
            for(it = this->begin(); it != end; ++it)
            {
                formatter.beginMember( it->name() );
                it->format(formatter);
                formatter.finishMember();
            }

            formatter.finishStruct();
            break;
        }

        case Dict:
        {
            formatter.beginDict( _Name, this->typeName(), this->id() );

            SerializationInfo::Iterator it;
            SerializationInfo::Iterator end = this->end();
            for(it = this->begin(); it != end; ++it)
            {
                formatter.beginDictElement();

                SerializationInfo::Iterator kv = it->begin();
                if( kv != it->end() )
                {
                    formatter.beginDictKey();
                    kv->format(formatter);
                    formatter.finishDictKey();
                }
                
                if( ++kv != it->end() )
                {
                    formatter.beginDictValue();
                    kv->format(formatter);
                    formatter.finishDictValue();
                }

                formatter.finishDictElement();
            }

            formatter.finishDict();
            break;
        }

        case Sequence:
        {
            formatter.beginSequence( _Name, this->typeName(), this->id() );

            SerializationInfo::Iterator it;
            SerializationInfo::Iterator end = this->end();
            for(it = this->begin(); it != end; ++it)
            {
                formatter.beginElement();
                it->format(formatter);
                formatter.finishElement();
            }

            formatter.finishSequence();
            break;
        }

        default:
            break;
    }
}


SerializationInfo::~SerializationInfo()
{
    this->clearValue();

    freeRefStr2(_Name, _flags, NAME_REF_BIT);
    freeRefStr2(_TypeName, _flags, TYPENAME_REF_BIT);
    freeRefStr2(_id, _flags, ID_REF_BIT);
}


void SerializationInfo::clear()
{
    this->clearValue();

    freeRefStr2(_Name, _flags, NAME_REF_BIT);
    freeRefStr2(_TypeName, _flags, TYPENAME_REF_BIT);
    freeRefStr2(_id, _flags, ID_REF_BIT);

    _bound = false;
    _isCompound = false;
    _type = Void;
}


void SerializationInfo::clearValue()
{
    switch(_type)
    {
        case Struct:
        case Dict:
        case DictElement:
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
        //case Str8:
        //{
        //    delete [] _value.cstr;
        //    break;
        //}
        case Str:
        {
            delete [] _value.ustr.str;
            break;
        }

        case Binary:
        {
            if(_isAlloc)
            {
                delete [] _value.blob.data;
                _isAlloc = false;
            }
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
    copyRefStr2(_Name, _flags, NAME_REF_BIT, name.c_str(), name.size());
}


void SerializationInfo::setName(const char* name)
{
    const std::size_t len = std::strlen(name);
    copyRefStr2(_Name, _flags, NAME_REF_BIT, name, len);
}


void SerializationInfo::setName(const char* name, std::size_t len)
{
    copyRefStr2(_Name, _flags, NAME_REF_BIT, name, len);
}


void SerializationInfo::setName(const LiteralPtr<char>& name)
{
    setRefStr2(_Name, _flags, NAME_REF_BIT, name.get() );
}



void SerializationInfo::setTypeName(const std::string& type)
{
    copyRefStr2(_TypeName, _flags, TYPENAME_REF_BIT, type.c_str(), type.size());
}


void SerializationInfo::setTypeName(const char* type)
{
    const std::size_t len = std::strlen(type);
    copyRefStr2(_TypeName, _flags, TYPENAME_REF_BIT, type, len);
}


void SerializationInfo::setTypeName(const char* type, std::size_t len)
{
    copyRefStr2(_TypeName, _flags, TYPENAME_REF_BIT, type, len);
}


void SerializationInfo::setTypeName(const LiteralPtr<char>& type)
{
    setRefStr2( _TypeName, _flags, TYPENAME_REF_BIT, type.get() );
}


void SerializationInfo::setId(const std::string& id)
{
    copyRefStr2(_id, _flags, ID_REF_BIT, id.c_str(), id.size());
}


void SerializationInfo::setId(const char* id)
{
    const std::string::size_type len = std::strlen(id);
    copyRefStr2(_id, _flags, ID_REF_BIT, id, len);
}


void SerializationInfo::setId(const char* id, std::size_t len)
{
    copyRefStr2(_id, _flags, ID_REF_BIT, id, len);
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


void SerializationInfo::setDict()
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

    _type = Dict;
}


void SerializationInfo::setContextual(SerializationContext& ctx)
{
    if(_type == Context)
    {
        _context = &ctx;
        return;
    }

    this->clearValue();
    _isCompound = false;
    _type = Context;
    _context = &ctx;
}


// called during serialization, when a reference needs to be unlinked
void SerializationInfo::setReference(const void* ref)
{
    if( _type == Context )
    {
        if( _context && this->context()->isReferencing() )
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
void SerializationInfo::setReference(const char* id, std::size_t idlen)
{
    if(_type != Reference)
    {
        this->clearValue();

        _value.ref.refId = new char[ idlen + 1 ];
        std::memcpy(_value.ref.refId, id, idlen + 1);
        _type = Reference;
        _isCompound = false;
    }
    else
    {
        char* str = new char[ idlen + 1 ];
        delete [] _value.ref.refId;
        _value.ref.refId = str;
        std::memcpy(_value.ref.refId, id, idlen + 1);
    }

    _value.ref.address = 0;
}


const SerializationSurrogate* SerializationInfo::getSurrogate(const std::type_info& ti) const
{
    if(_context) {
        return _context->getSurrogate(ti);
    }

    return 0;
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


void SerializationInfo::setVoid()
{
    if(_type != Void)
    {
        this->clearValue();
        _type = Void;
        _isCompound = false;
    }
}


const char* SerializationInfo::getBinary(std::size_t& length) const
{
    const char* ret = 0;

    if(_type != Binary)
        throw SerializationError("not a binary value");
    
    if(_isAlloc)
    {
        length = _value.blob.length;
        ret = _value.blob.data;
    }
    else
    {
        ret = reinterpret_cast<char*>(&_value);
        const char* last = ret + sizeof(Variant) - 1;
        length = *last;
    }

    return ret;
}


void SerializationInfo::setBinary(const char* data, std::size_t length)
{
    if( _type == Context )
        return;

    if(_type != Void)
    {
        this->clearValue();
        _type = Void;
        _isCompound = false;
    }

    if( length < sizeof(Variant) )
    {
        char* first = reinterpret_cast<char*>(&_value);
        std::memcpy(first, data, length);
        char* last = first + sizeof(Variant) - 1;
        *last = static_cast<Pt::uint8_t>(length);
        _isAlloc = false;
    }
    else
    {
        _value.blob.data = new char[length];
        std::memcpy(_value.blob.data, data, length);
        _value.blob.length = length;
        _isAlloc = true;
    }

    _type = Binary;
    _isCompound = false;
}


//Pt::String SerializationInfo::toString() const
//{
//    Pt::String s;
//
//    switch(_type)
//    {
//        case Str:
//            s.assign(_value.ustr.str, _value.ustr.length);
//            break;
//        
//        case Boolean:
//            convert(s, _value.b);
//            break;
//    
//        case Char:
//            s.clear();
//            s += Pt::Char(_value.ui32);
//            break;
//    
//        case Int8:
//        case Int16:
//        case Int32:
//        case Int64:
//            convert(s, _value.l);
//            break;
//    
//        case UInt8:
//        case UInt16:
//        case UInt32:
//        case UInt64:
//            convert(s, _value.ul);
//            break;
//    
//        case Float:
//        case Double:
//        case LongDouble:
//            convert(s, _value.f);
//            break;
//        
//        default:
//            throw SerializationError("expected scalar value");
//    }
//
//    return s;
//}


void SerializationInfo::getString(std::string& s, const TextCodec<Pt::Char, char>& codec) const
{
    switch(_type)
    {
        case Str:
        {
            char to[16];
            MBState state;
            std::codecvt_base::result r;
            const Pt::Char* from = _value.ustr.str;
            std::size_t size = _value.ustr.length;
            s.clear();

            do 
            {
                const Pt::Char* from_next;
                char* to_next = to;
                r = codec.out(state, from, from + size, from_next, to, to + sizeof(to), to_next);

                if (r == std::codecvt_base::error)
                    throw ConversionError("character conversion failed");

                s.append(to, to_next);

                size -= (from_next - from);
                from = from_next;
            } 
            while (r == std::codecvt_base::partial);

            break;
        }
        
        //case Str8:
        //    s = _value.cstr;
        //    break;
        //
        //case Boolean:
        //    convert(s, _value.b);
        //    break;

        //case Char:
        //case Char8:
        //    s.clear();
        //    s += char(int(_value.ui32));
        //    break;

        //case Int8:
        //case Int16:
        //case Int32:
        //case Int64:
        //    convert(s, _value.l);
        //    break;

        //case UInt8:
        //case UInt16:
        //case UInt32:
        //case UInt64:
        //    convert(s, _value.ul);
        //    break;

        //case Float:
        //case Double:
        //case LongDouble:
        //    convert(s, _value.f);
        //    break;
        
        default:
            throw SerializationError("not a string value");
    }
}


void SerializationInfo::getString(std::string& s) const
{
    Utf8Codec codec;
    getString( s, codec );
    
    //switch(_type)
    //{
    //    case Str:
    //        s = Pt::String(_value.ustr.str).narrow();
    //        break;
    //    
    //    case Str8:
    //        s = _value.cstr;
    //        break;
    //    
    //    case Boolean:
    //        convert(s, _value.b);
    //        break;

    //    case Char:
    //    case Char8:
    //        s.clear();
    //        s += char(int(_value.ui32));
    //        break;

    //    case Int8:
    //    case Int16:
    //    case Int32:
    //    case Int64:
    //        convert(s, _value.l);
    //        break;

    //    case UInt8:
    //    case UInt16:
    //    case UInt32:
    //    case UInt64:
    //        convert(s, _value.ul);
    //        break;

    //    case Float:
    //    case Double:
    //    case LongDouble:
    //        convert(s, _value.f);
    //        break;
    //    
    //    default:
    //        throw SerializationError("not a string value");
    //}
}


void SerializationInfo::getString(Pt::String& s) const
{
    switch(_type)
    {
        case Str:
            s.assign(_value.ustr.str, _value.ustr.length);
            break;
        
        //case Str8:
        //    s = Pt::String::widen(_value.cstr);
        //    break;
        //
        //case Boolean:
        //    convert(s, _value.b);
        //    break;
    
        //case Char:
        //case Char8:
        //    s.clear();
        //    s += Pt::Char(_value.ui32);
        //    break;
    
        //case Int8:
        //case Int16:
        //case Int32:
        //case Int64:
        //    convert(s, _value.l);
        //    break;
    
        //case UInt8:
        //case UInt16:
        //case UInt32:
        //case UInt64:
        //    convert(s, _value.ul);
        //    break;
    
        //case Float:
        //case Double:
        //case LongDouble:
        //    convert(s, _value.f);
        //    break;
        
        default:
            throw SerializationError("not a string value");
    }
}


void SerializationInfo::setString(const char* value, std::size_t size, const TextCodec<Pt::Char, char>& codec)
{
    if( _type == Context )
        return;

    if(_type != Void)
    {
        this->clearValue();
        _type = Void;
        _isCompound = false;
    }

    Pt::String s;
    const unsigned toSize = 16;
    Pt::Char to[toSize];
    MBState state;
    std::codecvt_base::result r;
    const char* from = value;

    do 
    {
        const char* from_next;
        Pt::Char* to_next = to;
        r = codec.in(state, from, from + size, from_next, to, to + toSize, to_next);

        if (r == std::codecvt_base::error)
            throw ConversionError("character conversion failed");

        s.append(to, to_next);

        size -= (from_next - from);
        from = from_next;
    } 
    while (r == std::codecvt_base::partial);

    std::size_t len = s.length();
    std::size_t vsize = len + 1;
    _value.ustr.str = new Pt::Char[vsize];
    _value.ustr.length = len;
    std::char_traits<Pt::Char>::copy(_value.ustr.str, s.c_str(), vsize);

    _isCompound = false;
    _type = Str;
}


void SerializationInfo::setString(const char* s)
{
    // TODO: use codec from SerializationContext
    Utf8Codec codec;
    setString( s, std::strlen(s), codec );

    //if( _type == Context )
    //    return;

    //if(_type != Void)
    //{
    //    this->clearValue();
    //    _type = Void;
    //    _isCompound = false;
    //}

    //std::size_t length = std::strlen(s);
    //_value.cstr = new char[length+1];
    //std::memcpy(_value.cstr, s, length);
    //_value.cstr[length] = 0;

    //_type = Str8;
    //_isCompound = false;
}


void SerializationInfo::setString(const std::string& s)
{
    // TODO: use codec from SerializationContext
    Utf8Codec codec;
    setString( s.c_str(), s.size(), codec );

    //if( _type == Context )
    //    return;

    //if(_type != Void)
    //{
    //    this->clearValue();
    //    _type = Void;
    //    _isCompound = false;
    //}

    //std::size_t length = s.size();
    //_value.cstr = new char[length+1];
    //std::memcpy(_value.cstr, s.c_str(), length);
    //_value.cstr[length] = 0;

    //_type = Str8;
    //_isCompound = false;
}


void SerializationInfo::setString(const Pt::Char* value, std::size_t len)
{
    if( _type == Context )
        return;

    if(_type != Void)
    {
        this->clearValue();
        _type = Void;
        _isCompound = false;
    }

    std::size_t size = len + 1;
    _value.ustr.str = new Pt::Char[size];
    _value.ustr.length = len;
    std::char_traits<Pt::Char>::copy(_value.ustr.str, value, size);

    _isCompound = false;
    _type = Str;
}


void SerializationInfo::getChar(char c) const
{
    Pt::Char ch;
    this->getChar(ch);
    // TODO: consider SerializationError on overflow
    c = static_cast<char>( ch.value() );
}


void SerializationInfo::setChar(char c)
{
    if( _type == Context )
        return;

    this->clearValue();

    _isCompound = false;
    _value.ui32 = c;
    _type = Char;
}


void SerializationInfo::getChar(Pt::Char& c) const
{
    switch(_type)
    {
        case Char:
        //case Char8:
            c = _value.ui32;
            break;

        case Str:
        {
            if( _value.ustr.length != 1 )
                throw SerializationError("expected character value");
    
            c = _value.ustr.str[0];
            break;
        }
    
        //case Str8:
        //{
        //    if( _value.cstr[0] == '\0' )
        //        throw SerializationError("expected character value");
    
        //    c = _value.cstr[0];
        //    break;
        //}

        default:
            throw SerializationError("expected character value");
    }
}


void SerializationInfo::setChar(const Pt::Char& c)
{
    if( _type == Context )
        return;

    this->clearValue();

    _isCompound = false;
    _value.ui32 = c.value();
    _type = Char;
}


void SerializationInfo::getBool(bool& value) const
{
    switch(_type)
    {
        case Boolean:
            value = _value.b;
            break;

        case Int8:
        case Int16:
        case Int32:
        case Int64:
            value = 0 != _value.l;
            break;

        case UInt8:
        case UInt16:
        case UInt32:
        case UInt64:
            value = 0 != _value.ul;
            break;

        case Float:
            value = 0 != _value.f;
            break;

        case Double:
            value = 0 != _value.f;
            break;

        case LongDouble:
            value = 0 != _value.f;
            break;

        /*case Str:
        {
            const Pt::String* str = reinterpret_cast<const Pt::String*>(_value.str);
            convert(value, *str);
            break;
        }
        case Str8:
        {
            convert(value, _value.cstr);
            break;
        }*/

        default:
            throw SerializationError("expected bool value");
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


void SerializationInfo::getInt8(Pt::int8_t& n) const
{
    Pt::int64_t l = 0;
    this->getInt64(l);

    if( l > std::numeric_limits<Pt::int8_t>::max() )
        throw SerializationError("expected int8 value");

    n = static_cast<Pt::int8_t>(l);
}


void SerializationInfo::setInt8(Pt::int8_t n)
{
    if( _type == Context )
        return;

    if(_type != Void)
        this->clearValue();

    _isCompound = false;
    _value.l = n;
    _type = Int8; 
}


void SerializationInfo::getInt16(Pt::int16_t& n) const
{
    Pt::int64_t l = 0;
    this->getInt64(l);

    if( l > std::numeric_limits<Pt::int16_t>::max() )
        throw SerializationError("expected int16 value");

    n = static_cast<Pt::int16_t>(l);
}


void SerializationInfo::setInt16(Pt::int16_t n)
{
    if( _type == Context )
        return;

    if(_type != Void)
        this->clearValue();

    _isCompound = false;
    _value.l = n;
     _type = Int16; 
}


void SerializationInfo::getInt32(Pt::int32_t& i) const
{
    Pt::int64_t l = 0;
    this->getInt64(l);

    if( l > std::numeric_limits<Pt::int32_t>::max() )
        throw SerializationError("expected int32 value");

    i = static_cast<Pt::int32_t>(l);
}


void SerializationInfo::setInt32(Pt::int32_t n)
{
    if( _type == Context )
        return;

    if(_type != Void)
        this->clearValue();

    _isCompound = false;
    _value.l = n;
    _type = Int32; 
}


void SerializationInfo::getInt64(Pt::int64_t& l) const
{
    switch(_type)
    {
        case Boolean:
            l =  static_cast<Pt::int64_t>(_value.b);
            break;

        case Int8:
        case Int16:
        case Int32:
        case Int64:
            l =  static_cast<Pt::int64_t>(_value.l);
            break;

        case UInt8:
        case UInt16:
        case UInt32:
        case UInt64:
            l =  static_cast<Pt::int64_t>(_value.ul);
            break;

        case Float:
        case Double:
        case LongDouble:
            l =  static_cast<Pt::int64_t>(_value.f);
            break;

        /*case Str:
        {
            const Pt::String* str = reinterpret_cast<const Pt::String*>(_value.str);
            convert(l, *str);
            break;
        }

        case Str8:
            convert(l, _value.cstr);
            break;*/

        default:
            throw SerializationError("expected int value");
    }
}


void SerializationInfo::setInt64(Pt::int64_t l)
{
    if( _type == Context )
        return;

    if(_type != Void)
        this->clearValue();

    _isCompound = false;
    _value.l = l;
    _type = Int64;
}


void SerializationInfo::getUInt8(Pt::uint8_t& n) const
{
    Pt::uint64_t l = 0;
    this->getUInt64(l);

    if( l > std::numeric_limits<Pt::uint8_t>::max() )
        throw SerializationError("expected uint8 value");

    n = static_cast<Pt::uint8_t>(l);
}


void SerializationInfo::setUInt8(Pt::uint8_t n)
{
    if( _type == Context )
        return;

    this->clearValue();

    _isCompound = false;
    _value.ul = n;
    _type = UInt8; 
}


void SerializationInfo::getUInt16(Pt::uint16_t& n) const
{
    Pt::uint64_t l = 0;
    this->getUInt64(l);

    if( l > std::numeric_limits<Pt::uint16_t>::max() )
        throw SerializationError("expected uint16 value");

    n = static_cast<Pt::uint16_t>(l);
}


void SerializationInfo::setUInt16(Pt::uint16_t n)
{
    if( _type == Context )
        return;

    this->clearValue();

    _isCompound = false;
    _value.ul = n;
     _type = UInt16; 
}


void SerializationInfo::getUInt32(Pt::uint32_t& n) const
{
    Pt::uint64_t l = 0;
    this->getUInt64(l);

    if( l > std::numeric_limits<Pt::uint32_t>::max() )
        throw SerializationError("expected uint32 value");

    n = static_cast<Pt::uint32_t>(l);
}


void SerializationInfo::setUInt32(Pt::uint32_t n)
{
    if( _type == Context )
        return;

    this->clearValue();

    _isCompound = false;
    _value.ul = n;
    _type = UInt32; 
}


void SerializationInfo::getUInt64(Pt::uint64_t& l) const
{
    switch(_type)
    {
        case Boolean:
            l =  static_cast<Pt::uint64_t>(_value.b);
            break;

        case Int8:
        case Int16:
        case Int32:
        case Int64:
            l =  static_cast<Pt::uint64_t>(_value.l);
            break;

        case UInt8:
        case UInt16:
        case UInt32:
        case UInt64:
            l =  static_cast<Pt::uint64_t>(_value.ul);
            break;

        case Float:
        case Double:
        case LongDouble:
            l =  static_cast<Pt::uint64_t>(_value.f);
            break;

        /*case Str:
        {
            const Pt::String* str = reinterpret_cast<const Pt::String*>(_value.str);
            convert(l, *str);
            break;
        }
        case Str8:
            convert(l, _value.cstr);
            break;*/

        default:
            throw SerializationError("expected unsigned int value");
    }
}


void SerializationInfo::setUInt64(Pt::uint64_t l)
{
    if( _type == Context )
        return;

    this->clearValue();

    _isCompound = false;
    _value.ul = l;
    _type = UInt64;
}


void SerializationInfo::getFloat(float& f) const
{
    long double d = 0.0;
    this->getLongDouble(d);

    if( d > std::numeric_limits<float>::max() )
        throw SerializationError("expected float value");

    f = static_cast<float>(d);
}


void SerializationInfo::setFloat(float f)
{
    if( _type == Context )
        return;

    this->setLongDouble(f); 
    _type = Float;
}


void SerializationInfo::getDouble(double& f) const
{
    long double d = 0.0;
    this->getLongDouble(d);

    if( d > std::numeric_limits<double>::max() )
        throw SerializationError("expected double value");

    f = static_cast<double>(d);
}


void SerializationInfo::setDouble(double f)
{
    if( _type == Context )
        return;

    this->setLongDouble(f); 
    _type = Double;
}


void SerializationInfo::getLongDouble(long double& value) const
{
    switch(_type)
    {
        case Boolean:
            value = static_cast<double>(_value.b);
            break;

        case Int8:
        case Int16:
        case Int32:
        case Int64:
            value = static_cast<double>(_value.l);
            break;

        case UInt8:
        case UInt16:
        case UInt32:
        case UInt64:
            value = static_cast<double>(_value.ul);
            break;

        case Float:
        case Double:
        case LongDouble:
            value = _value.f;
            break;
        
        /*case Str:
        {
            const Pt::String* str = reinterpret_cast<const Pt::String*>(_value.str);
            convert(value, *str);
            break;
        }
        
        case Str8:
            convert(value, _value.cstr);
            break;*/

        default:
            throw SerializationError("expected float value");
    }
}


void SerializationInfo::setLongDouble(long double value)
{
    if( _type == Context )
        return;

    this->clearValue();

    _isCompound = false;
    _value.f = value;
    _type = LongDouble;
}


bool SerializationInfo::beginSave(const void* p)
{
    if( ! this->context() || ! this->context()->isReferencing() )
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
            setRefStr2(_id, _flags, ID_REF_BIT, id);
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
    if( _type == Context && this->context() && this->context()->isReferencing() )
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
    if(_context && _context->isReferencing() && (_parent == 0 || _parent->_bound) )
    {
        _bound = true;
        _context->beginLoad(p, ti, _Name, _id);
    }
}


void SerializationInfo::finishLoad() const
{
    if(_context && _context->isReferencing() && _bound)
    {
        _context->finishLoad();
    }
}


SerializationInfo& SerializationInfo::addMember(const char* name, std::size_t len)
{
    if( _type == Context )
    {
        this->setName(name, len);
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
    si.setName(name, len);
    return si;
}


SerializationInfo& SerializationInfo::addMember(const LiteralPtr<char>& name)
{
    if( _type == Context )
    {
        this->setName(name);
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
    si.setName(name);
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
        freeRefStr2(_Name, _flags, NAME_REF_BIT);
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


SerializationInfo& SerializationInfo::addDictElement()
{
    if( _type == Context )
    {
        freeRefStr2(_Name, _flags, NAME_REF_BIT);
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

    _type = Dict;

    return this->addChild();
}


SerializationInfo& SerializationInfo::addDictKey()
{
    if( _type == Context )
    {
        freeRefStr2(_Name, _flags, NAME_REF_BIT);
        return *this;
    }

    if( ! _isCompound )
    {
        clearValue();

        _value.seq.size = 0;
        _value.seq.first = 0;
        _value.seq.last = 0;
        _isCompound = true;
    }

    _type = DictElement;

    return this->addChild();
}


SerializationInfo& SerializationInfo::addDictValue()
{
    if( _type == Context )
    {
        freeRefStr2(_Name, _flags, NAME_REF_BIT);
        return *this;
    }

    if( ! _isCompound )
    {
        clearValue();

        _value.seq.size = 0;
        _value.seq.first = 0;
        _value.seq.last = 0;
        _isCompound = true;
    }

    _type = DictElement;

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

    throw SerializationError("Missing info for '" + std::string(name) + "'");
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


std::size_t SerializationInfo::memberCount() const
{
    if( _isCompound )
    {
        return _value.seq.size;
    }

    return 0;
}

} // namespace Pt
