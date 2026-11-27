/*
 * Copyright (C) 2012 by Marc Boris Duerner
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

#ifndef Pt_Http_Message_h
#define Pt_Http_Message_h

#include <Pt/Http/Api.h>
#include <Pt/NonCopyable.h>
#include <iostream>
#include <streambuf>
#include <string>
#include <cstring>
#include <cstddef>

namespace Pt {

namespace Http {

class Connection;
class MessageHeader;

/** @brief HTTP message header.

    %MessageHeader is the field list and HTTP version of a message. It
    is the header half of %Message, and through that of %Request and
    %Reply. Callers do not usually construct a header on its own; they
    use %header() on the message the client or responder already holds.

    Fields are names and stored values. %set() replaces the value for
    a name, %add() appends another value for the same name, and
    %remove() deletes it. %get() returns the stored value or a null
    pointer. %has(name) reports presence. %contains(name, value)
    reports that a list token appears for that name, across every
    stored line and after comma-splitting. %matches(name, value)
    reports that the first field of that name has exactly that one
    list value. Both comparisons ignore ASCII case. %isSet() compares
    the whole first stored string exactly. %field() returns an iterator to the first field of that
    name, or %end() when the name is absent. %begin() and %end() walk
    the fields as %FieldIterator.
    Each field is one stored value. A comma-separated list stays
    inside that value, and %begin() and %end() on the field walk it as
    list values.

    Keep-alive, chunked transfer coding, content length and Upgrade are
    not a second header model: they are derived from those fields.
    %isKeepAlive(), %isChunked(), %contentLength() and %isUpgrade()
    read them, while %setKeepAlive() and %setUpgrade() write the
    corresponding fields. HTTP version is %versionMajor() and
    %versionMinor(), set together with %setVersion().

    The header has a fixed size, so the caller must not rely on filling
    it beyond that limit. %clear() empties the fields so the same
    header can be used for another message.

    @ingroup Pt-Http-Messages
*/
class PT_HTTP_API MessageHeader : private Pt::NonCopyable
{
    public:
        class Field;
        class FieldIterator;
        class Value;
        class ValueIterator;

        /** @brief One value of a list-based header field.

            The value refers to bytes in the stored field. It is valid
            for the life of that field view. %data() is not
            null-terminated at %size().
        */
        class PT_HTTP_API Value
        {
            public:
                /** @brief Creates an empty value.
                */
                Value()
                : _data(0)
                , _size(0)
                {}

                /** @brief Returns the first byte of the value.
                */
                const char* data() const
                { return _data; }

                /** @brief Returns the number of bytes in the value.
                */
                std::size_t size() const
                { return _size; }

                /** @brief Returns true if the value contains no bytes.
                */
                bool empty() const
                { return _size == 0; }

                /** @brief Returns true if the value equals @a value.

                    The comparison is case-sensitive.
                */
                bool equals(const char* value) const;

                /** @brief Returns true if the value matches @a value.

                    The comparison ignores ASCII case.
                */
                bool matches(const char* value) const;

            private:
                friend class ValueIterator;

                Value(const char* data, std::size_t size)
                : _data(data)
                , _size(size)
                {}

            private:
                const char* _data;
                std::size_t _size;
        };

        /** @brief Iterator over the values of one header field.
        */
        class PT_HTTP_API ValueIterator
        {
            public:
                /** @brief Creates the end iterator.
                */
                ValueIterator()
                : _current(0)
                , _end(0)
                , _value()
                {}

                /** @brief Returns true if both iterators are at the same value.
                */
                bool operator==(const ValueIterator& other) const;

                /** @brief Returns true if the iterators are not at the same value.
                */
                bool operator!=(const ValueIterator& other) const
                { return ! operator==(other); }

                /** @brief Advances to the next value.
                */
                ValueIterator& operator++();

                /** @brief Returns the current value.
                */
                Value operator*() const
                { return _value; }

                /** @brief Returns the current value.
                */
                const Value* operator->() const
                { return &_value; }

            private:
                friend class Field;

                ValueIterator(const char* begin, const char* end);

                void advance();

            private:
                const char* _current;
                const char* _end;
                Value _value;
        };

