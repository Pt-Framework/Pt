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

/** @brief Reads XML as a Stream of XML Nodes.

     This class operates on an input stream from which XML character data
     is read and parsed.

     The parser will only parse the XML document as far as the user read
     data from it. To read the current element (Node) the method get() can
     be used. To parse and read the next element the method next() can be
     used. Only when next() or any corresponding method or operator is
     called, the next chunk of XML input data is parsed.

     To parse a XML-document, a XmlReader constructed with an input stream
     from which the XML document is to be read.

     The current XML element (Node) can be read using get(). Every call to
     next() will parse the next element, position the cursor to the next
     element and return the parsed element. The returned element is of type
     Node, which is the super-class for all XML element classes. The class
     Node has a method type() which returns the type of the read element.
     Depending on the type the generic Node object may be cast to the more
     concrete element object. For example a Node object with a node type of
     Node::StartElement can be cast to StartElement.

     Parsing using next() will continue until the end of the document is
     reached which will resultin a EndDocument element to be returned by
     next() and get().

     This class also provides the method current() to obtain an iterator
     which basically works the same way like using using get() and next()
     directly. The iterator can be set to the next element by using the
     ++ operator. The current element can be accessed by dereferencing
     the iterator.

     @see Node
*/
class PT_XML_API XmlReader : private NonCopyable
{
    public:
        class Iterator;

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

        /** @brief Clears the reader state and input.

            All input sources are removed and the parser state is reset to
            parse a new document. The XmlResolver not removed and the reporting
            options are not changed.
        */
        void reset();

        // TODO: add methods for attach(), detach(), discard(), reset(), clear()

        /** @brief Starts parsing with an input source.

            All previous input is removed and the parser is reset to parse
            a new document. This is essentially the same as calling clear()
            followed by addInput().
        */
        void reset(InputSource& is);

        /** @brief Adds an external input source.

            This method can be used to add additional input streams e.g.
            to resolve an external entity reference, indicated by an
            EntityReference node.
        */
        void addInput(InputSource& in);

        XmlResolver* resolver() const;

        // Max number of characters the parser may allocate.
        //
        // prevents "long name attack" and 
        // "long content attack", "long attribute-list attack"
        void setMaxInputSize(std::size_t n);

        std::size_t maxSize() const;

        std::size_t usedSize() const;

        // add isChunk method to Characters
        void setChunkSize(std::size_t n);

        // prevents the "billion laughs attack"
        void setMaxInputDepth(std::size_t n);

        void reportStartDocument(bool value);

        void reportDocType(bool value);

        void reportProcessingInstructions(bool value);

        void reportCData(bool value);

        void reportComments(bool value);

        void reportEntityReferences(bool value);

        // whitespace between start tags or end tags
        // void reportBoundaryWhitespace(bool value);

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
        Iterator current();

        /** @brief Returns an iterator to the end of the document.
        */
        Iterator end() const;

        //! @brief Get current element.
        Node& get();

        //! @brief Get next element from stream.
        Node& next();

        /** @brief Process availabe data from underlying input source.
        */
        Node* advance();

        /** @brief Returns the current input source or nullptr if none is set.
        */
        InputSource* input();

    private:
        class XmlReaderImpl* _impl;

    public:
        XmlReaderImpl* impl()
        { return _impl; }
};

// TODO: rename InputIterator
class XmlReader::Iterator
{
    public:
        Iterator()
        : _stream(0)
        , _node(0)
        { }

        explicit Iterator(XmlReader& xis)
        : _stream(&xis)
        , _node(0)
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

        inline Node& operator*()
        { return *_node; }

        inline Node* operator->()
        { return _node; }

        Iterator& operator++()
        {
            if(_node->type() == Node::EndDocument)
                _node = 0;
            else
                _node = &_stream->next();

            return *this;
        }

        inline bool operator==(const Iterator& it) const
        { return _node == it._node; }

        inline bool operator!=(const Iterator& it) const
        { return _node != it._node; }

    private:
        XmlReader* _stream;
        Node* _node;
};


inline XmlReader::Iterator XmlReader::current()
{
    return Iterator(*this); 
}


inline XmlReader::Iterator XmlReader::end() const
{
    return Iterator(); 
}

}

}

#endif
