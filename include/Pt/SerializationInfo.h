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
#include <Pt/Convert.h>
#include <Pt/FixupInfo.h>
#include <Pt/SerializationError.h>
#include <typeinfo>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <deque>

namespace Pt {

class SerializationSurrogate;
class SerializationContext;
class SerializationNode;
class Formatter;

/** @brief Represents arbitrary types during serialization.
*/
class PT_API SerializationInfo
{
    public:
        typedef void (*FixupHandler)(void* fixme,
                                     void* target, const std::type_info& targetType);

        enum Category {
            Void = 0, Scalar = 1, Struct = 2, Sequence = 3, Reference = 4, Context = 5
        };

        class Iterator;
        class ConstIterator;

    public:
        SerializationInfo()
        : _node(0)
        , _context(0)
        , _parent(0)
        , _next(0)
        , _bound(0)
        { }

        explicit SerializationInfo(SerializationContext* context)
        : _node( 0 )
        , _context(context)
        , _parent(0)
        , _next(0)
        , _bound(0)
        { }

        ~SerializationInfo();

    private:
        SerializationInfo(const SerializationInfo& si)
        {}

        SerializationInfo& operator=(const SerializationInfo& si)
        { return *this; }

    public:
        void clear();

        SerializationNode* releaseNode();

        Category category() const;

        void setCategory(Category category);

        void setContextual()
        { this->setCategory(Context); }

        SerializationContext* context() const
        { return _context; }

        void setContext(SerializationContext* context);

        SerializationSurrogate getSurrogate(const char* name) const;

        void rebind(void* obj) const;

        void rebindFixup(void* obj) const;

        SerializationInfo* parent()
        { return _parent; }

        const SerializationInfo* parent() const
        { return _parent; }

        const std::string& typeName() const
        { return _type; }

        void setTypeName(const std::string& type)
        { _type = type; }

        void setTypeName(const char* type)
        { _type = type; }

        const std::string& name() const
        { return _name; }

        void setName(const std::string& name)
        { _name = name; }

        void setName(const char* name)
        { _name = name; }

        const std::string& id() const
        { return _id; }

        void setId(const std::string& id)
        { _id = id; }

        void setId(const char* id)
        { _id = id; }

        /** @brief Returns the content as string.
        */
        const Pt::String& toString() const;

        /** @brief Deserialization of flat child value types
        */
        template <typename T>
        void getValue(T& value) const
        {
            convert( value, this->toString() );
        }

        /** @brief Serialization of flat data-types
        */
        template <typename T>
        void setValue(const T& value)
        {
            Pt::String* str = initString();
            if(str)
                convert( *str, value );
        }

        // TODO setValue/getValue for String
        void getValue(bool& b) const;

        void setValue(bool b);

        void getValue(short& s) const;

        void setValue(short s)
        { this->setValue( long(s) ); }

        void getValue(int& i) const;

        void setValue(int i)
        { this->setValue( long(i) ); }

        void getValue(long& l) const;

        void setValue(long l);

        void getValue(unsigned short& us) const;

        void setValue(unsigned short us)
        { this->setValue( static_cast<unsigned long>(us) ); }

        void getValue(unsigned int& ui) const;

        void setValue(unsigned int ui)
        { this->setValue( static_cast<unsigned long>(ui) ); }

        void getValue(unsigned long& ul) const;

        void setValue(unsigned long ul);

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
        SerializationInfo& addMember(const std::string& name);

        /** @brief Serialization of member data
        */
        SerializationInfo& addElement();

        /** @brief Deserialization of member data
        */
        const SerializationInfo& getMember(const std::string& name) const;

        /** @brief Find member data by name

            This method returns the data for an object with the name \a name.
            or null if it is not present.
        */
        const SerializationInfo* findMember(const std::string& name) const;

        /** @brief Find member data by name

            This method returns the data for an object with the name \a name.
            or null if it is not present.
        */
        SerializationInfo* findMember(const std::string& name);

        size_t memberCount() const;

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
        void loadReference(T& fixme) const
        {
            this->load(&fixme, FixupThunk<T>::fixupReference);
        }

        /** @brief Deserialization of weak pointers
        */
        template <typename T>
        void loadPointer(T*& fixme) const
        {
            this->load(&fixme, FixupThunk<T>::fixupPointer);
        }

        bool beginFormat(Formatter& formatter);

        void endFormat(Formatter& formatter);

        void format(Formatter& formatter);

    public:
        /** @internal DEPRECATED
        */
        template <typename T>
        void toValue(T& value) const
        { this->getValue(value); }

        /** @internal DEPRECATED
        */
        template <typename T>
        T toValue() const
        { return convert<T>( this->toString() ); }

        /** @internal DEPRECATED
            This is needed as a workaround for some compilers (GCC 3.x) to
            allow access to 'T getValue(const std::string& name) const'.
         */
        template <typename T>
        friend T getValue(const std::string& name, SerializationInfo* si);

        /** @internal DEPRECATED
        */
        template <typename T>
        T getValue(const std::string& name) const
        {
            T value;
            const SerializationInfo& info = this->getMember(name);
            info.toValue(value);
            return value;
        }

        /** @internal DEPRECATED
        */
        template <typename T>
        void addValue(const std::string& name, const T& value)
        { this->addMember(name) <<= value; }

        /** @internal DEPRECATED
        */
        template <typename T>
        void getValue(const std::string& name, T& value) const
        { this->getMember(name) >>= value; }

        SerializationInfo* sibling() const
        { return _next; }

