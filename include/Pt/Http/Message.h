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

                /** @brief Sets the field name.
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

        /** @brief Denstructor.
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
        
        bool isUpgrade() const;

        void setUpgrade();

        // Returns a properly formatted current time-string, as needed in http.
        // The buffer must have at least 30 bytes.
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

/** @brief HTTP message progress.
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
