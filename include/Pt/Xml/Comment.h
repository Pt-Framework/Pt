#ifndef Pt_Xml_Comment_h
#define Pt_Xml_Comment_h

#include <Pt/Xml/Node.h>
#include <Pt/Text/String.h>


namespace Pt {

	namespace Xml {
		
		/**
		 * @brief A Comment element (Node) of an XML document, containing the comment's content.
		 *
		 * A Comment element stores the content of a comment. There is no interpretation of the
		 * comment's text before it is stored.
		 *
		 * Use text() to get the content/text of the comment element without the &lt;!-- and -->.
		 *
		 * When parsing a comment %&lt;!-- This is a comment -->$ the following text will be
		 * returned by text(): $This is a comment$
		 *
		 * @see Node
		 */
		class PT_API Comment : public Node {
			public:
				/**
				 * @brief Constructs a new Comment object with the given string as content/text.
				 *
				 * @param text The content/text of the Comment object.
				 */
				Comment(const String& text);

				//! Empty destructor
				~Comment();

				/**
				 * @brief Clones this Comment object by creating a duplicate on the heap and returning it.
				 * @return A cloned version of this Comment object.
				 */
				Comment* clone() const
				{return new Comment(*this);}

				/**
				 * @brief Returns the content/text of this Comment object.
				 *
				 * The content includes everything that is between the start and end "tags" of the comment
				 * without being parsed. When parsing a comment %&lt;!-- This is a comment -->$ the following
				 * text will be returned: $This is a comment$
				 *
				 * @return The text of this Comment object.
				 */
				String& text();

				/**
				 * @brief Returns the content/text of this Comment object.
				 *
				 * The content includes everything that is between the start and end "tags" of the comment
				 * without being parsed. When parsing a comment %&lt;!-- This is a comment -->$ the following
				 * text will be returned: $This is a comment$
				 *
				 * @return The text of this Comment object.
				 */
				const String& text() const;

				/**
				 * @brief Sets the text of this Comment object.
				 * @param content The new text for this Comment object.
				 */
				void setText(const String text);
	
			private:
				//! The text of this Comment object.
				String _text;
		};

	}

}

#endif
