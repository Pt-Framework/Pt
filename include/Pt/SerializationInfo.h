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

#ifndef Pt_SerializationInfo_h
#define Pt_SerializationInfo_h

#include <Pt/Api.h>
#include <Pt/String.h>
#include <Pt/Types.h>
#include <Pt/LiteralPtr.h>
#include <Pt/FixupInfo.h>
#include <Pt/TextCodec.h>
#include <Pt/SerializationError.h>
#include <Pt/SerializationSurrogate.h>
#include <typeinfo>
#include <limits>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <deque>
#include <cstring>

namespace Pt {

class SerializationContext;
class Formatter;

/** @brief Represents arbitrary types during serialization.

    @ingroup Serialization
*/
class PT_API SerializationInfo
{
    public:
        class Iterator;
        class ConstIterator;

        //! @brief Type identifier.
        enum Type {
            Void       = 0,  //!< Void
            Context    = 1,  //!< Contextual
            Reference  = 2,  //!< Reference
            Boolean    = 3,  //!< Boolean
            Char       = 4,  //!< Char
            Str        = 5,  //!< Str
            Int8       = 6,  //!< 8-bit integer
            Int16      = 7,  //!< 16-bit integer
            Int32      = 8,  //!< 32-bit integer
            Int64      = 9,  //!< 64-bit integer
            UInt8      = 10,  //!< 8-bit unsigned integer
            UInt16     = 11,  //!< 16-bit unsigned integer
            UInt32     = 12,  //!< 32-bit unsigned integer
            UInt64     = 13,  //!< 64-bit unsigned integer
            Float      = 14,  //!< Floating point
            Double     = 15,  //!< Double floating point
            LongDouble = 16,  //!< Long double floating point
            Binary     = 17,  //!< Binary
            Struct     = 18,  //!< Structural compound type
            Sequence   = 19,  //!< Sequence compound type
            Dict       = 20,  //!< Dictionary compound type
            DictElement= 21   //!< Dictionary element
        };

    public:
        /** @brief Default constructor.
        */
        SerializationInfo()
        : _context(0)
        , _parent(0)
        , _next(0)
        , _Name("")
        , _TypeName("")
        , _id("")
        , _bound(false)
        , _isCompound(false)
        , _isAlloc(false)
        , _type(Void)
        , _flags(0)
        { }

        /** @brief Construct with context.
        */
        explicit SerializationInfo(SerializationContext* context)
        : _context(context)
        , _parent(0)
        , _next(0)
        , _Name("")
        , _TypeName("")
        , _id("")
        , _bound(false)
        , _isCompound(false)
        , _isAlloc(false)
        , _type(Void)
        , _flags(0)
        { }

        /** @brief Destructor.
        */
        ~SerializationInfo();

    public:
        /** @brief Clears all content.
        */
        void clear();

        /** @brief Returns the type identifier.
        */
        inline Type type() const
        { return static_cast<Type>(_type); }

        /** @brief Returns true if not set to a type.
        */
        inline bool isVoid() const
        { return _type == Void; }
         
        /** @brief Set to void type.
        */
        void setVoid();

        /** @brief Returns true if scalar type.
        */
        inline bool isScalar() const
        { return _isCompound == false; }

        /** @brief Returns true if struct type.
        */
        inline bool isStruct() const
        { return _type == Struct; }

        /** @brief Returns true if dictionary type.
        */
        inline bool isDict() const
        { return _type == Dict; }

        /** @brief Returns true if sequence type.
        */
        inline bool isSequence() const
        { return _type == Sequence; }

        /** @brief Returns true if reference.
        */
        inline bool isReference() const
        { return _type == Reference; }

        /** @brief Set to sequence type.
        */
        void setSequence();

        /** @brief Set to dictionary type.
        */
        void setDict();

        /** @brief Set to struct type.
        */
        void setStruct();

        /** @brief Returns the used context.
        */
        SerializationContext* context() const
        { return _context; }

        //! @brief Returns true if the type could be composed with a surrogate.
        template <typename T>
        bool compose(T& type) const;

        //! @brief Returns true if the type could be decomposed with a surrogate.
        template <typename T>
        bool decompose(const T& type);

        /** @brief Rebind to new address.
        */
        void rebind(void* obj) const;

        /** @brief Rebind to new fixup address.
        */
        void rebindFixup(void* obj) const;

        /** @brief Returns the parent node.
        */
        SerializationInfo* parent()
        { return _parent; }

