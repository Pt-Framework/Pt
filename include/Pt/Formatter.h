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
#include <string>
#include <cstddef>

namespace Pt {

class Composer;

/** @brief Support for serialization to different formats.

    @ingroup Serialization
*/
class Formatter
{
    public:
        virtual ~Formatter()
        { }

        /** @brief Formats a string value.

            Serialization:
                Surrogate: calls addString with typename set to actual object, like Pt::DateTime
                Formatter: can be set up to format string values differently, depending on typename

            Deserialization:
                Parser: can be set up to to report special types as string
                Surrogate: builds actual object like Pt::DateTime from string, in which
                           case the typename is not string, but the format specific one
        */
        void addString(const std::string& name, const std::string& type,
                       const Pt::String& value, const std::string& id)
        { onAddString(name.c_str(), type.c_str(), value.c_str(), id.c_str()); }

        void addString(const char* name, const char* type,
                       const Pt::Char* value, const char* id)
        { onAddString(name, type, value, id); }

        /** @brief Formats a binary value.
        */
        void addBinary(const std::string& name, const std::string& type,
                       const char* value, std::size_t length, const std::string& id)
        { onAddBinary(name.c_str(), type.c_str(), value, length, id.c_str()); }

        void addBinary(const char* name, const char* type,
                       const char* value, std::size_t length, const char* id)
        { onAddBinary(name, type, value, length, id); }

        /** @brief Formats a boolean value.
        */
        void addBool(const std::string& name, bool value,
                     const std::string& id)
        { onAddBool(name.c_str(), value, id.c_str()); }

        void addBool(const char* name, bool value,
                     const char* id)
        { onAddBool(name, value, id); }

        /** @brief Formats a character value.
        */
        void addChar(const std::string& name, const Pt::Char& value,
                     const std::string& id)
        { onAddChar(name.c_str(), value, id.c_str()); }

        void addChar(const char* name, const Pt::Char& value,
                     const char* id)
        { onAddChar(name, value, id); }

        /** @brief Formats a 8-bit signed integer value.
        */
        void addInt8(const std::string& name, Pt::int8_t value,
                     const std::string& id)
        { onAddInt8(name.c_str(), value, id.c_str()); }

        void addInt8(const char* name, Pt::int8_t value,
                     const char* id)
        { onAddInt8(name, value, id); }

        /** @brief Formats a 16-bit signed integer value.
        */
        void addInt16(const std::string& name, Pt::int16_t value,
                      const std::string& id)
        { onAddInt16(name.c_str(), value, id.c_str()); }

        void addInt16(const char* name, Pt::int16_t value,
                      const char* id)
        { onAddInt16(name, value, id); }

        /** @brief Formats a 32-bit signed integer value.
        */
        void addInt32(const std::string& name, Pt::int32_t value,
                      const std::string& id)
        { onAddInt32(name.c_str(), value, id.c_str()); }

        void addInt32(const char* name, Pt::int32_t value,
                      const char* id)
        { onAddInt32(name, value, id); }

        /** @brief Formats a 64-bit signed integer value.
        */
        void addInt64(const std::string& name, Pt::int64_t value,
                      const std::string& id)
        { onAddInt64(name.c_str(), value, id.c_str()); }

        void addInt64(const char* name, Pt::int64_t value,
                      const char* id)
        { onAddInt64(name, value, id); }

        /** @brief Formats a 8-bit unsigned integer value.
        */
        void addUInt8(const std::string& name, Pt::uint8_t value,
                      const std::string& id)
        { onAddUInt8(name.c_str(), value, id.c_str()); }

        void addUInt8(const char* name, Pt::uint8_t value,
                      const char* id)
        { onAddUInt8(name, value, id); }

        /** @brief Formats a 16-bit unsigned integer value.
        */
        void addUInt16(const std::string& name, Pt::uint16_t value,
                       const std::string& id)
        { onAddUInt16(name.c_str(), value, id.c_str()); }

