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

        /** @brief Begins composition of a dict key.
        */
        Composer* beginDictKey()
        { return onBeginDictKey(); }

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

        //! @brief Set type name.
        virtual void onSetTypeName(const char*, std::size_t)
        {}

        //! @brief Set reference ID.
        virtual void onSetId(const char* id, std::size_t len) = 0;

        //! @brief Compose a string value.
        virtual void onSetString(const Pt::Char*, std::size_t)
        { throw SerializationError("unexpected string value"); }

        //! @brief Compose a binary value.
        virtual void onSetBinary(const char*, std::size_t)
        { throw SerializationError("unexpected binary value"); }

        //! @brief Compose a character value.
        virtual void onSetChar(const Pt::Char&)
        { throw SerializationError("unexpected char value"); }

        //! @brief Compose a bool value.
        virtual void onSetBool(bool)
        { throw SerializationError("unexpected bool value"); }

        //! @brief Compose a integer value.
        virtual void onSetInt(Pt::int64_t)
        { throw SerializationError("unexpected integer value"); }
        
        //! @brief Compose a unsigned integer value.
        virtual void onSetUInt(Pt::uint64_t)
        { throw SerializationError("unexpected unsigned value"); }

        //! @brief Compose a floating point value.
        virtual void onSetFloat(long double)
        { throw SerializationError("unexpected float value"); }

        //! @brief Compose a reference.
        virtual void onSetReference(const char*, std::size_t)
        { throw SerializationError("unexpected reference"); }

        //! @brief Begin composition os a struct member.
        virtual Composer* onBeginMember(const char*, std::size_t)
        { throw SerializationError("unexpected struct"); }

        /** @brief Begins composition of a sequence member.
        */
        virtual Composer* onBeginElement()
        { throw SerializationError("unexpected sequence"); }

        /** @brief Begins composition of a dict key.
        */
        virtual Composer* onBeginDictElement()
        { throw SerializationError("unexpected dict"); }

        /** @brief Begins composition of a dict key.
        */
        virtual Composer* onBeginDictKey()
        { throw SerializationError("unexpected dict"); }

        /** @brief Begins composition of a dict value.
        */
        virtual Composer* onBeginDictValue()
        { throw SerializationError("unexpected dict"); }

        /** @brief Finishes composition of a struct or sequence member.
        */
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
        //! @brief Construct with context.
        BasicComposer(SerializationContext* context = 0)
        : _type(0)
        , _si(context)
        , _current(&_si)
        { }

        //! @brief Begin composing a type.
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
        // inherit docs
        void onSetId(const char* id, std::size_t len)
        {
            _current->setId(id, len);
        }

        // inherit docs
        void onSetTypeName(const char* type, std::size_t len)
        {
            _current->setTypeName(type, len);
        }

        void onSetString(const Pt::Char* value, std::size_t len)
        {
            _current->setString(value, len);
        }

        // inherit docs
        void onSetBinary(const char* data, std::size_t length)
        {
            _current->setBinary(data, length);
        }

        // inherit docs
        void onSetChar(const Pt::Char& ch)
        {
            _current->setChar(ch);
        }

        // inherit docs
        void onSetBool(bool value)
        {
            _current->setBool(value);
        }

        // inherit docs
        void onSetInt(Pt::int64_t value)
        {
            _current->setInt64(value);
        }

        // inherit docs
        void onSetUInt(Pt::uint64_t value)
        {
            _current->setUInt64(value);
        }

        // inherit docs
        void onSetFloat(long double value)
        {
            _current->setLongDouble(value);
        }

        // inherit docs
        void onSetReference(const char* id, std::size_t len)
        {
           _current->setReference(id, len);
        }

        // inherit docs
        Composer* onBeginMember(const char* name, std::size_t len)
        {
            SerializationInfo& child = _current->addMember(name, len);
            _current = &child;
            return this;
        }

        // inherit docs
        Composer* onBeginElement()
        {
            SerializationInfo& child = _current->addElement();
            _current = &child;
            return this;
        }

        // inherit docs
        Composer* onBeginDictElement()
        {
            SerializationInfo& child = _current->addDictElement();
            _current = &child;
            return this;
        }

        // inherit docs
        Composer* onBeginDictKey()
        {
            SerializationInfo& child = _current->addDictKey();
            _current = &child;
            return this;
        }

        // inherit docs
        Composer* onBeginDictValue()
        {
            SerializationInfo& child = _current->addDictValue();
            _current = &child;
            return this;
        }

        // inherit docs
        Composer* onFinish()
        {
            if( ! _current->parent() )
            {
                *_current >> Pt::load() >>= *_type;
                _si.clear();
                _type = 0;
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
