/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/WebSocketMessage.h>
#include <vector>
#include <stdexcept>

namespace Pt {

namespace Http {

class WebSocketMessage::PayloadBuffer : public std::streambuf
{
    public:
        PayloadBuffer()
        {
        }

        void reset()
        {
            _payload.clear();
            setp(0, 0);
            setg(0, 0, 0);
        }

        void append(const char* data, std::size_t n)
        {
            if(n == 0)
                return;

            _payload.insert(_payload.end(), data, data + n);
        }

        void prepareRead()
        {
            if( _payload.empty() )
            {
                setg(0, 0, 0);
                return;
            }

            setg(&_payload[0], &_payload[0], &_payload[0] + _payload.size());
        }

        const char* sendData() const
        {
            if( _payload.empty() )
                return 0;

            return &_payload[0];
        }

        std::size_t sendSize() const
        {
            PayloadBuffer* self = const_cast<PayloadBuffer*>(this);
            if( self->pptr() && self->pbase() )
                return static_cast<std::size_t>(self->pptr() - self->pbase());

            return _payload.size();
        }

        void consume(std::size_t n)
        {
            if(n == 0)
                return;

            if(n >= _payload.size())
            {
                reset();
                return;
            }

            _payload.erase(_payload.begin(), _payload.begin() + static_cast<std::ptrdiff_t>(n));
            setp(0, 0);
            setg(0, 0, 0);
        }

        std::size_t available() const
        {
            PayloadBuffer* self = const_cast<PayloadBuffer*>(this);
            if( ! self->gptr() || ! self->egptr() || self->gptr() >= self->egptr() )
                return 0;

            return static_cast<std::size_t>(self->egptr() - self->gptr());
        }

    protected:
        virtual int_type overflow(int_type ch)
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

        virtual std::streamsize xsputn(const char* data, std::streamsize n)
        {
            std::size_t off = pptr() ? static_cast<std::size_t>(pptr() - pbase())
                                     : _payload.size();
            _payload.insert(_payload.end(), data, data + n);
            setp(&_payload[0], &_payload[0] + _payload.size());
            pbump( static_cast<int>(off + static_cast<std::size_t>(n)) );
            return n;
        }

        virtual int_type underflow()
        {
            if(gptr() && gptr() < egptr())
                return traits_type::to_int_type(*gptr());

            return traits_type::eof();
        }

    private:
        std::vector<char> _payload;
};


WebSocketMessage::WebSocketMessage()
: _type(Unknown)
, _buffer(new PayloadBuffer())
, _body(_buffer)
{
}


WebSocketMessage::~WebSocketMessage()
{
    _body.rdbuf(0);
    delete _buffer;
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
    return _buffer->available();
}


std::size_t WebSocketMessage::pending() const
{
    return _buffer->sendSize();
}


void WebSocketMessage::discard()
{
    _buffer->reset();
    _body.clear();
}


void WebSocketMessage::clear()
{
    _type = Unknown;
    _buffer->reset();
    _body.clear();
}


void WebSocketMessage::append(const char* data, std::size_t n)
{
    _buffer->append(data, n);
}


void WebSocketMessage::prepareRead()
{
    _buffer->prepareRead();
}


const char* WebSocketMessage::sendData() const
{
    return _buffer->sendData();
}


std::size_t WebSocketMessage::sendSize() const
{
    return _buffer->sendSize();
}


void WebSocketMessage::consume(std::size_t n)
{
    _buffer->consume(n);
}

} // namespace Http

} // namespace Pt
