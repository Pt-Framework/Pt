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

#ifndef Pt_Xml_XmlSerializer_h
#define Pt_Xml_XmlSerializer_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/XmlFormatter.h>
#include <Pt/Xml/XmlSerializationContext.h>
#include <Pt/Serializer.h>

namespace Pt {

namespace Xml {

class XmlWriter;

/** @brief Serialize objects or data to XML
*/
class XmlSerializer : public Serializer
{
    public:
        /** @brief Default Constructor.
        */
        XmlSerializer()
        {
            this->reset( &_xmlcontext );
            this->setFormatter(_formatter);
        }

        /** @brief Construct to use an %XmlWriter.
        */
        explicit XmlSerializer(XmlWriter& writer)
        : _formatter(writer)
        {
            this->reset( &_xmlcontext );
            this->setFormatter(_formatter);
        }

        /** @brief Attach to an %XmlWriter.
        */
        void attach(XmlWriter& writer)
        {
            _formatter.attach(writer);
        }
        
        /** @brief Detach from its %XmlWriter.
        */
        void detach()
        {
            _formatter.detach();
        }

        /** @brief Returns the attached %XmlWriter or a nullptr.
        */
        XmlWriter* writer()
        {
            return _formatter.writer();
        }

    private:
        XmlFormatter _formatter;
        XmlSerializationContext _xmlcontext;
};

} // namespace Xml

} // namespace Pt

#endif // Pt_Xml_XmlSerializer_h
