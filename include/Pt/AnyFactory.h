/***************************************************************************
 *   Copyright (C) 2007 by Marc Duerner                                    *
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
#ifndef Pt_TypeFactory_h
#define Pt_TypeFactory_h

#include <Pt/Any.h>
#include <Pt/SerializationInfo.h>

namespace Pt {


template <typename T>
struct SerializeAny
{
    static const char* typeName()
    { return TypeTraits<T>::typeName(); }

    static void serialize(Pt::SerializationInfo& si, const Pt::Any& a)
    {
        T t = any_cast<T>(a);
        put(si, t);
    }

    static void deserialize(const Pt::SerializationInfo& si, Any& any)
    {
        T value = T();
        get(si, value);
        any = value;
    }
};


class TypeFactory : public Singleton<TypeFactory>
{
    friend class Singleton<TypeFactory>;

    public:
        ~TypeFactory();

        template <typename T>
        void assertType()
        {
            const char* type = SerializeAny<T>::typeName();
            _serializer[type] = &SerializeAny<T>::serialize;
            _deserializer[type] = &SerializeAny<T>::deserialize;
        }

        Any create(const Pt::SerializationInfo& si);

        Any create(const Pt::SerializationInfo& si, const std::string& typeName);

        void serialize(Pt::SerializationInfo& si, const Pt::Any& a);

    protected:
        TypeFactory();

        bool hasBuilder(const std::string& type) const;

    private:
        typedef void (*Serialize)(SerializationInfo&, const Any&);
        std::map<std::string, Serialize> _serializer;

        typedef void (*Deserialize)(const SerializationInfo&, Any&);
        std::map<std::string, Deserialize> _deserializer;

        void* _reserved;
};


template <typename T>
class AssertType
{
    public:
        AssertType()
        { TypeFactory::instance().assertType<T>(); }
};


static AssertType<bool> assertBool;
static AssertType<int> assertInt;
static AssertType<float> assertFloat;
static AssertType<double> assertDouble;
static AssertType<std::string> assertStdString;
static AssertType<Pt::String> assertPtString;

} // namespace Pt

#endif