        void addUInt16(const char* name, Pt::uint16_t value,
                       const char* id)
        { onAddUInt16(name, value, id); }

        /** @brief Formats a 32-bit unsigned integer value.
        */
        void addUInt32(const std::string& name, Pt::uint32_t value,
                       const std::string& id)
        { onAddUInt32(name.c_str(), value, id.c_str()); }

        void addUInt32(const char* name, Pt::uint32_t value,
                       const char* id)
        { onAddUInt32(name, value, id); }

        /** @brief Formats a 64-bit unsigned integer value.
        */
        void addUInt64(const std::string& name, Pt::uint64_t value,
                       const std::string& id)
        { onAddUInt64(name.c_str(), value, id.c_str()); }

        void addUInt64(const char* name, Pt::uint64_t value,
                       const char* id)
        { onAddUInt64(name, value, id); }

        /** @brief Formats a float value.
        */
        void addFloat(const std::string& name, float value,
                      const std::string& id)
        { onAddFloat(name.c_str(), value, id.c_str()); }

        void addFloat(const char* name, float value,
                      const char* id)
        { onAddFloat(name, value, id); }

        /** @brief Formats a double value.
        */
        void addDouble(const std::string& name, double value,
                       const std::string& id)
        { onAddDouble(name.c_str(), value, id.c_str()); }

        void addDouble(const char* name, double value,
                       const char* id)
        { onAddDouble(name, value, id); }

        /** @brief Formats a long double value.
        */
        void addLongDouble(const std::string& name, long double value,
                           const std::string& id)
        { onAddLongDouble(name.c_str(), value, id.c_str()); }

        void addLongDouble(const char* name, long double value,
                           const char* id)
        { onAddLongDouble(name, value, id); }

        /** @brief Formats a reference.
        */
        void addReference(const std::string& name, const std::string& refId)
        { onAddReference(name.c_str(), refId.c_str()); }

        void addReference(const char* name, const char* refId)
        { onAddReference(name, refId); }

        /** @brief Formats the begin of a struct.
        */
        void beginStruct(const std::string& name, const std::string& type,
                         const std::string& id)
        { onBeginStruct(name.c_str(), type.c_str(), id.c_str()); }

        void beginStruct(const char* name, const char* type,
                         const char* id)
        { onBeginStruct(name, type, id); }

        /** @brief Formats the begin of a struct member.
        */
        void beginMember(const std::string& name)
        { onBeginMember(name.c_str()); }

        void beginMember(const char* name)
        { onBeginMember(name); }

        /** @brief Formats the end of a struct member.
        */
        void finishMember()
        { onFinishMember(); }

        /** @brief Formats the end of a struct.
        */
        void finishStruct()
        { onFinishStruct(); }

        /** @brief Formats the begin of an array.
        */
        void beginSequence(const std::string& name, const std::string& type,
                           const std::string& id)
        { onBeginSequence(name.c_str(), type.c_str(), id.c_str()); }

        void beginSequence(const char* name, const char* type,
                           const char* id)
        { onBeginSequence(name, type, id); }

        /** @brief Formats the begin of an array element.
        */
        void beginElement()
        { onBeginElement(); }

        /** @brief Formats the end of an array element.
        */
        void finishElement()
        { onFinishElement(); }

        /** @brief Formats the end of an array.
        */
        void finishSequence()
        { onFinishSequence(); }

        /** @brief Formats the begin of a dict.
        */
        void beginDict(const std::string& name, const std::string& type,
                           const std::string& id)
        { onBeginDict(name.c_str(), type.c_str(), id.c_str()); }

        void beginDict(const char* name, const char* type,
                           const char* id)
        { onBeginDict(name, type, id); }

        void beginDictElement()
        { onBeginDictElement(); }

        void finishDictElement()
        { onFinishDictElement(); }

