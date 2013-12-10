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

class PT_HTTP_API MessageHeader : private Pt::NonCopyable
{
    public:
        class Field
        {
            public:
                Field()
                : _name(0)
                , _value(0)
                {}

                Field(const char* f, const char* s)
                : _name(f)
                , _value(s)
                {}

                const char* name() const
                { return _name; }

                void setName(const char* name)
                { _name = name; }

                const char* value() const
                { return _value; }

                void setValue(const char* value)
                { _value = value; }

            private:
                const char* _name;
                const char* _value;
        };

        class ConstIterator
        {
            friend class MessageHeader;

            public:
                ConstIterator()
                { }

                explicit ConstIterator(const char* p)
                : current(p, p)
                {
                    fixup();
                }

                bool operator== (const ConstIterator& it) const
                { return current.name() == it.current.name(); }

                bool operator!= (const ConstIterator& it) const
                { return current.name() != it.current.name(); }

                ConstIterator& operator++()
                {
                    moveForward();
                    return *this;
                }

                ConstIterator operator++(int)
                {
                    ConstIterator ret = *this;
                    moveForward();
                    return ret;
                }

                const Field& operator*() const   
                { return current; }
                
                const Field* operator->() const  
                { return &current; }

            private:
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

                void moveForward()
                {
                    current.setName( current.value() + std::strlen(current.value()) + 1 );
                    fixup();
                }

            private:
                Field current;
        };

    public:
        MessageHeader();

        ~MessageHeader();

        void clear();

        void set(const char* key, const char* value);

        void add(const char* key, const char* value);

        void remove(const char* key);

        const char* get(const char* key) const;

        bool has(const char* key) const
        { return get(key) != 0; }

        bool isSet(const char* key, const char* value) const;

        ConstIterator begin() const
        { return ConstIterator(_rawdata); }

        ConstIterator end() const
        { return ConstIterator(); }

        unsigned versionMajor() const
        { return _httpVersionMajor; }

        unsigned versionMinor() const
        { return _httpVersionMinor; }

        void setVersion(unsigned major, unsigned minor)
        {
            _httpVersionMajor = major;
            _httpVersionMinor = minor;
        }

        bool isChunked() const;

        std::size_t contentLength() const;

        bool isKeepAlive() const;

        /// Returns a properly formatted current time-string, as needed in http.
        /// The buffer must have at least 30 bytes.
        static char* htdateCurrent(char* buffer);

    private:
        char* eptr() 
        { return _rawdata + _endOffset; }

    private:
        static const unsigned MaxHeaderSize = 4096;
        char _rawdata[MaxHeaderSize];  // key_1\0value_1\0key_2\0value_2\0...key_n\0value_n\0\0
        std::size_t _endOffset;
        unsigned _httpVersionMajor;
        unsigned _httpVersionMinor;
};


class MessageProgress
{
    private:
        enum Result
        {
            InProgress = 1,
            Header     = 2,
            Body       = 4,
            Finished   = 8,
            Trailer    = 16, // NOTE: questionable if we need this
        };

    public:
        MessageProgress()
        : _result(InProgress)
        {}

        bool header() const
        { return (_result & Header) == Header; }

        bool body() const
        { return (_result & Body) == Body; }

        bool trailer() const
        { return (_result & Trailer) == Trailer; }

        bool finished() const
        { return (_result & Finished) == Finished; }

        void setFinished()
        { _result |= Finished ; }

        void setHeader()
        { _result |= Header; }
        
        void setBody()
        { _result |= Body; }

        void setTrailer()
        { _result |= Trailer; }

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
        // @brief Constructs an empty buffer.
        MessageBuffer();

        // @brief Destructor.
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


class PT_HTTP_API Message
{
    friend class Connection;

    public:
        explicit Message(Http::Connection& conn);

        Connection& connection()
        { return *_conn; }

        MessageHeader& header()
        { return _header; }

        const MessageHeader& header() const
        { return _header; }

        std::iostream& body()
        { return _ios; }

        bool isSending() const
        { return _isSending; }

        bool isReceiving() const
        { return _isReceiving; }

        bool isFinished() const
        { return _finished; }

        void discard();
   
        MessageBuffer& buffer()
        { return _buf; }

    protected:
        void setBuffer(std::streambuf& sb)
        { _ios.rdbuf(&sb); }

        void setSending(bool b)
        { _isSending = b; }
        
        void setReceiving(bool b)
        { _isReceiving = b; }
        
        void setFinished(bool b)
        { _finished = b; }

    private:
        Http::Connection* _conn;
        MessageHeader _header;
        MessageBuffer _buf;
        std::iostream _ios;
        bool _isSending;
        bool _isReceiving;
        bool _finished;
};

} // namespace Http

} // namespace Pt

#endif // Pt_Http_Message_h
