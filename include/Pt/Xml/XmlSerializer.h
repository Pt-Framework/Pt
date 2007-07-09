#ifndef Pt_Xml_XmlSerializer_h
#define Pt_Xml_XmlSerializer_h

#include <Pt/Xml/Api.h>
#include <Pt/String.h>
#include <Pt/SerializationData.h>
#include <memory>


namespace Pt {

namespace Xml {
    class XmlWriter;

    class PT_XML_API XmlSerializer
    {
        public:
            XmlSerializer(std::ostream& os);

            ~XmlSerializer();

            void putData(const SerializationData& data);

            template <typename T>
            void serialize(const T& t, const Pt::String& name)
            {
                SerializationData data(0, name);
                data << t;
                this->putData( data );
            }

        protected:
            void writeData(const SerializationData& data);

        private:
            XmlWriter* _writer;
            std::auto_ptr<XmlWriter> _deleter;
    };

} // namespace Xml

} // namespace Pt

#endif
