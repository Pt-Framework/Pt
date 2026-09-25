/*
 * Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
#include <Pt/Http/WebSocket.h>
#include "Parser.h"
#include "WebSocketStream.h"
#include <Pt/TextStream.h>
#include <Pt/Base64Codec.h>
#include <Pt/Byteorder.h>
#include <Pt/System/Uri.h>
#include <sstream>
#include <ctime>

namespace Pt {
namespace Http   {

/** @internal
    @brief Adapts Pt's HeaderParser/MessageHeader to the WebSocket handshake.

    Captures the HTTP status code and text of the server reply, which the
    MessageHeader itself does not store, and offers a buffer based advance().
*/
class WebSocketParser : public HeaderParser::MessageHeaderEvent
{
    public:
        WebSocketParser()
        : HeaderParser::MessageHeaderEvent(_header)
        , _parser(*this, true)
        , _returnCode(0)
        {
        }

        /// Parses a buffer of @a n bytes. Returns true if the header is complete.
        bool advance(const char* buffer, std::size_t n)
        {
            for (std::size_t i = 0; i < n; ++i)
            {
                if (_parser.parse(buffer[i]))
                    return true;
            }

            return false;
        }

        void reset(bool client)
        {
            _header.clear();
            _returnCode = 0;
            _returnText.clear();
            _parser.reset(client);
        }

        unsigned returnCode() const
        { return _returnCode; }

        const std::string& returnText() const
        { return _returnText; }

        MessageHeader& header()
        { return _header; }

        virtual void onHttpReturn(unsigned ret, const std::string& text)
        {
            _returnCode = ret;
            _returnText = text;
        }

    private:
        MessageHeader _header;
        HeaderParser  _parser;
        unsigned      _returnCode;
        std::string   _returnText;
};

static const char PongFrame[] = { (char)0x8A, (char)0x0 };
static const char PingFrame[] = { (char)0x89, (char)0x0 };
static const char CloseFrame[] = { (char)0x88, (char)0x82, (char)0xE1, (char)0xCF, (char)0x54, (char)0x77, (char)0xE2, (char)0x26 };

WebSocket::WebSocket()
: _host("127.0.0.1")
, _path("/")
, _protocol("ws")
, _connectHeader("")
, _port(80)
, _ioStream(0)
, _connected()
, _httpParser(new WebSocketParser())
, _timeout(3000)
, _keepAlive(true)
, _dummyLoop()
, _payloadSize(0)
, _currentOffset(0)
, _userReceiveBuffer(nullptr)
, _userReceiveBufferLenght(0)
, _userSendBufferLenght(0)
, _buffer(1024)
, _sendMask(0)
, _sendFrameMode(Text)
, _receiveframeMode(Unknow)
, _hasMask(false)
, _mask(0)
, _currentBuffer()
, _payloadLenghtType(0)
, _error(false)
, _state(Unknown)
{

}

WebSocket::WebSocket(Pt::Http::IOStream* stream)
: WebSocket()
{
    accept(stream);
}

WebSocket::~WebSocket()
{
    close();
}

void WebSocket::parseUrl(const std::string& url, const std::string& origin)
{
    Pt::System::Uri uri(url);

    _host = uri.host();
    _port = uri.port();
    _path = uri.path();
    _protocol = uri.protocol();

    std::stringstream ss;

    ss << "GET " << _path << " HTTP/1.1\r\n";

    if (_port == 80)
        ss << "Host: " << _host << "\r\n";
    else
        ss << "Host: " << _host << ":" << _port << "\r\n";

    ss << "Connection: Upgrade\r\n";
    ss << "Upgrade: websocket\r\n";

    if (!origin.empty())
        ss << "Origin: " << origin << "\r\n";

    ss << "Sec-WebSocket-Version: 13\r\n";
    ss << "Sec-WebSocket-Key: " << createKey() << "\r\n";

    ss << "\r\n";

    _connectHeader = ss.str();
}

std::string WebSocket::createKey()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    std::stringstream ss;

    Pt::BasicTextOStream<char, char> stream(ss, new Pt::Base64Codec());

    Pt::uint32_t val = (Pt::uint32_t)std::rand();

    stream.write((char*)&val, 4);

    val = (Pt::uint32_t)std::rand();
    stream.write((char*)&val, 4);

    val = (Pt::uint32_t)std::rand();
    stream.write((char*)&val, 4);

    val = (Pt::uint32_t)std::rand();
    stream.write((char*)&val, 4);

    stream.flush();

    return ss.str();
}

