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
#include <Pt/String.h>
#include <Pt/Formatter.h>
#include <Pt/Serializer.h>
#include <memory>

namespace Pt {

namespace Xml {

class XmlWriter;

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
        XmlFormatter(std::ostream& os);


        /** @brief Construct a serializer writing to the given XmlWriter object

            The serializer will write the objects to the given XmlWriter object.
            This class will not free the given XmlWriter object. The caller is
            responsible to free it if needed.
        */
        XmlFormatter(XmlWriter* writer);

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

        /** @brief Detaches the currently set writer from this object.

            Before detaching the writer, the underlaying stream is flushed.
            If there is no currently set writer, nothing happens.
        */
        void detach();

        //! @internal
        void flush();

        void addValue(const std::string& name, const std::string& type,
                        const Pt::String& value, const std::string& id);

        void addReference(const std::string& name, const Pt::String& value);

        void beginArray(const std::string& name, const std::string& id);

        void finishArray();

        void beginObject(const std::string& name, const std::string& id);

        void beginMember(const std::string& name);

        void finishMember();

        void finishObject();

        void finish();

    private:
        //! @internal
        XmlWriter* _writer;

        //! @internal
        std::auto_ptr<XmlWriter> _deleter;
};


/** @brief Serialize objects or object data to XML

    Thic class performs XML serialization of a single object or
    object data.
*/
class PT_XML_API XmlSerializer
{
    public:
        /** @brief Construct a serializer without initializing the
                    serializer for writing.

            The serializer can be "opened" for writing by calling
            method attach().
        */
        XmlSerializer();

        /** @brief Construct a serializer writing to a byte stream

            The serializer will write the objects as XML with
            UTF-8 encoding to the output stream.
        */
        XmlSerializer(std::ostream& os);

        /** @brief Construct a serializer writing to the given XmlWriter object

            The serializer will write the objects to the given XmlWriter object.
            This class will not free the given XmlWriter object. The caller is
            responsible to free it if needed.
        */
        XmlSerializer(XmlWriter* writer);

        //! @brief Destructor
        ~XmlSerializer();

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

        /** @brief Detaches the currently set writer from this object.

            Before detaching the writer, the underlaying stream is flushed.
            If there is no currently set writer, nothing happens.
        */
        void detach();

        /** @brief Serialize an object to XML

            The serializer will serialize the object \a type as
            XML to the assigned stream. The string \a name will be used
            as the instance name of \a type and appear as the name of the
            XML element. The type must be serializable.
        */
        template <typename T>
        void serialize(const T& type, const std::string& name)
        {
            ISerializer* serializer = _context.push(type);
            serializer->setName(name);
        }

        void finish()
        {
            _context.fixdown(_formatter);
            _context.clear();
        }

        //! @internal
        void flush();

    private:
        XmlFormatter _formatter;

        //! @internal
        SerializationContext _context;
};

} // namespace Xml

} // namespace Pt

#endif
