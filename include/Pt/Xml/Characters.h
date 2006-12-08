#ifndef Pt_Xml_Characters_h
#define Pt_Xml_Characters_h

#include <Pt/Api.h>
#include <Pt/Text/String.h>
#include <Pt/Xml/Node.h>

namespace Pt {

namespace Xml {

		/**
		 * @brief A Character element (Node) of an XML document, containing the body's text of a tag.
		 *
		 * A Character element stores the data of a tag's body. The data is interpreted before it
		 * is set as content of a Character element. This means that entities were translated into
		 * their corresponding character sequence, ...
		 *
		 * Use content() to get the content of the CDATA element.
		 *
		 * When parsing a tag $&lt;a>This is the body's text&lt;/a>$ the following content will be
		 * returned by content(): $This is the body's text$
		 *
		 * @see Node
		 */
		class PT_EXPORT Characters : public Node {
			public:
				/**
				 * @brief Constructs a new Character object with the given (optional) string as content.
				 *
				 * @param content The content of the Character object. This is an optional parameter.
				 * Default is an empty string.
				 */
				Characters( const String& content = String() );

				//! Empty destructor
				~Characters();

				/**
				 * @brief Clones this Character object by creating a duplicate on the heap and returning it.
				 * @return A cloned version of this Character object.
				 */
				Characters* clone() const
				{ return new Characters(*this); }

				/**
				 * @brief Returns $true$ if the content of this Character object is empty; $false$ otherwise.
				 * @return $true$ if the content of this Character object is empty; $false$ otherwise.
				 */
				bool empty() const;

				/**
				 * @brief Returns the content of this Character object.
				 *
				 * The content includes the text inside a tag's body. The text is interpreted before it
				 * is returned, this means that for example entities are translated into their corresponding
				 * character sequence. When parsing a tag $<a>This is the body's text</a>$ the followin
				 * content will be returned: $This is the body's text$
				 *
				 * @return The content of this Character object.
				 */
				String& content();

				/**
				 * @brief Returns the content of this Character object.
				 *
				 * The content includes the text inside a tag's body. The text is interpreted before it
				 * is returned, this means that for example entities are translated into their corresponding
				 * character sequence. When parsing a tag $<a>This is the body's text</a>$ the followin
				 * content will be returned: $This is the body's text$
				 *
				 * @return The content of this Character object.
				 */
				const String& content() const;

				/**
				 * @brief Sets the content of this Character object.
				 * @param content The new content for this Character object.
				 */
				void setContent(const String& content);

				/**
				 * @brief Compares this Character object with the given node.
				 *
				 * This method returns $true$ if the given node also is a Character object and
				 * the content of both Character objects is the same. Otherwise it returns $false$.
				 *
				 * @param node This Node object is compared to the current Character node object.
				 * @return $true if this Character object is the same as the given node.
				 */
				virtual bool operator==(const Node& node) const;

			private:
				//! The content of this Character object.
				String _content;
		};


	}

}

#endif
