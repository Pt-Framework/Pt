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
#include <Pt/SerializationContext.h>
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
            Void      = 0,
            Context   = 1,
            Reference = 2,
            Boolean   = 3,
            Char      = 4,
            Str       = 5,
            Int       = 6,
            UInt      = 7,
            Float     = 8,
            Binary    = 9,
            Blob      = 10,
            Struct    = 11,
            Sequence  = 12
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

        void setContextual();

        void setSequence();

        SerializationContext* context() const
        { return _context; }

        template <typename T>
        bool compose(T& type) const
        {
            if(_context)
            {
                const BasicSerializationSurrogate<T>* surr = _context->getSurrogate<T>();
                if(surr)
                {
                    surr->compose(*this, type);
                    return true;
                }
            }

            return false;
        }

        template <typename T>
        bool decompose(const T& type)
        {
            if(_context)
            {
                const BasicSerializationSurrogate<T>* surr = _context->getSurrogate<T>();
                if(surr)
                {
                    surr->decompose(*this, type);
                    this->setTypeName(surr->typeName());
                    return true;
                }
            }

            return false;
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
        
        void setName(const char* name, bool copy = true);

        const char* id() const
        { return _id; }

        void setId(const std::string& id);

        void setId(const char* id);

        /** @brief Returns the content as string.
        */
        Pt::String toString() const
        {
            Pt::String value;
            this->getValue(value);
            return value;
        }

        void getValue(std::string& s) const
        {
            Pt::String value;
            this->getValue(value);
            convert(s, value);
        }

        void setValue(const std::string& s)
        {
            Pt::String value;
            convert(value, s);
            this->setValue(value);
            this->setTypeName("string");
        }
        
        void setValue(const char* s);
        
        void getValue(Pt::String& s) const;

        void setValue(const Pt::String& s);

        const char* getBinary(size_t& length) const;

        void setBinary(const char* data, size_t length);

        void getValue(char& c) const;

        void setValue(char c);
        
        void getValue(Pt::Char& c) const;

        void setValue(const Pt::Char& c);
        
        void getValue(bool& b) const;

        void setValue(bool b);

        inline void getValue(signed char& c) const
        {
            long long l = 0;
            this->getValue(l);
            // TODO: consider SerializationError on overflow
            c = static_cast<signed char>(l);
        }

        void getValue(short& s) const;

        void setValue(short s)
        { this->setValue( static_cast<long long>(s) ); }

        inline void getValue(int& i) const
        {
            long long l = 0;
            this->getValue(l);
            // TODO: consider SerializationError on overflow
            i = static_cast<int>(l);
        }

        void setValue(int i)
        { this->setValue( static_cast<long long>(i) ); }

        void getValue(long& l) const;

        void setValue(long l)
        { this->setValue( static_cast<long long>(l) ); }

        void getValue(long long& l) const;

        void setValue(long long l);

        inline void getValue(unsigned char& c) const
        {
            long long l = 0;
            this->getValue(l);
            // TODO: consider SerializationError on overflow
            c = static_cast<unsigned char>(l);
        }

        void getValue(unsigned short& us) const;

        void setValue(unsigned short us)
        { this->setValue( static_cast<unsigned long long>(us) ); }

        void getValue(unsigned int& ui) const;

        void setValue(unsigned int ui)
        { this->setValue( static_cast<unsigned long long>(ui) ); }

        void getValue(unsigned long& ul) const;

        void setValue(unsigned long ul)
        { this->setValue( static_cast<unsigned long long>(ul) ); }

        void getValue(unsigned long long& ul) const;

        void setValue(unsigned long long ul);

        void getValue(float& f) const;

        void setValue(float f)
        { this->setValue( double(f) ); }

        void getValue(double& f) const;

        void setValue(double f);

        bool beginSave(const void* p);

        void finishSave();

        void beginLoad(void* p, const std::type_info& ti) const;

        void finishLoad() const;

        /** @brief Serialization of member data
        */
        SerializationInfo& addMember(const std::string& name)
        { return this->addMember( name.c_str() ); }
        
        SerializationInfo& addMember(const char* name, bool copy = true);

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
         */
        template <typename T>
        friend T getValue(const std::string& name, SerializationInfo* si);

        /** @internal rename to getMember
        */
        template <typename T>
        T getValue(const std::string& name) const
        {
            T value;
            this->getMember(name).getValue(value);
            return value;
        }

    protected:
        void load(void* fixme, FixupInfo::FixupHandler fh, unsigned mid) const;

        void clearValue();

        SerializationInfo& addChild();
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
            double f;
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
    si.setTypeName("reference");
}


inline void operator >>=(const SerializationInfo& si, bool& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, bool n)
{
    si.setValue(n);
    si.setTypeName("bool");
}


inline void operator >>=(const SerializationInfo& si, signed char& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, signed char n)
{
    si.setValue(n);
    si.setTypeName("char");
}


inline void operator >>=(const SerializationInfo& si, unsigned char& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, unsigned char n)
{
    si.setValue(n);
    si.setTypeName("char");
}


inline void operator >>=(const SerializationInfo& si, char& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, char n)
{
    si.setValue(n);
    si.setTypeName("char");
}


inline void operator >>=(const SerializationInfo& si, short& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, short n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, unsigned short& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, unsigned short n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, int& n)
{
    si.getValue(n);
}



inline void operator <<=(SerializationInfo& si, const int& n)
{
    si.setValue(n);
    si.setTypeName( Pt::LiteralPtr<char>("int") );
}


inline void operator >>=(const SerializationInfo& si, unsigned int& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, unsigned int n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, long& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, long n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, unsigned long& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, unsigned long n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, long long& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, long long n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, unsigned long long& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, unsigned long long n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, float& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, float n)
{
    si.setValue(n);
    si.setTypeName("double");
}


inline void operator >>=(const SerializationInfo& si, double& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, double n)
{
    si.setValue(n);
    si.setTypeName("double");
}


inline void operator <<=(SerializationInfo& si, const char* str)
{
    si.setValue(str);
    si.setTypeName("string");
}


inline void operator >>=(const SerializationInfo& si, std::string& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, const std::string& n)
{
    si.setValue(n);
    si.setTypeName("string");
}


inline void operator >>=(const SerializationInfo& si, Pt::String& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, const Pt::String& n)
{
    si.setValue(n);
    si.setTypeName("string");
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

    si.setTypeName("list");
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

    si.setTypeName("deque");
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

    si.setTypeName("set");
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

    si.setTypeName("multiset");
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
    si.setTypeName("pair");
    si.addMember("first") <<= p.first;
    si.addMember("second") <<= p.second;
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

    si.setTypeName("map");
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

    si.setTypeName("multimap");
    si.setSequence();
}

} // namespace Pt

#endif