        /** @brief Returns the parent node.
        */
        const SerializationInfo* parent() const
        { return _parent; }

        /** @brief Returns the type name.
        */
        const char* typeName() const
        { return _TypeName; }

        /** @brief Sets the type name.
        */
        void setTypeName(const std::string& type);

        /** @brief Sets the type name.
        */
        void setTypeName(const char* type);

        /** @brief Sets the type name.
        */
        void setTypeName(const char* type, std::size_t len);
        
        /** @brief Sets the type name.
        */
        void setTypeName(const LiteralPtr<char>& type);

        /** @brief Returns the instance name.
        */
        const char* name() const
        { return _Name; }

        /** @brief Sets the instance name.
        */
        void setName(const std::string& name);

        /** @brief Sets the instance name.
        */
        void setName(const char* name);

        /** @brief Sets the instance name.
        */
        void setName(const char* type, std::size_t len);
        
        /** @brief Sets the instance name.
        */
        void setName(const LiteralPtr<char>& type);

        /** @brief Returns the reference ID.
        */
        const char* id() const
        { return _id; }

        /** @brief Sets the reference ID.
        */
        void setId(const std::string& id);

        /** @brief Sets the reference ID.
        */
        void setId(const char* id);

        /** @brief Sets the reference ID.
        */
        void setId(const char* id, std::size_t len);

        /** @brief Get value as a string.
        */
        void getString(std::string& s, const TextCodec<Pt::Char, char>& codec) const;

        /** @brief Get value as a string.
        */
        void getString(std::string& s) const;

        /** @brief Get value as a string.
        */
        void getString(Pt::String& s) const;

        /** @brief Set to string value.
        */
        void setString(const char* s);

        /** @brief Set to string value.
        */
        void setString(const char* s, std::size_t len, const TextCodec<Pt::Char, char>& codec);

        /** @brief Set to string value.
        */
        void setString(const std::string& s);

        /** @brief Set to string value.
        */
        void setString(const std::string& str, const TextCodec<Pt::Char, char>& codec)
        { setString(str.c_str(), str.size(), codec); }

        /** @brief Set to string value.
        */
        void setString(const Pt::String& s)
        { setString( s.c_str(), s.length() ); }

        /** @brief Set to string value.
        */
        void setString(const Pt::Char* s, std::size_t len);

        /** @brief Get value as a binary object.
        */
        const char* getBinary(std::size_t& length) const;

        /** @brief Set to binary value.
        */
        void setBinary(const char* data, std::size_t length);

        /** @brief Get value as a character.
        */
        void getChar(char& c) const;

        /** @brief Set to character value.
        */
        void setChar(char c);

        /** @brief Get value as a character.
        */
        void getChar(Pt::Char& c) const;

        /** @brief Set to character value.
        */
        void setChar(const Pt::Char& c);

        /** @brief Get value as a bool.
        */
        void getBool(bool& b) const;

        /** @brief Set to bool value.
        */
        void setBool(bool b);

        /** @brief Get value as a 8-bit integer.
        */
        void getInt8(Pt::int8_t& n) const;
        
        /** @brief Set to 8-bit integer value.
        */
        void setInt8(Pt::int8_t n);
        
        /** @brief Get value as a 16-bit integer.
        */
        void getInt16(Pt::int16_t& n) const;
        
        /** @brief Set to 16-bit integer value.
        */
        void setInt16(Pt::int16_t n);

        /** @brief Get value as a 32-bit integer.
        */
        void getInt32(Pt::int32_t& i) const;

        /** @brief Set to 32-bit integer value.
        */
        void setInt32(Pt::int32_t n);

        /** @brief Get value as a 64-bit integer.
        */
        void getInt64(Pt::int64_t& l) const;

        /** @brief Set to 64-bit integer value.
        */
        void setInt64(Pt::int64_t l);

        /** @brief Get value as a 8-bit unsigned integer.
        */
        void getUInt8(Pt::uint8_t& n) const;

        /** @brief Set to 8-bit unsigned integer value.
        */
        void setUInt8(Pt::uint8_t n);

        /** @brief Get value as a 16-bit unsigned integer.
        */
        void getUInt16(Pt::uint16_t& n) const;

        /** @brief Set to 16-bit unsigned integer value.
        */
        void setUInt16(Pt::uint16_t n);

        /** @brief Get value as a 32-bit unsigned integer.
        */
        void getUInt32(Pt::uint32_t& n) const;

        /** @brief Set to 32-bit unsigned integer value.
        */
        void setUInt32(Pt::uint32_t n);

