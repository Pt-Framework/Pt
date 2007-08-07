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
/*

template <typename T>
struct SerializationTraits
{
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

    typedef void (*Serialize)(SerializationInfo&, const Any&);
    typedef void (*Deserialize)(const SerializationInfo&, Any&);

    public:
        ~TypeFactory();

        void registerType(const char* type, Serialize s, Deserialize d)
        {
            _serializer[type] = s;
            _deserializer[type] = d;
        }

        Any create(const Pt::SerializationInfo& si);

        Any create(const Pt::SerializationInfo& si, const std::string& typeName);

        void serialize(Pt::SerializationInfo& si, const Pt::Any& a);

    protected:
        TypeFactory();

    private:
        std::map<std::string, Serialize> _serializer;
        std::map<std::string, Deserialize> _deserializer;
        void* _reserved;
};


// RegisterSerializable
template <typename T>
class RegisterType
{
    static bool registered;

    public:
        RegisterType()
        {
            if( !registered )
            {
                TypeFactory::instance().registerType( TypeTraits<T>::typeName(),
                                                      &SerializationTraits<T>::serialize,
                                                      &SerializationTraits<T>::deserialize );
                registered = true;
            }
        }
};


template <typename T>
bool RegisterType<T>::registered = false;

static RegisterType<bool> assertBool;
static RegisterType<int> assertInt;
static RegisterType<float> assertFloat;
static RegisterType<double> assertDouble;
static RegisterType<std::string> assertStdString;
static RegisterType<Pt::String> assertPtString;
*/
} // namespace Pt

#endif
