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
#ifndef Pt_SerializationInfo_h
#define Pt_SerializationInfo_h

#include <Pt/Api.h>
#include <Pt/String.h>
#include <Pt/Types.h>
#include <Pt/Convert.h>
#include <Pt/LiteralPtr.h>
#include <Pt/FixupInfo.h>
#include <Pt/SerializationError.h>
#include <Pt/SerializationSurrogate.h>
#include <typeinfo>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <deque>

namespace Pt {

class SerializationContext;
class Formatter;

/** @brief Represents arbitrary types during serialization.
*/
class PT_API SerializationInfo
{
    public:
        enum Type {
            Void       = 0,
            Context    = 1,
            Reference  = 2,
            Boolean    = 3,
            Char8      = 4,
            Char       = 5,
            Str8       = 6,
            Str        = 7,
            Int8       = 8,
            Int16      = 9,
            Int32      = 10,
            Int64      = 11,
            UInt8      = 12,
            UInt16     = 13,
            UInt32     = 14,
            UInt64     = 15,
            Float      = 16,
            Double     = 17,
            LongDouble = 18,
            Binary     = 19,
            Blob       = 20,
            Struct     = 21,
            Sequence   = 22
        };

        // type info layout
        // 0 - public / private
        // 1 - scalar / compound
        // 2 - type id
        // 3 - type id
        // 4 - type id
        // 5 - type id
        // 6 - type id
        // 7 - type id

        class Iterator;
        class ConstIterator;

    public:
        SerializationInfo()
        : _context(0)
        , _parent(0)
        , _next(0)
        , _Name("")
        , _TypeName("")
        , _id("")
        , _bound(false)
        , _isCompound(false)
        , _type(Void)
        , _nameRef(true)
        , _tnRef(true)
        , _idRef(true)
        
        { }

        explicit SerializationInfo(SerializationContext* context)
        : _context(context)
        , _parent(0)
        , _next(0)
        , _Name("")
        , _TypeName("")
        , _id("")
        , _bound(false)
        , _isCompound(false)
        , _type(Void)
        , _nameRef(true)
        , _tnRef(true)
        , _idRef(true)
        { }

        ~SerializationInfo();

    private:
        SerializationInfo(const SerializationInfo& si)
        {}

        SerializationInfo& operator=(const SerializationInfo& si)
        { return *this; }

    public:
        void clear();

        inline bool isScalar() const
        { return _isCompound == false; }

        inline bool isStruct() const
        { return _type == Struct; }

        inline bool isSequence() const
        { return _type == Sequence; }

        inline bool isReference() const
        { return _type == Reference; }

        void setSequence();

        SerializationContext* context() const
        { return _context; }

        template <typename T>
        bool compose(T& type) const
        {
            const SerializationSurrogate* s = this->getSurrogate(typeid(T));
            if(0 == s)
                return false;

            const BasicSerializationSurrogate<T>* surr = static_cast<const BasicSerializationSurrogate<T>*>(s);
            surr->compose(*this, type);
            return true;
        }

        template <typename T>
        bool decompose(const T& type)
        {
            const SerializationSurrogate* s = this->getSurrogate(typeid(T));
            if(0 == s)
                return false;

            const BasicSerializationSurrogate<T>* surr = static_cast<const BasicSerializationSurrogate<T>*>(s);
            surr->decompose(*this, type);
            this->setTypeName( surr->typeName() );
            return true;
        }

        void rebind(void* obj) const;

        void rebindFixup(void* obj) const;

        SerializationInfo* parent()
        { return _parent; }

        const SerializationInfo* parent() const
        { return _parent; }

        const char* typeName() const
        { return _TypeName; }

        void setTypeName(const std::string& type);

        void setTypeName(const char* type);
        
        void setTypeName(const LiteralPtr<char>& type);

        const char* name() const
        { return _Name; }

        void setName(const std::string& name);

        void setName(const char* type);
        
        void setName(const LiteralPtr<char>& type);

        const char* id() const
        { return _id; }

        void setId(const std::string& id);

        void setId(const char* id);

        void getString8(std::string& s) const;

        void setString8(const char* s);

        void setString8(const std::string& s);