        /** @brief One field of an HTTP message header.

            %Field is a name and one stored value. It is a view into
            the header that holds it. The view is valid until that
            header is modified. Callers obtain it from
            %MessageHeader::field() or from a %FieldIterator, and use
            it through that iterator. They do not keep it across a
            change of the header.

            %name() is the field name. %equals() compares that name
            exactly, and %matches() compares it while ignoring ASCII
            case. A null pointer never matches. %value() is the whole
            stored value, including commas and optional whitespace. A
            comma-separated list is one stored value. %begin() and
            %end() walk that stored value as list values. A %Value is
            a view into the same stored bytes. Reading it does not
            allocate. Empty values are skipped. A value that is not a
            token ends the sequence.

            A token is one or more visible ASCII characters other than
            the separators `( ) < > @ , ; : \\ " / [ ] ? = { }` and
            other than space, controls, and DEL.
            %MessageHeader::isToken() reports that classification. A
            field name is a token. A pseudo-header name begins with
            `:` and is not a token.

            @ingroup Pt-Http-Messages
        */
        class PT_HTTP_API Field
        {
            public:
                /** @brief Creates an empty field.
                */
                Field()
                : _name(0)
                , _nameSize(0)
                , _value(0)
                , _valueSize(0)
                {}

                /** @brief Returns the field name.

                    A missing field returns a null pointer.
                */
                const char* name() const
                { return _name; }

                /** @brief Returns the number of bytes in the field name.
                */
                std::size_t nameSize() const
                { return _nameSize; }

                /** @brief Returns true if the field name equals @a name.

                    The comparison is case-sensitive. A null pointer
                    never matches.
                */
                bool equals(const char* name) const;

                /** @brief Returns true if the field name matches @a name.

                    The comparison ignores ASCII case. A null pointer
                    never matches.
                */
                bool matches(const char* name) const;

                /** @brief Returns the stored field value.

                    The value includes commas and optional whitespace.
                    A missing field returns a null pointer.
                */
                const char* value() const
                { return _value; }

                /** @brief Returns the number of bytes in the stored value.
                */
                std::size_t valueSize() const
                { return _valueSize; }

                /** @brief Returns true if this field contains no list value.
                */
                bool empty() const
                { return begin() == end(); }

                /** @brief Returns the first value of this field.
                */
                ValueIterator begin() const;

                /** @brief Returns the end of the value sequence.
                */
                ValueIterator end() const
                { return ValueIterator(); }

            private:
                friend class FieldIterator;

                Field(const char* name, std::size_t nameSize,
                      const char* value, std::size_t valueSize)
                : _name(name)
                , _nameSize(nameSize)
                , _value(value)
                , _valueSize(valueSize)
                {}

            private:
                const char* _name;
                std::size_t _nameSize;
                const char* _value;
                std::size_t _valueSize;
        };

        /** @brief Iterator over the fields of a header.
        */
        class FieldIterator
        {
            friend class MessageHeader;

            public:
                /** @brief Creates the end iterator.
                */
                FieldIterator()
                { }

                //! @internal
                explicit FieldIterator(const char* p)
                : _current(p, p ? std::strlen(p) : 0, p, 0)
                {
                    fixup();
                }

                /** @brief Returns true if both iterators are at the same field.
                */
                bool operator==(const FieldIterator& it) const
                { return _current.name() == it._current.name(); }

                /** @brief Returns true if the iterators are not at the same field.
                */
                bool operator!=(const FieldIterator& it) const
                { return _current.name() != it._current.name(); }

                /** @brief Advances to the next field.
                */
                FieldIterator& operator++()
                {
                    moveForward();
                    return *this;
                }

                /** @brief Advances to the next field.
                */
                FieldIterator operator++(int)
                {
                    FieldIterator ret = *this;
                    moveForward();
                    return ret;
                }

                /** @brief Returns the header field.
                */
                const Field& operator*() const
                { return _current; }

                /** @brief Returns the header field.
                */
                const Field* operator->() const
                { return &_current; }

            private:
                //! @internal
                void fixup()
                {
                    if( _current.name() && *_current.name() )
                    {
                        const char* value = _current.name()
                                          + std::strlen(_current.name()) + 1;
                        _current = Field(_current.name(),
                                         std::strlen(_current.name()),
                                         value, std::strlen(value));
                    }
                    else
                    {
                        _current = Field();
                    }
                }

                //! @internal
                void moveForward()
                {
                    const char* name = _current.value() + _current.valueSize() + 1;
                    _current = Field(name, std::strlen(name), name, 0);
                    fixup();
                }

            private:
                Field _current;
        };

    public:
        /** @brief Default constructor.
        */
        MessageHeader();

        /** @brief Destructor.
        */
        ~MessageHeader();

        /** @brief Clears all content.
        */
        void clear();

        /** @brief Sets a header field.
        */
        void set(const char* key, const char* value);

        /** @brief Adds a header field.
        */
        void add(const char* key, const char* value);

        /** @brief Removes a header field.
        */
        void remove(const char* key);

