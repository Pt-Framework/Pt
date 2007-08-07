#ifndef Pt_Xml_XmlDeserializer_h
#define Pt_Xml_XmlDeserializer_h

#include <Pt/Xml/Api.h>
#include <Pt/String.h>
#include <Pt/SerializationInfo.h>
#include <memory>


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

            /** @brief Deserialize object data from XML

                This method will append the object data generated
                from an XML format to \a data.
            */
            void getData(SerializationInfo& si);

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
            }

        protected:
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
    };

} // namespace Xml

} // namespace Pt

#endif