        /** @brief Returns the content as string.
        */
        Pt::String toString() const
        {
            Pt::String value;
            this->getString(value);
            return value;
        }

        void getString(Pt::String& s) const;

        void setString(const Pt::String& s);

        const char* getBinary(size_t& length) const;

        void setBinary(const char* data, size_t length);

        void getChar8(char c) const;

        void setChar8(char c);

        void getChar(Pt::Char& c) const;

        void setChar(const Pt::Char& c);

        void getBool(bool& b) const;

        void setBool(bool b);

        inline void getInt8(Pt::int8_t& n) const
        {
            Pt::int64_t l = 0;
            this->getInt64(l);
            // TODO: consider SerializationError on overflow
            n = static_cast<Pt::int8_t>(l);
        }
        
        void setInt8(Pt::int8_t n);
        
        void getInt16(Pt::int16_t& n) const
        {
            Pt::int64_t l = 0;
            this->getInt64(l);
            // TODO: consider SerializationError on overflow
            n = static_cast<Pt::int16_t>(l);
        }
        
        void setInt16(Pt::int16_t n);

        inline void getInt32(Pt::int32_t& i) const
        {
            Pt::int64_t l = 0;
            this->getInt64(l);
            // TODO: consider SerializationError on overflow
            i = static_cast<Pt::int32_t>(l);
        }

        void setInt32(Pt::int32_t n);

        void getInt64(Pt::int64_t& l) const;

        void setInt64(Pt::int64_t l);

        inline void getUInt8(Pt::uint8_t& n) const
        {
            Pt::uint64_t l = 0;
            this->getUInt64(l);
            // TODO: consider SerializationError on overflow
            n = static_cast<Pt::uint8_t>(l);
        }

        void setUInt8(Pt::uint8_t n);

        void getUInt16(Pt::uint16_t& n) const
        {
            Pt::uint64_t l = 0;
            this->getUInt64(l);
            // TODO: consider SerializationError on overflow
            n = static_cast<Pt::uint16_t>(l);
        }

        void setUInt16(Pt::uint16_t n);

        void getUInt32(Pt::uint32_t& n) const
        {
            Pt::uint64_t l = 0;
            this->getUInt64(l);
            // TODO: consider SerializationError on overflow
            n = static_cast<Pt::uint32_t>(l);
        }

        void setUInt32(Pt::uint32_t n);

        void getUInt64(Pt::uint64_t& n) const;

        void setUInt64(Pt::uint64_t n);

        void getFloat(float& f) const
		{
			long double d = 0.0;
			this->getLongDouble(d);
			// TODO: consider SerializationError on overflow
			f = static_cast<float>(d);
		}

        void setFloat(float f);

        void getDouble(double& f) const
		{
			long double d = 0.0;
			this->getLongDouble(d);
			// TODO: consider SerializationError on overflow
			f = static_cast<double>(d);
		}

        void setDouble(double f);
                
        void getLongDouble(long double& d) const;

        void setLongDouble(long double d);
        
        bool beginSave(const void* p);

        void finishSave();

        void beginLoad(void* p, const std::type_info& ti) const;

        void finishLoad() const;

        /** @brief Serialization of member data
        */
        SerializationInfo& addMember(const std::string& name)
        { return this->addMember( name.c_str() ); }
        
        SerializationInfo& addMember(const char* name);
        
        SerializationInfo& addMember(const LiteralPtr<char>& name);

        /** @brief Serialization of member data
        */
        void removeMember(const std::string& name)
        { return this->removeMember( name.c_str() ); }
        
        void removeMember(const char* name);

        /** @brief Serialization of member data
        */
        SerializationInfo& addElement();

        /** @brief Deserialization of member data
        */
        const SerializationInfo& getMember(const std::string& name) const
        { return this->getMember( name.c_str() ); }

        /** @brief Deserialization of member data
        */ 
        const SerializationInfo& getMember(const char* name) const;

        /** @brief Find member data by name

            This method returns the data for an object with the name \a name.
            or null if it is not present.
        */
        const SerializationInfo* findMember(const std::string& name) const
        { return this->findMember( name.c_str() ); }
        
