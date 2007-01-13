#ifndef PTV_Xml_XmlStream_h
#define PTV_Xml_XmlStream_h

#include <Pt/Xml/Node.h>
#include <Pt/Xml/NamespaceContext.h>
#include <Pt/Xml/ParseError.h>
#include <Pt/Xml/Resolver.h>

#include <Pt/Text/TextStream.h>
#include <Pt/Text/Utf8Codec.h>

#include <memory>

namespace Pt {

namespace Xml {
	class Node;
	class XmlDeclaration;
	class StartElement;
	class EndElement;
	class Characters;
	class ProcessingInstruction;
	class Comment;
	class CData;


	class PT_API XmlToken {
		public:
			enum Type {
				Unknown,
				Tag,
				Characters,
				EndDocument
			};

		public:
			XmlToken(Type type = Unknown, const String& buffer = "")
			: _type(type), _buffer(buffer)
			{}

			~XmlToken() {};
			Type type() const {return _type;}
			void setType(const Type& type) {_type = type;}
			String& buffer() {return _buffer;}
			const String& buffer() const {return _buffer;}
			void clear() {_buffer.clear(); _type = Unknown;}
			bool empty() const {return _buffer.empty() && _type == Unknown;}

		private:
			Type _type;
			String _buffer;
	};


	class PT_API XmlStreamBuffer : public Pt::TextStreamBuffer {
		public:
			XmlStreamBuffer(std::streambuf* endpoint)
			: TextStreamBuffer( endpoint, new Utf8Codec() )
			{}

			XmlStreamBuffer(std::istream& endpoint)
			: TextStreamBuffer( endpoint, new Utf8Codec() )
			{}

			XmlStreamBuffer(std::ostream& endpoint)
			: TextStreamBuffer( endpoint, new Utf8Codec() )
			{}

			XmlStreamBuffer(std::iostream& endpoint)
			: TextStreamBuffer( endpoint, new Utf8Codec() )
			{}

			~XmlStreamBuffer() throw();

			std::auto_ptr<Xml::Node> next();
			void putback(std::auto_ptr<Xml::Node>& node);

		protected:
			size_t readToken(XmlToken& token);
			void readTag(XmlToken& token);
			void readCharacters(XmlToken& token);
			void readCData(XmlToken& token);
			Xml::Node::Type analyse(XmlToken& token);
			void parseStartElement(const XmlToken& token, StartElement& element) throw(ParseError);
			void parseEndElement(const XmlToken& token, EndElement& element) throw(ParseError);
			void parseComment(const XmlToken& token, Comment& comment) throw(ParseError);
			void parseProcessingInstruction(const XmlToken& token, ProcessingInstruction& procInst) throw(ParseError);
			void parseXmlDeclaration(const XmlToken& token, XmlDeclaration& XmlDecl) throw(ParseError);

		private:
			std::vector<Xml::Node*> _inputBuffer;
	};


	class PT_API XmlStreamBase {
		public:
			enum Flags {
				Normal = 0x00,
				SkipComments = 0x01,
				SkipProcessingInstructions = 0x02,
				SkipWhitespace = 0x04,
				IgnoreNamespaces = 0x08
			};

		public:
			explicit XmlStreamBase(XmlStreamBuffer* buffer, int flags)
			: _buffer(buffer), _flags(flags)
			{}

			virtual ~XmlStreamBase()
			{}

			int flags() const
			{ return _flags; }

			void setFlags(int flags)
			{ _flags = flags; }

			NamespaceContext& namespaceContext()
			{ return _namespaceContext; }

			Resolver& resolver()
			{ return _resolver; }

			void resolveEntities(String& str) throw(ParseError);

			XmlStreamBuffer* buffer()
			{ return _buffer; }

		private:
			int _flags;
			Xml::Resolver _resolver;
			NamespaceContext _namespaceContext;
			XmlStreamBuffer* _buffer;
	};


