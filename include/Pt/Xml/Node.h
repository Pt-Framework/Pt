#ifndef PTV_Xml_Node_h
#define PTV_Xml_Node_h


#include <Pt/Xml/Api.h>
#include <Pt/Clonable.h>


namespace Pt {

	namespace Xml {

		/**
		 * @brief The super-class for all specific Node type of an XML document.
		 *
		 * A Node may for example be a opening tag, a closing tag, a comment or a doctype declaration.
		 * The supported node types are contained in the enum Type. To determine the type of a Node the
		 * method type() can be used.
		 *
		 * For every supported node type (except "Unknown") a specialized class exists that is derived
		 * from this Node class. Those classes contain more data and access methods to allow the user
		 * to determine the information specific to the node, for example the tag name for a StartElement.
		 *
		 * This class mainly provides the method type() to determine the type of the Node. The user
		 * may use this information to determine to which specialized class that is associated
		 * with the type this object can be cast; for the Node::StartElement type the Node object can be
		 * cast to StartElement, for example.
		 *
		 * @see Type
		 */
		class PT_XML_API Node : public Clonable<Node> {
			public:
				enum Type {
					//! Unknown Node type (may not currently be supported)
					Unknown,
					//! Xml declaration (see class XmlDeclaration)
					XmlDeclaration,
					//! Doctype (see class DocType)
					DocType,
					//! End of the document (see EndDocument)
					EndDocument,
					//! Start element aka opening tag (see StartElement)
					StartElement,
					//! End element aka closing tag (see EndElement)
					EndElement,
					//! Parsed content of a tag's body (see Characters)
					Characters,
					//! Unparsed content of an XML document (see CData)
					CData,
					//! Comment (see Comment)
					Comment,
					//! Processing instruction (see ProcessingInstruction)
					ProcessingInstruction
				};

			public:
				/**
				 * @brief Constructs a new Node object with the specified node type
				 * @see Type
				 */
				Node(Type type)
				: _type(type)
				{ }

				//! Empty destructor
				virtual ~Node()
				{ }

				/**
				 * @brief Returns the type of this Node that can be used to determine what specific
				 * Node this object is.
				 *
				 * This information may be used to determine to which specialized Node class that is associated
				 * with the type, this Node object can be cast; for the Node::StartElement type the Node object
				 * can be cast to StartElement, for example.
				 *
				 * @return The type of this node.
				 */
				Type type() const
				{return _type;}

				/**
				 * @brief Compares this Node object with the given node.
				 *
				 * The return value of the generic operator== method is always false. Class which derive
				 * from this class should always override this method and provide a useful comparison, for
				 * example by comparing the node type and contents of the current and given Node object
				 *
				 * @param node In subclasses this Node object is compared to the current Node object.
				 * @return In sub-classes $true is returned if this Node object is the same as the given
				 * Node object. In this generic class $false$ is always returned.
				 */
				virtual bool operator==(const Node& node) const
				{ return false; }

			private:
				//! The type of this Node.
				Type _type;
		};

	}

}

#endif
