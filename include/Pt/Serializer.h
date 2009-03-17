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
#include <Pt/SerializationInfo.h>

namespace Pt {

class Formatter
{
    public:
        Formatter()
        {}

        virtual ~Formatter()
        {}

        virtual void addValue(const std::string& type, const Pt::String& value) = 0;

        virtual void beginArray() = 0;

        virtual void finishArray() = 0;

        virtual void beginObject() = 0;

        virtual void beginMember(const std::string& name) = 0;

        virtual void finishMember() = 0;

        virtual void finishObject() = 0;

        virtual void finish() = 0;
};


class ISerializer
{
    public:
        ISerializer()
        {}

        virtual ~ISerializer()
        {}

        virtual void decompose(Formatter& formatter) = 0;
};


template <typename T>
class Serializer : public ISerializer
{
    public:
        Serializer(const T& type)
        : _type(type)
        , _current(&_si)
        {}

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


} // namespace Pt

#endif
