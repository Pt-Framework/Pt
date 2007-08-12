#ifndef Pt_Xml_XmlDeserializer_h
#define Pt_Xml_XmlDeserializer_h

#include <Pt/Xml/Api.h>
#include <Pt/String.h>
#include <Pt/SerializationInfo.h>
#include <memory>
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
            XmlDeserializer(XmlReader& reader);

            XmlDeserializer(std::istream& is);

            //! @brief Destructor
            ~XmlDeserializer();

            /** @brief Deserialize an object to XML

                This method will deserialize the object \a type from an
                XML format. The type \a type must be serializable.
            */
            template <typename T>
            void deserialize(T& type)
            {
                SerializationInfo si;
                this->getData( si );
                si >>= type;

                if( ! si.id().empty() )
                {
                    _objects[ si.id() ] = &type;
                    _fixups[ si.id() ] = &XmlDeserializer::do_fixup<T>;
                }
            }

            void fixup()
            {}

        protected:
            /** @brief Deserialize object data from XML

                This method will append the object data generated
                from an XML format to \a data.
            */
            void getData(SerializationInfo& si);

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
            //! @internal
            XmlReader* _reader;

            //! @internal
            std::auto_ptr<XmlReader> _deleter;

            //! @internal
            typedef void (XmlDeserializer::*ProcessNode)(const Node&);

            //! @internal
            ProcessNode _processNode;

            //! @internal
            SerializationInfo* _current;

            //! @internal
            String _nodeName;

            void fixup(const Pt::SerializationInfo& si)
            {
                // TODO: we only need to store all references during parsing
                // and fixup pointer addresses later when Deserializer::fixup is called
                Pt::SerializationInfo::ConstIterator it;
                for(it = si.begin(); it != si.end(); ++it)
                {
                    if(it->category() == Pt::SerializationInfo::Reference)
                    {
                        void* target = _objects[ it->id() ];

                        void* d = it->toValue<void*>();
                        void** destination = (void**)d;
                        _fixups[ it->id() ]( destination, target);
                    }

                    if(it->category() == Pt::SerializationInfo::Object)
                    {
                        this->fixup(*it);
                    }
                }
            }

            std::map<std::string, void*> _objects;

            typedef void (*Fixup)(void**, void*);
            std::map<std::string, Fixup> _fixups;

            template <typename T>
            static void do_fixup(void** ref , void* val)
            {
                *( (T**)(ref) ) = (T*)(val);
            }
    };

} // namespace Xml

} // namespace Pt

#endif
