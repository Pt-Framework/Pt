/***************************************************************************
 *   Copyright (C) 2005-2008 by Dr. Marc Boris Duerner                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef Pt_SerializationInfo_h
#define Pt_SerializationInfo_h

#include <Pt/String.h>
#include <Pt/Convert.h>
#include <Pt/NonCopyable.h>
#include <vector>
#include <typeinfo>

namespace Pt {

/** @brief Error during serialization of a type

    This Exception indicates a error during serialization caused by
    missing or invalid object attributes.
*/
class PT_API SerializationError : public std::logic_error
{
    public:
        /** @brief Construct with message and source-info
        */
        SerializationError(const std::string& msg, const SourceInfo& si);

        SerializationError(const char* msg);

        //! @brief Destructor
        ~SerializationError() throw()
        {}
};


class PT_API SerializationInfo
{
    typedef std::vector<SerializationInfo> Nodes;

    public:
        enum Category {
            Void = 0, Value = 1, Object = 2, Reference = 4
        };

        class Iterator;
        class ConstIterator;

    public:
        SerializationInfo();

        SerializationInfo(const SerializationInfo& si);

        ~SerializationInfo()
        {}

        void reserve(size_t n);

        Category category() const
        {
            return _category;
        }

        void setCategory(Category cat)
        {
            _category = cat;
        }

        SerializationInfo* parent()
        {
            return _parent;
        }

        const SerializationInfo* parent() const
        {
            return _parent;
        }

        const std::string& typeName() const
        {
            return _type;
        }

        void setTypeName(const std::string& type)
        {
            _type = type;
        }

        const std::string& name() const
        {
            return _name;
        }

        void setName(const std::string& name)
        {
            _name = name;
        }

        void setId(const std::string& id)
        {
            _id = id;
        }

        const std::string& id() const
        {
            return _id;
        }

        void setReference(void* ref);

        SerializationInfo& addReference(const std::string& name, void* ref);

        template <typename T>
        void toReference(T*& type) const
        {
            this->getReference( reinterpret_cast<void*&>(type), typeid(T) );
        }

        template <typename T>
        void getReference(const std::string& name, T*& type) const
        {
            this->getMember(name).getReference( reinterpret_cast<void*&>(type), typeid(T) );
        }

        template <typename T>
        void setValue(const T& value)
        {
            convert(_value, value);
            _category = Value;
        }

        template <typename T>
        T toValue() const
        {
            return convert<T>(_value);
        }

        template <typename T>
        void toValue(T& value) const
        {
            convert(value, _value);
        }

        const Pt::String& toString() const;

        template <typename T>
        SerializationInfo& addValue(const std::string& name, const T& value)
        {
            SerializationInfo& info = this->addMember(name);
            info.setValue(value);
            return info;
        }

        SerializationInfo& addMember(const std::string& name);

        const SerializationInfo& getMember(const std::string& name) const;

        // This is needed for some compilers (GCC 3.x) to allow access to
        // method 'T getValue(const std::string& name) const' below.
        template <typename T>
        friend T getValue(const std::string& name, SerializationInfo* si);

        template <typename T>
        T getValue(const std::string& name) const
        {
            T value;
            const SerializationInfo& info = this->getMember(name);
            info.toValue(value);
            return value;
        }

        template <typename T>
        void getValue(const std::string& name, T& value) const
        {
            const SerializationInfo& info = this->getMember(name);
            return info.toValue(value);
        }

        /** @brief Find object data by name

            This method returns the data for an object with the name \a name.
            or null if it is not present.
        */
        const SerializationInfo* findMember(const std::string& name) const;

        /** @brief Find object data by name

            This method returns the data for an object with the name \a name.
            or null if it is not present.
        */
        SerializationInfo* findMember(const std::string& name);

        size_t memberCount() const
        {
            return _nodes.size();
        }

        Iterator begin();

        Iterator end();

        ConstIterator begin() const;

        ConstIterator end() const;

        SerializationInfo& operator =(const SerializationInfo& si);

        void* fixupAddr() const;

        const std::type_info& fixupInfo() const;

    protected:
        void getReference(void*& type, const std::type_info& ti) const;

        void setParent(SerializationInfo& si)
        { _parent = &si; }

    private:
        SerializationInfo* _parent;
        Category _category;
        std::string _name;
        std::string _type;
        std::string _id;
        mutable void* _fixupAddr; // only refs
        mutable const std::type_info* _fixupInfo; // only refs
        Pt::String _value;        // values/refs
        Nodes _nodes;             // objects/arrays
};


class SerializationInfo::Iterator
{
    public:
        Iterator();

        Iterator(const Iterator& other);

        Iterator(SerializationInfo* info);

        Iterator& operator=(const Iterator& other);

        Iterator& operator++();

        SerializationInfo& operator*();

        SerializationInfo* operator->();

        bool operator!=(const Iterator& other) const;

    private:
        SerializationInfo* _info;
};


class SerializationInfo::ConstIterator
{
    public:
        ConstIterator();

        ConstIterator(const ConstIterator& other);

        ConstIterator(const SerializationInfo* info);

