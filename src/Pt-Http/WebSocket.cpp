/* Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/WebSocket.h>
#include <Pt/Http/Stream.h>
#include <Pt/Http/Client.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/Http/Message.h>
#include <Pt/TextStream.h>
#include <Pt/Base64Codec.h>
#include <Pt/Byteorder.h>
#include <Pt/System/Uri.h>
#include <Pt/System/EventLoop.h>
#include <sstream>
#include <ctime>
#include <cstring>
#include <stdexcept>

namespace Pt {

namespace Http {

namespace {

bool isControl(WebSocketFrame::Type type)
{
    return type == WebSocketFrame::Close ||
           type == WebSocketFrame::Ping ||
           type == WebSocketFrame::Pong;
}

} // namespace


WebSocket::WebSocket(Client& client)
: StreamSession()
, _client(&client)
, _isClient(true)
, _path("/")
, _timeout(30000)
, _maxMessageSize(1024 * 1024)
, _idleTimeout(0)
, _error(false)
, _outputActive(false)
, _inputActive(false)
, _peerClose(false)
, _state(Idle)
, _fin(true)
, _frame(WebSocketFrame::Text)
, _masked(false)
, _mask(0)
, _payloadSize(0)
, _payloadGot(0)
, _headerNeed(2)
{
    _idleTimer.timeout() += Pt::slot(*this, &WebSocket::onIdleTimeout);
}


WebSocket::WebSocket()
: StreamSession()
, _client(0)
, _isClient(false)
, _path("/")
, _timeout(30000)
, _maxMessageSize(1024 * 1024)
, _idleTimeout(0)
, _error(false)
, _outputActive(false)
, _inputActive(false)
, _peerClose(false)
, _state(Idle)
, _fin(true)
, _frame(WebSocketFrame::Text)
, _masked(false)
, _mask(0)
, _payloadSize(0)
, _payloadGot(0)
, _headerNeed(2)
{
    _idleTimer.timeout() += Pt::slot(*this, &WebSocket::onIdleTimeout);
}


WebSocket::~WebSocket()
{
    try
    {
        shutdown( ! _outputActive );
    }
    catch(...)
    {
    }
}


void WebSocket::accept(Stream& stream)
{
    _isClient = false;
    open(stream);
    stream.setTimeout(_timeout);
    stream.inputReady() += Pt::slot(*this, &WebSocket::onInput);
    stream.outputReady() += Pt::slot(*this, &WebSocket::onOutput);
}


void WebSocket::beginConnect(const std::string& url, const std::string& origin)
{
    if( ! _client )
        throw std::logic_error("WebSocket has no client");

    _isClient = true;
    _error = false;
    _state = Connecting;
    parseUrl(url, origin);

    _client->requestSent() += Pt::slot(*this, &WebSocket::onRequestSent);
    _client->replyReceived() += Pt::slot(*this, &WebSocket::onReply);

    Request& request = _client->request();
    request.clear();
    request.setMethod("GET");
    request.setUrl(_path);
    request.header().set("Connection", "Upgrade");
    request.header().set("Upgrade", "websocket");
    request.header().set("Sec-WebSocket-Version", "13");

    const std::string key = createKey();
    request.header().set("Sec-WebSocket-Key", key.c_str());

    if( ! origin.empty() )
        request.header().set("Origin", origin.c_str());

    _client->beginSend(true);
}


void WebSocket::endConnect()
{
    if(_error || _state != Idle)
        throw std::runtime_error("WebSocket handshake failed");
}


void WebSocket::finishHandshake(bool failed)
{
    if(_client)
    {
        _client->requestSent() -= Pt::slot(*this, &WebSocket::onRequestSent);
        _client->replyReceived() -= Pt::slot(*this, &WebSocket::onReply);
    }

    _error = failed;
    _state = Idle;
    _connected.send(*this);
}


void WebSocket::onRequestSent(Client& client)
{
    try
    {
        MessageProgress progress = client.endSend();
        if( ! progress.finished() )
        {
            client.beginSend(true);
            return;
        }

        client.beginReceive();
    }
    catch(const std::exception&)
    {
        finishHandshake(true);
    }
}


void WebSocket::onReply(Client& client)
{
    try
    {
        MessageProgress progress = client.endReceive();
        if( ! progress.finished() )
        {
            client.beginReceive();
            return;
        }

        if( client.reply().statusCode() != 101 )
            throw std::runtime_error("WebSocket handshake failed");

        Stream& stream = client.upgrade();
        open(stream);
        stream.setTimeout(_timeout);
        stream.inputReady() += Pt::slot(*this, &WebSocket::onInput);
        stream.outputReady() += Pt::slot(*this, &WebSocket::onOutput);
        finishHandshake(false);
    }
    catch(const std::exception&)
    {
        finishHandshake(true);
    }
}


void WebSocket::beginSend()
{
    Stream* stream = this->stream();
    if( ! stream )
        throw std::logic_error("WebSocket has no stream");

    if(_outputActive)
        throw std::logic_error("WebSocket output transfer is active");

    if( isControl(_output.type()) && ! _output.fin() )
        throw std::logic_error("WebSocket control frame must be final");

    if(_output.type() == WebSocketFrame::Close)
        _output.composeClosePayload();

    if( isControl(_output.type()) && _output.size() > 125 )
        throw std::logic_error("WebSocket control frame is too large");

    _outputActive = true;
    _output.setBusy(true);
    writeFrame(_output.type(), _output.fin(), _output.data(), _output.size());
    stream->beginOutput();
}


void WebSocket::endSend()
{
    if(_idleTimeout != 0)
        _idleTimer.start(_idleTimeout);

    if(_error)
        throw std::runtime_error("WebSocket send failed");

    Stream* stream = this->stream();
    if( ! stream )
        throw std::logic_error("WebSocket has no stream");

    stream->endOutput();
    _outputActive = false;
    _output.setBusy(false);
}


void WebSocket::beginReceive()
{
    if( ! stream() )
        throw std::logic_error("WebSocket has no stream");

    if(_inputActive)
        throw std::logic_error("WebSocket input transfer is active");

    _peerClose = false;
    _header.clear();
    _payload.clear();
    _payloadSize = 0;
    _payloadGot = 0;
    _headerNeed = 2;
    _masked = false;
    _fin = true;
    _frame = WebSocketFrame::Text;
    _inputActive = true;
    _input.setBusy(true);
    _state = ReceiveHeader;
    beginFrameRead();
}


void WebSocket::endReceive()
{
    if(_idleTimeout != 0)
        _idleTimer.start(_idleTimeout);

    if(_error)
        throw std::runtime_error("WebSocket receive failed");

    _inputActive = false;
    _input.setBusy(false);
}


void WebSocket::sendPing()
{
    if(_outputActive)
        throw std::logic_error("WebSocket output transfer is active");

    _output.setType(WebSocketFrame::Ping);
    _output.setFin(true);
    beginSend();
}


void WebSocket::sendPong()
{
    if(_outputActive)
        throw std::logic_error("WebSocket output transfer is active");

    _output.setType(WebSocketFrame::Pong);
    _output.setFin(true);
    beginSend();
}


void WebSocket::close()
{
    if(_outputActive)
        throw std::logic_error("WebSocket output transfer is active");

    shutdown(true);
}


void WebSocket::shutdown(bool sendClose)
{
    if( sendClose )
    {
        if( Stream* stream = this->stream() )
        {
            _output.setType(WebSocketFrame::Close);
            _output.setFin(true);
            _output.composeClosePayload();
            writeFrame(_output.type(), _output.fin(), _output.data(), _output.size());
            stream->beginOutput();
            stream->endOutput();
        }
    }

    StreamSession::close();
}


void WebSocket::setTimeout(std::size_t timeout)
{
    _timeout = timeout;

    if( Stream* stream = this->stream() )
        stream->setTimeout(timeout);
}


void WebSocket::setMaxMessageSize(std::size_t maxSize)
{
    _maxMessageSize = maxSize;
}


void WebSocket::setIdleTimeout(std::size_t ms)
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


void WebSocket::onIdleTimeout()
{
    failStream();
}


void WebSocket::onCloseStream(Stream&)
{
    _error = true;
    _state = Idle;
    _inputActive = false;
    _outputActive = false;
    _input.setBusy(false);
    _output.setBusy(false);
    _closed.send(*this);
}


void WebSocket::failStream()
{
    _error = true;
    _state = Idle;
    _inputActive = false;
    _outputActive = false;
    _input.setBusy(false);
    _output.setBusy(false);

    if( Stream* stream = this->stream() )
        stream->close();
}


void WebSocket::parseUrl(const std::string& url, const std::string& /*origin*/)
{
    if(url.empty())
    {
        _path = "/";
        return;
    }

    if(url[0] == '/')
    {
        _path = url;
        return;
    }

    Pt::System::Uri uri(url);
    _path = uri.path().empty() ? "/" : uri.path();
    if( ! uri.query().empty() )
        _path += "?" + uri.query();
}