        /** @brief Get value as a 64-bit unsigned integer.
        */
        void getUInt64(Pt::uint64_t& n) const;

        /** @brief Set to 64-bit unsigned integer value.
        */
        void setUInt64(Pt::uint64_t n);

        /** @brief Get value as a float.
        */
        void getFloat(float& f) const;

        /** @brief Set to float value.
        */
        void setFloat(float f);

        /** @brief Get value as a double.
        */
        void getDouble(double& f) const;

        /** @brief Set to double value.
        */
        void setDouble(double f);
                
        /** @brief Get value as a long double.
        */
        void getLongDouble(long double& d) const;

        /** @brief Set to long double value.
        */
        void setLongDouble(long double d);
        
        /** @brief Begin saving.
        */
        bool beginSave(const void* p);

        /** @brief Finish saving.
        */
        void finishSave();

        /** @brief Begin loading.
        */
        void beginLoad(void* p, const std::type_info& ti) const;

        /** @brief Finish loading.
        */
        void finishLoad() const;

        /** @brief Add a struct member.
        */
        SerializationInfo& addMember(const std::string& name)
        { return this->addMember( name.c_str(), name.length() ); }

        /** @brief Add a struct member.
        */
        SerializationInfo& addMember(const char* name)
        { return this->addMember(name, std::strlen(name)); }
        
        /** @brief Add a struct member.
        */
        SerializationInfo& addMember(const char* name, std::size_t len);
        
        /** @brief Add a struct member.
        */
        SerializationInfo& addMember(const LiteralPtr<char>& name);

        /** @brief Remove a struct member.
        */
        void removeMember(const std::string& name)
        { return this->removeMember( name.c_str() ); }
        
        /** @brief Remove a struct member.
        */
        void removeMember(const char* name);

        /** @brief Remove a struct member.
        */
        void removeMember(const SerializationInfo& si);

        /** @brief Add a sequence element
        */
        SerializationInfo& addElement();

        /** @brief Add a dict element.
        */       
        SerializationInfo& addDictElement();

        /** @brief Add a dict key.
        */ 
        SerializationInfo& addDictKey();

        /** @brief Add a dict value.
        */ 
        SerializationInfo& addDictValue();

        /** @brief Get a struct member
        */
        const SerializationInfo& getMember(const std::string& name) const
        { return this->getMember( name.c_str() ); }

        /** @brief Get a struct member
        */
        const SerializationInfo& getMember(const char* name) const;

        /** @brief Find a struct member

            This method returns the data for a member with the name \a name.
            or null if it is not present.
        */
        const SerializationInfo* findMember(const std::string& name) const
        { return this->findMember( name.c_str() ); }
        
        /** @brief Find a struct member

            This method returns the data for a member with the name \a name.
            or null if it is not present.
        */
        const SerializationInfo* findMember(const char* name) const;

        /** @brief Find a struct member

            This method returns the data for a member with the name \a name.
            or null if it is not present.
        */
        SerializationInfo* findMember(const std::string& name)
        { return this->findMember( name.c_str() ); }
        
        /** @brief Find a struct member

            This method returns the data for a member with the name \a name.
            or null if it is not present.
        */
        SerializationInfo* findMember(const char* name);

        /** @brief Returns the number of members.
        */
        std::size_t memberCount() const;

        /** @internal @brief Returns the sibling node.
        */
        SerializationInfo* sibling() const
        { return _next; }

        /** @internal @brief Sets the sibling node.
        */
        void setSibling(SerializationInfo* si)
        { _next = si; }

        /** @brief Returns an iterator to the begin of child elements.
        */
        Iterator begin();
        
        /** @brief Returns an iterator to the end of child elements.
        */
        Iterator end();

        /** @brief Returns an iterator to the begin of child elements.
        */
        ConstIterator begin() const;

        /** @brief Returns an iterator to the end of child elements.
        */
        ConstIterator end() const;

        /** @brief Set to reference for which to create an ID.
        */
        void setReference(const void* ref);

        /** @brief Set to reference with ID to fixup.
        */
        void setReference(const std::string& id)
        { setReference( id.c_str(), id.length() ); }

        /** @brief Set to reference with ID to fixup.
        */
        void setReference(const char* id, std::size_t idlen);

        /** @brief Load a reference during deserialization.
        */
        template <typename T>
        void loadReference(T& fixme, unsigned mid = 0) const
        {
            this->load(&fixme, FixupThunk<T>::fixupReference, mid);
        }