        ConstIterator& operator=(const ConstIterator& other);

        ConstIterator& operator++();

        const SerializationInfo& operator*() const;

        const SerializationInfo* operator->() const;

        bool operator!=(const ConstIterator& other) const;

    private:
        const SerializationInfo* _info;
};


inline SerializationInfo::Iterator::Iterator()
: _info(0)
{}


inline SerializationInfo::Iterator::Iterator(const Iterator& other)
: _info(other._info)
{}


inline SerializationInfo::Iterator::Iterator(SerializationInfo* info)
: _info(info)
{}


inline SerializationInfo::Iterator& SerializationInfo::Iterator::operator=(const Iterator& other)
{
    _info = other._info;
    return *this;
}


inline SerializationInfo::Iterator& SerializationInfo::Iterator::operator++()
{
    ++_info;
    return *this;
}


inline SerializationInfo& SerializationInfo::Iterator::operator*()
{
    return *_info;
}


inline SerializationInfo* SerializationInfo::Iterator::operator->()
{
    return _info;
}


inline bool SerializationInfo::Iterator::operator!=(const Iterator& other) const
{
    return _info != other._info;
}


inline SerializationInfo::ConstIterator::ConstIterator()
: _info(0)
{}


inline SerializationInfo::ConstIterator::ConstIterator(const ConstIterator& other)
: _info(other._info)
{}


inline SerializationInfo::ConstIterator::ConstIterator(const SerializationInfo* info)
: _info(info)
{}


inline SerializationInfo::ConstIterator& SerializationInfo::ConstIterator::operator=(const ConstIterator& other)
{
    _info = other._info;
    return *this;
}


inline SerializationInfo::ConstIterator& SerializationInfo::ConstIterator::operator++()
{
    ++_info;
    return *this;
}


inline const SerializationInfo& SerializationInfo::ConstIterator::operator*() const
{
    return *_info;
}


inline const SerializationInfo* SerializationInfo::ConstIterator::operator->() const
{
    return _info;
}


inline bool SerializationInfo::ConstIterator::operator!=(const ConstIterator& other) const
{
    return _info != other._info;
}


template <typename T>
void operator >>=(const Pt::SerializationInfo& si, T& x)
{
    x.deserialize(si);
}


template <typename T>
void operator <<=(Pt::SerializationInfo& si, const T& x)
{
    x.serialize(si);
}


inline void operator >>=(const SerializationInfo& si, bool& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, bool n)
{
    si.setValue(n);
    si.setTypeName("bool");
}


inline void operator >>=(const SerializationInfo& si, signed char& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, signed char n)
{
    si.setValue(n);
    si.setTypeName("char");
}


inline void operator >>=(const SerializationInfo& si, unsigned char& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, unsigned char n)
{
    si.setValue(n);
    si.setTypeName("char");
}


inline void operator >>=(const SerializationInfo& si, char& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, char n)
{
    si.setValue(n);
    si.setTypeName("char");
}


inline void operator >>=(const SerializationInfo& si, short& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, short n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, unsigned short& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, unsigned short n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, int& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, int n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, unsigned int& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, unsigned int n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, long& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, long n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, unsigned long& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, unsigned long n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, float& n)
{
    si.toValue<float>(n);
}


inline void operator <<=(SerializationInfo& si, float n)
{
    si.setValue(n);
    si.setTypeName("double");
}


inline void operator >>=(const SerializationInfo& si, double& n)
{
    si.toValue<double>(n);
}


inline void operator <<=(SerializationInfo& si, double n)
{
    si.setValue(n);
    si.setTypeName("double");
}


inline void operator >>=(const SerializationInfo& si, std::string& n)
{
    si.toValue<std::string>(n);
}


inline void operator <<=(SerializationInfo& si, const std::string& n)
{
    si.setValue(n);
    si.setTypeName("string");
}


inline void operator >>=(const SerializationInfo& si, Pt::String& n)
{
    si.toValue<Pt::String>(n);
}


inline void operator <<=(SerializationInfo& si, const Pt::String& n)
{
    si.setValue(n);
    si.setTypeName("string");
}


template <typename T>
inline void operator >>=(const SerializationInfo& si, std::vector<T>& vec)
{
    for(SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
    {
        vec.resize( vec.size() + 1 );
        *it >>=  vec.back();
    }
}


template <typename T>
inline void operator <<=(SerializationInfo& si, const std::vector<T>& vec)
{
    typename std::vector<T>::const_iterator it;

    for(it = vec.begin(); it != vec.end(); ++it)
        si <<= *it;
}

inline void operator >>=(const SerializationInfo& si, std::vector<int>& vec)
{
    for(SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
    {
        vec.resize( vec.size() + 1 );
        *it >>=  vec.back();
    }
}

inline void operator <<=(SerializationInfo& si, const std::vector<int>& vec)
{
    std::vector<int>::const_iterator it;

    for(it = vec.begin(); it != vec.end(); ++it)
    {
        SerializationInfo& newSi = si.addMember("int");
        newSi <<= *it;
    }
}

} // namespace Pt


#endif
