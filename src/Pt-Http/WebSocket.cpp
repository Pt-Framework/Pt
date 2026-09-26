/* Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/WebSocket.h>
#include <Pt/Http/Client.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/Http/Message.h>
#include <Pt/TextStream.h>
#include <Pt/Base64Codec.h>
#include <Pt/Byteorder.h>
#include <Pt/System/Uri.h>
#include <sstream>
#include <ctime>
#include <cstring>
#include <stdexcept>

namespace Pt {

namespace Http {

class WebSocket::PayloadBuffer : public std::streambuf
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


WebSocket::WebSocket(Client& client)
: _client(&client)
, _stream()
, _isClient(true)
, _path("/")
, _timeout(30000)
, _error(false)
, _state(Idle)
, _frame(Unknown)
, _masked(false)
, _mask(0)
, _payloadSize(0)
, _payloadGot(0)
, _headerNeed(2)
, _payloadBuffer(new PayloadBuffer(_payload))
{
}


WebSocket::WebSocket(Stream& stream)
: _client(0)
, _stream()
, _isClient(false)
, _path("/")
, _timeout(30000)
, _error(false)
, _state(Idle)
, _frame(Unknown)
, _masked(false)
, _mask(0)
, _payloadSize(0)
, _payloadGot(0)
, _headerNeed(2)
, _payloadBuffer(new PayloadBuffer(_payload))
{
    accept(stream);
}


WebSocket::~WebSocket()
{
    close();
    delete _payloadBuffer;
}


void WebSocket::accept(Stream& stream)
{
    _isClient = false;
    _stream = stream;
    _stream.retain();
    _stream.setTimeout(_timeout);
    _stream.inputReady() += Pt::slot(*this, &WebSocket::onInput);
    _stream.outputReady() += Pt::slot(*this, &WebSocket::onOutput);
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

        _stream = client.upgrade();
        _stream.retain();
        _stream.setTimeout(_timeout);
        _stream.inputReady() += Pt::slot(*this, &WebSocket::onInput);
        _stream.outputReady() += Pt::slot(*this, &WebSocket::onOutput);
        finishHandshake(false);
    }
    catch(const std::exception&)
    {
        finishHandshake(true);
    }
}


std::streambuf& WebSocket::buffer()
{
    return *_payloadBuffer;
}


void WebSocket::beginSend(Frame frame)
{
    if( ! _stream.isValid() )
        throw std::logic_error("WebSocket has no stream");

    _state = Sending;
    writeFrame(frame, _payload.empty() ? 0 : &_payload[0], _payload.size());
    _payloadBuffer->reset();
    _stream.beginOutput();
}


void WebSocket::endSend()
{
    if(_error)
        throw std::runtime_error("WebSocket send failed");

    _stream.endOutput();
    _state = Idle;
}


void WebSocket::beginReceive()
{
    if( ! _stream.isValid() )
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


void WebSocket::endReceive()
{
    if(_error)
        throw std::runtime_error("WebSocket receive failed");

    _payloadBuffer->prepareGet();
}


void WebSocket::sendPing()
{
    if( ! _stream.isValid() )
        throw std::logic_error("WebSocket has no stream");

    writeFrame(Ping, 0, 0);
    _stream.beginOutput();
    _stream.endOutput();
}


void WebSocket::sendPong()
{
    if( ! _stream.isValid() )
        throw std::logic_error("WebSocket has no stream");

    writeFrame(Pong, 0, 0);
    _stream.beginOutput();
    _stream.endOutput();
}


void WebSocket::setTimeout(std::size_t timeout)
{
    _timeout = timeout;

    if( _stream.isValid() )
        _stream.setTimeout(timeout);
}


void WebSocket::close()
{
    if( _stream.isValid() )
        _stream.close();
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


void WebSocket::writeFrame(Frame frame, const char* payload, std::size_t n)
{
    std::streambuf* buf = _stream.buffer();
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
    std::streambuf* buf = _stream.buffer();
    if(buf && buf->in_avail() > 0)
    {
        if( parseAvailable() )
            return;
    }

    _stream.beginInput();
}


bool WebSocket::parseAvailable()
{
    std::streambuf* buf = _stream.buffer();
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
                _inputReady.send(*this);
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
                _inputReady.send(*this);
                return true;
            }
        }
    }

    return false;
}


void WebSocket::onInput()
{
    try
    {
        _stream.endInput();

        if( parseAvailable() )
            return;

        _stream.beginInput();
    }
    catch(const std::exception&)
    {
        _error = true;
        _state = Idle;
        _inputReady.send(*this);
    }
}


void WebSocket::onOutput()
{
    try
    {
        _stream.endOutput();
        _state = Idle;
        _outputReady.send(*this);
    }
    catch(const std::exception&)
    {
        _error = true;
        _state = Idle;
        _outputReady.send(*this);
    }
}

}}