        /** @brief Formats the begin of a dict key.
        */
        void beginDictKey()
        { onBeginDictKey(); }

        /** @brief Formats the end of a dict key.
        */
        void finishDictKey()
        { onFinishDictKey(); }

        /** @brief Formats the begin of a dict value.
        */
        void beginDictValue()
        { onBeginDictValue(); }

        /** @brief Formats the end of a dict value.
        */
        void finishDictValue()
        { onFinishDictValue(); }

        /** @brief Formats the end of a dict.
        */
        void finishDict()
        { onFinishDict(); }

    public:
        //! @brief Begin to parse to a composer
        void beginParse(Composer& composer)
        { onBeginParse(composer); }

        //! @brief Returns true if composer completes, false if no more data available
        bool parseSome()
        { return onParseSome(); }

        //! @brief Parse until composer completes.
        void parse()
        { onParse(); }

    protected:
        virtual void onAddString(const char* name, const char* type,
                                 const Pt::Char* value, const char* id) = 0;

        virtual void onAddBinary(const char* name, const char* type,
                                 const char* value, std::size_t length, const char* id) = 0;

        virtual void onAddBool(const char* name, bool value,
                               const char* id) = 0;

        virtual void onAddChar(const char* name, const Pt::Char& value,
                               const char* id) = 0;

        virtual void onAddInt8(const char* name, Pt::int8_t value,
                               const char* id) = 0;

        virtual void onAddInt16(const char* name, Pt::int16_t value,
                                const char* id) = 0;

        virtual void onAddInt32(const char* name, Pt::int32_t value,
                                const char* id) = 0;

        virtual void onAddInt64(const char* name, Pt::int64_t value,
                                const char* id) = 0;

        virtual void onAddUInt8(const char* name, Pt::uint8_t value,
                                const char* id) = 0;

        virtual void onAddUInt16(const char* name, Pt::uint16_t value,
                                 const char* id) = 0;

        virtual void onAddUInt32(const char* name, Pt::uint32_t value,
                                 const char* id) = 0;

        virtual void onAddUInt64(const char* name, Pt::uint64_t value,
                                 const char* id) = 0;

        virtual void onAddFloat(const char* name, float value,
                                const char* id) = 0;

        virtual void onAddDouble(const char* name, double value,
                                 const char* id) = 0;

        virtual void onAddLongDouble(const char* name, long double value,
                                     const char* id) = 0;

        virtual void onAddReference(const char* name, const char* refId) = 0;

        virtual void onBeginStruct(const char* name, const char* type,
                                   const char* id) = 0;

        virtual void onBeginMember(const char* name) = 0;

        virtual void onFinishMember() = 0;

        virtual void onFinishStruct() = 0;

        virtual void onBeginSequence(const char* name, const char* type,
                                     const char* id) = 0;

        virtual void onBeginElement() = 0;

        virtual void onFinishElement() = 0;

        virtual void onFinishSequence() = 0;

        virtual void onBeginDict(const char* name, const char* type,
                                 const char* id)
        { onBeginSequence(name, type, id); }

        virtual void onBeginDictElement()
        { onBeginSequence("", "", ""); }

        virtual void onBeginDictKey()
        { onBeginElement(); }

        virtual void onFinishDictKey()
        { onFinishElement(); }

        virtual void onBeginDictValue()
        {  onBeginElement();  }

        virtual void onFinishDictValue()
        { onFinishElement(); }

        virtual void onFinishDictElement()
        { onFinishSequence(); }

        virtual void onFinishDict()
        { onFinishSequence(); }

    protected:
        //! @brief Begin to parse to a composer
        virtual void onBeginParse(Composer& composer) = 0;

        //! @brief Returns true if composer completes, false if no more data available
        virtual bool onParseSome() = 0;

        //! @brief Parse until composer completes.
        virtual void onParse() = 0;

    protected:
        Formatter()
        {}
};

} // namespace Pt

#endif
