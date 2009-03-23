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

class DeserializationContext;


class PT_API IDeserializer
{
    public:
        IDeserializer()
        : _parent(0)
        {}

        virtual ~IDeserializer()
        {}

        void setParent(IDeserializer* parent)
        { _parent = parent; }

        IDeserializer* parent()
        { return _parent; }

        virtual void setName(const std::string& name) = 0;

        virtual void setValue(const Pt::String& value) = 0;

        virtual void setId(const std::string& id) = 0;

        virtual void setReference(const std::string& id) = 0;

        virtual IDeserializer* beginMember(const std::string& name) = 0;

        virtual IDeserializer* leaveMember() = 0;

        virtual void fixup(DeserializationContext& ctx) = 0;

    protected:
        void fixupEach(void* obj, Pt::SerializationInfo& si, DeserializationContext& ctx);

    private:
        IDeserializer* _parent;
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
            _type = &type;
            _current = &_si;
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
                *_current >>= *_type;

                if( ! this->parent() )
                    throw std::runtime_error("invalid member");

                return this->parent();
            }

            _current = _current->parent();
            return this;
        }

        virtual void fixup(DeserializationContext& ctx)
        {
            // SI's for unfixed pointers contain the fixup address now
            // other types may only point to _type,but not to its members
            *_current >>= *_type;

            fixupEach(_type, _si, ctx);
        }

    private:
        T* _type;
        Pt::SerializationInfo _si;
        Pt::SerializationInfo* _current;
};


class DeserializationContext
{
    public:
        DeserializationContext()
        {}

        virtual ~DeserializationContext()
        {
            this->clear();
        }

        template <typename T>
        IDeserializer* push(T& type)
        {
            Deserializer<T>* deser = new Deserializer<T>;
            deser->begin(type);
            _stack.push_back(deser);
            return deser;
        }

        void addObject(const std::string& id, void* obj)
        {
            _objects[id] = obj;
        }

        void addFixup(const std::string& id, void* obj)
        {
            _pointers[obj] = id;
        }

        void clear()
        {
            _objects.clear();
            _pointers.clear();

            std::vector<IDeserializer*>::iterator it;
            for(it = _stack.begin(); it != _stack.end(); ++it)
            {
                delete *it;
            }
            _stack.clear();
        }

        void fixup()
        {
            std::vector<IDeserializer*>::iterator iter;
            for(iter = _stack.begin(); iter != _stack.end(); ++iter)
            {
                IDeserializer* deser = *iter;
                deser->fixup(*this);
            }

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


        //TODO: we also have to allow the user to register allowed casts from
        //     one type name to another. type id alone only does not take care
        //     of assigning derived classes to base pointers
        template <typename T>
        static void do_fixup(void** fixme, const std::type_info& fixmeInfo , void* obj)
        {
            if( fixmeInfo != typeid(T) )
                throw SerializationError( PT_ERROR_MSG("reference fixup failed, type mismatch") );

            *( (T**)(fixme) ) = (T*)(obj);
        }

    private:
        std::map<std::string, void*> _objects;
        std::map<void*, std::string> _pointers;
        std::vector<IDeserializer*> _stack;
};

} // namespace Pt

#endif
