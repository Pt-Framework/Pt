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


class SerializationInfo
{
    typedef std::vector<SerializationInfo*> Nodes;

    public:
        enum Category {
            Void = 0, Value = 1, Object = 2, Array = 3, Reference = 4
        };

        class ConstIterator;

    public:
        SerializationInfo();

        explicit SerializationInfo(SerializationInfo& parent);

        ~SerializationInfo();

        Category category() const;

        SerializationInfo* parent();

        const SerializationInfo* parent() const;

        const std::string& typeName() const;

        void setTypeName(const std::string& type);

        const std::string& name() const;

        void setName(const std::string& name);

        void setValue(const Pt::Variant& value);

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

        const Pt::String& toString() const
        {
            return _value.str();
        }

        template <typename T>
        SerializationInfo& addValue(const std::string& name, const T& value)
        {
            SerializationInfo& info = this->addValue(name);
            info.setValue(value);
            return info;
        }

        SerializationInfo& addValue(const std::string& name);

        ConstIterator begin() const;

        ConstIterator end() const;

        const SerializationInfo& getObjectData(const std::string& name) const;

        template <typename T>
        T getValue(const std::string& name) const
        {
            T value;
            const SerializationInfo& info = this->getObjectData(name);
            info.toValue(value);
            return value;
        }

        template <typename T>
        void getValue(const std::string& name, T& value) const
        {
            const SerializationInfo& info = this->getObjectData(name);
            return info.toValue(value);
        }

        /** @brief Find object data by name

            This method returns the data for an object with the name \a name.
            or null if it is not present.
        */
        const SerializationInfo* findObjectData(const std::string& name) const;

        /** @brief Find object data by name

            This method returns the data for an object with the name \a name.
            or null if it is not present.
        */
        SerializationInfo* findObjectData(const std::string& name);

        size_t memberCount() const;

    private:
        SerializationInfo* _parent;
        Category _category;
        std::string _name;
        std::string _type;
        Pt::Variant _value;
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
inline void get(const Pt::SerializationInfo& si, T& x)
{
    x.deserialize(si);
}


template <typename T>
inline void put(Pt::SerializationInfo& si, const T& x)
{
    x.serialize(si);
}


inline void get(const SerializationInfo& si, bool& n)
{
    si.toValue(n);
}


inline void put(SerializationInfo& si, bool n)
{
    si.setValue(n);
    si.setTypeName("bool");
}


inline void get(const SerializationInfo& si, int& n)
{
    si.toValue(n);
}


inline void put(SerializationInfo& si, int n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void get(const SerializationInfo& si, float& n)
{
    si.toValue<float>(n);
}


inline void put(SerializationInfo& si, float n)
{
    si.setValue(n);
    si.setTypeName("float");
}


inline void get(const SerializationInfo& si, double& n)
{
    si.toValue<double>(n);
}


inline void put(SerializationInfo& si, double n)
{
    si.setValue(n);
    si.setTypeName("double");
}


inline void get(const SerializationInfo& si, std::string& n)
{
    si.toValue<std::string>(n);
}


inline void put(SerializationInfo& si, const std::string& n)
{
    si.setValue(n);
    si.setTypeName("string");
}


inline void get(const SerializationInfo& si, Pt::String& n)
{
    si.toValue<Pt::String>(n);
}


inline void put(SerializationInfo& si, const Pt::String& n)
{
    si.setValue(n);
    si.setTypeName("string");
}


template <typename T>
inline void get(const SerializationInfo& si, std::vector<T>& vec)
{
    for(SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
    {
        vec.resize( vec.size() + 1 );
        get( *it, vec.back() );
    }
}


template <typename T>
inline void put(SerializationInfo& si, const std::vector<T>& vec)
{
    typename std::vector<T>::const_iterator it;
    for(it = vec.begin(); it != vec.end(); ++it)
    {
        SerializationInfo& elem = si.addValue("");
        put(elem, *it);
    }
}

} // namespace Pt


#endif
