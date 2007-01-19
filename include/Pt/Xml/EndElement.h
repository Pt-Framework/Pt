#ifndef PTV_Xml_EndElement_h
#define PTV_Xml_EndElement_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/Node.h>
#include <Pt/Text/String.h>


namespace Pt {

	namespace Xml {

		/**
		 * @brief An end element (Node) which represents a closing tag of an XML document.
		 *
		 * An end element is created when the parser reaches an end tag, for example $&lt;/a>$.
		 * An EndElement object only stores the name of the tag. To access the attributes of the tag the
		 * start tag has to be read. The body of the tag can be accessed by reading the previous
		 * Character node(s).
		 *
		 * Use name() to get the name of the tag which was closed.
		 *
		 * When parsing $<a>test</a>$ a StartElement, a Character and finally an EndElement node is
		 * created. If an empty tag is parsed, like for example $</a>$, a StartElement and an EndElement
		 * is created.
		 *
		 * @see StartElement
		 * @see Node
		 */
		class PT_XML_API EndElement : public Node {
			public:
				/**
				 * @brief Constructs a new EndElement object with the given (optional) string as tag name.
				 *
				 * @param name The name of the EndElement object. This is an optional parameter.
				 * Default is an empty string.
				 */
				EndElement(const String& name = String());

				//! Empty destructor
				~EndElement();

				/**
				 * @brief Clones this EndElement object by creating a duplicate on the heap and returning it.
				 * @return A cloned version of this EndElement object.
				 */
				EndElement* clone() const
				{return new EndElement(*this);}

				/**
				 * @brief Returns the tag name of the closing tag for which this EndElement object was created.
				 *
				 * When parsing <a>test</a> a StartElement, a Character and finally an EndElement node is
				 * created. The EndElement has the name "a". If an empty tag is parsed, like for example </a>,
				 * a StartElement and an EndElement ("a") is created.
				 *
				 * @return The tag name of the closing tag for which this EndElement object was created.
				 */
				String& name();

				/**
				 * @brief Returns the tag name of the closing tag for which this EndElement object was created.
				 *
				 * When parsing <a>test</a> a StartElement, a Character and finally an EndElement node is
				 * created. The EndElement has the name "a". If an empty tag is parsed, like for example </a>,
				 * a StartElement and an EndElement ("a") is created.
				 *
				 * @return The tag name of the closing tag for which this EndElement object was created.
				 */
				const String& name() const;

				/**
				 * @brief Sets the tag name of the end tag for which this EndElement object was created.
				 * @param content The new name for this EndElement object.
				 */
				void setName(const String name);

				/**
				 * @brief Compares this EndElement object with the given node.
				 *
				 * This method returns $true$ if the given node also is a EndElement object and
				 * the name of both EndElement objects is the same. Otherwise it returns $false$.
				 *
				 * @param node This Node object is compared to the current EndElement node object.
				 * @return $true if this EndElement object is the same as the given node.
				 */
				virtual bool operator==(const Node& node) const;

			private:
				//! The tag name of this end tag.
				String _name;
		};

	}

}

#endif