        /** @brief Load a reference during deserialization.
        */
        template <typename T>
        void loadPointer(T*& fixme, unsigned mid = 0) const
        {
            this->load(&fixme, FixupThunk<T>::fixupPointer, mid);
        }

        /** @brief Begin formatting.
        */
        Iterator beginFormat(Formatter& formatter);

        /** @brief End formatting.
        */
        void endFormat(Formatter& formatter);

        /** @brief Format complete value or all members.
        */
        void format(Formatter& formatter) const;

    protected:
        //! @internal
        void setContextual(SerializationContext& ctx);

        //! @internal
        template <typename T>
        const BasicSerializationSurrogate<T>* getSurrogate() const;

        //! @internal Workaround for some compilers (GCC 3.x).
        template <typename T>
        friend const BasicSerializationSurrogate<T>* getSurrogate(SerializationInfo*);

        //! @internal
        const SerializationSurrogate* getSurrogate(const std::type_info& ti) const;

        //! @internal
        void load(void* fixme, FixupInfo::FixupHandler fh, unsigned mid) const;

        //! @internal
        void clearValue();

        //! @internal
        SerializationInfo& addChild();

    private:
        SerializationInfo(const SerializationInfo&)
        {}

        SerializationInfo& operator=(const SerializationInfo&)
        { return *this; }

    private:
        struct Ref
        {
            void* address;
            char* refId;
        };

        struct BlobValue
        {
            char* data;
            std::size_t length;
        };

        struct StrValue
        {
            Pt::Char* str;
            std::size_t length;
        };

        struct Seq
        {
            SerializationInfo* first;
            SerializationInfo* last;
            std::size_t size;
        };

        union Variant
        {
            bool b;
            uint32_t ui32;
            long long l;
            unsigned long long ul;
            long double f;
            StrValue ustr;
            BlobValue blob;
            Ref ref;
            Seq seq;
        };

    private:
        mutable Variant _value;
        SerializationContext* _context;
        SerializationInfo* _parent;
        SerializationInfo* _next;
        const char* _Name;
        const char* _TypeName;
        const char* _id;
        mutable bool _bound; // TODO: join into bitfield
        bool _isCompound;    // TODO: join into bitfield
        bool _isAlloc;       // TODO: join into bitfield
        Pt::uint8_t _type;   // TODO: join into bitfield
        Pt::uint8_t _flags;

        // TODO: possible type info layout
        // 0 - public / private
        // 1 - scalar / compound
        // 2 - type id
        // 3 - type id
        // 4 - type id
        // 5 - type id
        // 6 - type id
        // 7 - type id
};


template <typename T>
inline const BasicSerializationSurrogate<T>* SerializationInfo::getSurrogate() const
{
    const SerializationSurrogate* surr = this->getSurrogate( typeid(T) );
    if( ! surr )
        return 0;

    return static_cast<const BasicSerializationSurrogate<T>*>(surr);
}


template <typename T>
inline bool SerializationInfo::compose(T& type) const
{
    const BasicSerializationSurrogate<T>* surr = this->getSurrogate<T>();
    if( ! surr )
        return false;

    surr->compose(*this, type);
    return true;
}


template <typename T>
inline bool SerializationInfo::decompose(const T& type)
{
    const BasicSerializationSurrogate<T>* surr = this->getSurrogate<T>();
    if( ! surr )
        return false;

    surr->decompose(*this, type);
    this->setTypeName( surr->typeName() );
    return true;
}

/** @brief Forward %Iterator for child elements.
*/
class SerializationInfo::Iterator
{
    public:
        Iterator()
        : _si(0)
        {}

        Iterator(const Iterator& other)
        : _si(other._si)
        {}

        explicit Iterator(SerializationInfo* si)
        : _si(si)
        {}

        Iterator& operator=(const Iterator& other)
        {
            _si = other._si;
            return *this;
        }

        Iterator& operator++()
        {
            _si = _si->sibling();
            return *this;
        }

        SerializationInfo& operator*() const
        { return *_si; }

        SerializationInfo* operator->() const
        { return _si; }

        bool operator!=(const Iterator& other) const
        { return _si != other._si; }

        bool operator==(const Iterator& other) const
        { return _si == other._si; }

    private:
        SerializationInfo* _si;
};

/** @brief Const forward iterator for child elements.
*/
class SerializationInfo::ConstIterator
{
    public:
        ConstIterator()
        : _si(0)
        {}

        ConstIterator(const ConstIterator& other)
        : _si(other._si)
        {}

