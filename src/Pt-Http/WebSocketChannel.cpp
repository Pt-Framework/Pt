/* Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include "WebSocketChannel.h"
#include <Pt/Http/Stream.h>
#include <Pt/Byteorder.h>
#include <Pt/System/EventLoop.h>
#include <cstring>
#include <ctime>
#include <stdexcept>

namespace Pt {

namespace Http {

namespace {

const std::size_t MaxFramePayload = 4096;

bool feedUtf8(unsigned& need, unsigned char& lead, bool& error,
              const char* data, std::size_t n)
{
    if(error)
        return false;

    const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
    for(std::size_t i = 0; i < n; ++i)
    {
        unsigned char c = p[i];
        if(need == 0)
        {
            lead = c;
            if(c <= 0x7F)
                continue;

            if(c >= 0xC2 && c <= 0xDF)
            {
                need = 1;
                continue;
            }

            if(c >= 0xE0 && c <= 0xEF)
            {
                need = 2;
                continue;
            }

            if(c >= 0xF0 && c <= 0xF4)
            {
                need = 3;
                continue;
            }

            error = true;
            return false;
        }

        if( (c & 0xC0) != 0x80 )
        {
            error = true;
            return false;
        }

        if(need == 2 && lead == 0xE0 && c < 0xA0)
        {
            error = true;
            return false;
        }

        if(need == 2 && lead == 0xED && c > 0x9F)
        {
            error = true;
            return false;
        }

        if(need == 3 && lead == 0xF0 && c < 0x90)
        {
            error = true;
            return false;
        }

        if(need == 3 && lead == 0xF4 && c > 0x8F)
        {
            error = true;
            return false;
        }

        --need;
    }

    return true;
}


bool finishUtf8(unsigned need, bool error)
{
    return ! error && need == 0;
}


bool isValidUtf8(const char* data, std::size_t n)
{
    unsigned need = 0;
    unsigned char lead = 0;
    bool error = false;
    if( ! feedUtf8(need, lead, error, data, n) )
        return false;

    return finishUtf8(need, error);
}

} // namespace


WebSocketChannel::WebSocketChannel()
: Channel()
, _clientMask(false)
, _opened(false)
, _ended(false)
, _timeout(30000)
, _maxMessageSize(1024 * 1024)
, _idleTimeout(0)
, _maskSeed( static_cast<Pt::uint32_t>(std::time(0)) )
, _inputState(InputIdle)
, _receiveOutstanding(false)
, _messageOpen(false)
, _frameFin(false)
, _frameMasked(false)
, _frameOpcode(0)
, _messageOpcode(0)
, _frameMask(0)
, _payloadSize(0)
, _payloadGot(0)
, _headerNeed(2)
, _messageSize(0)
, _utf8Need(0)
, _utf8Lead(0)
, _utf8Error(false)
, _outputState(OutputIdle)
, _outputKind(OutputNone)
, _sendOutstanding(false)
, _sendStarted(false)
, _firstFragment(true)
, _dataFinWritten(false)
, _awaitingEndSend(false)
, _closeQueued(false)
, _closeSent(false)
, _closeReceived(false)
, _closeCode(0)
{
    _idleTimer.timeout() += Pt::slot(*this, &WebSocketChannel::onIdleTimeout);
}


WebSocketChannel::~WebSocketChannel()
{
    close();
}


void WebSocketChannel::open(Stream& stream, bool clientMask)
{
    _clientMask = clientMask;
    _opened = true;
    Channel::open(stream);
    stream.setTimeout(_timeout);
    stream.inputReady() += Pt::slot(*this, &WebSocketChannel::onInput);
    stream.outputReady() += Pt::slot(*this, &WebSocketChannel::onOutput);
}


void WebSocketChannel::requireOpen() const
{
    if( ! _opened || ! stream() )
        throw std::logic_error("WebSocket handshake is not finished");
}


void WebSocketChannel::requireNotEnded() const
{
    if(_ended)
        throw std::logic_error("WebSocket is closed");
}


void WebSocketChannel::beginSend()
{
    requireNotEnded();
    requireOpen();

    if(_sendOutstanding)
        throw std::logic_error("WebSocket send is outstanding");

    if(_closeQueued || _closeSent)
        throw std::logic_error("WebSocket is closing");

    if( ! _sendStarted )
    {
        WebSocketMessage::Type type = _outgoing.type();
        if(type != WebSocketMessage::Text && type != WebSocketMessage::Binary)
            throw std::invalid_argument("WebSocket message type");

        std::size_t n = _outgoing.sendSize();
        if(_maxMessageSize != 0 && n > _maxMessageSize)
            throw std::invalid_argument("WebSocket message too large");

        if(type == WebSocketMessage::Text)
        {
            const char* data = _outgoing.sendData();
            if( ! isValidUtf8(data, n) )
                throw std::invalid_argument("WebSocket text is not UTF-8");
        }

        _firstFragment = true;
        _dataFinWritten = false;
        _sendStarted = true;
    }

    _sendOutstanding = true;
    _awaitingEndSend = false;
    _sendProgress = MessageProgress();
    _sendProgress.setHeader();
    pumpOutput();
}


MessageProgress WebSocketChannel::endSend()
{
    requireNotEnded();

    if( ! _sendOutstanding )
        throw std::logic_error("WebSocket send is not outstanding");

    restartIdleTimer();
    _sendOutstanding = false;
    _awaitingEndSend = false;

    MessageProgress progress = _sendProgress;
    if(progress.finished())
        _sendStarted = false;

    pumpOutput();
    return progress;
}


void WebSocketChannel::beginReceive()
{
    requireNotEnded();
    requireOpen();

    if(_receiveOutstanding)
        throw std::logic_error("WebSocket receive is outstanding");

    _receiveOutstanding = true;
    _receiveProgress = MessageProgress();

    if( ! _messageOpen )
    {
        _incoming.setTypeFromEngine(WebSocketMessage::Unknown);
        _incoming.discard();
        _messageSize = 0;
        _utf8Need = 0;
        _utf8Lead = 0;
        _utf8Error = false;
    }

    if( _incoming.type() != WebSocketMessage::Unknown )
        _receiveProgress.setHeader();

    beginInputPump();
}


MessageProgress WebSocketChannel::endReceive()
{
    requireNotEnded();

    if( ! _receiveOutstanding )
        throw std::logic_error("WebSocket receive is not outstanding");

    restartIdleTimer();
    _incoming.prepareRead();
    _receiveOutstanding = false;

    MessageProgress progress = _receiveProgress;
    if(progress.finished())
        _messageOpen = false;

    return progress;
}


void WebSocketChannel::ping(const char* payload, std::size_t n)
{
    requireNotEnded();
    requireOpen();

    if(n > 125)
        throw std::invalid_argument("WebSocket ping payload");

    enqueueControl(0x09, payload, n);
    pumpOutput();
}


void WebSocketChannel::shutdown(unsigned code, const std::string& reason)
{
    requireNotEnded();
    requireOpen();

    if(_closeQueued || _closeSent)
        throw std::logic_error("WebSocket already closing");

    if(code == 1005 || code == 1006 || code == 1015)
        throw std::invalid_argument("WebSocket close code");

    if(reason.size() > 123)
        throw std::invalid_argument("WebSocket close reason");

    if( ! isValidUtf8(reason.data(), reason.size()) )
        throw std::invalid_argument("WebSocket close reason");

    _closeCode = code;
    _closeReason = reason;
    enqueueClose(code, reason);
    pumpOutput();
}


void WebSocketChannel::close()
{
    if(_ended)
        return;

    _ended = true;
    _opened = false;
    _receiveOutstanding = false;
    _sendOutstanding = false;
    _sendStarted = false;
    _idleTimer.stop();
    _incoming.clear();
    _outgoing.clear();

    if(_closeCode == 0)
        _closeCode = 1006;

    if(Stream* stream = this->stream())
    {
        stream->cancel();
        Channel::close();
    }
}



void WebSocketChannel::setTimeout(std::size_t timeout)
{
    _timeout = timeout;

    if( Stream* stream = this->stream() )
        stream->setTimeout(timeout);
}


void WebSocketChannel::setMaxMessageSize(std::size_t maxSize)
{
    _maxMessageSize = maxSize;
}


void WebSocketChannel::setIdleTimeout(std::size_t ms)
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


void WebSocketChannel::onIdleTimeout()
{
    if(_closeCode == 0)
        _closeCode = 1006;

    if( Stream* stream = this->stream() )
        stream->close();
}


void WebSocketChannel::onCloseStream(Stream&)
{
    _ended = true;
    _opened = false;
    _receiveOutstanding = false;
    _sendOutstanding = false;
    _sendStarted = false;

    if( ! _closeReceived && _closeCode == 0)
        _closeCode = 1006;

    _closed.send();
}


void WebSocketChannel::enqueueControl(unsigned opcode, const char* payload, std::size_t n)
{
    ControlFrame frame;
    frame.opcode = opcode;
    frame.size = n;
    if(n != 0 && payload)
        std::memcpy(frame.payload, payload, n);

    _controlQueue.push_back(frame);
}


void WebSocketChannel::enqueueClose(unsigned code, const std::string& reason)
{
    if(_closeQueued || _closeSent)
        return;

    _closeQueued = true;

    char payload[125];
    std::size_t n = 0;
    Pt::uint16_t be = Pt::hostToBe( static_cast<Pt::uint16_t>(code) );
    std::memcpy(payload, &be, 2);
    n = 2;
    if( ! reason.empty() )
    {
        std::memcpy(payload + 2, reason.data(), reason.size());
        n += reason.size();
    }

    enqueueControl(0x08, payload, n);
}


void WebSocketChannel::protocolFail(unsigned code)
{
    if(_ended)
        return;

    if(_closeCode == 0)
        _closeCode = code;

    enqueueClose(code, std::string());
    pumpOutput();
}


void WebSocketChannel::beginInputPump()
{
    if(_ended)
        return;

    Stream* stream = this->stream();
    if( ! stream || ! stream->isValid() )
        return;

    if( ! stream->loop() )
        return;

    if( parseAvailable() )
        return;

    stream->beginInput();
}


void WebSocketChannel::pumpOutput()
{
    if(_ended || _outputState == OutputWriting)
        return;

    Stream* stream = this->stream();
    if( ! stream )
        return;

    if( ! _controlQueue.empty() )
    {
        const ControlFrame& frame = _controlQueue.front();
        writeFrame(frame.opcode, true, frame.payload, frame.size);

        if(frame.opcode == 0x09)
            _unansweredPings.push_back( std::string(frame.payload, frame.size) );

        if(frame.opcode == 0x08)
            _closeSent = true;

        _controlQueue.pop_front();
        _outputKind = OutputControl;
        _outputState = OutputWriting;
        stream->beginOutput();
        return;
    }

    if(_awaitingEndSend)
        return;

    if( ! _sendOutstanding || _dataFinWritten )
        return;

    std::size_t remaining = _outgoing.sendSize();
    std::size_t n = remaining;
    if(n > MaxFramePayload)
        n = MaxFramePayload;

    bool fin = remaining <= MaxFramePayload;
    unsigned opcode = 0;
    if(_firstFragment)
    {
        opcode = (_outgoing.type() == WebSocketMessage::Text) ? 0x01 : 0x02;
        _firstFragment = false;
    }

    const char* data = (n == 0) ? 0 : _outgoing.sendData();
    writeFrame(opcode, fin, data, n);
    _outgoing.consume(n);
    _dataFinWritten = fin;
    _outputKind = OutputData;
    _outputState = OutputWriting;
    stream->beginOutput();
}


void WebSocketChannel::writeFrame(unsigned opcode, bool fin,
                                     const char* payload, std::size_t n)
{
    Stream* stream = this->stream();
    std::streambuf* buf = stream ? stream->buffer() : 0;
    if( ! buf )
        throw std::logic_error("WebSocket has no stream");

    char header[14];
    std::size_t headerLen = 2;

    header[0] = static_cast<char>(opcode);
    if(fin)
        header[0] = static_cast<char>( static_cast<unsigned char>(header[0]) | 0x80 );

    header[1] = _clientMask ? static_cast<char>(0x80) : 0;

    if(n < 126)
    {
        header[1] = static_cast<char>(
            static_cast<unsigned char>(header[1]) | static_cast<unsigned char>(n) );
    }
    else if(n < 65536)
    {
        header[1] = static_cast<char>( static_cast<unsigned char>(header[1]) | 126 );
        Pt::uint16_t size = Pt::hostToBe( static_cast<Pt::uint16_t>(n) );
        std::memcpy(header + 2, &size, 2);
        headerLen = 4;
    }
    else
    {
        header[1] = static_cast<char>( static_cast<unsigned char>(header[1]) | 127 );
        Pt::uint64_t size = Pt::hostToBe( static_cast<Pt::uint64_t>(n) );
        std::memcpy(header + 2, &size, 8);
        headerLen = 10;
    }

    char maskBytes[4];
    if(_clientMask)
    {
        Pt::uint32_t mask = nextMask();
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


Pt::uint32_t WebSocketChannel::nextMask()
{
    _maskSeed = _maskSeed * 1664525u + 1013904223u;
    return _maskSeed;
}


bool WebSocketChannel::parseAvailable()
{
    Stream* stream = this->stream();
    std::streambuf* buf = stream ? stream->buffer() : 0;
    if( ! buf )
        return false;

    if(_inputState == InputIdle)
    {
        _header.clear();
        _headerNeed = 2;
        _payloadSize = 0;
        _payloadGot = 0;
        _controlPayload.clear();
        _inputState = InputHeader;
    }

    while( buf->in_avail() > 0 )
    {
        int ch = buf->sbumpc();
        if(ch < 0)
            break;

        char byte = static_cast<char>(ch);

        if(_inputState == InputHeader || _inputState == InputLength ||
           _inputState == InputMask)
        {
            _header.push_back(byte);
            if(_header.size() < _headerNeed)
                continue;

            if(_inputState == InputHeader)
            {
                unsigned char b0 = static_cast<unsigned char>(_header[0]);
                unsigned char b1 = static_cast<unsigned char>(_header[1]);
                _frameFin = (b0 & 0x80) != 0;
                unsigned rsv = b0 & 0x70;
                _frameOpcode = b0 & 0x0F;
                _frameMasked = (b1 & 0x80) != 0;
                unsigned len7 = b1 & 0x7F;

                if(rsv != 0)
                {
                    protocolFail(1002);
                    return false;
                }

                bool control = _frameOpcode == 0x08 || _frameOpcode == 0x09 ||
                               _frameOpcode == 0x0A;
                if(control && ! _frameFin)
                {
                    protocolFail(1002);
                    return false;
                }

                if(control && len7 > 125)
                {
                    protocolFail(1002);
                    return false;
                }

                if(_frameOpcode == 0x00)
                {
                    if( ! _messageOpen )
                    {
                        protocolFail(1002);
                        return false;
                    }
                }
                else if(_frameOpcode == 0x01 || _frameOpcode == 0x02)
                {
                    if(_messageOpen)
                    {
                        protocolFail(1002);
                        return false;
                    }
                }
                else if( ! control )
                {
                    protocolFail(1002);
                    return false;
                }

                if(_clientMask)
                {
                    if(_frameMasked)
                    {
                        protocolFail(1002);
                        return false;
                    }
                }
                else if( ! _frameMasked )
                {
                    protocolFail(1002);
                    return false;
                }

                if(len7 == 126)
                {
                    _inputState = InputLength;
                    _headerNeed = 4;
                    continue;
                }

                if(len7 == 127)
                {
                    _inputState = InputLength;
                    _headerNeed = 10;
                    continue;
                }

                _payloadSize = len7;
            }
            else if(_inputState == InputLength)
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

            bool control = _frameOpcode == 0x08 || _frameOpcode == 0x09 ||
                           _frameOpcode == 0x0A;
            if( ! control )
            {
                if(_maxMessageSize != 0 &&
                   (_payloadSize > _maxMessageSize ||
                    _messageSize > _maxMessageSize - _payloadSize))
                {
                    protocolFail(1009);
                    return false;
                }

                _messageSize += _payloadSize;
            }

            if(_frameMasked && _inputState != InputMask)
            {
                _inputState = InputMask;
                _headerNeed = _header.size() + 4;
                continue;
            }

            if(_frameMasked)
                std::memcpy(&_frameMask, &_header[_header.size() - 4], 4);

            _payloadGot = 0;
            _controlPayload.clear();
            _inputState = InputPayload;

            if(_payloadSize == 0)
            {
                if(control)
                {
                    if( onControlFrame() )
                        return true;
                }
                else if( onDataFrameComplete() )
                    return true;

                continue;
            }
        }
        else if(_inputState == InputPayload)
        {
            if(_frameMasked)
            {
                const char* maskBytes = reinterpret_cast<const char*>(&_frameMask);
                byte = static_cast<char>(byte ^ maskBytes[_payloadGot % 4]);
            }

            bool control = _frameOpcode == 0x08 || _frameOpcode == 0x09 ||
                           _frameOpcode == 0x0A;
            if(control)
                _controlPayload.push_back(byte);
            else if(_receiveOutstanding)
            {
                _incoming.append(&byte, 1);
                _receiveProgress.setBody();

                if(_messageOpcode == 0x01 || _frameOpcode == 0x01)
                {
                    if( ! feedUtf8(_utf8Need, _utf8Lead, _utf8Error, &byte, 1) )
                    {
                        protocolFail(1007);
                        return false;
                    }
                }
            }

            ++_payloadGot;
            if(_payloadGot != _payloadSize)
                continue;

            if(control)
            {
                if( onControlFrame() )
                    return true;
            }
            else if( onDataFrameComplete() )
                return true;
        }
    }

    if(_receiveOutstanding && _receiveProgress.body() &&
       _inputState == InputPayload)
    {
        _incoming.prepareRead();
        _inputReady.send();
        return true;
    }

    return false;
}


bool WebSocketChannel::onDataFrameComplete()
{
    _inputState = InputIdle;

    if(_frameOpcode == 0x01 || _frameOpcode == 0x02)
    {
        _messageOpen = true;
        _messageOpcode = _frameOpcode;
        WebSocketMessage::Type type = (_frameOpcode == 0x01)
                                    ? WebSocketMessage::Text
                                    : WebSocketMessage::Binary;
        _incoming.setTypeFromEngine(type);
        _receiveProgress.setHeader();
    }

    if(_frameFin)
    {
        if(_messageOpcode == 0x01 && ! finishUtf8(_utf8Need, _utf8Error) )
        {
            protocolFail(1007);
            return false;
        }

        _receiveProgress.setFinished();
    }

    if(_receiveOutstanding)
    {
        _incoming.prepareRead();
        _inputReady.send();
        return true;
    }

    _incoming.discard();
    if(_frameFin)
        _messageOpen = false;

    return false;
}


bool WebSocketChannel::onControlFrame()
{
    _inputState = InputIdle;

    if(_frameOpcode == 0x09)
    {
        enqueueControl(0x0A,
                       _controlPayload.empty() ? 0 : &_controlPayload[0],
                       _controlPayload.size());
        restartIdleTimer();
        pumpOutput();
        return false;
    }

    if(_frameOpcode == 0x0A)
    {
        std::string payload(_controlPayload.begin(), _controlPayload.end());
        if( ! _unansweredPings.empty() && _unansweredPings.front() == payload )
            _unansweredPings.pop_front();

        restartIdleTimer();
        return false;
    }

    _closeReceived = true;

    if(_controlPayload.size() == 1)
    {
        protocolFail(1002);
        return false;
    }

    if(_controlPayload.size() >= 2)
    {
        Pt::uint16_t be = 0;
        std::memcpy(&be, &_controlPayload[0], 2);
        unsigned code = Pt::beToHost(be);
        std::string reason(_controlPayload.begin() + 2, _controlPayload.end());

        if( ! isValidUtf8(reason.data(), reason.size()) )
        {
            protocolFail(1002);
            return false;
        }

        if(_closeCode == 0)
        {
            _closeCode = code;
            _closeReason = reason;
        }
    }
    else if(_closeCode == 0)
    {
        _closeCode = 1005;
    }

    if(_messageOpen)
    {
        _incoming.discard();
        _messageOpen = false;
    }

    unsigned reply = 1000;
    if(_closeCode != 1005 && _closeCode != 1006 && _closeCode != 1015 &&
       _closeCode != 0)
        reply = _closeCode;

    enqueueClose(reply, std::string());
    pumpOutput();

    if(_closeSent && _closeReceived && _outputState == OutputIdle)
    {
        if( Stream* stream = this->stream() )
            stream->close();
    }

    return true;
}


void WebSocketChannel::restartIdleTimer()
{
    if(_idleTimeout != 0)
        _idleTimer.start(_idleTimeout);
}


void WebSocketChannel::onInput()
{
    Stream* stream = this->stream();
    if( ! stream || _ended )
        return;

    try
    {
        if( stream->endInput() == 0 )
        {
            if(_closeCode == 0)
                _closeCode = 1006;

            stream->close();
            return;
        }

        if( parseAvailable() )
            return;

        stream->beginInput();
    }
    catch(const std::exception&)
    {
        if(_closeCode == 0)
            _closeCode = 1006;

        stream->close();
    }
}


void WebSocketChannel::onOutput()
{
    Stream* stream = this->stream();
    if( ! stream || _ended )
        return;

    try
    {
        stream->endOutput();
        OutputKind kind = _outputKind;
        _outputState = OutputIdle;
        _outputKind = OutputNone;

        if(kind == OutputControl)
        {
            if(_closeSent && _closeReceived)
            {
                stream->close();
                return;
            }

            pumpOutput();
            if(_outputState == OutputIdle && _closeSent &&
               ! _closeReceived && ! _receiveOutstanding)
                beginInputPump();

            return;
        }

        if(kind == OutputData)
        {
            _sendProgress.setBody();
            if(_dataFinWritten && _outgoing.sendSize() == 0)
                _sendProgress.setFinished();

            _awaitingEndSend = true;
            _outputReady.send();
            return;
        }

        pumpOutput();
    }
    catch(const std::exception&)
    {
        if(_closeCode == 0)
            _closeCode = 1006;

        stream->close();
    }
}

} // namespace Http

} // namespace Pt
