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
#ifndef Pt_Serializer_h
#define Pt_Serializer_h

#include <Pt/Api.h>
#include <Pt/Convert.h>
#include <Pt/SerializationInfo.h>
#include <map>

namespace Pt {

class Formatter;
class SerializationContext;

class PT_API ISerializer
{
    public:
        virtual ~ISerializer()
        {}

        virtual void fixdown(SerializationContext& context) = 0;

        virtual void setName(const std::string& name) = 0;

        virtual void setId(const std::string& id) = 0;

        virtual void format(Formatter& formatter) = 0;

    protected:
        ISerializer()
        {}

        static void fixdownEach(Pt::SerializationInfo& si, SerializationContext& context);

        static void formatEach(const Pt::SerializationInfo& si, Formatter& formatter);
};

// TODO Marshaller
template <typename T>
class Serializer : public ISerializer
{
    public:
        Serializer()
        : _type(0)
        , _current(&_si)
        { }

        void begin(const T& type)
        {
            _type = &type;
            _si <<= *_type;
        }

        virtual void fixdown(SerializationContext& context)
        {
            this->fixdownEach( _si, context );
        }

        virtual void setId(const std::string& id)
        {
            _si.setId(id);
        }

        virtual void setName(const std::string& name)
        {
            _si.setName(name);
        }

        virtual void format(Formatter& formatter)
        {
            this->formatEach( _si, formatter );
        }

    private:
        const T* _type;
        SerializationInfo _si;
        SerializationInfo* _current;
};


class SerializationContext
{
    public:
        SerializationContext()
        {}

        virtual ~SerializationContext()
        {
            this->clear();
        }

        void clear()
        {
            _omap.clear();

            std::vector<ISerializer*>::iterator it;
            for(it = _stack.begin(); it != _stack.end(); ++it)
            {
                delete *it;
            }
            _stack.clear();
        }

        template <typename T>
        ISerializer* push(const T& type)
        {
            Serializer<T>* serializer = new Serializer<T>;
            serializer->begin(type);
            _omap[&type] = serializer;
            _stack.push_back(serializer);
            return serializer;
        }

        ISerializer* find(const void* p) const
        {
            std::map<const void*, ISerializer*>::const_iterator it;
            it = _omap.find(p);
            if(it == _omap.end())
                return 0;

            return it->second;
        }

        void fixdown(Formatter& formatter)
        {
            std::vector<ISerializer*>::iterator it;
            for(it = _stack.begin(); it != _stack.end(); ++it)
            {
                ISerializer* serializer = *it;
                serializer->fixdown(*this);
            }

            _omap.clear();

            for(it = _stack.begin(); it != _stack.end(); ++it)
            {
                (*it)->format(formatter);
            }
        }

    private:
        std::map<const void*, ISerializer*> _omap;
        std::vector<ISerializer*> _stack;
};

} // namespace Pt

#endif
