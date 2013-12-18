/*
 * Copyright (C) 2008 by Marc Boris Duerner
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
#ifndef Pt_Composer_h
#define Pt_Composer_h

#include <Pt/Api.h>
#include <Pt/SerializationInfo.h>
#include <Pt/SerializationContext.h>
#include <cstddef>

namespace Pt {

/** @brief Composes types during serialization.

    @ingroup Serialization
*/
class Composer
{
    public:
        /** @brief Destructor.
        */
        virtual ~Composer()
        {}

        /** @brief Sets the parent composer.
        */
        void setParent(Composer* parent)
        { _parent = parent; }

        /** @brief Returns the parent composer.
        */
        Composer* parent() const
        { return _parent; }

        /** @brief Sets the type name of the type to compose.

            This is only supported by formats that save typename information.
        */
        void setTypeName(const std::string& type)
        { onSetTypeName( type.c_str(), type.size() ); }
        
        /** @brief Sets the type name of the type to compose.

            This is only supported by formats that save typename information.
        */
        void setTypeName(const char* type, std::size_t len)
        { onSetTypeName(type, len); }

        /** @brief Sets the reference id of the type to compose.

            This is only supported by formats that support references.
        */
        void setId(const std::string& id)
        { onSetId( id.c_str(), id.size() ); }
        
        /** @brief Sets the reference id of the type to compose.

            This is only supported by formats that support references.
        */
        void setId(const char* id, std::size_t len)
        { onSetId(id, len); }

        /** @brief Composes a string value.
        */
        void setString(const Pt::String& value)
        { onSetString( value.c_str(), value.size() ); }

        /** @brief Composes a string value.
        */
        void setString(const Pt::Char* value, std::size_t len)
        { onSetString(value, len); }

        /** @brief Composes a binary value.
        */
        void setBinary(const char* data, std::size_t length)
        { onSetBinary(data, length); }

        /** @brief Composes a char value.
        */
        void setChar(const Pt::Char& ch)
        { onSetChar(ch); }

        /** @brief Composes a boolean value.
        */
        void setBool(bool value)
        { onSetBool(value); }

        /** @brief Composes a signed integer type.

            There is only one method for all sizes of signed integer types,
            because that type information is not required for composition. 
        */
        void setInt(Pt::int64_t value)
        { onSetInt(value); }
        
        /** @brief Composes an unsigned integer type.

            There is only one method for all sizes of unsigned integer types,
            because that type information is not required for composition. 
        */
        void setUInt(Pt::int64_t value)
        { onSetUInt(value); }

        /** @brief Composes a float value.
        */
        void setFloat(long double value)
        { onSetFloat(value); }

        /** @brief Composes a reference.
        */
        void setReference(const std::string& id)
        { onSetReference(id.c_str(), id.size()); }
        
        /** @brief Composes a reference.
        */
        void setReference(const char* id, std::size_t len)
        { onSetReference(id, len); }

        /** @brief Begins composition of a struct member.
        */
        Composer* beginMember(const std::string& name)
        { return onBeginMember( name.c_str(), name.size() ); }
        
        /** @brief Begins composition of a struct member.
        */
        Composer* beginMember(const char* name, std::size_t len)
        { return onBeginMember(name, len); }

        /** @brief Begins composition of a sequence member.
        */
        Composer* beginElement()
        { return onBeginElement(); }

        /** @brief Begins composition of a dict key.

            Returns a composer for the key of the dict element. A subsequent
            call of beginDictValue returns a composer to the value of the
            dict element. For both finish() has to be called, after the value
            was completely composed.
        */
        Composer* beginDictElement()
        { return onBeginDictElement(); }

        /** @brief Begins composition of a dict value.
        */
        Composer* beginDictValue()
        { return onBeginDictValue(); }

        /** @brief Finishes composition of a struct or sequence member.
        */
        Composer* finish()
        { return onFinish(); }

    protected:
        /** @brief Constructor.
        */
        Composer()
        : _parent(0)
        {}

        virtual void onSetTypeName(const char* type, std::size_t len)
        {}