Pt::uint32_t WebSocket::createMask()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    return (Pt::uint32_t)std::rand();
}

void WebSocket::accept(Pt::Http::IOStream* stream)
{
    if (_ioStream)
    {
        delete _ioStream;
        _ioStream = 0;
    }

    detach();

    setActive(_dummyLoop);

    _ioStream = new WebSocketStream(*stream);

    _ioStream->setTimeout(_timeout);
    _ioStream->inputReady()  += Pt::slot(*this, &WebSocket::onInput);
    _ioStream->outputReady() += Pt::slot(*this, &WebSocket::onOutput);
}

void WebSocket::beginConnect(const std::string& url, const std::string& origin, bool keepAlive)
{
    if (_ioStream)
    {
        delete _ioStream;
        _ioStream = 0;
    }

    _ioStream = new WebSocketStream();
    _ioStream->inputReady()  += Pt::slot(*this, &WebSocket::onInput);
    _ioStream->outputReady() += Pt::slot(*this, &WebSocket::onOutput);
    _ioStream->connected()   += Pt::slot(*this, &WebSocket::onConnected);
    _ioStream->setActive(*_loop);

    _error = false;
    _keepAlive = keepAlive;
    parseUrl(url, origin);

    Pt::Net::Endpoint endpoint(_host, _port);

    _ioStream->setTimeout(_timeout);
    _ioStream->beginConnect(endpoint);
}

void WebSocket::onConnected()
{
    try
    {
        _ioStream->endConnect();
        _state = Connecting;
        _ioStream->beginWrite(_connectHeader.c_str(), _connectHeader.size());
    }
    catch (const std::exception&)
    {
        _state = Unknown;
        _error = true;
        _connected.send(*this);
    }
}

void WebSocket::onAttach(Pt::System::EventLoop& loop)
{
    Pt::System::IODevice::onAttach(_dummyLoop);
    _loop = &loop;
}

void WebSocket::onDetach(Pt::System::EventLoop& loop)
{
    Pt::System::IODevice::onDetach(_dummyLoop);
    _loop = 0;
}

void WebSocket::endConnect()
{
    _ioStream->endConnect();
}

void WebSocket::onInput()
{
    size_t size = 0;
    try
    {
        size = _ioStream->endRead();

        if (_state == Connecting)
        {
            if (_httpParser->advance(&_buffer[0], size))
            {
                if (_httpParser->returnCode() != 101)
                {
                    std::clog << "Code: " << _httpParser->returnCode() << " " << _httpParser->returnText() << std::endl;
                    std::clog << (char*)&_buffer[0] << std::endl;
                    throw std::runtime_error("switch server to WebSocketClient protocol failed");
                }
            }
            else
            {
                _ioStream->beginRead(&_buffer[0], 1);
                return;
            }
        }

        if (size == 0)
        {
            _ioStream->beginRead(&_buffer[0], _buffer.size());
            return;
        }

        switch (_state)
        {
            case Connecting:
            {
                _state = Unknown;
                _connected.send(*this);
            }
            break;

            default:
                handleInput(size);
            break;
        }

    }
    catch (const std::exception&)
    {
        _error = true;

        if (_state == Connecting)
        {
            _state = Unknown;
            _connected.send(*this);
        }
        else
        {
            _state = Unknown;
            inputReady().send(*this);
        }

        return;
    }
}

void WebSocket::onOutput()
{
    size_t size;

    try
    {
        size = _ioStream->endWrite();

        if (size == 0)
            throw std::runtime_error("socket closed");

        switch (_state)
        {
            case Connecting:
                _httpParser->reset(true);
                _ioStream->beginRead(&_buffer[0], 1);
            break;

            case Send:
                _state = Unknown;
                outputReady().send(*this);
            break;

            default:
            break;
        }
    }
    catch (const std::exception&)
    {
        _error = true;

        if (_state == Connecting)
        {
            _state = Unknown;
            _connected.send(*this);
            return;
        }

        _state = Unknown;
        outputReady().send(*this);
    }
}

