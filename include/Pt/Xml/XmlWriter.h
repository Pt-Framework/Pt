#ifndef PTV_Xml_XmlWriter_h
#define PTV_Xml_XmlWriter_h

#include <Pt/Xml/Api.h>
#include <Pt/String.h>
#include <Pt/Text/TextStream.h>
#include <stack>

namespace Pt {

namespace Xml {

    class Attribute;

    class PT_XML_API XmlWriter
    {
        public:
            XmlWriter(std::ostream& os);

            ~XmlWriter();

            void writeStartElement(const Pt::String& prefix, const Pt::String& localName, const Pt::String& ns);

            void writeStartElement(const Pt::String& localName);

            void writeStartElement(const Pt::String& localName, const Attribute* attr, size_t attrCount);

            void writeEndElement();

            void writeElement(const Pt::String& localName, const Pt::String& content);

            void writeElement(const Pt::String& localName, const Attribute* attr, size_t attrCount, const Pt::String& content);

            void writeCharacters(const Pt::String& text);

            void flush();

            void endl();

        private:
            Text::TextOStream _tos;
            std::stack<Pt::String> _elements;
    };

}

}

#endif
