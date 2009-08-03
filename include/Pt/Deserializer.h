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
#ifndef Pt_Deserializer_h
#define Pt_Deserializer_h

#include <Pt/Api.h>
#include <Pt/SerializationInfo.h>
#include <typeinfo>

namespace Pt {

class SerializationContext;

class IDeserializer
{
    public:
        virtual ~IDeserializer()
        {}

		virtual void setContext( SerializationContext* context ) = 0;

        virtual void setName(const std::string& name) = 0;

        virtual void setId(const std::string& id) = 0;

        virtual void setValue(const Pt::String& value) = 0;

        virtual void setBool(bool value) = 0;

        virtual void setInt(long value) = 0;

        virtual void setUInt(unsigned long value) = 0;

        virtual void setFloat(double value) = 0;

        virtual void setReference(const std::string& id) = 0;

        virtual IDeserializer* beginMember(const std::string& name) = 0;

        virtual IDeserializer* beginElement() = 0;

        virtual IDeserializer* finish() = 0;
        
    protected:
        IDeserializer()
        {}
};


template <typename T>
class Deserializer : public IDeserializer
{
    public:
        Deserializer()
        : _type(0)
        , _current(&_si)
        {}

        void begin(T& type)
        {
            _si.clear();
            _type = &type;
            _current = &_si;
        }

        virtual void setContext(SerializationContext* context) 
        {
			_si.setContext(context);
        }

        virtual void setName(const std::string& name)
        {
            _current->setName(name);
        }

        virtual void setId(const std::string& id)
        {
            _current->setId(id);
        }

        virtual void setValue(const Pt::String& value)
        {
            _current->setValue(value);
        }

        virtual void setBool(bool value)
        {
            _current->setValue(value);
        }

        virtual void setInt(long value)
        {
            _current->setValue(value);
        }

        virtual void setUInt(unsigned long value)
        {
            _current->setValue(value);
        }

        virtual void setFloat(double value)
        {
            _current->setValue(value);
        }

        virtual void setReference(const std::string& id)
        {
           _current->setReference(id);
        }

        virtual IDeserializer* beginMember(const std::string& name)
        {
            SerializationInfo& child = _current->addMember(name);
            _current = &child;
            return this;
        }

        virtual IDeserializer* beginElement()
        {
            SerializationInfo& child = _current->addElement();
            _current = &child;
            return this;
        }

        virtual IDeserializer* finish()
        {
            if( ! _current->parent() )
            {
                *_current >>= Pt::load() >>= *_type;
                return 0;
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