std::size_t WebSocket::onBeginWrite(Pt::System::EventLoop& loop, const char* buffer, std::size_t n)
{
    _error = false;
    _state = Send;

    const size_t bufferSize = n + 255;

    if(_buffer.size() < bufferSize)
        _buffer.resize(bufferSize);

    bool hasMask = _ioStream->isClient();

    _userSendBufferLenght = n;

    // Fin bit
    _buffer[0] = (char)0x80;

    // Frame type
    if (_sendFrameMode == Text)
        _buffer[0] |= 0x1;
    else if (_sendFrameMode == Binary)
        _buffer[0] |= 0x2;

    // Data mask
    _buffer[1] = hasMask ? (char)0x80 : (char)0x0;

    size_t msgLen;

    //Lenght and chunking
    if (n < 126)
    {
        _buffer[1] |= (Pt::uint8_t)n;
        msgLen = 2;
    }
    else if (n < 65536)
    {
        _buffer[1] |= 126;
        Pt::uint16_t size = Pt::hostToBe((Pt::uint16_t) n);
        memcpy(&_buffer[2], &size, 2);
        msgLen = 4;
    }
    else
    {
        _buffer[1] |= 127;
        Pt::uint64_t size = Pt::hostToBe((Pt::uint64_t)n);
        memcpy(&_buffer[2], &size, 8);
        msgLen = 10;
    }

    //Copy payload
    if (hasMask)
    { //Write the mask to buffer
        const Pt::uint32_t mask = createMask();
        const char* maskPtr = (const char*)&mask;

        for (size_t i = 0; i < 4; ++i)
            _buffer[msgLen + i] = maskPtr[i];

        msgLen += 4;

        //Mask the data
        for (size_t i = 0; i < n; ++i)
            _buffer[msgLen + i] = buffer[i] ^ maskPtr[i % 4];
    }
    else
    {
        memcpy(&_buffer[msgLen], buffer, n);
    }

    msgLen += n;
    _ioStream->beginWrite(&_buffer[0], msgLen);

    return 0;
}


std::size_t WebSocket::onWrite(const char* buffer, std::size_t count)
{
    return 0;
}

std::size_t WebSocket::onEndWrite(Pt::System::EventLoop& loop, const char* buffer, std::size_t n)
{
    if (_error)
        throw;

    return _userSendBufferLenght;
}

void WebSocket::onCancel()
{
    Pt::System::IODevice::onCancel();

    if(_ioStream)
        _ioStream->cancel();

    _state = Unknown;
    _error = false;
}

std::size_t WebSocket::onBeginRead(Pt::System::EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    const size_t bufferSize = n + 255;

    if (_buffer.size() < bufferSize)
        _buffer.resize(bufferSize);

    eof = false;
    _error = false;
    _receiveframeMode = Unknow;
    _state = ReceiveHeader;
    _userReceiveBuffer = buffer;
    _userReceiveBufferLenght = n;
    _payloadLenghtType = 0;
    _currentBuffer.clear();
    _hasMask = false;
    memset(&_buffer[0], 0, _buffer.size());

    _ioStream->beginRead(&_buffer[0], _buffer.size());
    return 0;
}

std::size_t WebSocket::onRead(char* buffer, std::size_t count, bool& eof)
{
    return 0;
}

std::size_t WebSocket::onEndRead(Pt::System::EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    eof = false;

    if (_error)
        throw;

    if (_hasMask)
    {// => unmask
        const Pt::uint8_t* mask = (const Pt::uint8_t*) & _mask;

        for (size_t i = 0; i < _payloadSize; ++i)
            _userReceiveBuffer[i] ^= mask[i % 4];
    }

    return _payloadSize;
}

void WebSocket::onSetTimeout(std::size_t timeout)
{
    _timeout = timeout;

    if(_ioStream)
        _ioStream->setTimeout(timeout);
}

void WebSocket::sendPongFrame()
{
    _ioStream->write(PongFrame, 2);
}

void WebSocket::sendPingFrame()
{
    _ioStream->write(PingFrame, 2);
}

