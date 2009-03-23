/*
 * Copyright (C) 2009 by Dr. Marc Boris Duerner
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
#ifndef Pt_XmlRpc_Formatter_h
#define Pt_XmlRpc_Formatter_h

#include <Pt/XmlRpc/Api.h>
#include <Pt/String.h>
#include <Pt/Serializer.h>
#include <iostream>
#include <string>

namespace Pt {

namespace XmlRpc {

class PT_XMLRPC_API Formatter : public Pt::Formatter
{
    public:
        Formatter()
        {}

        ~Formatter()
        {}

        void begin(std::ostream& out)
        {
            _out = &out;
            *_out << "<param>\n";
        }

        void addValue(const std::string& name, const std::string& type,
                      const Pt::String& value, const std::string& id)
        {
            *_out << "<value><" << type << ">" << value.narrow();
            *_out << "</" << type << "></value>\n";
        }

        void addReference(const std::string& name, const Pt::String& value)
        {
            throw SerializationError("references not supported");
        }

        void beginArray()
        {
            *_out << "<value><array><data>\n";
        }

        void finishArray()
        {
            *_out << "</data></array></value>\n";
        }

        void beginObject(const std::string& name, const std::string& id)
        {
            *_out << "<value><struct>\n";
        }

        void beginMember(const std::string& name)
        {
            *_out << "<member>\n";
            *_out << "<name>" << name << "</name>\n";
        }

        void finishMember()
        {
            *_out << "</member>\n";
        }

        void finishObject()
        {
            *_out << "</struct></value>\n";
        }

        void finish()
        {
            *_out << "</param>\n";
        }

    private:
        std::ostream* _out;
};

}

}

#endif
