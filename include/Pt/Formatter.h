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
#include <Pt/Char.h>
#include <Pt/String.h>
#include <string>

namespace Pt {

class IComposer;

class Formatter
{
    public:
        virtual ~Formatter()
        { }

        virtual void addString(const char* name, const char* type,
                               const Pt::String& value, const char* id) = 0;

        virtual void addBytes(const char* name, const char* type,
                              const char* value, size_t length, const char* id) = 0;

        virtual void addBool(const char* name, bool value,
                             const char* id) = 0;

        virtual void addChar(const char* name, const Pt::Char& value,
                             const char* id) = 0;

        virtual void addInt8(const char* name, Pt::int8_t value,
                             const char* id) = 0;
        
        virtual void addInt16(const char* name, Pt::int16_t value,
                              const char* id) = 0;
        
        virtual void addInt32(const char* name, Pt::int32_t value,
                              const char* id) = 0;
        
        virtual void addInt64(const char* name, Pt::int64_t value,
                              const char* id) = 0;
        
        virtual void addUInt8(const char* name, Pt::uint8_t value,
                              const char* id) = 0;
        
        virtual void addUInt16(const char* name, Pt::uint16_t value,
                               const char* id) = 0;
        
        virtual void addUInt32(const char* name, Pt::uint32_t value,
                               const char* id) = 0;
        
        virtual void addUInt64(const char* name, Pt::uint64_t value,
                               const char* id) = 0;

        virtual void addFloat(const char* name, float value,
                              const char* id) = 0;

        virtual void addDouble(const char* name, double value,
                               const char* id) = 0;
        
        virtual void addLongDouble(const char* name, long double value,
                                   const char* id) = 0;
        
        virtual void addReference(const char* name, const char* refId) = 0;

        virtual void beginArray(const char* name, const char* type,
                                const char* id) = 0;

        virtual void beginElement(const char* type, const char* id) = 0;

        virtual void finishElement() = 0;

        virtual void finishArray() = 0;

        virtual void beginObject(const char* name, const char* type,
                                 const char* id) = 0;

        virtual void beginMember(const char* name, const char* type,
                                 const char* id) = 0;

        virtual void finishMember() = 0;

        virtual void finishObject() = 0;

        //! @brief Returns true if composer completes, false if no more data available
        virtual bool parseSome(IComposer& composer) = 0;

        //! @brief Parse until composer completes.
        virtual void parse(IComposer& composer) = 0;

    protected:
        Formatter()
        {}
};

} // namespace Pt

#endif
