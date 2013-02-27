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
#include <Pt/Xml/InputSource.h>
#include <Pt/String.h>
#include <stack>
#include <iosfwd>

namespace Pt {

namespace Xml {

class Node;
class DocTypeDefinition;
class InputSource;

class InputStack
{
    public:
        typedef std::char_traits<Char>::int_type int_type;

    public:
        InputStack()
        : _input(&_nullInput)
        , _externalDtd(0)
        , _currentInput(&_nullInput)
        {}

        ~InputStack()
        {
            clear();
        }

        void bumpLine()
        { _currentInput->setLine( _currentInput->line() + 1 ); }

        std::size_t line() const
        { return _currentInput->line(); }

        bool empty() const
        { return _currentInput == &_nullInput; }

        void clear()
        {
            while( ! _external.empty() )
            {
                removeInput();
            }

            _currentInput = &_nullInput;
            _input = &_nullInput;
        }
                
        InputSource* currentInput()
        { return _currentInput; }

        void setInput(InputSource& is)
        {
            _input = &is;

            if( _external.empty() )
            {
                _currentInput = &is;
            }
        }

        void addInput(InputSource* is)
        {
            std::auto_ptr<InputSource> isPtr;
            if(is->refs() == 0)
                isPtr.reset(is);

            _external.push(is);
            isPtr.release();

            _currentInput = is;
        }

        void setExternalDtd(InputSource* is)
        {
            addInput(is);
            _externalDtd = is;
        }

        InputSource* externalDtd()
        { return _externalDtd; }

        bool isExternalDtd() const
        { return _externalDtd != 0; }

        void removeInput()
        {
            _currentInput = &_nullInput;

            if( ! _external.empty() )
            {
                InputSource* is = _external.top();                        

                if( is == _externalDtd ) 
                    _externalDtd = 0;

                if( is->refs() == 0 )
                    delete _external.top();

                _external.pop();
                        
                _currentInput = _external.empty() ? _input 
                                                  : _external.top();
            }
        }

    private:
        NullInputSource _nullInput;
        InputSource* _input;
        InputSource* _externalDtd;
        std::stack<InputSource*> _external;
        InputSource* _currentInput;
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

        enum Flags
        {
            ReportDtd = 1
        };

    public:
        /* TODO: Consider the following processing flags:
                     - ReportDtd
                     - ReportProcessingInstructions
                     - ReportWhitespace
                     - ReportComments
                     - ReportDocumentStart
                     - ReportCData (not as Characters)
        */
        explicit XmlReader(std::istream& is, int flags = 0);

        explicit XmlReader(InputSource& is, int flags = 0);

        ~XmlReader();

        // TODO: split into attach() and setFlags()
        // also add methods for discard() and reset()
        void attach(std::istream& is, int flags = 0);

        void attach(InputSource& is, int flags = 0);

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

        const DocTypeDefinition& dtd() const;

        size_t depth() const;

        std::size_t line() const;

        Iterator current();

        Iterator end() const;

        //! @brief Get current element
        Node& get();

        //! @brief Get next element from stream
        Node& next();

        /** @brief Process availabe data from underlying stream
            
            When the last stream is removed, EOF will be parsed and an
            EndDocument node becomes available.
        */
        bool advance();

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
