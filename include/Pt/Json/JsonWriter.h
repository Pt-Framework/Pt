/*
   Copyright (C) 2015-2024 by Dr. Marc Boris Duerner
  
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   
   As a special exception, you may use this file as part of a free
   software library without restriction. Specifically, if other files
   instantiate templates or use macros or inline functions from this
   file, or you compile this file and link it with other files to
   produce an executable, this file does not by itself cause the
   resulting executable to be covered by the GNU General Public
   License. This exception does not however invalidate any other
   reasons why the executable file might be covered by the GNU Library
   General Public License.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
   MA 02110-1301 USA
*/

#ifndef PT_JSON_JSONWRITER_H
#define PT_JSON_JSONWRITER_H

#include <Pt/Json/Api.h>
#include <Pt/IOStream.h>
#include <Pt/String.h>
#include <Pt/Types.h>
#include <cstddef>

namespace Pt {

namespace Json {

/** @brief Writes JSON to a text stream.
*/
class PT_JSON_API JsonWriter
{
    public:
        /** @brief Constructor.
        */
        JsonWriter();

        /** @brief Constructs with output stream.
        */
        JsonWriter(std::basic_ostream<Char>& os);

        /** @brief Destructor.
        */
        ~JsonWriter();

        /** @brief Returns true if indentation should be written.
        */
        bool isFormatting() const;
        
        /** @brief Indicates wether indentation should be written.
        */
        void setFormatting(bool value);

        /** @brief Returns the indentation string.
        */
        const Pt::String& indent() const;

        /** @brief Sets the indentation string.
        */
        void setIndent(const Pt::String& indent);

        /** @brief Clears the writer state and output.

            Ther output stream is removed and the writer state is reset to
            write a new document. The formatting options are not changed.
        */
        void reset();

        /** @brief Clears the writer state and output.

            The output stream is set and the writer state is reset to
            write a new document. The formatting options are not changed.
        */
        void reset(std::basic_ostream<Char>& os);

        /** @brief Returns the output stream or a nullptr if none was set.
        */
        std::basic_ostream<Char>* output();
        
        /** @brief Writes the begin of an object.
        */
        void writeObject();

        /** @brief Writes the end of an object.
        */
        void writeObjectEnd();

        /** @brief Writes the begin of an array.
        */
        void writeArray();

        /** @brief Writes the end of an array.
        */
        void writeArrayEnd();

        /** @brief Writes a member.
        */
        void writeMember(const Pt::Char* name, std::size_t nameSize);

        /** @brief Writes a member.
        */
        void writeMember(const Pt::String& name);

        /** @brief Writes a member.
        */
        void writeMember(const char* name);

        /** @brief Writes a string value.
        */
        void writeString(const Pt::Char* value, std::size_t valueSize);

        void writeString(const Pt::Char* value);

        /** @brief Writes a string value.
        */
        void writeString(const Pt::String& value);

        /** @brief Writes an integer value.
        */
        void writeInt(Pt::int64_t value);

        /** @brief Writes an unsigned integer value.
        */
        void writeUInt(Pt::uint64_t value);

        /** @brief Writes an float value.
        */
        void writeFloat(long double value);

        /** @brief Writes an boolean value.
        */
        void writeBool(bool value);

        /** @brief Writes an null value.
        */
        void writeNull();

    private:
        class JsonWriterImpl* _impl;
};


inline void JsonWriter::writeMember(const Pt::String& name)
{
    this->writeMember( name.c_str(), name.size() );
}


inline void JsonWriter::writeString(const Pt::String& value)
{
    this->writeString( value.c_str(), value.size() );
}

} // namespace

} // namespace

#endif // include guard
