/*
 * Copyright (C) 2008-2012 by Marc Boris Duerner
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
#ifndef Pt_Xml_XmlFormatter_h
#define Pt_Xml_XmlFormatter_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/XmlWriter.h>
#include <Pt/Xml/XmlReader.h>
#include <Pt/String.h>
#include <Pt/Formatter.h>

namespace Pt {

namespace Xml {

class XmlReader;
class XmlWriter;
class Node;
class StartElement;
class EndElement;

/** @brief Format objects or data to XML
*/
class PT_XML_API XmlFormatter : public Formatter
{
    public:
        /** @brief Default Constructor.
        */
        XmlFormatter();

        /** @brief Construct a formatter writing to a %XmlWriter.
        */
        explicit XmlFormatter(XmlWriter& writer);

        /** @brief Construct a formatter reading from a %XmlReader.
        */
        explicit XmlFormatter(XmlReader& reader);

        //! @brief Destructor
        ~XmlFormatter();

        /** @brief Attach to an %XmlWriter.
        */
        void attach(XmlWriter& writer);

        /** @brief Returns the attached %XmlWriter or a nullptr.
        */
        XmlWriter* writer()
        { return _writer;}

        /** @brief Attach to an %XmlReader.
        */
        void attach(XmlReader& reader);

        /** @brief Returns the attached %XmlReader or a nullptr.
        */
        XmlReader* reader()
        { return _reader;}

        /** @brief Detach from its %XmlReader and %XmlWriter.
        */
        void detach();

        // inherit docs
        void addString(const char*, const char* type,
                       const Pt::Char* value, const char* id);
        
        // inherit docs
        void addString8(const char* name, const char* value, 
                        const char* id);
        
        // inherit docs
        void addBool(const char* name, bool value,
                     const char* id);

        void addChar(const char* name, const Pt::Char& value,
                     const char* id);

        // inherit docs
        void addChar8(const char* name, char value,
                      const char* id);

        // inherit docs
        void addInt8(const char* name, Pt::int8_t value, const char* id);
        
        // inherit docs
        void addInt16(const char* name, Pt::int16_t value,  const char* id);
        
        // inherit docs
        void addInt32(const char* name, Pt::int32_t value, const char* id);
        
        // inherit docs
        void addInt64(const char* name, Pt::int64_t value, const char* id);

        // inherit docs
        void addUInt8(const char* name, Pt::uint8_t value, const char* id);
        
        // inherit docs
        void addUInt16(const char* name, Pt::uint16_t value,  const char* id);
        
        // inherit docs
        void addUInt32(const char* name, Pt::uint32_t value, const char* id);
        
        // inherit docs
        void addUInt64(const char* name, Pt::uint64_t value, const char* id);

        // inherit docs
        void addFloat(const char* name, float value,
                      const char* id);

        // inherit docs
        void addDouble(const char* name, double value,
                       const char* id);
        
        // inherit docs
        void addLongDouble(const char* name, long double value,
                           const char* id);
        
        // inherit docs
        void addBytes(const char* name, const char* type,
                      const char* value, size_t length, const char* id);
        
        // inherit docs
        void addReference(const char* name, const char* value);
        
        // inherit docs
        void beginArray(const char* name, const char* type,
                        const char* id);
        
        
        // inherit docs
        void beginElement();
        
        // inherit docs
        void finishElement();
        
        // inherit docs
        void finishArray();
        
        // inherit docs
        void beginObject(const char* name, const char* type,
                         const char* id);
        
        // inherit docs
        void beginMember(const char* name);
        
        // inherit docs
        void finishMember();
        
        // inherit docs
        void finishObject();

        virtual void beginParse(Composer& composer);

        // inherit docs
        virtual bool parseSome();

        // inherit docs
        virtual void parse();

    protected:
        //! @internal
        void OnBegin(const Node& node);

        //! @internal
        void OnReferenceBegin(const Node& node);

        //! @internal
        void OnMemberBegin(const Node& node);

        //! @internal
        void OnValue(const Node& node);

        //! @internal
        void OnMemberEnd(const Node& node);

        //! @internal
        void beginXmlMember(const StartElement& se);

        //! @internal
        void finishXmlMember(const EndElement& ee);

    private:
        //! @internal
        XmlWriter* _writer;

        //! @internal
        XmlReader* _reader;

        //! @internal
        Pt::String _value;

        //! @internal
        typedef void (XmlFormatter::*ProcessNode)(const Node&);

        //! @internal
        ProcessNode _processNode;

        //! @internal
        Composer* _composer;
};

} // namespace Xml

} // namespace Pt

#endif