        void setSibling(SerializationInfo* si)
        { _next = si; }

    protected:
        void load(void* fixme, FixupHandler fh) const;

        Pt::String* initString();

    private:
        mutable SerializationNode* _node;
        SerializationContext* _context;
        SerializationInfo* _parent;
        SerializationInfo* _next;
        std::string _name;
        std::string _type;
        std::string _id;
        mutable const void* _bound;
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

        SerializationInfo& operator*()
        { return *_si; }

        SerializationInfo* operator->()
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

        const SerializationInfo& operator*()
        { return *_si; }

        const SerializationInfo* operator->()
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


struct save
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


template <typename T>
struct Save
{
    explicit Save(const T& t)
    : type(&t)
    {}

    const T* type;
};


template <typename T>
inline Save<T> operator<<= (const save&, const T& type)
{
    return Save<T>(type);
}


template <typename T>
inline void operator<<= (SerializationInfo& si, const Save<T>& sv)
{
    SaveInfo info(si);
    info <<= *(sv.type);
}


template <typename T>
inline void operator<<= (SaveInfo& si, const T& type)
{
    if( ! si.save(type) )
    {
        si.out() <<= type;
    }
}


struct load
{};


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


template <typename T>
struct Load
{
    explicit Load(T& t)
    : type(&t)
    {}

    T* type;
};


template <typename T>
inline Load<T> operator >>= (const load&, T& type)
{
    return Load<T>(type);
}


template <typename T>
inline void operator >>=(const SerializationInfo& si, const Load<T>& ld)
{
    LoadInfo info(si);
    info >>= *(ld.type);
}


template <typename T>
inline void operator >>=(const LoadInfo& li, T& type)
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
    si.setTypeName("int");
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
        *it >>= Pt::load() >>= vec.back();
    }
}


template <typename T, typename A>
inline void operator <<=(SerializationInfo& si, const std::vector<T, A>& vec)
{
    typename std::vector<T, A>::const_iterator it;

    for(it = vec.begin(); it != vec.end(); ++it)
    {
        si.addElement() <<= Pt::save() <<= *it;
    }

    si.setTypeName("array");
    si.setCategory(SerializationInfo::Sequence);
}


template <typename T, typename A>
inline void operator >>=(const SerializationInfo& si, std::list<T, A>& list)
{
    list.clear();
    for(SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
    {
        list.resize( list.size() + 1 );
        *it >>= Pt::load() >>= list.back();
    }
}


template <typename T, typename A>
inline void operator <<=(SerializationInfo& si, const std::list<T, A>& list)
{
    typename std::list<T, A>::const_iterator it;

    for(it = list.begin(); it != list.end(); ++it)
    {
        si.addElement() <<= Pt::save() <<= *it;
    }

    si.setTypeName("list");
    si.setCategory(SerializationInfo::Sequence);
}


template <typename T, typename A>
inline void operator >>=(const SerializationInfo& si, std::deque<T, A>& deque)
{
    deque.clear();
    for(SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
    {
        // NOTE: push_back does not invalidate references to elements
        deque.push_back( T() );
        *it >>= Pt::load() >>= deque.back();
    }
}


template <typename T, typename A>
inline void operator <<=(SerializationInfo& si, const std::deque<T, A>& deque)
{
    typename std::deque<T, A>::const_iterator it;

    for(it = deque.begin(); it != deque.end(); ++it)
    {
        si.addElement() <<= Pt::save() <<= *it;
    }

    si.setTypeName("deque");
    si.setCategory(SerializationInfo::Sequence);
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
        si.addElement() <<= Pt::save() <<= *it;
    }

    si.setTypeName("set");
    si.setCategory(SerializationInfo::Sequence);
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
        si.addElement() <<= Pt::save() <<= *it;
    }

    si.setTypeName("multiset");
    si.setCategory(SerializationInfo::Sequence);
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
        si.getMember("first") >>= k;

        std::pair<K, V> elem( k, V() );
        pos = map.insert(elem);
        if( pos.second )
            si.getMember("second") >>= Pt::load() >>= pos.first->second;
    }
}


template <typename K, typename V, typename P, typename A>
inline void operator <<=(SerializationInfo& si, const std::map<K, V, P, A>& map)
{
    typename std::map<K, V, P, A>::const_iterator it;

    for(it = map.begin(); it != map.end(); ++it)
    {
        si.addElement() <<= Pt::save() <<= *it;
    }

    si.setTypeName("map");
    si.setCategory(SerializationInfo::Sequence);
}


template <typename K, typename V, typename P, typename A>
inline void operator >>=(const SerializationInfo& si, std::multimap<K, V, P, A>& multimap)
{
    typename std::multimap<K, V, P, A>::iterator mit;

    multimap.clear();
    for(SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
    {
        K k;
        si.getMember("first") >>= k;

        std::pair<K, V> elem( k, V() );
        mit = multimap.insert(elem);

        si.getMember("second") >>= Pt::load() >>= mit->second;
    }
}


template <typename T, typename C, typename P, typename A>
inline void operator <<=(SerializationInfo& si, const std::multimap<T, C, P, A>& multimap)
{
    typename std::multimap<T, C, P, A>::const_iterator it;

    for(it = multimap.begin(); it != multimap.end(); ++it)
    {
        si.addElement() <<= Pt::save() <<= *it;
    }

    si.setTypeName("multimap");
    si.setCategory(SerializationInfo::Sequence);
}

} // namespace Pt

#endif
