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
#include <Pt/String.h>
#include <Pt/Deserializer.h>
#include <memory>

namespace Pt {

namespace Xml {

class XmlReader;
class Node;
class StartElement;
class EndElement;

/** @brief Deserialize objects or object data to XML

    Thic class performs XML deserialization of a single object or
    object data.
*/
class PT_XML_API XmlDeserializer : public Deserializer
{
    public:
        explicit XmlDeserializer(XmlReader& reader);

        explicit XmlDeserializer(std::istream& is);

        //! @brief Destructor
        ~XmlDeserializer();

        XmlReader& reader()
        { return *_reader; }

    protected:
        virtual void onBegin(IComposer& deser);

        //! @brief Returns true when type is complete
        virtual bool onAdvance();

        //virtual void getold(IComposer* deser);
        virtual void get(IComposer& deser);

    private:
        void OnBegin(const Node& node);

        void OnReferenceBegin(const Node& node);

        void OnMemberBegin(const Node& node);

        void OnValue(const Node& node);

        void OnMemberEnd(const Node& node);

        void beginMember(const StartElement& se);

        void finishMember(const EndElement& ee);

    private:
        //! @internal
        XmlSerializationContext _xmlcontext;

        //! @internal
        XmlReader* _reader;

        //! @internal
        std::auto_ptr<XmlReader> _deleter;

        //! @internal
        typedef void (XmlDeserializer::*ProcessNode)(const Node&);

        //! @internal
        ProcessNode _processNode;

        //! @internal
        IComposer* _deser;

        //! @internal
        String _value;
};

} // namespace Xml

} // namespace Pt

#endif