        const SerializationInfo* findMember(const char* name) const;

        /** @brief Find member data by name

            This method returns the data for an object with the name \a name.
            or null if it is not present.
        */
        SerializationInfo* findMember(const std::string& name)
        { return this->findMember( name.c_str() ); }
        
        SerializationInfo* findMember(const char* name);

        size_t memberCount() const;

        SerializationInfo* sibling() const
        { return _next; }

        void setSibling(SerializationInfo* si)
        { _next = si; }

        Iterator begin();

        Iterator end();

        ConstIterator begin() const;

        ConstIterator end() const;

        /** @brief Serialization of weak pointers
        */
        void setReference(const void* ref);

        /** @brief Deserialization of weak pointers (parse phase)
        */
        void setReference(const std::string& id);

        /** @brief Deserialization of references
        */
        template <typename T>
        void loadReference(T& fixme, unsigned mid = 0) const
        {
            this->load(&fixme, FixupThunk<T>::fixupReference, mid);
        }

        /** @brief Deserialization of weak pointers
        */
        template <typename T>
        void loadPointer(T*& fixme, unsigned mid = 0) const
        {
            this->load(&fixme, FixupThunk<T>::fixupPointer, mid);
        }

        Iterator beginFormat(Formatter& formatter);

        void endFormat(Formatter& formatter);

        void format(Formatter& formatter);

        /** @internal DEPRECATED
            This is needed as a workaround for some compilers (GCC 3.x) to
            allow access to 'T getValue(const std::string& name) const'.

            template <typename T>
            friend T getValue(const std::string& name, SerializationInfo* si);
        */

    protected:
        void load(void* fixme, FixupInfo::FixupHandler fh, unsigned mid) const;

        void clearValue();

        SerializationInfo& addChild();

        void setContextual(SerializationContext& ctx);

        const SerializationSurrogate* getSurrogate(const std::type_info& ti) const;

	public:
        struct Ref
        {
            void* address;
            char* refId;
        };

        struct BlobValue
        {
            char* data;
            size_t length;
        };

        struct Seq
        {
            SerializationInfo* first;
            SerializationInfo* last;
            size_t size;
        };

        union Variant
        {
            bool b;
            uint32_t ui32;
            long long l;
            unsigned long long ul;
            long double f;
            char* cstr;
            char str[sizeof(Pt::String)];
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
        Pt::uint8_t _type;   // TODO: join into bitfield
        bool _nameRef;       // TODO: join into bitfield
        bool _tnRef;         // TODO: join into bitfield
        bool _idRef;         // TODO: join into bitfield
};

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




struct id
{};


class SaveInfo
{
    public:
        explicit SaveInfo(SerializationInfo& info)
        : si(&info)
        {}

        SerializationInfo& out() const
        { return *si; }

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


inline id save()
{
    return id();
}


inline SaveInfo operator <<(SerializationInfo& si, const id&)
{
    return SaveInfo(si);
}


template <typename T>
inline void operator <<=(SaveInfo info, const T& type)
{
    save( info, type );
}


template <typename T>
inline void save(SaveInfo& si, const T& type)
{
    if( ! si.save(type) )
    {
        si.out() <<= type;
    }
}




class LoadInfo
{
    public:
        explicit LoadInfo(const SerializationInfo& info)
        : si(&info)
        {}

