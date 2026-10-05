/* Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include "WebSocketConnection.h"
#include <Pt/Http/Stream.h>
#include <Pt/Byteorder.h>
#include <Pt/System/EventLoop.h>
#include <cstring>
#include <ctime>
#include <stdexcept>

namespace Pt {

namespace Http {

class WebSocketConnection::PayloadBuffer : public std::streambuf
{
    public:
        explicit PayloadBuffer(std::vector<char>& payload)
        : _payload(&payload)
        {
        }

        void reset()
        {
            _payload->clear();
            setp(0, 0);
            setg(0, 0, 0);
        }

        void prepareGet()
        {
            if( _payload->empty() )
            {
                setg(0, 0, 0);
                return;
            }

            setg(&(*_payload)[0], &(*_payload)[0], &(*_payload)[0] + _payload->size());
        }

    protected:
        virtual int_type overflow(int_type ch)
        {
            if( ch == traits_type::eof() )
                return traits_type::not_eof(ch);

            std::size_t off = pptr() ? static_cast<std::size_t>(pptr() - pbase()) : _payload->size();
            _payload->push_back( static_cast<char>(ch) );
            setp(&(*_payload)[0], &(*_payload)[0] + _payload->size());
            pbump( static_cast<int>(off + 1) );
            return ch;
        }

        virtual std::streamsize xsputn(const char* data, std::streamsize n)
        {
            std::size_t off = pptr() ? static_cast<std::size_t>(pptr() - pbase()) : _payload->size();
            _payload->insert(_payload->end(), data, data + n);
            setp(&(*_payload)[0], &(*_payload)[0] + _payload->size());
            pbump( static_cast<int>(off + static_cast<std::size_t>(n)) );
            return n;
        }

    private:
        std::vector<char>* _payload;
};


WebSocketConnection::WebSocketConnection()
: StreamSession()
, _clientMask(false)
, _timeout(30000)
, _maxMessageSize(1024 * 1024)
, _idleTimeout(0)
, _error(false)
, _state(Idle)
, _frame(Unknown)
, _masked(false)
, _mask(0)
, _payloadSize(0)
, _payloadGot(0)
, _headerNeed(2)
, _payloadBuffer(new PayloadBuffer(_payload))
, _body(_payloadBuffer)
{
    _idleTimer.timeout() += Pt::slot(*this, &WebSocketConnection::onIdleTimeout);
}


WebSocketConnection::~WebSocketConnection()
{
    close();
    _body.rdbuf(0);
    delete _payloadBuffer;
}


void WebSocketConnection::open(Stream& stream, bool clientMask)
{
    _clientMask = clientMask;
    StreamSession::open(stream);
    stream.setTimeout(_timeout);
    stream.inputReady() += Pt::slot(*this, &WebSocketConnection::onInput);
    stream.outputReady() += Pt::slot(*this, &WebSocketConnection::onOutput);
}


void WebSocketConnection::close()
{
    if( Stream* stream = this->stream() )
    {
        writeFrame(Close, 0, 0);
        stream->beginOutput();
        stream->endOutput();
    }

    StreamSession::close();
}


std::size_t WebSocketConnection::available() const
{
    std::streambuf* sb = _body.rdbuf();
    if( ! sb )
        return 0;

    std::streamsize n = sb->in_avail();
    return n > 0 ? static_cast<std::size_t>(n) : 0;
}


std::size_t WebSocketConnection::pending() const
{
    return _payload.size();
}


void WebSocketConnection::discard()
{
    _payloadBuffer->reset();
    _body.clear();
}


void WebSocketConnection::beginSend(Frame frame)
{
    Stream* stream = this->stream();
    if( ! stream )
        throw std::logic_error("WebSocket has no stream");

    _state = Sending;
    writeFrame(frame, _payload.empty() ? 0 : &_payload[0], _payload.size());
    _payloadBuffer->reset();
    stream->beginOutput();
}


void WebSocketConnection::endSend()
{
    if(_idleTimeout != 0)
        _idleTimer.start(_idleTimeout);

    if(_error)
        throw std::runtime_error("WebSocket send failed");

    Stream* stream = this->stream();
    if( ! stream )
        throw std::logic_error("WebSocket has no stream");

    stream->endOutput();
    _state = Idle;
}


void WebSocketConnection::beginReceive()
{
    if( ! stream() )
        throw std::logic_error("WebSocket has no stream");

    _frame = Unknown;
    _payload.clear();
    _payloadBuffer->reset();
    _header.clear();
    _payloadSize = 0;
    _payloadGot = 0;
    _headerNeed = 2;
    _masked = false;
    _state = ReceiveHeader;
    beginFrameRead();
}


void WebSocketConnection::endReceive()
{
    if(_idleTimeout != 0)
        _idleTimer.start(_idleTimeout);

    if(_error)
        throw std::runtime_error("WebSocket receive failed");

    _payloadBuffer->prepareGet();
}


void WebSocketConnection::sendPing()
{
    Stream* stream = this->stream();
    if( ! stream )
        throw std::logic_error("WebSocket has no stream");

    writeFrame(Ping, 0, 0);
    stream->beginOutput();
    stream->endOutput();
}


void WebSocketConnection::sendPong()
{
    Stream* stream = this->stream();
    if( ! stream )
        throw std::logic_error("WebSocket has no stream");

    writeFrame(Pong, 0, 0);
    stream->beginOutput();
    stream->endOutput();
}


void WebSocketConnection::setTimeout(std::size_t timeout)
{
    _timeout = timeout;

    if( Stream* stream = this->stream() )
        stream->setTimeout(timeout);
}


void WebSocketConnection::setMaxMessageSize(std::size_t maxSize)
{
    _maxMessageSize = maxSize;
}


void WebSocketConnection::setIdleTimeout(std::size_t ms)
{
    _idleTimeout = ms;

    if(ms == 0)
    {
        _idleTimer.stop();
        return;
    }

    if( Stream* stream = this->stream() )
    {
        if( System::EventLoop* loop = stream->loop() )
            _idleTimer.setActive(*loop);
    }

    _idleTimer.start(ms);
}


void WebSocketConnection::onIdleTimeout()
{
    failStream();
}


void WebSocketConnection::onCloseStream(Stream&)
{
    _error = true;
    _state = Idle;
    _closed.send();
}


void WebSocketConnection::failStream()
{
    _error = true;
    _state = Idle;

    if( Stream* stream = this->stream() )
        stream->close();
}


Pt::uint32_t WebSocketConnection::createMask()
{
    std::srand( static_cast<unsigned int>(std::time(0)) );
    return static_cast<Pt::uint32_t>(std::rand());
}


void WebSocketConnection::writeFrame(Frame frame, const char* payload, std::size_t n)
{
    Stream* stream = this->stream();
    std::streambuf* buf = stream ? stream->buffer() : 0;
    if( ! buf )
        throw std::logic_error("WebSocket has no stream");

    char header[14];
    std::size_t headerLen = 2;

    header[0] = (char)0x80;
    if(frame == Text)
        header[0] |= 0x01;
    else if(frame == Binary)
        header[0] |= 0x02;
    else if(frame == Ping)
        header[0] |= 0x09;
    else if(frame == Pong)
        header[0] |= 0x0A;
    else if(frame == Close)
        header[0] |= 0x08;

    header[1] = _clientMask ? (char)0x80 : 0;

    if(n < 126)
    {
        header[1] |= static_cast<char>(n);
    }
    else if(n < 65536)
    {
        header[1] |= 126;
        Pt::uint16_t size = Pt::hostToBe( static_cast<Pt::uint16_t>(n) );
        std::memcpy(header + 2, &size, 2);
        headerLen = 4;
    }
    else
    {
        header[1] |= 127;
        Pt::uint64_t size = Pt::hostToBe( static_cast<Pt::uint64_t>(n) );
        std::memcpy(header + 2, &size, 8);
        headerLen = 10;
    }

    char maskBytes[4];
    if(_clientMask)
    {
        Pt::uint32_t mask = createMask();
        std::memcpy(maskBytes, &mask, 4);
        std::memcpy(header + headerLen, maskBytes, 4);
        headerLen += 4;
    }

    buf->sputn(header, static_cast<std::streamsize>(headerLen));

    if(n == 0 || ! payload)
        return;

    if( ! _clientMask )
    {
        buf->sputn(payload, static_cast<std::streamsize>(n));
        return;
    }

    for(std::size_t i = 0; i < n; ++i)
    {
        char masked = static_cast<char>(payload[i] ^ maskBytes[i % 4]);
        buf->sputc(masked);
    }
}


void WebSocketConnection::beginFrameRead()
{
    Stream* stream = this->stream();
    if( ! stream )
        return;

    std::streambuf* buf = stream->buffer();
    if(buf && buf->in_avail() > 0)
    {
        if( parseAvailable() )
            return;
    }

    stream->beginInput();
}


bool WebSocketConnection::parseAvailable()
{
    Stream* stream = this->stream();
    std::streambuf* buf = stream ? stream->buffer() : 0;
    if( ! buf )
        return false;

    while( buf->in_avail() > 0 )
    {
        int ch = buf->sbumpc();
        if(ch < 0)
            break;

        char byte = static_cast<char>(ch);

        if(_state == ReceiveHeader || _state == ReceiveLength || _state == ReceiveMask)
        {
            _header.push_back(byte);

            if(_header.size() < _headerNeed)
                continue;

            if(_state == ReceiveHeader)
            {
                unsigned opcode = static_cast<unsigned char>(_header[0]) & 0x0F;
                if(opcode == 0x01)
                    _frame = Text;
                else if(opcode == 0x02)
                    _frame = Binary;
                else if(opcode == 0x09)
                    _frame = Ping;
                else if(opcode == 0x0A)
                    _frame = Pong;
                else if(opcode == 0x08)
                {
                    failStream();
                    return true;
                }
                else
                    _frame = Unknown;

                _masked = (_header[1] & 0x80) != 0;
                unsigned len7 = static_cast<unsigned char>(_header[1]) & 0x7F;

                if(len7 == 126)
                {
                    _state = ReceiveLength;
                    _headerNeed = 4;
                    continue;
                }

                if(len7 == 127)
                {
                    _state = ReceiveLength;
                    _headerNeed = 10;
                    continue;
                }

                _payloadSize = len7;
                if(_maxMessageSize != 0 && _payloadSize > _maxMessageSize)
                {
                    failStream();
                    return true;
                }
            }
            else if(_state == ReceiveLength)
            {
                if(_headerNeed == 4)
                {
                    Pt::uint16_t size = 0;
                    std::memcpy(&size, &_header[2], 2);
                    _payloadSize = Pt::beToHost(size);
                }
                else
                {
                    Pt::uint64_t size = 0;
                    std::memcpy(&size, &_header[2], 8);
                    _payloadSize = static_cast<std::size_t>( Pt::beToHost(size) );
                }

                if(_maxMessageSize != 0 && _payloadSize > _maxMessageSize)
                {
                    failStream();
                    return true;
                }
            }

            if(_masked && _state != ReceiveMask)
            {
                _state = ReceiveMask;
                _headerNeed = _header.size() + 4;
                continue;
            }

            if(_masked)
                std::memcpy(&_mask, &_header[_header.size() - 4], 4);

            _payloadGot = 0;
            _payload.clear();
            _payload.reserve(_payloadSize);
            _state = ReceivePayload;

            if(_payloadSize == 0)
            {
                _state = Idle;
                _inputReady.send();
                return true;
            }
        }
        else if(_state == ReceivePayload)
        {
            if(_masked)
            {
                const char* maskBytes = reinterpret_cast<const char*>(&_mask);
                byte = static_cast<char>(byte ^ maskBytes[_payloadGot % 4]);
            }

            _payload.push_back(byte);
            ++_payloadGot;

            if(_payloadGot == _payloadSize)
            {
                _state = Idle;
                _inputReady.send();
                return true;
            }
        }
    }

    return false;
}


void WebSocketConnection::onInput()
{
    Stream* stream = this->stream();
    if( ! stream )
        return;

    try
    {
        if( stream->endInput() == 0 )
        {
            failStream();
            return;
        }

        if( parseAvailable() )
            return;

        stream->beginInput();
    }
    catch(const std::exception&)
    {
        failStream();
    }
}


void WebSocketConnection::onOutput()
{
    Stream* stream = this->stream();
    if( ! stream )
        return;

    try
    {
        stream->endOutput();
        _state = Idle;
        _outputReady.send();
    }
    catch(const std::exception&)
    {
        failStream();
    }
}

} // namespace Http

} // namespace Pt
