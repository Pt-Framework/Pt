#ifndef Pt_Xml_XmlSerializer_h
#define Pt_Xml_XmlSerializer_h

#include <Pt/Xml/Api.h>
#include <Pt/String.h>
#include <Pt/SerializationData.h>
#include <memory>


namespace Pt {

namespace Xml {
    class XmlWriter;

    /** @brief Serialize objects or object data to XML

        Thic class performs XML serialization of a single object or
        object data.
    */
    class PT_XML_API XmlSerializer
    {
        public:
            /** @brief Construct a serializer writing to a byte stream

                The serializer will write the objects as XML with
                UTF-8 encoding to the output stream.
            */
            XmlSerializer(std::ostream& os);

            //! @brief Destructor
            ~XmlSerializer();

            /** @brief Serialize object data to XML

                The serializer will serialize the object data \a data
                as a complete XML document to the stream.
            */
            void putData(const SerializationData& data);

            /** @brief Serialize an object to XML

                The serializer will serialize the object \a type as a complete
                XML document to the stream. The string \a name will be used
                as the instance name of \a type and appear as the name of the
                root element. The type must be serializable.
            */
            template <typename T>
            void serialize(const T& type, const Pt::String& name)
            {
                SerializationData data(0, name);
                data << type;
                this->putData( data );
            }

            /** @brief Serialize object to XML

                Writes all pending data to the used stream.
            */
            void flush();

        protected:
            //! @internal
            void writeData(const SerializationData& data);

        private:
            //! @internal
            XmlWriter* _writer;

            //! @internal
            std::auto_ptr<XmlWriter> _deleter;
    };

} // namespace Xml

} // namespace Pt

#endif