        /** @brief Returns a field value.
        */
        const char* get(const char* key) const;

        /** @brief Returns true if the field is present.
        */
        bool has(const char* key) const
        { return get(key) != 0; }

        /** @brief Returns true if the first field has exactly @a value.

            The first field named @a key must contain one list value,
            and that value must match @a value. The comparison ignores
            ASCII case. A list and a missing field return false.
        */
        bool matches(const char* key, const char* value) const;

        /** @brief Returns true if a list value for @a key matches @a value.

            The search covers every stored line named @a key. A
            comma-separated list is split. The comparison ignores
            ASCII case.
        */
        bool contains(const char* key, const char* value) const;

        /** @brief Returns true if the value equals @a value.

            The comparison uses the whole stored string and is
            case-sensitive.
        */
        bool isSet(const char* key, const char* value) const;

        /** @brief Returns the begin of the header fields.
        */
        FieldIterator begin() const
        { return FieldIterator(_rawdata); }

        /** @brief Returns the end of the header fields.
        */
        FieldIterator end() const
        { return FieldIterator(); }

        /** @brief Returns the first field named @a name.

            A missing field compares equal to %end(). The iterator is
            valid until the next modifying operation on this header.
        */
        FieldIterator field(const char* name) const;

        /** @brief Returns the major HTTP version number.
        */
        unsigned versionMajor() const
        { return _httpVersionMajor; }

        /** @brief Returns the minor HTTP version number.
        */
        unsigned versionMinor() const
        { return _httpVersionMinor; }

        /** @brief Sets the HTTP version number.
        */
        void setVersion(unsigned major, unsigned minor)
        {
            _httpVersionMajor = major;
            _httpVersionMinor = minor;
        }

        /** @brief Returns true if chunked encoding is set.

            True when chunked is the last transfer coding. A coding
            after chunked returns false.
        */
        bool isChunked() const;

        /** @brief Returns the content length.

            The first Content-Length field is read as 1*DIGIT. A missing
            or invalid field returns 0.
        */
        std::size_t contentLength() const;

        /** @brief Returns true if HTTP keep-alive is set.
        */
        bool isKeepAlive() const;

        /** @brief Sets the HTTP keep-alive header.
        */
        void setKeepAlive();

        /** @brief Returns true if Upgrage header is set.
        */
        bool isUpgrade() const;

        /** @brief Sets the Upgrade header.
        */
        void setUpgrade();

        /** @brief Writes a current HTTP date into @a buffer.

            @a buffer must have at least 30 bytes.
        */
        static char* htdateCurrent(char* buffer);

        /** @brief Returns true if @a value is one token.
        */
        static bool isToken(const char* value);

        /** @brief Returns true if @a value is 1*DIGIT without overflow.
        */
        static bool isDigits(const char* value);

        /** @brief Returns true if @a a and @a b are equal.

            The comparison covers the whole strings and ignores ASCII
            case. A null pointer never matches.
        */
        static bool isMatch(const char* a, const char* b);

        /** @brief Returns true if @a ch is SP or HTAB.
        */
        static bool isOws(char ch)
        { return ch == ' ' || ch == '\t'; }

        /** @brief Returns true if @a ch is one token character.

            A token character is a visible ASCII character other than
            the separators `( ) < > @ , ; : \\ " / [ ] ? = { }` and
            other than space, controls, and DEL.
        */
        static bool isTokenChar(unsigned char ch);

        /** @brief Maximum size of the stored header block.
        */
        static const unsigned MaxHeaderSize = 4096;

    private:
        //! @internal
        char* eptr()
        { return _rawdata + _endOffset; }

    private:
        char _rawdata[MaxHeaderSize];  // key_1\0value_1\0key_2\0value_2\0...key_n\0value_n\0\0
        std::size_t _endOffset;
        unsigned _httpVersionMajor;
        unsigned _httpVersionMinor;
};

/** @brief Progress of an asynchronous HTTP send or receive.

    %MessageProgress is the result of one begin/end step on a message.
    The client returns it from %endSend() and %endReceive(), and the
    same flags describe how far a request or a reply has moved.

    The flags are independent and can be combined. %header() means the
    header of that message is now available to read or has been sent.
    %body() means body bytes were processed on this step. %finished()
    means the message is complete, so the caller must not begin the
    same send or receive again. %trailer() means trailer fields were
    processed.

    A short reply often arrives in one receive step, in which case
    header, body and finished are all true. A large body, or a socket
    that accepted only part of a write, returns finished as false, and
    the caller calls begin again. It is also possible that I/O made
    progress without a header or body becoming visible yet, so none of
    the three is true; begin again until finished.

    For a chunked send, finished means the current chunk has left the
    socket, not that the whole request is complete. The completion flag
    on the next %beginSend() decides whether another chunk follows.

    @ingroup Pt-Http-Messages
*/
class MessageProgress
{
    private:
        //! @internal
        enum Result
        {
            InProgress = 1,
            Header     = 2,
            Body       = 4,
            Finished   = 8,
            Trailer    = 16, // NOTE: questionable if we need this
        };