        const SerializationInfo& in() const
        { return *si; }

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


inline id load()
{
    return id();
}


inline LoadInfo operator >>(const SerializationInfo& si, const id&)
{
    return LoadInfo(si);
}


template <typename T>
inline void operator >>=(const LoadInfo& li, T& type)
{
    load(li, type);
}


template <typename T>
inline void load(const LoadInfo& li, T& type)
{
    li.load(type);
}


template <typename T>
inline void operator >>=(const SerializationInfo& si, T*& ptr)
{
    si.loadPointer(ptr);
}


template <typename T>
inline void operator <<=(SerializationInfo& si, const T* ptr)
{
    si.setReference( ptr );
    si.setTypeName( Pt::LiteralPtr<char>("reference") );
}


inline void operator >>=(const SerializationInfo& si, bool& n)
{
    si.getBool(n);
}


inline void operator <<=(SerializationInfo& si, bool n)
{
    si.setBool(n);
    si.setTypeName( Pt::LiteralPtr<char>("bool") );
}


inline void operator >>=(const SerializationInfo& si, Pt::int8_t& n)
{
    si.getInt8(n);
}


inline void operator <<=(SerializationInfo& si, Pt::int8_t n)
{
    si.setInt8(n);
    si.setTypeName( Pt::LiteralPtr<char>("char") );
}


inline void operator >>=(const SerializationInfo& si, Pt::int16_t& n)
{
    si.getInt16(n);
}


inline void operator <<=(SerializationInfo& si, Pt::int16_t n)
{
    si.setInt16(n);
    si.setTypeName( Pt::LiteralPtr<char>("int") );
}


inline void operator >>=(const SerializationInfo& si, Pt::int32_t& n)
{
    si.getInt32(n);
}


inline void operator <<=(SerializationInfo& si, Pt::int32_t n)
{
    si.setInt32(n);
    si.setTypeName( Pt::LiteralPtr<char>("int") );
}


inline void operator >>=(const SerializationInfo& si, Pt::int64_t& n)
{
    si.getInt64(n);
}


inline void operator <<=(SerializationInfo& si, Pt::int64_t n)
{
    si.setInt64(n);
    si.setTypeName( Pt::LiteralPtr<char>("int") );
}


inline void operator >>=(const SerializationInfo& si, Pt::uint8_t& n)
{
    si.getUInt8(n);
}


inline void operator <<=(SerializationInfo& si, Pt::uint8_t n)
{
    si.setUInt8(n);
    si.setTypeName( Pt::LiteralPtr<char>("char") );
}


inline void operator >>=(const SerializationInfo& si, Pt::uint16_t& n)
{
    si.getUInt16(n);
}


inline void operator <<=(SerializationInfo& si, Pt::uint16_t n)
{
    si.setUInt16(n);
    si.setTypeName( Pt::LiteralPtr<char>("int") );
}


inline void operator >>=(const SerializationInfo& si, Pt::uint32_t& n)
{
    si.getUInt32(n);
}


inline void operator <<=(SerializationInfo& si, Pt::uint32_t n)
{
    si.setUInt32(n);
    si.setTypeName( Pt::LiteralPtr<char>("int") );
}


inline void operator >>=(const SerializationInfo& si, Pt::uint64_t& n)
{
    si.getUInt64(n);
}


inline void operator <<=(SerializationInfo& si, Pt::uint64_t n)
{
    si.setUInt64(n);
    si.setTypeName( Pt::LiteralPtr<char>("int") );
}


inline void operator >>=(const SerializationInfo& si, float& n)
{
    si.getFloat(n);
}


inline void operator <<=(SerializationInfo& si, float n)
{
    si.setFloat(n);
    si.setTypeName( Pt::LiteralPtr<char>("double") );
}


inline void operator >>=(const SerializationInfo& si, double& n)
{
    si.getDouble(n);
}


inline void operator <<=(SerializationInfo& si, double n)
{
    si.setDouble(n);
    si.setTypeName( Pt::LiteralPtr<char>("double") );
}


inline void operator >>=(const SerializationInfo& si, char& ch)
{
	Pt::Char tmp;
    si.getChar(tmp);
    ch = static_cast<int>(tmp);
}


inline void operator <<=(SerializationInfo& si, char ch)
{
    si.setChar( Pt::Char(ch) );
    si.setTypeName( Pt::LiteralPtr<char>("char") );
}


inline void operator <<=(SerializationInfo& si, const char* str)
{
    si.setString8(str);
    si.setTypeName( Pt::LiteralPtr<char>("string") );
}


inline void operator >>=(const SerializationInfo& si, std::string& str)
{
	Pt::String tmp;
    si.getString(tmp);
    str = tmp.narrow();
}


inline void operator <<=(SerializationInfo& si, const std::string& str)
{
    si.setString8( str.c_str() );
    si.setTypeName( Pt::LiteralPtr<char>("string") );
}


inline void operator >>=(const SerializationInfo& si, Pt::String& str)
{
    si.getString(str);
}


inline void operator <<=(SerializationInfo& si, const Pt::String& str)
{
    si.setString(str);
    si.setTypeName( Pt::LiteralPtr<char>("string") );
}


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


template <typename T, typename A>
inline void operator <<=(SerializationInfo& si, const std::list<T, A>& list)
{
    typename std::list<T, A>::const_iterator it;

    for(it = list.begin(); it != list.end(); ++it)
    {
        si.addElement() << Pt::save() <<= *it;
    }

    si.setTypeName( Pt::LiteralPtr<char>("list") );
    si.setSequence();
}


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


template <typename T, typename A>
inline void operator <<=(SerializationInfo& si, const std::deque<T, A>& deque)
{
    typename std::deque<T, A>::const_iterator it;

    for(it = deque.begin(); it != deque.end(); ++it)
    {
        si.addElement() << Pt::save() <<= *it;
    }

    si.setTypeName( Pt::LiteralPtr<char>("deque") );
    si.setSequence();
}


/** @brief Deserializes a std::set

    Deserialization of references to or from set elements is not reliably
    possible, due to some of std::set's constraints. However you may
    overload this operator for your type.
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


template <typename T, typename C, typename A>
inline void operator <<=(SerializationInfo& si, const std::set<T, C, A>& set)
{
    typename std::set<T, C, A>::const_iterator it;

    for(it = set.begin(); it != set.end(); ++it)
    {
        si.addElement() << Pt::save() <<= *it;
    }

    si.setTypeName( Pt::LiteralPtr<char>("set") );
    si.setSequence();
}


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


template <typename T, typename C, typename A>
inline void operator <<=(SerializationInfo& si, const std::multiset<T, C, A>& multiset)
{
    typename std::multiset<T, C, A>::const_iterator it;

    for(it = multiset.begin(); it != multiset.end(); ++it)
    {
        si.addElement() << Pt::save() <<= *it;
    }

    si.setTypeName( Pt::LiteralPtr<char>("multiset") );
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
    si.setTypeName( Pt::LiteralPtr<char>("pair") );
    si.addMember( Pt::LiteralPtr<char>("first") ) <<= p.first;
    si.addMember( Pt::LiteralPtr<char>("second") ) <<= p.second;
}


template <typename K, typename V, typename P, typename A>
inline void operator >>=(const SerializationInfo& si, std::map<K, V, P, A>& map)
{
    typedef typename std::map<K, V, P, A>::iterator MapIterator;
    std::pair<MapIterator, bool> pos;

    map.clear();
    for(SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
    {
        K k;
        it->getMember("first") >>= k;

        std::pair<K, V> elem( k, V() );
        pos = map.insert(elem);
        if( pos.second )
            it->getMember("second") >> Pt::load() >>= pos.first->second;
    }
}


template <typename K, typename V, typename P, typename A>
inline void operator <<=(SerializationInfo& si, const std::map<K, V, P, A>& map)
{
    typename std::map<K, V, P, A>::const_iterator it;

    for(it = map.begin(); it != map.end(); ++it)
    {
        si.addElement() << Pt::save() <<= *it;
    }

    si.setTypeName( Pt::LiteralPtr<char>("map") );
    si.setSequence();
}


template <typename K, typename V, typename P, typename A>
inline void operator >>=(const SerializationInfo& si, std::multimap<K, V, P, A>& multimap)
{
    typename std::multimap<K, V, P, A>::iterator mit;

    multimap.clear();
    for(SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
    {
        K k;
        it->getMember("first") >>= k;

        std::pair<K, V> elem( k, V() );
        mit = multimap.insert(elem);

        it->getMember("second") >> Pt::load() >>= mit->second;
    }
}


template <typename T, typename C, typename P, typename A>
inline void operator <<=(SerializationInfo& si, const std::multimap<T, C, P, A>& multimap)
{
    typename std::multimap<T, C, P, A>::const_iterator it;

    for(it = multimap.begin(); it != multimap.end(); ++it)
    {
        si.addElement() << Pt::save() <<= *it;
    }

    si.setTypeName( Pt::LiteralPtr<char>("multimap") );
    si.setSequence();
}

} // namespace Pt

#endif
