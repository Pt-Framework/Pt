#ifndef Pt_Xml_XmlDeserializer_h
#define Pt_Xml_XmlDeserializer_h

#include <Pt/Xml/Api.h>
#include <Pt/String.h>
#include <Pt/SerializationData.h>
#include <memory>


namespace Pt {
    class SerializationData;

namespace Xml {
    class XmlReader;
    class Node;

    class PT_XML_API XmlDeserializer
    {
        public:
            XmlDeserializer(XmlReader& reader);

            XmlDeserializer(std::istream& is);

            ~XmlDeserializer();

            void getData(SerializationData& data);

            template <typename T>
            void deserialize(T& t)
            {
                SerializationData data;
                this->getData( data );
                data >> t;
            }

        protected:
            void beginDocument(const Node& node);
            void onRootElement(const Node& node);
            void onStartElement(const Node& node);
            void onWhitespace(const Node& node);
            void onContent(const Node& node);
            void onEndElement(const Node& node);

        private:
            XmlReader* _reader;
            std::auto_ptr<XmlReader> _deleter;
            typedef void (XmlDeserializer::*ProcessNode)(const Node&);
            ProcessNode _processNode;
            SerializationData* _current;
            String _nodeName;
    };

} // namespace Xml

} // namespace Pt

#endif
