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
#ifndef Pt_Xml_XmlFormatter_h
#define Pt_Xml_XmlFormatter_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/XmlWriter.h>
#include <Pt/Xml/XmlReader.h>
#include <Pt/String.h>
#include <Pt/Formatter.h>
#include <memory>

namespace Pt {

namespace Xml {

class XmlReader;
class XmlWriter;
class Node;
class StartElement;
class EndElement;

/** @brief Serialize objects or object data to XML

    Thic class performs XML serialization of a single object or
    object data.
*/
class PT_XML_API XmlFormatter : public Formatter
{
    public:
        /** @brief Construct a serializer without initializing the
                    serializer for writing.

            The serializer can be "opened" for writing by calling
            method attach().
        */
        XmlFormatter();

        /** @brief Construct a serializer writing to a byte stream

            The serializer will write the objects as XML with
            UTF-8 encoding to the output stream.
        */
        explicit XmlFormatter(std::ostream& os);

        /** @brief Construct a serializer writing to the given XmlWriter object

            The serializer will write the objects to the given XmlWriter object.
            This class will not free the given XmlWriter object. The caller is
            responsible to free it if needed.
        */
        explicit XmlFormatter(XmlWriter& writer);

        explicit XmlFormatter(std::istream& is);

        explicit XmlFormatter(XmlReader& reader);

        //! @brief Destructor
        ~XmlFormatter();

        /** @brief Opens this serializer for writing into the given stream.

            The serializer will write the objects as XML with
            UTF-8 encoding to the output stream.

            This method does not have to be called if this XmlSerializer object
            was constructed using the constructor that takes an ostream or
            XmlWriter object. If this method is called anyway or called twice an
            std::logic_error is thrown.
        */
        void attach(std::ostream& os);

        /** @brief Opens this serializer for writing into the given XmlWriter object.

            The serializer will write the objects to the given XmlWriter object.

            This method does not have to be called if this XmlSerializer object
            was constructed using the constructor that takes an ostream or
            XmlWriter object. If this method is called anyway or called twice an
            std::logic_error is thrown.

            This class will not free the given XmlWriter object. The caller is
            responsible to free it if needed.
        */
        void attach(XmlWriter& writer);

        XmlWriter* writer()
        { return _writer;}

        void attach(std::istream& is);

        void attach(XmlReader& reader);

        XmlReader* reader()
        { return _reader;}

        /** @brief Detaches the currently set writer from this object.

            Before detaching the writer, the underlaying stream is flushed.
            If there is no currently set writer, nothing happens.
        */
        void detach();

        //! @internal
        void flush();

        void addString(const char*, const char* type,
                       const Pt::String& value, const char* id);

        void addBool(const char* name, bool value,
                     const char* id);

        void addChar(const char* name, const Pt::Char& value,
                     const char* id);

        void addInt(const char* name, long long value,
                    const char* id);

        void addUInt(const char* name, unsigned long long value,
                     const char* id);

        void addFloat(const char* name, float value,
                      const char* id);

        void addDouble(const char* name, double value,
                       const char* id);

        void addLongDouble(const char* name, long double value,
                           const char* id);

        void addBytes(const char* name, const char* type,
                      const char* value, size_t length, const char* id);

        void addReference(const char* name, const char* value);

        void beginArray(const char* name, const char* type,
                        const char* id);

        void beginElement(const char* type, const char* id);

        void finishElement();

        void finishArray();

        void beginObject(const char* name, const char* type,
                         const char* id);

        void beginMember(const char* name, const char* type,
                         const char* id);

        void finishMember();

        void finishObject();

        void onBeginObject(const char* name, const char* type,
                           const char* id);

        void onFinishObject();

        bool parseSome(IComposer& composer);

        void parse(IComposer& composer);

    protected:
        void OnBegin(const Node& node);

        void OnReferenceBegin(const Node& node);

        void OnMemberBegin(const Node& node);

        void OnValue(const Node& node);

        void OnMemberEnd(const Node& node);

        void beginXmlMember(const StartElement& se);

        void finishXmlMember(const EndElement& ee);

    private:
        //! @internal
        XmlWriter* _writer;

        //! @internal
        std::auto_ptr<XmlWriter> _wrPtr;

        //! @internal
        XmlReader* _reader;

        //! @internal
        std::auto_ptr<XmlReader> _rdPtr;

        //! @internal
        Pt::String _value;

        //! @internal
        typedef void (XmlFormatter::*ProcessNode)(const Node&);

        //! @internal
        ProcessNode _processNode;

        //! @internal
        IComposer* _composer;
};

} // namespace Xml

} // namespace Pt

#endif
