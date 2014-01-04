/*
 * Copyright (C) 2009 by Marc Boris Duerner, Tommi Maekitalo
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

#include <Pt/Http/Message.h>
#include <Pt/Http/HttpError.h>
#include <Pt/System/Clock.h>
#include <cctype>
#include <sstream>
#include <stdio.h>
#include <cstring>

#define log_define(a)
#define log_trace(a)
#define log_debug(a)
#define log_info(a)
#define log_warn(a)
#define log_error(a)

log_define("Pt.Http.Message")

namespace {

int compareIgnoreCase(const char* s1, const char* s2)
{
    const char* it1 = s1;
    const char* it2 = s2;
    while (*it1 && *it2)
    {
        if (*it1 != *it2)
        {
            char c1 = std::toupper(*it1);
            char c2 = std::toupper(*it2);
            if (c1 < c2)
                return -1;
            else if (c2 < c1)
                return 1;
        }
        ++it1;
        ++it2;
    }

    return *it1 ? 1
                : *it2 ? -1 : 0;
}

} 

namespace Pt {

namespace Http {

MessageHeader::MessageHeader()
: _endOffset(0)
, _httpVersionMajor(1)
, _httpVersionMinor(1)
{
    _rawdata[0] = _rawdata[1] = '\0';
}


MessageHeader::~MessageHeader()  
{
}


const char* MessageHeader::get(const char* key) const
{
    for (ConstIterator it = begin(); it != end(); ++it)
    {
        if (compareIgnoreCase(key, it->name()) == 0)
            return it->value();
    }

    return 0;
}


bool MessageHeader::isSet(const char* key, const char* value) const
{
    const char* h = get(key);
    if (h == 0)
        return false;
    return compareIgnoreCase(h, value) == 0;
}


void MessageHeader::clear()
{
    _rawdata[0] = _rawdata[1] = '\0';
    _endOffset = 0;
    _httpVersionMajor = 1;
    _httpVersionMinor = 1;
}


void MessageHeader::set(const char* key, const char* value)
{
    log_debug("MessageHeader::set(\"" << key << "\", \"" << value << "\", " << replace << ')');
    remove(key);
    add(key, value);
}


void MessageHeader::add(const char* key, const char* value)
{ 
    log_debug("MessageHeader::add(\"" << key << "\", \"" << value << "\", " << replace << ')');

    if( ! *key)
        throw std::invalid_argument("header key is NULL");

    char* p = eptr();

    std::size_t lk = std::strlen(key);     // length of key
    std::size_t lv = std::strlen(value);   // length of value

    if (p - _rawdata + lk + lv + 2 > MaxHeaderSize)
        throw HttpError("message header too big");

    std::strcpy(p, key);   // copy key
    p += lk + 1;
    std::strcpy(p, value); // copy value
    p[lv + 1] = '\0';      // put new message end marker in place

    _endOffset = (p + lv + 1) - _rawdata;
}


void MessageHeader::remove(const char* key)
{
    if( ! *key)
        throw std::invalid_argument("header key is NULL");

    char* p = eptr();

    ConstIterator it = begin();
    while (it != end())
    {
        if (compareIgnoreCase(key, it->name()) == 0)
        {
            std::size_t slen = it->value() - it->name() + std::strlen(it->value()) + 1;

            std::memcpy(
                const_cast<char*>(it->name()),
                it->name() + slen,
                p - it->name() + slen);

            p -= slen;

            it.fixup();
        }
        else
            ++it;
    }

    _endOffset = p - _rawdata;
}


bool MessageHeader::isChunked() const
{
    return isSet("Transfer-Encoding", "chunked");
}


std::size_t MessageHeader::contentLength() const
{
    const char* s = get("Content-Length");
    if (s == 0)
        return 0;

    std::size_t size = 0;
    while (*s >= '0' && *s <= '9')
        size = size * 10 + (*s++ - '0');

    return size;
}


bool MessageHeader::isKeepAlive() const
{
    const char* ch = get("Connection");

    if (ch == 0)
        return versionMajor() == 1
            && versionMinor() >= 1;
    else
        return compareIgnoreCase(ch, "keep-alive") == 0;
}


char* MessageHeader::htdateCurrent(char* buffer)
{
    int year = 0;
    unsigned month = 0;
    unsigned day = 0;
    unsigned hour = 0;
    unsigned min = 0;
    unsigned sec = 0;
    unsigned msec = 0;

    DateTime dt = System::Clock::getSystemTime();
    dt.get(year, month, day, hour, min, sec, msec);
    unsigned dayOfWeek = dt.date().dayOfWeek();

    static const char* wdays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    static const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    sprintf(buffer, "%s, %02d %s %d %02d:%02d:%02d GMT",
                    wdays[dayOfWeek], day, months[month-1], year, hour, min, sec);

    return buffer;
}


MessageBuffer::MessageBuffer()
: _buffer(0)
, _bufferSize(0)
{
    setg(0,0,0);
    setp(0,0);
}


MessageBuffer::~MessageBuffer()
{
    delete [] _buffer;
}


MessageBuffer::int_type MessageBuffer::overflow(int_type ch)
{
    typedef MessageBuffer::traits_type traits_type;

    if( ! _buffer)
    {
        _bufferSize = BufferSize;
        _buffer = new char[_bufferSize];
        this->setp(_buffer, _buffer + _bufferSize);
        this->setg(_buffer, _buffer, _buffer);
    }
    else
    {
        std::size_t bufsize = _bufferSize + BufferSize;
        char* buf = new char[ bufsize ];
        traits_type::copy(buf, _buffer, _bufferSize);
        std::swap(_buffer, buf);

        this->setp(_buffer, _buffer + bufsize);
        this->pbump(_bufferSize);

        std::size_t gsize = gptr() - eback();
        this->setg(_buffer, _buffer + gsize, pptr());
        
        _bufferSize = bufsize;
        delete [] buf;
    }

    // if the overflow char is not EOF put it in buffer
    if(traits_type::eq_int_type(ch, traits_type::eof()) == false)
    {
        *pptr() = traits_type::to_char_type(ch);
        this->pbump(1);
    }

    return traits_type::not_eof(ch);
}


MessageBuffer::int_type MessageBuffer::underflow()
{ 
    if( this->gptr() < this->pptr() )
    {
        std::size_t gsize = gptr() - eback();
        this->setg(_buffer, _buffer + gsize, this->pptr());
    }
    
    if( this->gptr() < this->egptr() )
        return traits_type::to_int_type(*(this->gptr()));

    return traits_type::eof();
}


Message::Message(Http::Connection& conn)
: _conn(&conn)
, _ios(&_buf)
, _isSending(false)
, _isReceiving(false)
, _finished(false)
{ 
}


void Message::discard()
{ 
    _buf.discard();

    std::streambuf* sb = _ios.rdbuf();
    if(sb != &_buf)
    {
        std::streamsize avail = sb->in_avail();
        while(avail--)
            sb->sbumpc();
    }
}

} // namespace Http

} // namespace Pt
