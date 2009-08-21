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
#include <Pt/Xml/XmlSerializer.h>
#include <Pt/String.h>
#include <Pt/Deserializer.h>
#include <memory>

namespace Pt {

namespace Xml {

class XmlReader;
class Node;

class DeserializerBase
{
    public:
        DeserializerBase()
        : _current(0)
        {}

        virtual ~DeserializerBase()
        {}

        SerializationContext& context()
        { return *_context; }

        const SerializationContext& context() const
        { return *_context; }

        void setContext(SerializationContext& context)
        { _context = &context; }

        /** @brief Deserialize an object

            This method will deserialize the object \a type.
            The type \a type must be serializable.
        */
        template <typename T>
        void deserialize(T& type)
        {
            Deserializer<T> deser;
            deser.begin(type, _context);

            this->get(&deser);
            //deser.finish();
        }

        void deserialize(IDeserializer& deser)
        {
            this->get(&deser);
            deser.finish();
        }

        template <typename T>
        void begin(T& type)
        {
            Deserializer<T>* deser = new Deserializer<T>;
            deser->begin(type, _context);
            _current = deser;
        }

        bool advance()
        {
            if( ! _current )
                return false;

            _current = this->advance(_current);

            if( ! _current )
                return false;
        }
        
        virtual IDeserializer* advance(IDeserializer* deser) = 0;

        void finish()
        { _context->fixup(); }
        
    protected:
        virtual void get(IDeserializer* deser) = 0;
        
    private:
        SerializationContext* _context;
        IDeserializer*        _current;
};

/** @brief Deserialize objects or object data to XML

    Thic class performs XML deserialization of a single object or
    object data.
*/
class PT_XML_API XmlDeserializer
{
    public:
        XmlDeserializer(XmlReader& reader);

        XmlDeserializer(std::istream& is);

        //! @brief Destructor
        ~XmlDeserializer();

        SerializationContext& context()
        { return *_context; }

        const SerializationContext& context() const
        { return *_context; }

        void setContext(SerializationContext& context)
        { _context = &context; }

        XmlReader& reader()
        { return *_reader; }

        /** @brief Deserialize an object

            This method will deserialize the object \a type from an
            XML format. The type \a type must be serializable.
        */
        template <typename T>
        void deserialize(T& type)
        {
            Deserializer<T> deser;
            deser.begin(type, _context);

            this->get(&deser);
            //deser.finish();
        }
        
        void finish()
        { _xmlcontext.fixup(); }

        IDeserializer* advance(IDeserializer* deser);
        
    protected:
        void get(IDeserializer* deser);

        //! @internal
        void beginDocument(const Node& node);

        //! @internal
        void onRootElement(const Node& node);

        //! @internal
        void onStartElement(const Node& node);

        //! @internal
        void onWhitespace(const Node& node);

        //! @internal
        void onContent(const Node& node);

        //! @internal
        void onEndElement(const Node& node);

    private:
        XmlSerializationContext _xmlcontext;
        
        SerializationContext* _context;
        
        //! @internal
        XmlReader* _reader;

        //! @internal
        std::auto_ptr<XmlReader> _deleter;

        //! @internal
        typedef void (XmlDeserializer::*ProcessNode)(const Node&);

        //! @internal
        ProcessNode _processNode;

        size_t _startDepth;

        //! @internal
        IDeserializer* _deser;

        //! @internal
        String _nodeName;

        String _nodeId;
};

} // namespace Xml

} // namespace Pt

#endif
