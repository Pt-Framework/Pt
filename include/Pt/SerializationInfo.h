/***************************************************************************
 *   Copyright (C) 2005-2007 by Dr. Marc Boris Duener                      *
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
#include <Pt/Variant.h>
#include <Pt/Exception.h>
#include <Pt/NonCopyable.h>
#include <vector>


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

        //! @brief Destructor
        ~SerializationError() throw();
};


class SerializationInfo : protected Pt::NonCopyable
{
    typedef std::vector<SerializationInfo*> Nodes;

    public:
        enum Category {
            Void = 0, Value = 1, Object = 2, Array = 4, Reference = 8
        };

        class ConstIterator;

    public:
        SerializationInfo();

        ~SerializationInfo();

        Category category() const;

        void setCategory(Category cat);

        SerializationInfo* parent();

        const SerializationInfo* parent() const;

        const std::string& typeName() const;

        void setTypeName(const std::string& type);

        const std::string& name() const;

        void setName(const std::string& name);

        void setValue(const Pt::Variant& value);

        void setId(const std::string& id)
        {
            _id = id;
        }

        const std::string& id() const
        {
            return _id;
        }

        void setReference(void* ref)
        {
            _value = ref;
            _category = Reference;
        }


        template <typename T>
        void resolve(T*& type) const
        {
            _value = (void**)&type;
            type = 0;
        }

        template <typename T>
        T toValue() const
        {
            T value;
            _value.get(value);
            return value;
        }

        template <typename T>
        void toValue(T& value) const
        {
            _value.get(value);
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

        size_t memberCount() const;

        ConstIterator begin() const;

        ConstIterator end() const;

    private:
        SerializationInfo(const SerializationInfo&) {}
        SerializationInfo* _parent;
        Category _category;
        std::string _name;
        std::string _id;
        std::string _type;
        mutable Pt::Variant _value;
        Nodes _nodes;
};


class SerializationInfo::ConstIterator
{
    public:
        ConstIterator();

        ConstIterator(const ConstIterator& other);

        ConstIterator(SerializationInfo* const* info);

        ConstIterator& operator=(const ConstIterator& other);

        ConstIterator& operator++();

        const SerializationInfo& operator*() const;

        const SerializationInfo* operator->() const;

        bool operator!=(const ConstIterator& other) const;

    private:
        SerializationInfo* const* _info;
};


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


inline void operator >>=(const SerializationInfo& si, int& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, int n)
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
    si.setTypeName("float");
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
    {
        SerializationInfo& elem = si.addMember("");
        elem <<= *it;
    }
}

} // namespace Pt


#endif
