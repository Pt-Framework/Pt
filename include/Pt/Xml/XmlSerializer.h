#ifndef Pt_Xml_XmlSerializer_h
#define Pt_Xml_XmlSerializer_h

#include <Pt/Xml/Api.h>
#include <Pt/String.h>
#include <Pt/Serializer.h>
#include <Pt/SerializationInfo.h>
#include <memory>

namespace Pt {

namespace Xml {

    class XmlWriter;

    /** @brief Serialize objects or object data to XML

        Thic class performs XML serialization of a single object or
        object data.
    */
    class PT_XML_API XmlSerializer : public Serializer
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

            //! @internal
            void flush();

        protected:
            //! @internal
            virtual void write(const SerializationInfo& si);

        private:
            //! @internal
            XmlWriter* _writer;

            //! @internal
            std::auto_ptr<XmlWriter> _deleter;
    };

} // namespace Xml

} // namespace Pt

#endif