	class PT_API XmlIStream : virtual public XmlStreamBase, protected std::basic_istream<Char> {
		public:
			XmlIStream(std::istream& is, int flags = Normal) throw(IO::IOError)
			: XmlStreamBase( new XmlStreamBuffer(is), flags ),
			  std::basic_istream<Char>( this->buffer() ),
			  _buffer( this->buffer() )
			{ this->init(); }

			virtual ~XmlIStream() throw();

			operator void*() const;
			bool operator!() const;

			std::auto_ptr<Xml::Node> readNode() throw(IO::IOError, ParseError);
			std::auto_ptr<Xml::Node> peekNode() throw(IO::IOError, ParseError);
			std::auto_ptr<StartElement> readStartElement() throw(IO::IOError, ParseError);
			std::auto_ptr<StartElement> readStartElement(const String& name) throw(IO::IOError, ParseError);
			std::auto_ptr<EndElement> readEndElement() throw(IO::IOError, ParseError);
			std::auto_ptr<EndElement> readEndElement(const String& name) throw(IO::IOError, ParseError);
			std::auto_ptr<Characters> readCharacters() throw(IO::IOError, ParseError);
			std::auto_ptr<CData> readCData() throw(IO::IOError, ParseError);

			void sync()
			{ std::basic_istream<Char>::sync(); }

		protected:
			XmlIStream(XmlStreamBuffer* buffer, int flags = Normal) throw(IO::IOError)
			: XmlStreamBase(buffer, flags),
			  std::basic_istream<Char>(buffer)
			{ this->init(); }

			void init();

		private:
			std::auto_ptr< std::basic_streambuf<Char> > _buffer;
	};


	class PT_API XmlOStream : virtual public XmlStreamBase, protected std::basic_ostream<Char> {
		public:
			XmlOStream(std::ostream& os, int flags = Normal) throw(IO::IOError)
			: XmlStreamBase( new XmlStreamBuffer(os), flags ),
			  std::basic_ostream<Char>( this->buffer() ),
			  _buffer( this->buffer() )
			{ }

			virtual ~XmlOStream() throw()
			{ }

			operator void*() const
			{ return std::basic_ostream<Pt::Char>::operator void*(); }

			bool operator!() const
			{ return std::basic_ostream<Pt::Char>::operator!(); }

			XmlOStream& writeStartElement(const Pt::String& name)
			{
				this->buffer()->sputc('<');
				this->buffer()->sputn( name.c_str(), name.size() );
				this->buffer()->sputc('>');
				return *this;
			}

			XmlOStream& writeEndElement(const Pt::String& name)
			{
				this->buffer()->sputc('<');
				this->buffer()->sputc('/');
				this->buffer()->sputn( name.c_str(), name.size() );
				this->buffer()->sputc('>');
				return *this;
			}

			XmlOStream& writeCharacters(const Pt::String& chars)
			{
				this->buffer()->sputn( chars.c_str(), chars.size() );
				return *this;
			}

			void flush()
			{ std::basic_ostream<Char>::flush(); }

		protected:
			XmlOStream(XmlStreamBuffer* buffer, int flags = Normal) throw(IO::IOError)
			: XmlStreamBase(buffer, flags),
			  std::basic_ostream<Char>(buffer)
			{ }

		private:
			std::auto_ptr<XmlStreamBuffer> _buffer;
	};

	PT_API XmlOStream& operator<<(XmlOStream&, const Pt::Xml::StartElement& element);


	class PT_API XmlIOStream : public XmlIStream, public XmlOStream {
		public:
			XmlIOStream(std::iostream& ios, int flags = Normal) throw(IO::IOError)
			: XmlStreamBase(new XmlStreamBuffer(ios), flags),
			  XmlIStream( this->buffer(), flags ),
			  XmlOStream( this->buffer(), flags ),
			  _buffer( this->buffer() )
			{ }

			~XmlIOStream() throw()
			{ }

			operator void*() const
			{ return XmlIStream::operator void*(); }

			bool operator!() const
			{ return XmlIStream::operator!(); }

		private:
			std::auto_ptr<XmlStreamBuffer> _buffer;
	};

}

}

#endif
