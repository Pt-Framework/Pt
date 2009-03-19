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
#include <map>
#include <typeinfo>

namespace Pt {

// pass this to finish to get pointers which need fixup
class DeserializationContext
{
    public:
        // used by Deserializer
        void addObject(const std::string& id, void* obj)
        {
            _objects[id] = obj;
        }

        // IDeserializer calls this method on IDeserializer::finish()
        void addReference(const std::string& id, void* obj)
        {
            _pointers[obj] = id;
        }

        void clear()
        {
            _objects.clear();
            _pointers.clear();
        }

        void fixup()
        {
            std::map<void*, std::string>::iterator it;
            for(it = _pointers.begin(); it != _pointers.end(); ++it)
            {
                void* fixme = it->first;
                std::string id = it->second;
                void* obj = _objects[id];
                //std::cerr << "FIXING: " << fixme << " to " << obj << std::endl;

                void** vp =(void**)(fixme);
                *vp = obj;
            }

            clear();
        }

    private:
        std::map<std::string, void*> _objects;
        std::map<void*, std::string> _pointers;
};


class IDeserializer
{
    public:
        IDeserializer(DeserializationContext& ctx)
        : _parent(0)
        , _context( &ctx )
        {}

        virtual ~IDeserializer()
        {}

        void setParent(IDeserializer* parent)
        { _parent = parent; }

        IDeserializer* parent()
        { return _parent; }

        DeserializationContext* context()
        { return _context; }

        virtual void setName(const std::string& name) = 0;

        virtual void setValue(const Pt::String& value) = 0;

        virtual void setId(const std::string& id) = 0;

        virtual void setReference(const std::string& id) = 0;

        virtual IDeserializer* beginMember(const std::string& name) = 0;

        virtual IDeserializer* leaveMember() = 0;

        virtual void finish() = 0;

    private:
        IDeserializer* _parent;
        DeserializationContext* _context;
};


template <typename T>
class Deserializer : public IDeserializer
{
    public:
        Deserializer(T& type, DeserializationContext& ctx)
        : IDeserializer(ctx)
        , _type(&type)
        , _current(&_si)
        {}

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

        virtual void setReference(const std::string& id)
        {
           _current->setValue(id);
           _current->setCategory(SerializationInfo::Reference);
        }

        virtual IDeserializer* beginMember(const std::string& name)
        {
            SerializationInfo& child = _current->addMember(name);
            _current = &child;
            return this;
        }

        virtual IDeserializer* leaveMember()
        {
            if( ! _current->parent() )
            {
                this->finish();

                if( ! this->parent() )
                    throw std::runtime_error("invalid member");

                return this->parent();
            }

            _current = _current->parent();
            return this;
        }

        virtual void finish()
        {
            // SI's for unfixed pointers contain the fixup address now
            // other types may only point to _type,but not to its members
            *_current >>= *_type;

            Pt::SerializationInfo::Iterator it;
            for(it = _current->begin(); it != _current->end(); ++it)
            {
                if(it->category() == Pt::SerializationInfo::Reference)
                {
                    //std::cerr << "UNFIXED: " << it->fixupAddr() << " needs " << it->toValue<std::string>() << std::endl;

                    this->context()->addReference( it->toValue<std::string>(), it->fixupAddr() );
                }
            }

            if( ! _current->id().empty() )
            {
                this->context()->addObject(_current->id(), _type);
            }
        }

    private:
        T* _type;
        Pt::SerializationInfo _si;
        Pt::SerializationInfo* _current;
};

} // namespace Pt

#endif
