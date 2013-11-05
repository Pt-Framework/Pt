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

namespace Pt {

/** @brief Manages the composition of types during serialization.

    @ingroup Serialization
*/
class Composer
{
    public:
        virtual ~Composer()
        {}

        void setParent(Composer* parent)
        { _parent = parent; }

        Composer* parent() const
        { return _parent; }

        virtual void setTypeName(const std::string& type)
        {}

        virtual void setId(const std::string& id) = 0;

        virtual void setString(const Pt::String& value)
        { throw SerializationError("unexpected value"); }

        virtual void setBinary(const char* data, size_t length)
        { throw SerializationError("unexpected value"); }

        virtual void setChar(const Pt::Char& ch)
        { throw SerializationError("unexpected bool value"); }

        virtual void setBool(bool value)
        { throw SerializationError("unexpected bool value"); }

        /** @brief Compose a signed integer type.

            There is only one method for all sizes of signed integer types,
            because that type information is not required for composition. 
        */
        virtual void setInt(Pt::int64_t value)
        { throw SerializationError("unexpected integer value"); }
        
        /** @brief Compose a unsigned integer type.

            There is only one method for all sizes of unsigned integer types,
            because that type information is not required for composition. 
        */
        virtual void setUInt(Pt::int64_t value)
        { throw SerializationError("unexpected unsigned value"); }

        virtual void setDouble(double value)
        { throw SerializationError("unexpected float value"); }

        virtual void setReference(const std::string& id)
        { throw SerializationError("unexpected reference"); }

        virtual Composer* beginMember(const std::string& name)
        { throw SerializationError("unexpected struct"); }

        virtual Composer* beginElement()
        { throw SerializationError("unexpected sequence"); }

        virtual Composer* finish()
        { return _parent; }

    protected:
        Composer()
        : _parent(0)
        {}

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

        virtual void setId(const std::string& id)
        {
            _current->setId(id);
        }

        virtual void setTypeName(const std::string& type)
        {
            _current->setTypeName(type);
        }

        virtual void setString(const Pt::String& value)
        {
            _current->setString(value);
        }

        virtual void setBinary(const char* data, size_t length)
        {
            _current->setBinary(data, length);
        }

        virtual void setChar(const Pt::Char& ch)
        {
            _current->setChar(ch);
        }

        virtual void setBool(bool value)
        {
            _current->setBool(value);
        }

        virtual void setInt(Pt::int64_t value)
        {
            _current->setInt64(value);
        }

        virtual void setUInt(Pt::uint64_t value)
        {
            _current->setUInt64(value);
        }

        virtual void setDouble(double value)
        {
            _current->setDouble(value);
        }

        virtual void setReference(const std::string& id)
        {
           _current->setReference(id);
        }

        virtual Composer* beginMember(const std::string& name)
        {
            SerializationInfo& child = _current->addMember(name);
            _current = &child;
            return this;
        }

        virtual Composer* beginElement()
        {
            SerializationInfo& child = _current->addElement();
            _current = &child;
            return this;
        }

        virtual Composer* finish()
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