void WebSocket::handleReceiveHeader(const char* buffer)
{
    Pt::uint8_t frameFlags = (Pt::uint8_t) buffer[0];

    if (frameFlags & 0x01)
        _receiveframeMode = Text;
    else if (frameFlags & 0x2)
        _receiveframeMode = Binary;

    _hasMask = ((buffer[1] & 0x80) != 0);

    const Pt::uint8_t msgLenght = ((Pt::uint8_t) buffer[1]) & 0x7F;

    if ((frameFlags & 0x7F) == 0x9)
    {//Ping Frame
        _receiveframeMode = Ping;
        _payloadSize = 0;
        inputReady().send(*this);
    }
    if ((frameFlags & 0x7F) == 0xA)
    {//Pong Frame
        _receiveframeMode = Pong;
        _payloadSize = 0;
        inputReady().send(*this);
    }
    else if (msgLenght == 127)
    {
        _state = ReceivePayloadLenght;
        _payloadLenghtType = _hasMask ? 12 : 8;
    }
    else if (msgLenght == 126)
    {
        _state = ReceivePayloadLenght;
        _payloadLenghtType = _hasMask ? 6 : 2;
    }
    else
    {
        _payloadSize = msgLenght;

        if (_userReceiveBufferLenght < _payloadSize)
            throw std::invalid_argument("buffer to small for the message");

        if (_hasMask)
        {
            _state = ReceiveMask;
        }
        else
        {
            _state = ReceivePayloadChunk;
            _currentOffset = 0;
        }
    }
}

void WebSocket::handleReceiveMask(const char* buffer)
{
    memcpy(&_mask, buffer, 4);
    _state = ReceivePayloadChunk;
}

void WebSocket::handleReceivePayloadLenght(const char* buffer)
{
    if (_hasMask)
    {
        switch (_payloadLenghtType)
        {
            case 12:
                _payloadSize = Pt::beToHost(*((const Pt::uint64_t*) buffer));
                memcpy(&_mask, &buffer[8], 4);
            break;

            case 6:
                _payloadSize = Pt::beToHost(*((const Pt::uint16_t*) buffer));
                memcpy(&_mask, &buffer[2], 4);
            break;
        }
    }
    else
    {
        switch (_payloadLenghtType)
        {
            case 8:
                _payloadSize = Pt::beToHost(*((const Pt::uint64_t*) buffer));
            break;

            case 2:
                _payloadSize = Pt::beToHost(*((const Pt::uint16_t*) buffer));
            break;
        }
    }

    if (_userReceiveBufferLenght < _payloadSize)
        throw std::invalid_argument("buffer to small for the message");

    _state = ReceivePayloadChunk;
    _currentOffset = 0;
}

void WebSocket::handleInput(size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        switch (_state)
        {
            case ReceiveHeader:
            {
                _currentBuffer.push_back(_buffer[i]);

                if (_currentBuffer.size() == 2)
                {
                    handleReceiveHeader(&_currentBuffer[0]);
                    _currentBuffer.clear();
                }
            }
            break;

            case ReceiveMask:
            {
                _currentBuffer.push_back(_buffer[i]);
                if (_currentBuffer.size() == 4)
                {
                    handleReceiveMask(&_currentBuffer[0]);
                    _currentBuffer.clear();
                }
            }
            break;

            case ReceivePayloadLenght:
                _currentBuffer.push_back(_buffer[i]);

                if (_currentBuffer.size() == _payloadLenghtType)
                {
                    handleReceivePayloadLenght(&_currentBuffer[0]);
                    _currentBuffer.clear();
                    _payloadLenghtType = 0;
                }
            break;

            case ReceivePayloadChunk:
            {
                if (_currentOffset == _payloadSize)
                {
                    _currentOffset = 0;
                    _state = Unknown;
                    inputReady().send(*this);
                    return;//Finisch
                }
                else
                {
                    _userReceiveBuffer[_currentOffset] = _buffer[i];
                    _currentOffset++;
                }
            }
            break;

            case Unknown:
            {
            }
            break;

            default:
            break;
        }
    }

    if (_state == ReceivePayloadChunk)
    {
        if (_currentOffset == _payloadSize)
        {
            _currentOffset = 0;
            _state = Unknown;
            inputReady().send(*this);
            return;
        }
    }

    _ioStream->beginRead(&_buffer[0], _buffer.size());
}

void WebSocket::onClose()
{
    if (_ioStream)
    {
        try
        {
            _ioStream->write(CloseFrame, sizeof(CloseFrame));
        }
        catch (const std::exception&)
        {

        }

        delete _ioStream;
        _ioStream = 0;
    }
}

}}
