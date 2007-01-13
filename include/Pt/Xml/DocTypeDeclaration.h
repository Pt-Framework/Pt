#ifndef PTV_Xml_DocTypeDeclaration_h
#define PTV_Xml_DocTypeDeclaration_h

#include <Pt/Xml/Node.h>
#include <Pt/Text/String.h>


namespace Pt {

namespace Xml {

	/**
	 * @brief A DocType element (Node) of an XML document.
	 *
	 * A DocType element stores the document type of the document and contains an URI to a
	 * file which contains the document type definition.
	 *
	 * Use content() to get the content of the DocType element.
	 *
	 * @see Node
	 */
	class PT_API DocTypeDeclaration : public Node {
		public:
			/**
			 * @brief Constructs a new DocTypeDeclaration object with the given string as content.
			 *
			 * @param content The content of the DocTypeDeclaration object.
			 */
			DocTypeDeclaration( const String& content );

			//! Empty destructor
			~DocTypeDeclaration();

			/**
			 * @brief Clones this DocTypeDeclaration object by creating a duplicate on the heap and returning it.
			 * @return A cloned version of this DocTypeDeclaration object.
			 */
			DocTypeDeclaration* clone() const
			{ return new DocTypeDeclaration(*this); }

			/**
			 * @brief Returns the content of this DocTypeDeclaration object.
			 * @return The content of this DocTypeDeclaration object.
			 */
			const String& content() const;

			/**
			 * @brief Sets the content of this DocTypeDeclaration object.
			 * @param content The new content for this DocTypeDeclaration object.
			 */
			void setContent(const String& content);

		private:
			//! The content of this DocTypeDeclaration object.
			String _content;
	};

}

}
#endif