std::string WebSocket::createKey()
{
    std::srand( static_cast<unsigned int>(std::time(0)) );

    std::stringstream ss;
    Pt::BasicTextOStream<char, char> stream(ss, new Pt::Base64Codec());

    for(int i = 0; i < 4; ++i)
    {
        Pt::uint32_t val = static_cast<Pt::uint32_t>(std::rand());
        stream.write( reinterpret_cast<char*>(&val), 4 );
    }

    stream.flush();
    return ss.str();
}


Pt::uint32_t WebSocket::createMask()
{
    std::srand( static_cast<unsigned int>(std::time(0)) );
    return static_cast<Pt::uint32_t>(std::rand());
}


void WebSocket::writeFrame(WebSocketFrame::Type type, bool fin,
                           const char* payload, std::size_t n)
{
    Stream* stream = this->stream();
    std::streambuf* buf = stream ? stream->buffer() : 0;
    if( ! buf )
        throw std::logic_error("WebSocket has no stream");

    char header[14];
    std::size_t headerLen = 2;

    header[0] = fin ? (char)0x80 : 0;
    if(type == WebSocketFrame::Text)
        header[0] |= 0x01;
    else if(type == WebSocketFrame::Binary)
        header[0] |= 0x02;
    else if(type == WebSocketFrame::Close)
        header[0] |= 0x08;
    else if(type == WebSocketFrame::Ping)
        header[0] |= 0x09;
    else if(type == WebSocketFrame::Pong)
        header[0] |= 0x0A;

    header[1] = _isClient ? (char)0x80 : 0;

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
    if(_isClient)
    {
        Pt::uint32_t mask = createMask();
        std::memcpy(maskBytes, &mask, 4);
        std::memcpy(header + headerLen, maskBytes, 4);
        headerLen += 4;
    }

    buf->sputn(header, static_cast<std::streamsize>(headerLen));

    if(n == 0 || ! payload)
        return;

    if( ! _isClient )
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


void WebSocket::beginFrameRead()
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


bool WebSocket::parseAvailable()
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
                unsigned char b0 = static_cast<unsigned char>(_header[0]);
                if( (b0 & 0x70) != 0 )
                {
                    failStream();
                    return true;
                }

                _fin = (b0 & 0x80) != 0;
                unsigned opcode = b0 & 0x0F;
                if(opcode == 0x00)
                    _frame = WebSocketFrame::Continuation;
                else if(opcode == 0x01)
                    _frame = WebSocketFrame::Text;
                else if(opcode == 0x02)
                    _frame = WebSocketFrame::Binary;
                else if(opcode == 0x08)
                    _frame = WebSocketFrame::Close;
                else if(opcode == 0x09)
                    _frame = WebSocketFrame::Ping;
                else if(opcode == 0x0A)
                    _frame = WebSocketFrame::Pong;
                else
                {
                    failStream();
                    return true;
                }

                if( isControl(_frame) && ! _fin )
                {
                    failStream();
                    return true;
                }

                _masked = (_header[1] & 0x80) != 0;
                if(_isClient && _masked)
                {
                    failStream();
                    return true;
                }

                if( ! _isClient && ! _masked )
                {
                    failStream();
                    return true;
                }

                unsigned len7 = static_cast<unsigned char>(_header[1]) & 0x7F;
                if( isControl(_frame) && len7 > 125 )
                {
                    failStream();
                    return true;
                }

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

                if( isControl(_frame) && _payloadSize > 125 )
                {
                    failStream();
                    return true;
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
                _input.assign(_frame, _fin, 0, 0);
                _inputReady.send(*this);
                if(_frame == WebSocketFrame::Close)
                    failStream();
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
                if(_frame == WebSocketFrame::Close && _payload.size() == 1)
                {
                    failStream();
                    return true;
                }

                _state = Idle;
                _input.assign(_frame, _fin,
                              _payload.empty() ? 0 : &_payload[0],
                              _payload.size());
                _inputReady.send(*this);
                if(_frame == WebSocketFrame::Close)
                    failStream();
                return true;
            }
        }
    }

    return false;
}


void WebSocket::onInput()
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


void WebSocket::onOutput()
{
    Stream* stream = this->stream();
    if( ! stream )
        return;

    try
    {
        stream->endOutput();
        _outputReady.send(*this);
    }
    catch(const std::exception&)
    {
        failStream();
    }
}

} // namespace Http

} // namespace Pt
