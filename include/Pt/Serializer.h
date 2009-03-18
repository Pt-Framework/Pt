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

class Formatter
{
    public:
        Formatter()
        {}

        virtual ~Formatter()
        {}

        virtual void addValue(const std::string& name, const std::string& type,
                              const Pt::String& value, const std::string& id) = 0;

        virtual void addReference(const std::string& name, const Pt::String& value) = 0;

        virtual void beginArray() = 0;

        virtual void finishArray() = 0;

        virtual void beginObject(const std::string& name, const std::string& id) = 0;

        virtual void beginMember(const std::string& name) = 0;

        virtual void finishMember() = 0;

        virtual void finishObject() = 0;

        virtual void finish() = 0;
};

class ISerializer;
typedef std::map<const void*, ISerializer*> SerializationContext;


class ISerializer
{
    public:
        ISerializer()
        {}

        virtual ~ISerializer()
        {}

        virtual void fixdown(SerializationContext& ctx) = 0;

        virtual void setName(const std::string& name) = 0;

        virtual void setId(const std::string& id) = 0;

        virtual void decompose(Formatter& formatter) = 0;
};


template <typename T>
class Serializer : public ISerializer
{
    public:
        Serializer(const T& type)
        : _type(&type)
        , _current(&_si)
        {
            _si <<= *_type;
        }

        virtual void fixdown(SerializationContext& omap)
        {
            this->fixdownEach(_si, omap);
        }

        virtual void setId(const std::string& id)
        {
            _si.setId(id);
        }

        virtual void setName(const std::string& name)
        {
            _si.setName(name);
        }

        virtual void decompose(Formatter& formatter)
        {
            this->formatEach(_si, formatter);
        }

        static void fixdownEach(Pt::SerializationInfo& si, SerializationContext& omap)
        {
            if(si.category() == Pt::SerializationInfo::Reference)
            {
                const void* p = si.toValue<void*>();
                ISerializer* pointee = omap[p];
                pointee->setId( convert<std::string>(pointee) );
                si.setReference( pointee );
            }
            else if(si.category() == Pt::SerializationInfo::Object)
            {
                Pt::SerializationInfo::Iterator it;
                for(it = si.begin(); it != si.end(); ++it)
                {
                    fixdownEach(*it, omap);
                }
            }
        }

        static void formatEach(const Pt::SerializationInfo& si, Formatter& formatter)
        {
            if(si.category() == SerializationInfo::Value)
            {
                formatter.addValue( si.name(), si.typeName(), si.toString(), si.id() );
            }
            else if(si.category() == SerializationInfo::Object)
            {
                formatter.beginObject( si.name(), si.id() );

                SerializationInfo::ConstIterator it;
                for(it = si.begin(); it != si.end(); ++it)
                {
                    formatter.beginMember( it->name() );
                    formatEach(*it, formatter);
                    formatter.finishMember();
                }

                formatter.finishObject();
            }
            else if(si.category() == Pt::SerializationInfo::Reference)
            {
                formatter.addReference( si.name(), si.toString() );
            }

            //TODO arrays should use SerializationInfo Array
        }

    private:
        const T* _type;
        Pt::SerializationInfo _si;
        Pt::SerializationInfo* _current;
};

} // namespace Pt

#endif
