/*
   Copyright (C) 2015-2023 by Dr. Marc Boris Duerner
  
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   
   As a special exception, you may use this file as part of a free
   software library without restriction. Specifically, if other files
   instantiate templates or use macros or inline functions from this
   file, or you compile this file and link it with other files to
   produce an executable, this file does not by itself cause the
   resulting executable to be covered by the GNU General Public
   License. This exception does not however invalidate any other
   reasons why the executable file might be covered by the GNU Library
   General Public License.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
   MA 02110-1301 USA
*/

#ifndef PT_JSON_JSON_READER_H
#define PT_JSON_JSON_READER_H

#include <Pt/Json/Api.h>
#include <Pt/Json/Node.h>
#include <Pt/IOStream.h>
#include <Pt/String.h>
#include <Pt/NonCopyable.h>

namespace Pt {

namespace Json {

class Node;
class InputSource;
class InputIterator;
class JsonReaderImpl;

/** @brief Reads JSON as a Stream of Nodes.
*/
class PT_JSON_API JsonReader : private NonCopyable
{
    public:
        /** @brief Default Constructor.
        */
        JsonReader();

        /** @brief Construct with input source.
        */
        explicit JsonReader(std::basic_istream<Pt::Char>& is);

        /** @brief Destructor.
        */
        ~JsonReader();

        /** @brief Returns the current input source or nullptr if none is set.
        */
        std::basic_istream<Pt::Char>* input();

        /** @brief Sets the input source.
        */
        void attach(std::basic_istream<Pt::Char>& is);

        /** @brief Clears the reader state and input.

            All input sources are removed and the parser state is reset to
            parse a new document.
        */
        void reset();

        /** @brief Starts parsing with an input source.

            All previous input is removed and the parser is reset to parse
            a new document. This is essentially the same as calling reset()
            followed by addInput().
        */
        void reset(std::basic_istream<Pt::Char>& is);

        /** @brief Sets the max size of a characters block.

            If an JSON element contains more character data than this limit,
            the content is reported as multiple nodes.
        */
        void setChunkSize(std::size_t n);

        /** @brief Sets the max number of characters the parser may allocate.
        */
        void setMaxSize(std::size_t n);

        /** @brief Returns the number of characters the parser may allocate.
        */
        std::size_t maxSize() const;

        /** @brief Returns the number of characters the parser has allocated.
        */
        std::size_t usedSize() const;

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

    public:
        JsonReaderImpl* impl()
        { return _impl; }

    private:
        JsonReaderImpl* _impl;
};

/** @brief Input iterator to read JSON nodes with a reader.
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
        explicit InputIterator(JsonReader& xis)
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
        JsonReader* _stream;
        Node*       _node;
};


inline InputIterator JsonReader::current()
{
    return InputIterator(*this); 
}


inline InputIterator JsonReader::end() const
{
    return InputIterator(); 
}

} // namespace

} // namespace

#endif // include guard
