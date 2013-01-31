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
#include <Pt/Xml/EndDocument.h>
#include <Pt/String.h>
#include <Pt/StringStream.h>
#include <Pt/TextBuffer.h>
#include <Pt/Utf8Codec.h>
#include <iosfwd>

namespace Pt {

namespace Xml {

class Node;
class DocType;
class StartElement;
class EntityResolver;

class InputSource
{
    public:
        typedef std::basic_streambuf<Char>::int_type int_type;

    public:
        virtual ~InputSource()
        {}

        std::size_t refs() const
        { return _refs; }

        std::size_t line() const
        { return _line; }

        void setLine(std::size_t n)
        { _line = n; }

        void bumpLine()
        { ++_line; }

        bool advance()
        {                               
            if( ! rdbuf() )
                return false;
                    
            if( rdbuf()->in_avail() > 0 )
                return true;
                        
            return this->onAdvance();
        }

        std::basic_streambuf<Char>* rdbuf()
        { return _rdbuf; }

    protected:
        InputSource(std::basic_streambuf<Char>* sb = 0, std::size_t refcnt = 0)
        : _refs(refcnt)
        , _rdbuf(sb)
        , _line(1)
        {}

        void init(std::basic_streambuf<Char>* sb)
        { _rdbuf = sb; }

        virtual bool onAdvance() = 0;

        //TODO: close() -> user might want to recycle input sources

    private:
        std::size_t _refs;
        std::basic_streambuf<Char>* _rdbuf;
        std::size_t _line;
};

class StringInputSource : public InputSource
{
    public:
        StringInputSource(const String& str, std::size_t refcnt = 0)
        : InputSource(0, refcnt)
        , _sbuf(str)
        {
            init(&_sbuf);
        }

        virtual ~StringInputSource()
        { }

    protected:
        virtual bool onAdvance()
        {
            if(_sbuf.in_avail() <= 0)
                return false;

            return true;
        }

    private:
        StringBuffer _sbuf;
};

class ByteInputSource : public InputSource
{
    public:
        ByteInputSource(std::istream& is, std::size_t refcnt = 0)
        : InputSource(0, refcnt)
        , _tbuf(&is, new Utf8Codec)
        , _is(&is)
        {
            init(&_tbuf);
        }

        virtual ~ByteInputSource()
        { }

    protected:
        virtual bool onAdvance()
        {
            _tbuf.import();

            if( _is->eof() )
                return false;

            return true;
        }

    private:
        TextBuffer _tbuf;
        std::istream* _is;
};

class TextInputSource : public InputSource
{
    public:
        TextInputSource(TextBuffer& tb, std::size_t refcnt = 0)
        : InputSource(&tb, refcnt)
        , _tbuf(&tb)
        , _eof(false)
        { }

        virtual ~TextInputSource()
        { }

    protected:
        virtual bool onAdvance()
        {
            _tbuf->import();
            
            if( _eof )
                return false;

            return true;
        }

        void setEof()
        { _eof = true; }
    
    private:
        TextBuffer* _tbuf;
        bool _eof;
};

class TextInputSource2 : public InputSource
{
    public:
        TextInputSource2(std::basic_istream<Char>& is, std::size_t refcnt = 0)
        : InputSource(is.rdbuf(), refcnt)
        , _eof(false)
        { }

        virtual ~TextInputSource2()
        { }

    protected:
        virtual bool onAdvance()
        {
            return true;
        }

        void setEof()
        { _eof = true; }
    
    private:
        bool _eof;
};

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
class PT_XML_API XmlReader
{
    public:
        class Iterator;

    public:
        /* TODO: Consider the following processing flags:
                     - IgnoreProcessingInstructions
                     - IgnoreWhitespace
                     - ReportComments
                     - ReportDocumentStart
                     - ReportCData (not as Characters)

           TODO: how do we handle document encoding and codec selection?
                 - ctor with byte stream looks at leading bytes to guess codec
                 - ctor with unicode text stream uses user defined codec
        */
        explicit XmlReader(std::istream& is, int flags = 0);

        explicit XmlReader(std::basic_istream<Char>& is, int flags = 0);

        explicit XmlReader(InputSource& is, int flags = 0);

        ~XmlReader();

        // TODO: split into attach() and setFlags()
        // also add methods for discard() and reset()
        void attach(std::basic_istream<Char>& is, int flags = 0);

        void attach(std::istream& is, int flags = 0);

        void attach(InputSource& is, int flags);

        /** @brief Adds an external input source.

            This method can be used to add additional input streams e.g.
            to resolve an external entity reference, indicated by an
            EntityReference node.

            If the reference counter of the input source is 0, it will be
            deleted when it is no longer needed. So, if an input source is
            added which was created on the stack, its refernce count must
            be greater than 0.
        */
        void addInputSource(InputSource* in);

        void addInput(const Char* str);

        const Pt::String& version() const;

        const Pt::String& encoding() const;

        bool isStandalone() const;

        const DocType& docType() const;

        size_t depth() const;

        std::size_t line() const;

        Iterator current();

        Iterator end() const;

        //! @brief Get current element
        Node& get();

        //! @brief Get next element from stream
        Node& next();

        //! @brief Process availabe data from underlying stream
        bool advance();

        // TODO:
        // also need a way to finish() after advance(), so synthetic eof is 
        // processed and we fail parsing or advance to EndDocument

    private:
        class XmlReaderImpl* _impl;
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
