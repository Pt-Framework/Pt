#ifndef PTV_Xml_Comment_h
#define PTV_Xml_Comment_h

#include <Pt/Xml/Node.h>
#include <Pt/Text/String.h>


namespace Pt {

	namespace Xml {

		/**
		 * @brief A Comment element (Node) of an XML document, containing the comment's content.
		 *
		 * A Comment element stores the content of a comment. There is no interpretation of the
		 * comment's Text before it is stored.
		 *
		 * Use Text() to get the content/Text of the comment element without the &lt;!-- and -->.
		 *
		 * When parsing a comment %&lt;!-- This is a comment -->$ the following Text will be
		 * returned by Text(): $This is a comment$
		 *
		 * @see Node
		 */
		class PT_API Comment : public Node {
			public:
				/**
				 * @brief Constructs a new Comment object with the given string as content/Text.
				 *
				 * @param Text The content/Text of the Comment object.
				 */
				Comment(const String& Text);

				//! Empty destructor
				~Comment();

				/**
				 * @brief Clones this Comment object by creating a duplicate on the heap and returning it.
				 * @return A cloned version of this Comment object.
				 */
				Comment* clone() const
				{return new Comment(*this);}

				/**
				 * @brief Returns the content/Text of this Comment object.
				 *
				 * The content includes everything that is between the start and end "tags" of the comment
				 * without being parsed. When parsing a comment %&lt;!-- This is a comment -->$ the following
				 * Text will be returned: $This is a comment$
				 *
				 * @return The Text of this Comment object.
				 */
				String& Text();

				/**
				 * @brief Returns the content/Text of this Comment object.
				 *
				 * The content includes everything that is between the start and end "tags" of the comment
				 * without being parsed. When parsing a comment %&lt;!-- This is a comment -->$ the following
				 * Text will be returned: $This is a comment$
				 *
				 * @return The Text of this Comment object.
				 */
				const String& Text() const;

				/**
				 * @brief Sets the Text of this Comment object.
				 * @param content The new Text for this Comment object.
				 */
				void setText(const String Text);

			private:
				//! The Text of this Comment object.
				String _Text;
		};

	}

}

#endif
