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
#ifndef Pt_Xml_XmlDeserializer_h
#define Pt_Xml_XmlDeserializer_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/XmlSerializationContext.h>
#include <Pt/Xml/XmlFormatter.h>
#include <Pt/Deserializer.h>

namespace Pt {

namespace Xml {

class XmlReader;

/** @brief Deserialize objects or object data to XML

    Thic class performs XML deserialization of a single object or
    object data.
*/
class PT_XML_API XmlDeserializer : public Deserializer
{
    public:
        XmlDeserializer()
        {
            this->reset( &_xmlcontext );
            this->setFormatter(_formatter);
        }

        explicit XmlDeserializer(XmlReader& reader)
        : _formatter(reader)
        {
            this->reset( &_xmlcontext );
            this->setFormatter(_formatter);
        }

        explicit XmlDeserializer(std::istream& is)
        : _formatter(is)
        {
            this->reset( &_xmlcontext );
            this->setFormatter(_formatter);
        }

        void attach(std::istream& is)
        {
            _formatter.attach(is);
        }

        void attach(XmlReader& reader)
        {
            _formatter.attach(reader);
        }

        void detach()
        {
            _formatter.detach();
        }

        XmlReader* reader()
        {
            return _formatter.reader();
        }

    private:
        //! @internal
        XmlSerializationContext _xmlcontext;

        //! @internal
        XmlFormatter _formatter;
};

} // namespace Xml

} // namespace Pt

#endif
