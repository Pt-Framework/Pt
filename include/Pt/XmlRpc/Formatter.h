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
#include <Pt/Xml/XmlWriter.h>
#include <Pt/Formatter.h>
#include <Pt/String.h>
#include <string>
#include <map>

namespace Pt {

namespace XmlRpc {

class PT_XMLRPC_API Formatter : public Pt::Formatter
{
    public:
        Formatter(Xml::XmlWriter& writer)
        : _writer(&writer)
        { }

        void attach(Xml::XmlWriter& writer)
        { _writer = &writer; }

        void addString(const char* name, const char* type,
                       const Pt::String& value, const char* id);

        void addBool(const char* name, bool value, 
                     const char* id);

        void addChar(const char* name, const Pt::Char& value,
                     const char* id);

        void addInt(const char* name, long long value, 
                    const char* id);

        void addUInt(const char* name, unsigned long long value, 
                     const char* id);

        void addFloat(const char* name, double value, 
                      const char* id);

        void addBytes(const char* name, const char* type,
                      const char* value, size_t length, const char* id);

        void addReference(const char* name, const char* id);

        void beginArray(const char* name, const char* type,
                        const char* id);

        virtual void beginElement(const char* type, const char* id);

        virtual void finishElement();

        void finishArray();

        void beginObject(const char* name, const char* type,
                         const char* id);

        void beginMember(const char* name, const char* type,
                         const char* id);

        void finishMember();

        void finishObject();

        virtual bool parseSome(IComposer& composer)
        { return false; }

        virtual void parse(IComposer& composer)
        {}

    private:
        Xml::XmlWriter* _writer;
        Pt::String _value;
};

}

}

#endif