        explicit ConstIterator(const SerializationInfo* si)
        : _si(si)
        {}

        ConstIterator& operator=(const ConstIterator& other)
        {
            _si = other._si;
            return *this;
        }

        ConstIterator& operator++()
        {
            _si = _si->sibling();
            return *this;
        }

        const SerializationInfo& operator*() const
        { return *_si; }

        const SerializationInfo* operator->() const
        { return _si; }

        bool operator!=(const ConstIterator& other) const
        { return _si != other._si; }

        bool operator==(const ConstIterator& other) const
        { return _si == other._si; }

    private:
        const SerializationInfo* _si;
};


inline SerializationInfo::Iterator SerializationInfo::end()
{
    return SerializationInfo::Iterator();
}


inline SerializationInfo::ConstIterator SerializationInfo::end() const
{
    return SerializationInfo::ConstIterator();
}


/** @brief Saves referencable types.

    @ingroup Serialization
*/
class SaveInfo
{
    public:
        //! @internal
        explicit SaveInfo(SerializationInfo& info)
        : si(&info)
        {}

        /** @brief Returns the %SerializationInfo to save to.
        */
        SerializationInfo& out() const
        { return *si; }

        /** @brief Returns true if type was saved, false if type was already saved.
        */
        template <typename T>
        bool save(const T& type)
        {
            bool first = si->beginSave( &type );
            if(first)
            {
                *si <<= type;
                 si->finishSave();
            }

            return first;
        }

        SerializationInfo* si;
};

//! @internal
struct Save
{};

//! @internal
inline Save save()
{
    return Save();
}

//! @internal
inline SaveInfo operator <<(SerializationInfo& si, const Save&)
{
    return SaveInfo(si);
}

//! @internal
template <typename T>
inline void operator <<=(SaveInfo info, const T& type)
{
    save( info, type );
}

/** @brief Saves referencable types.

    @related SaveInfo
    @ingroup Serialization
*/
template <typename T>
inline void save(SaveInfo& si, const T& type)
{
    if( ! si.save(type) )
    {
        si.out() <<= type;
    }
}


/** @brief Loads referencable types.

    @ingroup Serialization
*/
class LoadInfo
{
    public:
        //! @internal
        explicit LoadInfo(const SerializationInfo& info)
        : si(&info)
        {}

        /** @brief Returns the %SerializationInfo to load from.
        */
        const SerializationInfo& in() const
        { return *si; }

        /** @brief Loads the type.
        */
        template <typename T>
        void load(T& type) const
        {
            T* tp = &type;

            si->beginLoad( tp, typeid(T) );
            *si >>= type;
            si->finishLoad();
        }

