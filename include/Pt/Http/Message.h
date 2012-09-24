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
#include <utility>
 
namespace Pt {

namespace Http {

class MessageProgress
{
    private:
        enum Result
        {
            Finished   = 1,
            InProgress = 2,
            Header     = 4,
            Body       = 8,
            Trailer    = 16,
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

class PT_HTTP_API MessageBuffer : public std::streambuf
{
    static const unsigned int BufferSize = 512;

    public:
        MessageBuffer();

        ~MessageBuffer();
        
        void reset()
        { this->setp(_obuffer, _obuffer + _obufferSize); }

        std::size_t size() const
        { return pptr() - pbase(); }

        const char* data() const
        { return _obuffer; }

    protected:
        virtual int_type overflow(int_type ch);

    private:
        char* _obuffer;
        std::size_t  _obufferSize;
};

class PT_HTTP_API MessageBody : public std::iostream
{
    friend class Connection;

    public:
        MessageBody();
        
        MessageBuffer& buffer()
        { return _buf; }

        void discard();

        void setInput(std::streambuf& sb)
        { this->rdbuf(&sb); }

        void setOutput()
        { this->rdbuf(&_buf); }

        void write(std::ostream& os)
        { os.write( _buf.data(), _buf.size() ); }

    private:
        MessageBuffer _buf;
};

class PT_HTTP_API MessageHeader : protected Pt::NonCopyable
{
    public:
        typedef std::pair<const char*, const char*> value_type;

        static const unsigned MaxHeaderSize = 4096;
        
        class ConstIterator
        {
            friend class MessageHeader;

            public:
                ConstIterator()
                : current_value((char*)0, (char*)0)
                { }

                explicit ConstIterator(const char* p)
                : current_value(p, p)
                {
                    fixup();
                }

                bool operator== (const ConstIterator& it) const
                { return current_value.first == it.current_value.first; }

                bool operator!= (const ConstIterator& it) const
                { return current_value.first != it.current_value.first; }

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

                const value_type& operator* () const   
                { return current_value; }
                
                const value_type* operator-> () const  
                { return &current_value; }

            private:
                void fixup()
                {
                    if (*current_value.first)
                        current_value.second = current_value.first + std::strlen(current_value.first) + 1;
                    else
                        current_value.first = current_value.second = 0;
                }

                void moveForward()
                {
                    current_value.first = current_value.second + std::strlen(current_value.second) + 1;
                    fixup();
                }

            private:
                value_type current_value;
        };

    public:
        MessageHeader()
        : _endOffset(0)
        , _httpVersionMajor(1)
        , _httpVersionMinor(1)
        {
            _rawdata[0] = _rawdata[1] = '\0';
        }

        ~MessageHeader()  
        {}

        void clear();

        void set(const char* key, const char* value, bool replace = true);

        void add(const char* key, const char* value)
        { set(key, value, false); }

        void remove(const char* key);

        const char* get(const char* key) const;

        bool has(const char* key) const
        { return get(key) != 0; }

        bool isValue(const char* key, const char* value) const;

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

        bool chunkedTransferEncoding() const;

        std::size_t contentLength() const;

        bool keepAlive() const;

        /// Returns a properly formatted current time-string, as needed in http.
        /// The buffer must have at least 30 bytes.
        static char* htdateCurrent(char* buffer);

    private:
        char* eptr() 
        { return _rawdata + _endOffset; }

    private:
        char _rawdata[MaxHeaderSize];  // key_1\0value_1\0key_2\0value_2\0...key_n\0value_n\0\0
        unsigned _endOffset;
        unsigned _httpVersionMajor;
        unsigned _httpVersionMinor;
};

} // namespace Http

} // namespace Pt

#endif
