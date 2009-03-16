/*
 * Copyright (C) 2009 by Dr. Marc Boris Duerner
 * Copyright (C) 2009 by Tommi Meakitalo
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
#ifndef Pt_XmlRpc_TypeHandler_h
#define Pt_XmlRpc_TypeHandler_h

#include <Pt/XmlRpc/Api.h>
#include <Pt/XmlRpc/Formatter.h>
#include <Pt/String.h>
#include <Pt/SerializationInfo.h>
#include <string>

namespace Pt {

namespace XmlRpc {

class ITypeHandler
{
    public:
        ITypeHandler()
        : _parent(0)
        {}

        virtual ~ITypeHandler()
        {}

        void setParent(ITypeHandler* parent)
        { _parent = parent; }

        ITypeHandler* parent()
        { return _parent; }

        virtual void setValue(const Pt::String& value) = 0;

        virtual ITypeHandler* beginMember(const std::string& name) = 0;

        virtual ITypeHandler* leaveMember() = 0;

        virtual void finish() = 0;

        virtual void decompose(Formatter& f) = 0;

    private:
        ITypeHandler* _parent;
};


template <typename T>
class TypeHandler : public ITypeHandler
{
    public:
        TypeHandler()
        : _type(0)
        , _current(&_si)
        {}

        void begin(T& type)
        { _type = & type; }

        void begin(const T& type)
        { _type = const_cast<T*>(&type); }

        virtual void setValue(const Pt::String& value)
        { //std::cerr << "-S SET VALUE " << value.narrow() << std::endl;
            _current->setValue(value);
        }

        virtual ITypeHandler* beginMember(const std::string& name)
        { //std::cerr << "-S BEGIN MEMBER" << std::endl;
            SerializationInfo& child = _current->addMember(name);
            _current = &child;
            return this;
        }

        virtual ITypeHandler* leaveMember()
        { //std::cerr << "-S LEAVE MEMBER" << std::endl;
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
        { //std::cerr << "-S FINISH" << std::endl;
            *_current >>= *_type;
        }

        virtual void decompose(Formatter& formatter)
        {
            _si <<= *_type;
            this->formatEach(_si, formatter);
        }

        static void formatEach(const Pt::SerializationInfo& si, Formatter& formatter)
        {
            if(si.category() == SerializationInfo::Value)
            {
                // TODO use formatter to adapt typenames to protocol specific typenames

                formatter.addValue( si.typeName(), si.toString() );
            }
            else if(si.category() == SerializationInfo::Object)
            {
                formatter.beginObject();

                SerializationInfo::ConstIterator it;
                for(it = si.begin(); it != si.end(); ++it)
                {
                    formatter.beginMember( it->name() );
                    formatEach(*it, formatter);
                    formatter.finishMember();
                }

                formatter.finishObject();
            }

            //TODO arrays should use SerializationInfo Array
        }

    private:
        T* _type;
        Pt::SerializationInfo _si;
        Pt::SerializationInfo* _current;
};


template <typename T>
class TypeHandler< std::vector<T> > : public ITypeHandler
{
    public:
        TypeHandler()
        : _type(0)
        {}

        void begin(std::vector<T>& type)
        { _type = & type; }

        void begin(const std::vector<T>& type)
        { _type = const_cast<std::vector<T>*>(&type); }

        void setValue(const Pt::String& value)
        { throw std::runtime_error("type mismatch"); }

        ITypeHandler* beginMember(const std::string& name)
        { //std::cerr << "V begin member" << std::endl;
            _type->push_back( T() );
            T& elem = _type->back();
            _elemBuilder.begin(elem);
            _elemBuilder.setParent(this);
            return &_elemBuilder;
        }

        virtual ITypeHandler* leaveMember()
        { //std::cerr << "V begin member" << std::endl;
            ITypeHandler* parent = this->parent();
            if( ! parent )
                throw std::runtime_error("invalid member");

            _elemBuilder.finish();
            return parent;
        }

        virtual void finish()
        { }

        void decompose(Formatter& formatter)
        {
            formatter.beginArray();

            typename std::vector<T>::iterator it;
            for(it = _type->begin(); it != _type->end(); ++it)
            {
                _elemBuilder.begin(*it);
                _elemBuilder.decompose(formatter);
            }

            formatter.finishArray();
        }

    private:
        TypeHandler<T>  _elemBuilder;
        std::vector<T>* _type;
};


template <>
class TypeHandler<int> : public ITypeHandler
{
    public:
        TypeHandler()
        : _type(0)
        {}

        void begin(int& type)
        { _type = & type; }

        void begin(const int& type)
        { _type = const_cast<int*>(&type); }

        void setValue(const Pt::String& value)
        {
            convert(*_type, value);
        }

        ITypeHandler* beginMember(const std::string& name)
        {
            return 0;
        }

        virtual ITypeHandler* leaveMember()
        {
            return this->parent();
        }

        virtual void finish()
        { }

        void decompose(Formatter& formatter)
        {
            String s;
            convert(s, *_type);
            formatter.addValue("int", s);
        }

    private:
        int* _type;
};

}

}

#endif