        virtual void onSetId(const char* id, std::size_t len) = 0;

        virtual void onSetString(const Pt::Char* value, std::size_t len)
        { throw SerializationError("unexpected string value"); }

        virtual void onSetBinary(const char* data, std::size_t length)
        { throw SerializationError("unexpected binary value"); }

        virtual void onSetChar(const Pt::Char& ch)
        { throw SerializationError("unexpected char value"); }

        virtual void onSetBool(bool value)
        { throw SerializationError("unexpected bool value"); }

        virtual void onSetInt(Pt::int64_t value)
        { throw SerializationError("unexpected integer value"); }
        
        virtual void onSetUInt(Pt::uint64_t value)
        { throw SerializationError("unexpected unsigned value"); }

        virtual void onSetFloat(long double value)
        { throw SerializationError("unexpected float value"); }

        virtual void onSetReference(const char* id, std::size_t len)
        { throw SerializationError("unexpected reference"); }

        virtual Composer* onBeginMember(const char* name, std::size_t len)
        { throw SerializationError("unexpected struct"); }

        virtual Composer* onBeginElement()
        { throw SerializationError("unexpected sequence"); }

        virtual Composer* onBeginDictElement()
        { throw SerializationError("unexpected dict"); }

        virtual Composer* onBeginDictKey()
        { throw SerializationError("unexpected dict"); }

        virtual Composer* onBeginDictValue()
        { throw SerializationError("unexpected dict"); }

        virtual Composer* onFinish()
        { return _parent; }

    private:
        Composer* _parent;
};

/** @brief Manages the composition of types during serialization.

    @ingroup Serialization
*/
template <typename T>
class BasicComposer : public Composer
{
    public:
        BasicComposer(SerializationContext* context = 0)
        : _type(0)
        , _si(context)
        , _current(&_si)
        { }

        void begin(T& type)
        {
            if(_type)
            {
                _si.clear();
            }

            _type = &type;
            _current = &_si;
        }

    protected:
        void onSetId(const char* id, std::size_t len)
        {
            _current->setId(id, len);
        }

        void onSetTypeName(const char* type, std::size_t len)
        {
            _current->setTypeName(type, len);
        }

        void onSetString(const Pt::Char* value, std::size_t len)
        {
            _current->setString(value, len);
        }

        void onSetBinary(const char* data, std::size_t length)
        {
            _current->setBinary(data, length);
        }

        void onSetChar(const Pt::Char& ch)
        {
            _current->setChar(ch);
        }

        void onSetBool(bool value)
        {
            _current->setBool(value);
        }

        void onSetInt(Pt::int64_t value)
        {
            _current->setInt64(value);
        }

        void onSetUInt(Pt::uint64_t value)
        {
            _current->setUInt64(value);
        }

        void onSetFloat(long double value)
        {
            _current->setDouble(value);
        }

        void onSetReference(const char* id, std::size_t len)
        {
           _current->setReference(id, len);
        }

        Composer* onBeginMember(const char* name, std::size_t len)
        {
            SerializationInfo& child = _current->addMember(name, len);
            _current = &child;
            return this;
        }

        Composer* onBeginElement()
        {
            SerializationInfo& child = _current->addElement();
            _current = &child;
            return this;
        }

        Composer* onBeginDictElement()
        {
            SerializationInfo& child = _current->addDictElement();
            _current = &child;
            return this;
        }

        Composer* onBeginDictKey()
        {
            SerializationInfo& child = _current->addDictKey();
            _current = &child;
            return this;
        }

        Composer* onBeginDictValue()
        {
            SerializationInfo& child = _current->addDictValue();
            _current = &child;
            return this;
        }

        Composer* onFinish()
        {
            if( ! _current->parent() )
            {
                *_current >> Pt::load() >>= *_type;
                return parent();
            }

            _current = _current->parent();
            return this;
        }

    private:
        T* _type;
        Pt::SerializationInfo _si;
        Pt::SerializationInfo* _current;
};

} // namespace Pt

#endif
