/*
 * Copyright (C) 2009-2012 Marc Boris Duerner
 *
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

#ifndef Pt_Xml_XmlReader_h
#define Pt_Xml_XmlReader_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/Node.h>
#include <Pt/String.h>
#include <Pt/NonCopyable.h>
#include <iosfwd>

namespace Pt {

namespace Xml {

class Node;
class DocTypeDefinition;
class InputSource;
class XmlResolver;
class InputIterator;

/** @brief Reads XML as a Stream of XML Nodes.

     This class operates on an input source from which XML character data
     is read and parsed. The content of the XML document is reported as XML
     nodes.

     The parser will only parse the XML document as far as the user read
     data from it. To acces the current node the method get() can
     be used. To parse and read the next node the method next() can be
     used. Only when next() or any corresponding method or operator is
     called, the next chunk of XML input data is parsed.

     The current XML node can be read using get(). Every call to
     next() will parse the next node, position the cursor to the next
     node and return the parsed node. The returned value is of type
     Node, which is the super-class for all XML node classes.

     Depending on the type, the generic node object may be cast to the more
     concrete node object. For example a Node object with a node type of
     Node::StartElement can be cast to StartElement.

     Parsing using next() will continue until the end of the document is
     reached which will result in a EndDocument node to be returned by
     next() and get().

     This class also provides the method current() to obtain an iterator
     which basically works the same way like using using get() and next()
     directly. The iterator can be set to the next node by using the
     ++ operator. The current node can be accessed by dereferencing
     the iterator.
*/
class PT_XML_API XmlReader : private NonCopyable
{
    public:
        /** @brief Default Constructor.
        */
        XmlReader();

        /** @brief Construct with input source.
        */
        explicit XmlReader(InputSource& is);

        /** @brief Construct with resolver and input source.
        */
        XmlReader(XmlResolver& r, InputSource& is);

        /** @brief Destructor.
        */
        ~XmlReader();

        /** @brief Returns the resolver or nullptr is none was set.
        */
        XmlResolver* resolver() const;

        /** @brief Returns the current input source or nullptr if none is set.
        */
        InputSource* input();

        /** @brief Adds an external input source.

            This method can be used to add additional input streams e.g.
            to resolve an external entity reference, indicated by an
            EntityReference node.
        */
        void addInput(InputSource& in);

        /** @brief Clears the reader state and input.

            All input sources are removed and the parser state is reset to
            parse a new document. The XmlResolver not removed and the reporting
            options are not changed.
        */
        void reset();

        /** @brief Starts parsing with an input source.

            All previous input is removed and the parser is reset to parse
            a new document. This is essentially the same as calling reset()
            followed by addInput().
        */
        void reset(InputSource& is);

        /** @brief Sets the max size of a characters block.

            If an XML element contains more character data than this limit,
            the content is reported as multiple Characters or CData nodes.
        */
        void setChunkSize(std::size_t n);

        /** @brief Sets the max expansion depth of input sources.
        */
        void setMaxInputDepth(std::size_t n);

        /** @brief Sets the max number of characters the parser may allocate.
        */
        void setMaxSize(std::size_t n);

        /** @brief Returns the number of characters the parser may allocate.
        */
        std::size_t maxSize() const;

        /** @brief Returns the number of characters the parser has allocated.
        */
        std::size_t usedSize() const;

        /** @brief Configures the parser to report the start of the document.
        */
        void reportStartDocument(bool value);

        /** @brief Configures the parser to report DOCTYPEs.
        */
        void reportDocType(bool value);

        /** @brief Configures the parser to report processing instructions.
        */
        void reportProcessingInstructions(bool value);

        /** @brief Configures the parser to report CDATA sections.
        */
        void reportCData(bool value);
        
        /** @brief Configures the parser to report comments.
        */
        void reportComments(bool value);

        /** @brief Configures the parser to report entity references.
        */
        void reportEntityReferences(bool value);

        /** @brief Returns current DTD of the document.
        */
        DocTypeDefinition& dtd();

        /** @brief Returns current DTD of the document.
        */
        const DocTypeDefinition& dtd() const;

        /** @brief Returns the XML tree depth.
        */
        std::size_t depth() const;

        /** @brief Returns the current line of the primary input source.
        */
        std::size_t line() const;

        /** @brief Returns an iterator to the current node.
        */
        InputIterator current();

        /** @brief Returns an iterator to the end of the document.
        */
        InputIterator end() const;

        /** @brief Get current node.
        */
        Node& get();

        /** @brief Get next node.
        */
        Node& next();

        /** @brief Process availabe data from underlying input source.
        */
        Node* advance();

    private:
        class XmlReaderImpl* _impl;

    public:
        XmlReaderImpl* impl()
        { return _impl; }
};

/** @brief Input iterator to read XML nodes with an XmlReader.
*/
class InputIterator
{
    public:
        /** @brief Default Constructor.
        */
        InputIterator()
        : _stream(0)
        , _node(0)
        { }

        /** @brief Construct iterator to point to current document position.
        */
        explicit InputIterator(XmlReader& xis)
        : _stream(&xis)
        , _node(0)
        { _node = &_stream->get(); }

        /** @brief Copy constructor.
        */
        InputIterator(const InputIterator& it)
        : _stream(it._stream), _node(it._node)
        { }

        /** @brief Destructor.
        */
        ~InputIterator()
        { }

        /** @brief Assignment operator.
        */
        InputIterator& operator=(const InputIterator& it)
        {
            _stream = it._stream;
            _node = it._node;
            return *this;
        }

        /** @brief Derefences the iterator.
        */
        inline Node& operator*()
        { return *_node; }

        /** @brief Derefences the iterator.
        */
        inline Node* operator->()
        { return _node; }

        /** @brief Increments the iterator position.
        */
        InputIterator& operator++()
        {
            if(_node->type() == Node::EndDocument)
                _node = 0;
            else
                _node = &_stream->next();

            return *this;
        }

        /** @brief Returns true if both iterators point at the same node.
        */
        inline bool operator==(const InputIterator& it) const
        { return _node == it._node; }

        /** @brief Returns true if iterators point to different nodes.
        */
        inline bool operator!=(const InputIterator& it) const
        { return _node != it._node; }

    private:
        XmlReader* _stream;
        Node* _node;
};


inline InputIterator XmlReader::current()
{
    return InputIterator(*this); 
}


inline InputIterator XmlReader::end() const
{
    return InputIterator(); 
}

} // namespace Xml

} // namespace Pt

#endif // Pt_Xml_XmlReader_h
