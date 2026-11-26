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

#define PT_LOG_DEFINE(a)
#define PT_LOG_TRACE(a)
#define PT_LOG_DEBUG(a)
#define PT_LOG_INFO(a)
#define PT_LOG_WARN(a)
#define PT_LOG_ERROR(a)

PT_LOG_DEFINE("Pt.Http.Message")

namespace {

char asciiUpper(char ch)
{
    const unsigned char c = static_cast<unsigned char>(ch);
    if(c >= 'a' && c <= 'z')
        return static_cast<char>(c - 'a' + 'A');

    return static_cast<char>(c);
}


int compareIgnoreCase(const char* s1, const char* s2)
{
    const char* it1 = s1;
    const char* it2 = s2;
    while (*it1 && *it2)
    {
        if (*it1 != *it2)
        {
            char c1 = asciiUpper(*it1);
            char c2 = asciiUpper(*it2);
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


bool isOws(char ch)
{
    return ch == ' ' || ch == '\t';
}


bool isTokenChar(unsigned char ch)
{
    if(ch <= 32 || ch >= 127)
        return false;

    switch(ch)
    {
        case '(': case ')': case '<': case '>': case '@':
        case ',': case ';': case ':': case '\\': case '"':
        case '/': case '[': case ']': case '?': case '=':
        case '{': case '}':
            return false;

        default:
            return true;
    }
}

}

namespace Pt {

namespace Http {

bool MessageHeader::Value::equals(const char* value) const
{
    if( ! value )
        return false;

    const char* p = _data;
    const char* end = _data + _size;
    for(; p != end && *value; ++p, ++value)
    {
        if(*p != *value)
            return false;
    }

    return p == end && *value == '\0';
}


bool MessageHeader::Value::matches(const char* value) const
{
    if( ! value )
        return false;

    const char* p = _data;
    const char* end = _data + _size;
    for(; p != end && *value; ++p, ++value)
    {
        if(asciiUpper(*p) != asciiUpper(*value))
            return false;
    }

    return p == end && *value == '\0';
}


MessageHeader::ValueIterator::ValueIterator(const char* begin, const char* end)
: _current(begin)
, _end(end)
, _value()
{}


bool MessageHeader::ValueIterator::operator==(const ValueIterator& other) const
{
    return _current == other._current;
}


MessageHeader::ValueIterator& MessageHeader::ValueIterator::operator++()
{
    if( ! _current )
        return *this;

    const char* p = _current + _value.size();
    while(p != _end && isOws(*p))
        ++p;

    if(p != _end && *p == ',')
        ++p;
    else if(p != _end)
    {
        _current = 0;
        _end = 0;
        _value = Value();
        return *this;
    }

    _current = p;
    advance();
    return *this;
}


void MessageHeader::ValueIterator::advance()
{
    const char* p = _current;
    while(p != _end && isOws(*p))
        ++p;

    while(p != _end && *p == ',')
    {
        ++p;
        while(p != _end && isOws(*p))
            ++p;
    }

    if(p == _end)
    {
        _current = 0;
        _end = 0;
        _value = Value();
        return;
    }

    const char* begin = p;
    while(p != _end && *p != ',' && ! isOws(*p))
        ++p;

    _value = Value(begin, static_cast<std::size_t>(p - begin));
    _current = begin;
}


MessageHeader::ValueIterator MessageHeader::Field::begin() const
{
    if( ! _value )
        return end();

    ValueIterator it(_value, _value + _valueSize);
    it.advance();
    return it;
}


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
    for(FieldIterator it = begin(); it != end(); ++it)
    {
        if (compareIgnoreCase(key, it->name()) == 0)
            return it->value();
    }

    return 0;
}


MessageHeader::FieldIterator MessageHeader::field(const char* name) const
{
    for(FieldIterator it = begin(); it != end(); ++it)
    {
        if(compareIgnoreCase(name, it->name()) == 0)
            return it;
    }

    return end();
}


bool MessageHeader::matches(const char* key, const char* value) const
{
    for(FieldIterator it = begin(); it != end(); ++it)
    {
        if(compareIgnoreCase(key, it->name()) != 0)
            continue;

        for(ValueIterator vit = it->begin(); vit != it->end(); ++vit)
        {
            if( vit->matches(value) )
                return true;
        }
    }

    return false;
}


bool MessageHeader::contains(const char* key, const char* value) const
{
    for(FieldIterator it = begin(); it != end(); ++it)
    {
        if(compareIgnoreCase(key, it->name()) != 0)
            continue;

        for(ValueIterator vit = it->begin(); vit != it->end(); ++vit)
        {
            if( vit->equals(value) )
                return true;
        }
    }

    return false;
}


bool MessageHeader::isSet(const char* key, const char* value) const
{
    if( ! value )
        return false;

    const char* h = get(key);
    if (h == 0)
        return false;
    return std::strcmp(h, value) == 0;
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
    PT_LOG_DEBUG("MessageHeader::set(\"" << key << "\", \"" << value << "\", " << replace << ')');
    remove(key);
    add(key, value);
}


void MessageHeader::add(const char* key, const char* value)
{
    PT_LOG_DEBUG("MessageHeader::add(\"" << key << "\", \"" << value << "\", " << replace << ')');

    if( ! MessageHeader::isToken(key) )
        throw std::invalid_argument("header field name is not a token");

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
    if( ! MessageHeader::isToken(key) )
        throw std::invalid_argument("header field name is not a token");

    char* p = eptr();

    FieldIterator it = begin();
    while (it != end())
    {
        if (compareIgnoreCase(key, it->name()) == 0)
        {
            std::size_t slen = it->value() - it->name() + std::strlen(it->value()) + 1;

            std::memmove(
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
    return matches("Transfer-Encoding", "chunked");
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
    if( ! has("Connection") )
    {
        return versionMajor() == 1
            && versionMinor() >= 1;
    }

    return matches("Connection", "keep-alive");
}

void MessageHeader::setKeepAlive()
{
    set("Connection", "keep-alive");
}


bool MessageHeader::isUpgrade() const
{
    if( ! matches("Connection", "upgrade") )
        return false;

    const FieldIterator protocol = field("Upgrade");
    return protocol != end() && ! protocol->empty();
}


void MessageHeader::setUpgrade()
{
    set("Connection", "upgrade");
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


bool MessageHeader::isToken(const char* value)
{
    if( ! value || ! *value )
        return false;

    for(const unsigned char* p = reinterpret_cast<const unsigned char*>(value);
        *p;
        ++p)
    {
        if( ! isTokenChar(*p) )
            return false;
    }

    return true;
}


bool MessageHeader::isMatch(const char* a, const char* b)
{
    return a && b && compareIgnoreCase(a, b) == 0;
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
        this->pbump(static_cast<int>(_bufferSize));

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


std::size_t Message::available() const
{
    std::streambuf* sb = _ios.rdbuf();
    return sb ? _ios.rdbuf()->in_avail() : 0;
}


std::size_t Message::pending() const
{
    return _buf.size();
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
