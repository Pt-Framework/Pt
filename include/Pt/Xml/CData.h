#ifndef PTV_Xml_CData_h
#define PTV_Xml_CData_h

#include <Pt/Xml/Api.h>
#include <Pt/String.h>
#include <Pt/Xml/Node.h>

namespace Pt {

	namespace Xml {

		/**
		 * @brief A CDATA element (Node) of an XML document.
		 *
		 * A CDATA element stores data which is not interpreted by the parser and may contain
		 * any data, also XML-tags and entities which are contained in this object undecoded.
		 *
		 * Use content() to get the content of the CDATA element.
		 *
		 * When parsing $<![CDATA[&lt;Element>This is a cdata element&lt;/Element>%]%]>$ the following
		 * content will be returned by content(): $&lt;Element>This is a cdata element&lt;/Element>$
		 *
		 * @see Node
		 */
		class PT_XML_API CData : public Node {
			public:
				/**
				 * @brief Constructs a new CData object with the given (optional) string as content.
				 *
				 * @param content The content of the CData object. This is an optional parameter.
				 * Default is an empty string.
				 */
				CData(const String& content = String());

				//! Empty destructor
				~CData();

				/**
				 * @brief Clones this CData object by creating a duplicate on the heap and returning it.
				 * @return A cloned version of this CData object.
				 */
				CData* clone() const
				{ return new CData(*this); }

				/**
				 * @brief Returns $true$ if the content of this CData object is empty; $false$ otherwise.
				 * @return $true$ if the content of this CData object is empty; $false$ otherwise.
				 */
				bool empty() const;

				/**
				 * @brief Returns the content of this CData object.
				 *
				 * The content includes the data in the XML's CData-element without the surrounding
				 * CDATA-"tag". When parsing $<![CDATA[<Element>This is a cdata element</Element>]]>$ the
				 * following content will be returned: $<Element>This is a cdata element</Element>$
				 *
				 * @return The content of this CData object.
				 */
				String& content();

				/**
				 * @brief Returns the content of this CData object.
				 *
				 * The content contains the data in the XML's CData-element without the surrounding
				 * CDATA-"tag". When parsing $<![CDATA[&lt;Element>This is a cdata element&lt;/Element>%]%]>$
				 * the following content will be returned: $&lt;Element>This is a cdata element&lt;/Element>$
				 *
				 * @return The content of this CData object.
				 */
				const String& content() const;

				/**
				 * @brief Sets the content of this CData object.
				 * @param content The new content for this CData object.
				 */
				void setContent(const String& content);

				/**
				 * @brief Compares this CData object with the given node.
				 *
				 * This method returns $true$ if the given node also is a CData object and
				 * the content of both CData objects is the same. Otherwise it returns $false$.
				 *
				 * @param node This Node object is compared to the current CData node object.
				 * @return $true if this CData object is the same as the given node.
				 */
				virtual bool operator==(const Node& node) const;

			private:
				//! The content of this CData object.
				String _content;
		};

	}

}

#endif
