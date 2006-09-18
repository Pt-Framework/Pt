#ifndef Pt_Xml_DocTypeDeclaration_h
#define Pt_Xml_DocTypeDeclaration_h

#include <Pt/Xml/Node.h>
#include <Pt/Text/String.h>


namespace Pt {

namespace Xml {

	class PT_EXPORT DocTypeDeclaration : public Node {
		public:
			DocTypeDeclaration( const String& content );

			~DocTypeDeclaration();

			DocTypeDeclaration* clone() const
			{ return new DocTypeDeclaration(*this); }

			const String& content() const;

			void setContent(const String& content);

		private:
			String _content;
	};

}

}
#endif
