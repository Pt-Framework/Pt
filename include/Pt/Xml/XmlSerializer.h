#ifndef Pt_Xml_XmlSerializer_h
#define Pt_Xml_XmlSerializer_h

#include <Pt/Xml/Api.h>
#include <Pt/String.h>

namespace Pt {
    class SerializationData;

namespace Xml {
    class XmlReader;
    class Node;

    class PT_XML_API XmlSerializer
    {

    };

    class PT_XML_API XmlDeserializer
    {
        public:
            XmlDeserializer(XmlReader& reader);

            void deserialize(SerializationData& data);

        protected:
            void beginDocument(const Node& node);
            void onStartElement(const Node& node);
            void onWhitespace(const Node& node);
            void onContent(const Node& node);
            void onEndElement(const Node& node);
        
        private:
            XmlReader* _reader;
            typedef void (XmlDeserializer::*ProcessNode)(const Node&);
            ProcessNode _processNode;
            SerializationData* _current;
            String _nodeName;
    };

} // namespace Xml

} // namespace Pt

#endif