    private:
        const SerializationInfo* si;
};

//! @internal
struct Load
{};

//! @internal
inline Load load()
{
    return Load();
}

//! @internal
inline LoadInfo operator >>(const SerializationInfo& si, const Load&)
{
    return LoadInfo(si);
}

//! @internal
template <typename T>
inline void operator >>=(const LoadInfo& li, T& type)
{
    load(li, type);
}

/** @brief Loads referencable types.

    @related LoadInfo
    @ingroup Serialization
*/
template <typename T>
inline void load(const LoadInfo& li, T& type)
{
    li.load(type);
}


/** @brief Deserializes a pointer reference.

    @related SerializationInfo
*/
template <typename T>
inline void operator >>=(const SerializationInfo& si, T*& ptr)
{
    si.loadPointer(ptr);
}

/** @brief Serializes a pointer reference.

    @related SerializationInfo
*/
template <typename T>
inline void operator <<=(SerializationInfo& si, const T* ptr)
{
    si.setReference( ptr );
}

/** @brief Deserializes a bool

    @related SerializationInfo
*/
inline void operator >>=(const SerializationInfo& si, bool& n)
{
    si.getBool(n);
}

/** @brief Serializes a bool

    @related SerializationInfo
*/
inline void operator <<=(SerializationInfo& si, bool n)
{
    si.setBool(n);
}


/** @brief Deserializes an 8-bit integer

    @related SerializationInfo
*/
inline void operator >>=(const SerializationInfo& si, Pt::int8_t& n)
{
    si.getInt8(n);
}

/** @brief Serializes a 8-bit integer

    @related SerializationInfo
*/
inline void operator <<=(SerializationInfo& si, Pt::int8_t n)
{
    si.setInt8(n);
}

/** @brief Deserializes a 16-bit integer

    @related SerializationInfo
*/
inline void operator >>=(const SerializationInfo& si, Pt::int16_t& n)
{
    si.getInt16(n);
}

/** @brief Serializes a 16-bit integer

    @related SerializationInfo
*/
inline void operator <<=(SerializationInfo& si, Pt::int16_t n)
{
    si.setInt16(n);
}

/** @brief Deserializes a 32-bit integer

    @related SerializationInfo
*/
inline void operator >>=(const SerializationInfo& si, Pt::int32_t& n)
{
    si.getInt32(n);
}

/** @brief Serializes a 32-bit integer

    @related SerializationInfo
*/
inline void operator <<=(SerializationInfo& si, Pt::int32_t n)
{
    si.setInt32(n);
}

/** @brief Deserializes a 64-bit integer

    @related SerializationInfo
*/
inline void operator >>=(const SerializationInfo& si, Pt::int64_t& n)
{
    si.getInt64(n);
}

/** @brief Serializes a 64-bit integer

    @related SerializationInfo
*/
inline void operator <<=(SerializationInfo& si, Pt::int64_t n)
{
    si.setInt64(n);
}

/** @brief Deserializes a 8-bit unsigned integer

    @related SerializationInfo
*/
inline void operator >>=(const SerializationInfo& si, Pt::uint8_t& n)
{
    si.getUInt8(n);
}

/** @brief Serializes a 8-bit unsigned integer

    @related SerializationInfo
*/
inline void operator <<=(SerializationInfo& si, Pt::uint8_t n)
{
    si.setUInt8(n);
}

/** @brief Deserializes a 16-bit unsigned integer

    @related SerializationInfo
*/
inline void operator >>=(const SerializationInfo& si, Pt::uint16_t& n)
{
    si.getUInt16(n);
}

/** @brief Serializes a 16-bit unsigned integer

    @related SerializationInfo
*/
inline void operator <<=(SerializationInfo& si, Pt::uint16_t n)
{
    si.setUInt16(n);
}

/** @brief Deserializes a 32-bit unsigned integer

    @related SerializationInfo
*/
inline void operator >>=(const SerializationInfo& si, Pt::uint32_t& n)
{
    si.getUInt32(n);
}

/** @brief Serializes a 32-bit unsigned integer

    @related SerializationInfo
*/
inline void operator <<=(SerializationInfo& si, Pt::uint32_t n)
{
    si.setUInt32(n);
}

/** @brief Deserializes a 64-bit unsigned integer

    @related SerializationInfo
*/
inline void operator >>=(const SerializationInfo& si, Pt::uint64_t& n)
{
    si.getUInt64(n);
}

/** @brief Serializes a 64-bit unsigned integer

    @related SerializationInfo
*/
inline void operator <<=(SerializationInfo& si, Pt::uint64_t n)
{
    si.setUInt64(n);
}

/** @brief Deserializes a float value.

    @related SerializationInfo
*/
inline void operator >>=(const SerializationInfo& si, float& n)
{
    si.getFloat(n);
}

/** @brief Serializes a float value.

    @related SerializationInfo
*/
inline void operator <<=(SerializationInfo& si, float n)
{
    si.setFloat(n);
}

/** @brief Deserializes a double value.

    @related SerializationInfo
*/
inline void operator >>=(const SerializationInfo& si, double& n)
{
    si.getDouble(n);
}

/** @brief Serializes a double value.

    @related SerializationInfo
*/
inline void operator <<=(SerializationInfo& si, double n)
{
    si.setDouble(n);
}

/** @brief Deserializes a long double value.

    @related SerializationInfo
*/
inline void operator >>=(const SerializationInfo& si, long double& n)
{
    si.getLongDouble(n);
}

/** @brief Serializes a long double value.

    @related SerializationInfo
*/
inline void operator <<=(SerializationInfo& si, long double n)
{
    si.setLongDouble(n);
}

/** @brief Deserializes a character value.

    @related SerializationInfo
*/
inline void operator >>=(const SerializationInfo& si, char& ch)
{
    Pt::Char tmp;
    si.getChar(tmp);
    ch = static_cast<char>( tmp.value() );
}

/** @brief Serializes a character value.

    @related SerializationInfo
*/
inline void operator <<=(SerializationInfo& si, char ch)
{
    si.setChar( Pt::Char(ch) );
}

/** @brief Serializes a string.

    @related SerializationInfo
*/
inline void operator <<=(SerializationInfo& si, const char* str)
{
    si.setString(str);
}

/** @brief Deserializes a std::string

    @related SerializationInfo
*/
inline void operator >>=(const SerializationInfo& si, std::string& str)
{
    si.getString(str);
}

/** @brief Serializes a std::string

    @related SerializationInfo
*/
inline void operator <<=(SerializationInfo& si, const std::string& str)
{
    si.setString( str.c_str() );
}

/** @brief Deserializes a string

    @related SerializationInfo
*/
inline void operator >>=(const SerializationInfo& si, Pt::String& str)
{
    si.getString(str);
}

/** @brief Serializes a string

    @related SerializationInfo
*/
inline void operator <<=(SerializationInfo& si, const Pt::String& str)
{
    si.setString(str);
}

/** @brief Deserializes a std::vector

    @related SerializationInfo
*/
template <typename T, typename A>
inline void operator >>=(const SerializationInfo& si, std::vector<T, A>& vec)
{
    T elem = T();
    vec.clear();
    vec.reserve( si.memberCount() );

    SerializationInfo::ConstIterator end = si.end();
    for(SerializationInfo::ConstIterator it = si.begin(); it != end; ++it)
    {
        vec.push_back(elem);
        *it >> Pt::load() >>= vec.back();
    }
}

/** @brief Serializes a std::vector

    @related SerializationInfo
*/
template <typename T, typename A>
inline void operator <<=(SerializationInfo& si, const std::vector<T, A>& vec)
{
    typename std::vector<T, A>::const_iterator it;

    for(it = vec.begin(); it != vec.end(); ++it)
    {
        si.addElement() << Pt::save() <<= *it;
    }

    si.setTypeName( Pt::LiteralPtr<char>("std::vector") );
    si.setSequence();
}

/** @brief Deserializes a std::list

    @related SerializationInfo
*/
template <typename T, typename A>
inline void operator >>=(const SerializationInfo& si, std::list<T, A>& list)
{
    list.clear();
    for(SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
    {
        list.resize( list.size() + 1 );
        *it >> Pt::load() >>= list.back();
    }
}

/** @brief Serializes a std::list

    @related SerializationInfo
*/
template <typename T, typename A>
inline void operator <<=(SerializationInfo& si, const std::list<T, A>& list)
{
    typename std::list<T, A>::const_iterator it;

    for(it = list.begin(); it != list.end(); ++it)
    {
        si.addElement() << Pt::save() <<= *it;
    }

    si.setTypeName( Pt::LiteralPtr<char>("std::list") );
    si.setSequence();
}

/** @brief Deserializes a std::deque

    @related SerializationInfo
*/
template <typename T, typename A>
inline void operator >>=(const SerializationInfo& si, std::deque<T, A>& deque)
{
    deque.clear();
    for(SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
    {
        // NOTE: push_back does not invalidate references to elements
        deque.push_back( T() );
        *it >> Pt::load() >>= deque.back();
    }
}

/** @brief Serializes a std::deque

    @related SerializationInfo
*/
template <typename T, typename A>
inline void operator <<=(SerializationInfo& si, const std::deque<T, A>& deque)
{
    typename std::deque<T, A>::const_iterator it;

    for(it = deque.begin(); it != deque.end(); ++it)
    {
        si.addElement() << Pt::save() <<= *it;
    }

    si.setTypeName( Pt::LiteralPtr<char>("std::deque") );
    si.setSequence();
}


/** @brief Deserializes a std::set

    Deserialization of references to or from set elements is not reliably
    possible, due to some of std::set's constraints. However you may
    overload this operator for your type.

    @related SerializationInfo
*/
template <typename T, typename C, typename A>
inline void operator >>=(const SerializationInfo& si, std::set<T, C, A>& set)
{
    // typedef typename std::set<T, C, A>::iterator SetIterator;
    // std::pair<SetIterator, bool> pos;

    set.clear();
    for(SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
    {
        T t;
        *it >>= t;
        set.insert(t);

        // T t;
        // *it >>= Pt::load() >>= t;
        // pos = set.insert(t);
        // if( ! pos.second )
        //     it->rebind(0);

    }
}

/** @brief Serializes a std::set

    @related SerializationInfo
*/
template <typename T, typename C, typename A>
inline void operator <<=(SerializationInfo& si, const std::set<T, C, A>& set)
{
    typename std::set<T, C, A>::const_iterator it;

    for(it = set.begin(); it != set.end(); ++it)
    {
        si.addElement() << Pt::save() <<= *it;
    }

    si.setTypeName( Pt::LiteralPtr<char>("std::set") );
    si.setSequence();
}

/** @brief Deserializes a std::multiset

    Deserialization of references to or from set elements is not reliably
    possible, due to some of std::set's constraints. However you may
    overload this operator for your type.

    @related SerializationInfo
*/
template <typename T, typename C, typename A>
inline void operator >>=(const SerializationInfo& si, std::multiset<T, C, A>& multiset)
{
    // typename std::multiset<T>::iterator pos;

    multiset.clear();
    for(Pt::SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
    {
        T t;
        *it >>= t;
        multiset.insert(t);

        // T tmp;
        // *it >>= Pt::load() >>= tmp;
        // pos = multiset.insert(tmp);

        // T& t = const_cast<T&>(*pos);
        // it->rebind(&t);
    }
}

/** @brief Serializes a std::multiset

    @related SerializationInfo
*/
template <typename T, typename C, typename A>
inline void operator <<=(SerializationInfo& si, const std::multiset<T, C, A>& multiset)
{
    typename std::multiset<T, C, A>::const_iterator it;

    for(it = multiset.begin(); it != multiset.end(); ++it)
    {
        si.addElement() << Pt::save() <<= *it;
    }

    si.setTypeName( Pt::LiteralPtr<char>("std::multiset") );
    si.setSequence();
}


template <typename A, typename B>
inline void operator >>=(const SerializationInfo& si, std::pair<A, B>& p)
{
    si.getMember("first") >>= p.first;
    si.getMember("second") >>= p.second;
}


template <typename A, typename B>
inline void operator <<=(SerializationInfo& si, const std::pair<A, B>& p)
{
    si.setTypeName( Pt::LiteralPtr<char>("std::pair") );
    si.addMember( Pt::LiteralPtr<char>("first") ) <<= p.first;
    si.addMember( Pt::LiteralPtr<char>("second") ) <<= p.second;
}

/** @brief Deserializes a std::map

    @related SerializationInfo
*/
template <typename K, typename V, typename P, typename A>
inline void operator >>=(const SerializationInfo& si, std::map<K, V, P, A>& map)
{
    typedef typename std::map<K, V, P, A>::iterator MapIterator;
    std::pair<MapIterator, bool> pos;

    map.clear();
    for(SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
    {
        K k;
        
        SerializationInfo::ConstIterator kv = it->begin();
        if( kv != it->end() )
            *kv >>= k;

        std::pair<K, V> elem( k, V() );
        pos = map.insert(elem);

        if( pos.second && ++kv != it->end() )
            *kv >> Pt::load() >>= pos.first->second;
    }
}

/** @brief Serializes a std::map

    @related SerializationInfo
*/
template <typename K, typename V, typename P, typename A>
inline void operator <<=(SerializationInfo& si, const std::map<K, V, P, A>& map)
{
    typename std::map<K, V, P, A>::const_iterator it;

    for(it = map.begin(); it != map.end(); ++it)
    {
        SerializationInfo& elem = si.addDictElement();
        elem.addDictKey() <<= it->first;
        elem.addDictValue() << Pt::save() <<= it->second;
    }

    si.setTypeName( Pt::LiteralPtr<char>("std::map") );
    si.setDict();
}

/** @brief Deserializes a std::multimap

    @related SerializationInfo
*/
template <typename K, typename V, typename P, typename A>
inline void operator >>=(const SerializationInfo& si, std::multimap<K, V, P, A>& multimap)
{
    typename std::multimap<K, V, P, A>::iterator mit;

    multimap.clear();
    for(SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
    {
        K k;
        
        SerializationInfo::ConstIterator kv = it->begin();
        if( kv != it->end() )
            *kv >>= k;

        std::pair<K, V> elem( k, V() );
        mit = multimap.insert(elem);

        if( ++kv != it->end() )
            *kv >> Pt::load() >>= mit->second;
    }
}

/** @brief Serializes a std::multimap

    @related SerializationInfo
*/
template <typename T, typename C, typename P, typename A>
inline void operator <<=(SerializationInfo& si, const std::multimap<T, C, P, A>& multimap)
{
    typename std::multimap<T, C, P, A>::const_iterator it;

    for(it = multimap.begin(); it != multimap.end(); ++it)
    {
        SerializationInfo& elem = si.addDictElement();
        elem.addDictKey() <<= it->first;
        elem.addDictValue() << Pt::save() <<= it->second;
    }

    si.setTypeName( Pt::LiteralPtr<char>("std::multimap") );
    si.setDict();
}

} // namespace Pt

#endif // Pt_SerializationInfo_h
