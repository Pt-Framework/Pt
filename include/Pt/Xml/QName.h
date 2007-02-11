#ifndef PTV_Xml_QName_h
#define PTV_Xml_QName_h

#include <Pt/String.h>
#include <Pt/Xml/Api.h>


namespace Pt {

    namespace Xml {

        class PT_XML_API QName {
            public:
                QName();

                QName(const String& localPart);

                QName(const String& namespaceURI, const String& localPart);

                QName(const String& namespaceURI, const String& localPart, const String& prefix);

                ~QName();

                const String& prefix() const;

                void setPrefix(const String& prefix);

                const String& localPart() const;

                void setLocalPart(const String& localPart);

                const String& namespaceUri() const;

                void setNamespaceUri(const String& namespaceUri);

            private:
                String _prefix;
                String _localPart;
                String _namespaceUri;
        };

    }

}
#endif
