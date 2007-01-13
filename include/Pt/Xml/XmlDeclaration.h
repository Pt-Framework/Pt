#ifndef PTV_Xml_XmlDeclaration_h
#define PTV_Xml_XmlDeclaration_h

#include <Pt/Xml/Node.h>
#include <Pt/Text/String.h>


namespace Pt {

	namespace Xml {

		class PT_API XmlDeclaration : public Node {
			public:
				XmlDeclaration();

				~XmlDeclaration();

				XmlDeclaration* clone() const
				{return new XmlDeclaration(*this);}

				const String& version() const;

				void setVersion(const String& version);

				const String& encoding() const;

				void setEncoding(const String& encoding);

				bool standalone() const;

				void setStandalone(bool standalone);

			private:
				String _version;
				String _encoding;
				bool _standalone;
		};

	}

}
#endif
