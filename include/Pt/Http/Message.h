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

/** @brief HTTP message header.

    %MessageHeader is the field list and HTTP version of a message. It
    is the header half of %Message, and through that of %Request and
    %Reply. Callers do not usually construct a header on its own; they
    use %header() on the message the client or responder already holds.

    Fields are names and values. %set() replaces the value for a name,
    %add() appends another value for the same name, and %remove()
    deletes it. %get() returns the value or a null pointer, %has()
    reports presence, and %isSet() tests that a name has a particular
    value. %begin() and %end() walk the fields as %Field values, each
    with a name and a value pointer into the header's buffer.

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
        /** @brief %Field of a HTTP header.
        */
        class Field
        {
            public:
                /** @brief Default constructor.
                */
                Field()
                : _name(0)
                , _value(0)
                {}

                /** @brief Construct with field name and value.
                */
                Field(const char* f, const char* s)
                : _name(f)
                , _value(s)
                {}

                /** @brief Returns the field name.
                */
                const char* name() const
                { return _name; }

                 /** @brief Sets the field name.
                */
                void setName(const char* name)
                { _name = name; }

                /** @brief Returns the field value.
                */
                const char* value() const
                { return _value; }

                /** @brief Sets the field value.
                */
                void setValue(const char* value)
                { _value = value; }

            private:
                const char* _name;
                const char* _value;
        };

        /** @brief HTTP header field iterator.
        */
        class ConstIterator
        {
            friend class MessageHeader;

            public:
                /** @brief Default constructor.
                */
                ConstIterator()
                { }
                 //! @internal
                explicit ConstIterator(const char* p)
                : current(p, p)
                {
                    fixup();
                }

                /** @brief Equal comparison.
                */
                bool operator== (const ConstIterator& it) const
                { return current.name() == it.current.name(); }

                /** @brief Inequal comparison.
                */
                bool operator!= (const ConstIterator& it) const
                { return current.name() != it.current.name(); }

                /** @brief Advance the iterator.
                */
                ConstIterator& operator++()
                {
                    moveForward();
                    return *this;
                }

                /** @brief Advance the iterator.
                */
                ConstIterator operator++(int)
                {
                    ConstIterator ret = *this;
                    moveForward();
                    return ret;
                }

                /** @brief Returns the header field.
                */
                const Field& operator*() const   
                { return current; }
                
                /** @brief Returns the header field.
                */
                const Field* operator->() const  
                { return &current; }

            private:
                //! @internal
                void fixup()
                {
                    if( *current.name() )
                    {
                        current.setValue( current.name() + std::strlen(current.name()) + 1 );
                    }
                    else
                    {
                        current.setName(0);
                        current.setValue(0);
                    }
                }

                //! @internal
                void moveForward()
                {
                    current.setName( current.value() + std::strlen(current.value()) + 1 );
                    fixup();
                }

            private:
                Field current;
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

        /** @brief Returns true if the field is set to the value.
        */
        bool isSet(const char* key, const char* value) const;

        /** @brief Returns the begin of the header fields.
        */
        ConstIterator begin() const
        { return ConstIterator(_rawdata); }

        /** @brief Returns the end of the header fields.
        */
        ConstIterator end() const
        { return ConstIterator(); }

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
        */
        bool isChunked() const;

        /** @brief Returns the content length.
        */
        std::size_t contentLength() const;

        /** @brief Returns true if HTTP keep-alive is set.
        */
        bool isKeepAlive() const;

        /** @brief Sets the HTTP keep-alive header.
        */
        void setKeepAlive();
        
        /** @brief Returns true if the Upgrade header is set.
        */
        bool isUpgrade() const;

        /** @brief Sets the Upgrade header.
        */
        void setUpgrade();

        /** @brief Writes a current HTTP date into @a buffer.

            @a buffer must have at least 30 bytes.
        */
        static char* htdateCurrent(char* buffer);

    private:
        //! @internal
        char* eptr() 
        { return _rawdata + _endOffset; }

    private:
        static const unsigned MaxHeaderSize = 4096;
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
