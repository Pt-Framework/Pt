#ifndef PTV_Xml_XmlStream_h
#define PTV_Xml_XmlStream_h

#include <Pt/Text/String.h>

#define IMPORT_TEST __declspec(dllimport)

#include <Pt/Text/TextStream.h>
#undef IMPORT_TEST

#include <Pt/Xml/Node.h>
#include <Pt/Xml/EndDocument.h>

#include <memory>
#include <queue>


namespace Pt {

namespace Xml {

	class Node;
	class XmlDeclaration;
	class StartElement;
	class EndElement;
	class EndDocument;
	class Characters;
	class ProcessingInstruction;
	class Comment;
	class CData;
	class XmlStreamIterator;


	/**
	 * @brief An input stream for parsing an XML document utilizing Pull Parsing.
	 *
	 * This class operates on another istream or TextStream from which it reads the XML character
	 * data, parses it and lastly makes available the parsed XML elements (Nodes).
	 *
	 * A pull parser will only parse the XML document as far as the user read data from it.
	 * To read the current element (Node) the method get() can be used. To parse and read the next
	 * element the method next() can be used. Only when next() or any corresponding method or operator
	 * is called the next chunk of XML input data is parsed.
	 *
	 * To parse an XML-document an object of this class is created and passed the input stream
	 * for the document. The constructor initializes the parser, already parses the Xml-declaration
	 * and positions the internal cursor to the first actual element (Node) of the XML document.
	 *
	 * The current XML element (Node) can be read using get(). Every call to next() will parse the
	 * next element, position the cursor to the next element and return the parsed element. The
	 * returned element is of type Node, which is the super-class for all XML element classes. The
	 * class Node has a method type() which returns the type of the read element. Depending on the
	 * type the generic Node object may be cast to the more concrete element object. For example
	 * a Node object with a node type of Node::StartElement can be cast to StartElement.
	 *
	 * Parsing using next() will continue until the end of the document is reached which will result
	 * in a EndDocument element to be returned by next() and get().
	 *
	 * This class also provides the method current() to obtain an iterator which basically works the
	 * same as using using get() and next() directly. The cursor can be set to the next element by
	 * using the ++ operator of the iterator. The current element can be accessed by using the *
	 * operator.
	 *
	 * @see Node
	 */
	class PT_API XmlIStream
	{
		public:
			static const EndDocument& documentEnd()
			{
				static const EndDocument _enddoc;
				return _enddoc;
			}

		public:
			XmlIStream(std::istream& is);

			XmlIStream(Text::TextStream& is);

			~XmlIStream();

			XmlStreamIterator current();

			XmlStreamIterator end() const;

			//! @brief Get current element
			const Node& get();

			//! @brief Get next element from stream
			const Node& next();

			XmlIStream& operator>>(StartElement& se);

			XmlIStream& operator>>(EndElement& e);

			XmlIStream& operator>>(Characters& e);

		private:
			void init();

			void onDocType();

			void onStartElement();

			void onEndElement();

			void onTextElement();

			void onComment();

			bool parseAttribute(String& name, String& value);

			void parseStartElement(StartElement& to);

			void parseEndElement(EndElement& to);

			void parseTextElement(Characters& to);

			void findOf(const String& str);

			void findNotOf(const String& str);

			void getUntil(String& buffer, const String& stop);

		private:
			std::basic_streambuf<Char>* _textBuffer;
			std::basic_streambuf<Char>* _buffer;
			std::queue<Node*> _nodeBuffer;
			Char _token[512];
			const size_t _tokenMax;
	};


	class PT_API XmlStreamIterator
	{
		public:
			XmlStreamIterator()
			: _stream(0), _node( &XmlIStream::documentEnd() )
			{ }

			XmlStreamIterator(XmlIStream& xis)
			: _stream(&xis), _node(0)
			{ _node = &_stream->get(); }

			XmlStreamIterator(const XmlStreamIterator& it)
			: _stream(it._stream), _node(it._node)
			{ }

			~XmlStreamIterator()
			{ }

			XmlStreamIterator& operator=(const XmlStreamIterator& it)
			{
				_stream = it._stream;
				_node = it._node;
				return *this;
			}

			inline const Node& operator*() const
			{ return *_node; }

			XmlStreamIterator& operator++()
			{
				_node = &_stream->next();
				return *this;
			}

			inline bool operator==(const XmlStreamIterator& it) const
			{ return _node == it._node; }

			inline bool operator!=(const XmlStreamIterator& it) const
			{ return _node != it._node; }

		private:
			XmlIStream* _stream;
			const Node* _node;
	};

}

}

#endif
