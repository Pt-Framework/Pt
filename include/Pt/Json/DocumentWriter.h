/*
   Copyright (C) 2015-2023 by Dr. Marc Boris Duerner
  
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

#ifndef PT_JSON_DOCUMENT_WRITER_H
#define PT_JSON_DOCUMENT_WRITER_H

#include <Pt/Json/Api.h>
#include <Pt/Json/Document.h>
#include <Pt/String.h>
#include <string>

namespace Pt {

namespace Json {

class Document;

/** @brief JSON Document writer.
*/
class PT_JSON_API DocumenWriter
{
    
    public:
        DocumenWriter();

        DocumenWriter(std::basic_ostream<Pt::Char>& os, const Document& doc);

        void reset();

        void reset(std::basic_ostream<Pt::Char>& os, const Document& doc);
        
        void write();

        size_t spaces() const
        {
            return _spaces;
        }

        void setSpaces(size_t v)
        {
            _spaces = v;
        }

    private:
        void output(const Pt::SerializationInfo& info, size_t spaces);

        void outNull(const Pt::SerializationInfo& entry, bool last);

        void outBool(const Pt::SerializationInfo& entry, bool last);

        void outValue(const Pt::SerializationInfo& entry, bool last);

        void outStr(const Pt::SerializationInfo& entry, bool last);

        void outObject(const Pt::SerializationInfo& entry, bool last);

        void outArray(const Pt::SerializationInfo& entry, bool last);

        inline Pt::String space() const
        {
            const size_t spaces = _depth * _spaces;

            switch (spaces)
            {
                case 4:
                    return "    ";

                case 8:
                    return "        ";

                case 16:
                    return "                ";

                case 32:
                    return "                                ";

                default:
                {
                    Pt::String str;

                    for (size_t i = 0; i < spaces; ++i)
                        str += " ";

                    return str;
                }
            }

            return "";
        }

    private:
        const Document* _doc;
        size_t        _spaces;
        size_t        _depth;
        std::basic_ostream<Pt::Char>* _os;

};

} // namespace

} // namespace

#endif
