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
#include <Pt/String.h>
#include <Pt/SerializationInfo.h>
#include <Pt/Deserializer.h>
#include <memory>
#include <list>
#include <map>

namespace Pt {

namespace Xml {

    class XmlReader;
    class Node;

    /** @brief Deserialize objects or object data to XML

        Thic class performs XML deserialization of a single object or
        object data.
    */
    class PT_XML_API XmlDeserializer
    {
        public:
            typedef void (*Fixup)(void**, const std::type_info&, void*);

        public:
            XmlDeserializer(XmlReader& reader);

            XmlDeserializer(std::istream& is);

            //! @brief Destructor
            ~XmlDeserializer();

            XmlReader& reader()
            { return *_reader; }

            /** @brief Deserialize an object

                This method will deserialize the object \a type from an
                XML format. The type \a type must be serializable.
            */
            template <typename T>
            void deserialize(T& type)
            {
                Deserializer<T> deser(type);
                this->get(&deser);
                deser.finish();
                //SerializationInfo& si = this->get();
                //si >>= type;
                //this->markFixup(si, &type, &XmlDeserializer::do_fixup<T>);
            }

            //SerializationInfo& peek();

            void finish();

        protected:
            /** @brief Deserialize object data from XML

                This method will append the object data generated
                from an XML format to \a data.
            */
            //void read(SerializationInfo& si);

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
            //Pt::SerializationInfo& get();

            void get(IDeserializer* deser);

            void markFixup(Pt::SerializationInfo& si, void* type, Fixup fixup);

            void fixup(const Pt::SerializationInfo& si);

        private:
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
            //SerializationInfo* _current;
            IDeserializer* _deser;

            //! @internal
            String _nodeName;

            String _nodeId;

            std::list<Pt::SerializationInfo> _stack;

            bool _peeking;

            std::map<std::string, void*> _objects;

            std::map<std::string, Fixup> _fixups;

            std::map<void*, std::string> _pointers;

            template <typename T>
            static void do_fixup(void** fixme, const std::type_info& fixmeInfo , void* obj)
            {
                if( fixmeInfo != typeid(T) )
                    throw SerializationError( PT_ERROR_MSG("reference fixup failed, type mismatch") );

                *( (T**)(fixme) ) = (T*)(obj);
            }
    };

} // namespace Xml

} // namespace Pt

#endif
