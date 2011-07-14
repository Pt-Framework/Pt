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
#ifndef Pt_Formatter_h
#define Pt_Formatter_h

#include <Pt/Api.h>
#include <Pt/String.h>
#include <Pt/SerializationContext.h>
#include <string>

namespace Pt {

class Formatter
{
    public:
        virtual ~Formatter()
        { }

        virtual void addValue(const std::string& name, const std::string& type,
                              const Pt::String& value, const std::string& id) = 0;

        virtual void addBool(const std::string& name, bool value,
                             const std::string& id) = 0;

        virtual void addInt(const std::string& name, long long value,
                            const std::string& id) = 0;

        virtual void addUInt(const std::string& name, unsigned long long value,
                             const std::string& id) = 0;

        virtual void addFloat(const std::string& name, double value,
                              const std::string& id) = 0;

        virtual void addBytes(const std::string& name, const std::string& type,
                              const char* value, size_t length, const std::string& id) = 0;

        virtual void addReference(const std::string& name, const std::string& refId) = 0;

        virtual void beginArray(const std::string& name, const std::string& type,
                                const std::string& id) = 0;

        virtual void beginElement(const std::string& type, const std::string& id) = 0;

        virtual void finishElement() = 0;

        virtual void finishArray() = 0;

        virtual void beginObject(const std::string& name, const std::string& type,
                                 const std::string& id) = 0;

        virtual void beginMember(const std::string& name, const std::string& type,
                                 const std::string& id) = 0;

        virtual void finishMember() = 0;

        virtual void finishObject() = 0;

    protected:
        Formatter()
        {}
};

} // namespace Pt

#endif
