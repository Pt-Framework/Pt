#ifndef Pt_Xml_EndDocument_h
#define Pt_Xml_EndDocument_h

#include <Pt/Api.h>
#include <Pt/Xml/Node.h>


namespace Pt {

	namespace Xml {

		class PT_EXPORT EndDocument : public Node {
			public:
				EndDocument()
				: Node( Xml::Node::EndDocument )
				{}

				~EndDocument()
				{}

				EndDocument* clone() const
				{ return new EndDocument(*this); }

		};

	}

}

#endif