    public:
        /** @brief Default Constructor.
        */
        MessageProgress()
        : _result(InProgress)
        {}

        /** @brief Returns true if the header was processed.
        */
        bool header() const
        { return (_result & Header) == Header; }

        /** @brief Returns true if the body was processed.
        */
        bool body() const
        { return (_result & Body) == Body; }

        /** @brief Returns true if the trailer was processed.
        */
        bool trailer() const
        { return (_result & Trailer) == Trailer; }

        /** @brief Returns true if message is complete.
        */
        bool finished() const
        { return (_result & Finished) == Finished; }

        //! @internal
        void setFinished()
        { _result |= Finished ; }

        //! @internal
        void setHeader()
        { _result |= Header; }

        //! @internal
        void setBody()
        { _result |= Body; }

        //! @internal
        void setTrailer()
        { _result |= Trailer; }

        //! @internal
        unsigned long mask() const
        { return _result; }

    private:
        unsigned long _result;
};

/** @internal
    @brief Output buffer for HTTP messages.
*/
class MessageBuffer : public std::streambuf
{
    public:
        //! @brief Constructs an empty buffer.
        MessageBuffer();

        //! @brief Destructor.
        ~MessageBuffer();

        //! @brief Discards the buffered data.
        void discard()
        {
            this->setp(_buffer, _buffer + _bufferSize);
            this->setg(0,0,0);
        }

        //! @brief Returns the size of the buffered data.
        std::size_t size() const
        { return pptr() - pbase(); }

        //! @brief Returns a pointer to the buffered data.
        const char* data() const
        { return _buffer; }

    protected:
        // @internal
        virtual int_type overflow(int_type ch);

        // @internal
        virtual int_type underflow();

    private:
        static const unsigned int BufferSize = 512;
        char* _buffer;
        std::size_t _bufferSize;
};

/** @brief HTTP message with header and body.

    %Message is the header-and-body object that %Request and %Reply
    extend. The header is %header(), and the body is %body(), an
    iostream. Write the body before sending, and read it when progress
    reports that body bytes are available.

    Callers do not construct a %Message. A %Client owns the request and
    reply, and a %Responder is passed the server-side pair. The
    connection that carries the message is %connection(), which is the
    HTTP connection the client or server already opened.

    %available() is how many body bytes can be read without blocking,
    and %pending() is how many body bytes still have to be written.
    %discard() drops the buffered body so the stream can be reused for
    another message.

    @ingroup Pt-Http-Messages
*/
class PT_HTTP_API Message
{
    friend class Connection;

    public:
        //! @brief Constructs with connection to use.
        explicit Message(Http::Connection& conn);

        //! @brief Returns the used connection.
        Connection& connection()
        { return *_conn; }

        //! @brief Returns the header of the message.
        MessageHeader& header()
        { return _header; }

        //! @brief Returns the header of the message.
        const MessageHeader& header() const
        { return _header; }

        //! @brief Returns the body of the message.
        std::iostream& body()
        { return _ios; }

        //! @brief Returns the number of bytes available to read.
        std::size_t available() const;

        //! @brief Returns the number of bytes pending to be written.
        std::size_t pending() const;

        //! @brief Discards the message body.
        void discard();

        //! @internal
        bool isSending() const
        { return _isSending; }

        //! @internal
        bool isReceiving() const
        { return _isReceiving; }

        //! @internal
        bool isFinished() const
        { return _finished; }

        //! @internal
        MessageBuffer& buffer()
        { return _buf; }

    protected:
        //! @internal
        void setBuffer(std::streambuf& sb)
        { _ios.rdbuf(&sb); }

        //! @internal
        void setSending(bool b)
        { _isSending = b; }

        //! @internal
        void setReceiving(bool b)
        { _isReceiving = b; }

        //! @internal
        void setFinished(bool b)
        { _finished = b; }

    private:
        Http::Connection* _conn;
        MessageHeader     _header;
        MessageBuffer     _buf;
        std::iostream     _ios;
        bool              _isSending;
        bool              _isReceiving;
        bool              _finished;
};

} // namespace Http

} // namespace Pt

#endif // Pt_Http_Message_h
