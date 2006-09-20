#ifndef Pt_Xml_XmlStream_h
#define Pt_Xml_XmlStream_h

#include <Pt/Text/String.h>
#include <Pt/Text/TextStream.h>

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


	class PT_EXPORT XmlIStream
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


	class PT_EXPORT XmlStreamIterator
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
