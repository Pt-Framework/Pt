#ifndef PTV_Xml_EndDocument_h
#define PTV_Xml_EndDocument_h

#include <Pt/Api.h>
#include <Pt/Xml/Node.h>


namespace Pt {

	namespace Xml {

		/**
		 * @brief A Node which represents the end of the XML document.
		 *
		 * The last Node/Element which is read from a document is the EndDocument-node. It is read after
		 * the last tag, Text or comment was read from the XML document. This is similar to an eof character
		 * at the end of a file read.
		 *
		 * @see Node
		 */
		class PT_API EndDocument : public Node {
			public:
				//! Creates an EndDocument object.
				EndDocument()
				: Node( Xml::Node::EndDocument )
				{}

				//! Destructs this EndDocument object.
				~EndDocument()
				{}

				/**
				 * @brief Clones this EndDocument object by creating a duplicate on the heap and returning it.
				 * @return A cloned version of this EndDocument object.
				 */
				EndDocument* clone() const
				{ return new EndDocument(*this); }

		};

	}

}

#endif









