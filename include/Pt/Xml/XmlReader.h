/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef PTV_Xml_XmlStream_h
#define PTV_Xml_XmlStream_h

#include <Pt/String.h>
#include <Pt/Text/TextStream.h>
#include <Pt/Xml/Api.h>
#include <Pt/Xml/Node.h>
#include <Pt/Xml/EndDocument.h>
#include <Pt/Xml/ProcessingInstruction.h>
#include <Pt/Xml/StartElement.h>
#include <Pt/Xml/EndElement.h>
#include <Pt/Xml/Characters.h>
#include <Pt/Xml/EndDocument.h>
#include <Pt/Xml/Resolver.h>
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

    class XmlParseState;

/** @brief An input stream for parsing an XML document utilizing Pull Parsing.

     This class operates on another istream or TextStream from which it reads the XML character
     data, parses it and lastly makes available the parsed XML elements (Nodes).

     A pull parser will only parse the XML document as far as the user read data from it.
     To read the current element (Node) the method get() can be used. To parse and read the next
     element the method next() can be used. Only when next() or any corresponding method or operator
     is called the next chunk of XML input data is parsed.

     To parse an XML-document an object of this class is created and passed the input stream
     for the document. The constructor initializes the parser, already parses the Xml-declaration
     and positions the internal cursor to the first actual element (Node) of the XML document.

     The current XML element (Node) can be read using get(). Every call to next() will parse the
     next element, position the cursor to the next element and return the parsed element. The
     returned element is of type Node, which is the super-class for all XML element classes. The
     class Node has a method type() which returns the type of the read element. Depending on the
     type the generic Node object may be cast to the more concrete element object. For example
     a Node object with a node type of Node::StartElement can be cast to StartElement.

     Parsing using next() will continue until the end of the document is reached which will result
     in a EndDocument element to be returned by next() and get().

     This class also provides the method current() to obtain an iterator which basically works the
     same as using using get() and next() directly. The cursor can be set to the next element by
     using the ++ operator of the iterator. The current element can be accessed by using the *
     operator.

     @see Node
*/
class PT_XML_API XmlReader
{
    public:
        static EndDocument& documentEnd()
        {
            static EndDocument _enddoc;
            return _enddoc;
        }

        class Iterator
        {
            public:
                Iterator()
                : _stream(0), _node( &XmlReader::documentEnd() )
                { }

                Iterator(XmlReader& xis)
                : _stream(&xis), _node(0)
                { _node = &_stream->get(); }

                Iterator(const Iterator& it)
                : _stream(it._stream), _node(it._node)
                { }

                ~Iterator()
                { }

                Iterator& operator=(const Iterator& it)
                {
                    _stream = it._stream;
                    _node = it._node;
                    return *this;
                }

                inline const Node& operator*() const
                { return *_node; }

                inline const Node* operator->() const
                { return _node; }

                Iterator& operator++()
                {
                    _node = &_stream->next();
                    return *this;
                }

                inline bool operator==(const Iterator& it) const
                { return _node == it._node; }

                inline bool operator!=(const Iterator& it) const
                { return _node != it._node; }

            private:
                XmlReader* _stream;
                const Node* _node;
        };

    public:
        XmlReader(std::istream& is);

        XmlReader(Text::TextStream& is);

        ~XmlReader();

        const Pt::String& version() const
        { return _version; }

        const Pt::String& encoding() const
        { return _encoding; }

        bool standalone() const
        { return _standalone; }

        size_t depth() const
        {
            return _depth;
        }

        Iterator current()
        {
            return Iterator(*this);
        }

        Iterator end() const
        {
            return Iterator();
        }

        //! @brief Get current element
        const Node& get();

        //! @brief Get next element from stream
        const Node& next();

        bool advance();

        const StartElement& nextElement();

        const Node& nextTag();
/*
        XmlReader& operator>>(StartElement& se);

        XmlReader& operator>>(EndElement& e);

        XmlReader& operator>>(Characters& e);
*/
        std::size_t line() const
        { return _line; }

        void resolveEntities(String& str);

    public:
        std::basic_streambuf<Char>* _textBuffer;
        std::basic_streambuf<Char>* _buffer;
        Resolver _resolver; /// TODO must be EntityResolver
        size_t _depth;
        std::size_t _line;

    public:

        Pt::String _version;
        Pt::String _encoding;
        bool _standalone;


        XmlParseState* _state;
        Node* _current;
        ProcessingInstruction _procInstr;
        StartElement _startElem;
        EndElement _endElem;
        Characters _chars;
        Attribute _attr;
};

}

}

#endif
