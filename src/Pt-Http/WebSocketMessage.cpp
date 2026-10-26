/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/WebSocketMessage.h>
#include <stdexcept>

namespace Pt {

namespace Http {

WebSocketMessageBuffer::WebSocketMessageBuffer()
{
}


WebSocketMessageBuffer::~WebSocketMessageBuffer()
{
}


void WebSocketMessageBuffer::discard()
{
    _payload.clear();
    setp(0, 0);
    setg(0, 0, 0);
}


void WebSocketMessageBuffer::discard(std::size_t n)
{
    if(gptr() && egptr() && gptr() < egptr())
    {
        std::size_t avail = static_cast<std::size_t>(egptr() - gptr());
        std::size_t take = n < avail ? n : avail;
        gbump( static_cast<int>(take) );
    }

    std::size_t pend = size();
    std::size_t take = n < pend ? n : pend;
    if(take == 0)
        return;

    if(take >= _payload.size())
    {
        discard();
        return;
    }

    _payload.erase(_payload.begin(),
                   _payload.begin() + static_cast<std::ptrdiff_t>(take));
    setp(0, 0);
    setg(0, 0, 0);
}


std::size_t WebSocketMessageBuffer::size() const
{
    WebSocketMessageBuffer* self = const_cast<WebSocketMessageBuffer*>(this);
    if( self->pptr() && self->pbase() )
        return static_cast<std::size_t>(self->pptr() - self->pbase());

    return _payload.size();
}


const char* WebSocketMessageBuffer::data() const
{
    if( _payload.empty() )
        return 0;

    return &_payload[0];
}


WebSocketMessageBuffer::int_type WebSocketMessageBuffer::overflow(int_type ch)
{
    if( ch == traits_type::eof() )
        return traits_type::not_eof(ch);

    std::size_t off = pptr() ? static_cast<std::size_t>(pptr() - pbase())
                             : _payload.size();
    _payload.push_back( static_cast<char>(ch) );
    setp(&_payload[0], &_payload[0] + _payload.size());
    pbump( static_cast<int>(off + 1) );
    return ch;
}


std::streamsize WebSocketMessageBuffer::xsputn(const char* data, std::streamsize n)
{
    std::size_t off = pptr() ? static_cast<std::size_t>(pptr() - pbase())
                             : _payload.size();
    _payload.insert(_payload.end(), data, data + n);
    setp(&_payload[0], &_payload[0] + _payload.size());
    pbump( static_cast<int>(off + static_cast<std::size_t>(n)) );
    return n;
}


WebSocketMessageBuffer::int_type WebSocketMessageBuffer::underflow()
{
    if(gptr() && gptr() < egptr())
        return traits_type::to_int_type(*gptr());

    return traits_type::eof();
}


WebSocketMessage::WebSocketMessage()
: _type(Unknown)
, _body(&_buf)
{
}


WebSocketMessage::~WebSocketMessage()
{
    _body.rdbuf(0);
}


void WebSocketMessage::setType(Type type)
{
    if(type != Text && type != Binary)
        throw std::invalid_argument("WebSocket message type");

    _type = type;
}


void WebSocketMessage::setTypeFromEngine(Type type)
{
    _type = type;
}


std::size_t WebSocketMessage::available() const
{
    std::streambuf* sb = _body.rdbuf();
    return sb ? static_cast<std::size_t>(sb->in_avail()) : 0;
}


std::size_t WebSocketMessage::pending() const
{
    return _buf.size();
}


void WebSocketMessage::discard()
{
    std::streambuf* sb = _body.rdbuf();
    if(sb && sb != &_buf)
    {
        std::streamsize avail = sb->in_avail();
        while(avail-- > 0)
            sb->sbumpc();
    }

    _buf.discard();
    _body.clear();
}


void WebSocketMessage::discard(std::size_t n)
{
    std::streambuf* sb = _body.rdbuf();
    if(sb && sb != &_buf)
    {
        std::streamsize avail = sb->in_avail();
        std::streamsize take = avail < static_cast<std::streamsize>(n)
                             ? avail
                             : static_cast<std::streamsize>(n);
        while(take-- > 0)
            sb->sbumpc();
    }

    _buf.discard(n);
}


void WebSocketMessage::clear()
{
    _type = Unknown;
    discard();
}


void WebSocketMessage::setBuffer(std::streambuf& sb)
{
    _body.rdbuf(&sb);
}

} // namespace Http

} // namespace Pt
