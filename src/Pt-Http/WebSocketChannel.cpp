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
#include <exception>
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
, _timeout(30000)
, _maxMessageSize(1024 * 1024)
, _idleTimeout(0)
, _maskSeed( static_cast<Pt::uint32_t>(std::time(0)) )
, _inputPending(false)
, _outputPending(false)
, _closeState(CloseOpen)
, _closeCode(0)
{
    _idleTimer.timeout() += Pt::slot(*this, &WebSocketChannel::onIdleTimeout);
}


WebSocketChannel::~WebSocketChannel()
{
    close();
}


void WebSocketChannel::setMaxMessageSize(std::size_t maxSize)
{
    _maxMessageSize = maxSize;
    _wsbuf.setMaxMessageSize(maxSize);
}


void WebSocketChannel::resetState()
{
    _inputPending = false;
    _receiveProgress = MessageProgress();
    _inputFailure = std::exception_ptr();

    _outputPending = false;
    _outputQueue.clear();
    _outputFailure = std::exception_ptr();
    _sendProgress = MessageProgress();

    _send.cancel();
    _receive.cancel();
    _ping.cancel();
    _shutdown.cancel();

    _idleTimer.stop();

    _incoming.clear();
    _outgoing.clear();
}


void WebSocketChannel::open(Stream& stream, bool clientMask)
{
    Channel::open(stream);

    _clientMask = clientMask;
    _closeState = CloseOpen;
    _closeCode = 0;
    _closeReason.clear();
    _send.reset();
    _receive.reset();
    _ping.reset();
    _shutdown.reset();
    resetState();

    stream.setTimeout(_timeout);
    stream.inputReady() += Pt::slot(*this, &WebSocketChannel::onInput);
    stream.outputReady() += Pt::slot(*this, &WebSocketChannel::onOutput);

    if( std::streambuf* sb = stream.buffer() )
        _wsbuf.reset(*sb);

    _wsbuf.setMasking( ! clientMask );
    _wsbuf.setMaxMessageSize(_maxMessageSize);
    _incoming.setBuffer(_wsbuf);

    armIdleTimer();
}


void WebSocketChannel::requireOpen() const
{
    if( ! isOpen() )
        throw std::logic_error("WebSocket is not open");
}


bool WebSocketChannel::wantsInput() const
{
    if( ! isOpen() )
        return false;

    if(_closeState == CloseSent)
        return true;

    return _closeState == CloseOpen && _receive.isPending();
}


void WebSocketChannel::beginSend(bool finished)
{
    requireOpen();

    if( ! _send.isIdle() )
        throw std::logic_error("WebSocket send is outstanding");

    if(_closeState != CloseOpen)
        throw std::logic_error("WebSocket is closing");

    if(_outputFailure)
        std::rethrow_exception(_outputFailure);

    bool isBegin = ! _sendProgress.header() && ! _sendProgress.body();

    if(isBegin)
    {
        WebSocketMessage::Type type = _outgoing.type();
        if(type != WebSocketMessage::Text && type != WebSocketMessage::Binary)
            throw std::invalid_argument("WebSocket message type");

        std::size_t n = _outgoing.sendSize();
        if(_maxMessageSize != 0 && n > _maxMessageSize)
            throw std::invalid_argument("WebSocket message too large");

        if(finished && type == WebSocketMessage::Text)
        {
            const char* data = _outgoing.sendData();
            if( ! isValidUtf8(data, n) )
                throw std::invalid_argument("WebSocket text is not UTF-8");
        }
    }

    _send.begin();
    _sendProgress = MessageProgress();
    if(isBegin)
        _sendProgress.setHeader();

    enqueueData(isBegin, finished);
}


MessageProgress WebSocketChannel::endSend()
{
    if( ! _send.isReady() )
        throw std::logic_error("WebSocket send is not ready");

    _send.finish();

    MessageProgress progress = _sendProgress;
    if( progress.finished() )
        _sendProgress = MessageProgress();

    return progress;
}


void WebSocketChannel::beginReceive()
{
    requireOpen();

    if( ! _receive.isIdle() )
        throw std::logic_error("WebSocket receive is outstanding");

    if(_inputFailure)
        std::rethrow_exception(_inputFailure);

    _receive.begin();
    _receiveProgress = MessageProgress();
    requestInput();
}


MessageProgress WebSocketChannel::endReceive()
{
    if( ! _receive.isReady() )
        throw std::logic_error("WebSocket receive is not ready");

    _receive.finish();
    return _receiveProgress;
}


void WebSocketChannel::beginPing(const char* payload, std::size_t n)
{
    requireOpen();

    if( ! _ping.isIdle() )
        throw std::logic_error("WebSocket ping is outstanding");

    if(_closeState != CloseOpen)
        throw std::logic_error("WebSocket is closing");

    if(n > 125 || (n != 0 && ! payload))
        throw std::invalid_argument("WebSocket ping payload");

    if(_outputFailure)
        std::rethrow_exception(_outputFailure);

    _ping.begin();
    enqueueControl(0x09, PendingFrame::Ping, payload, n);
}


void WebSocketChannel::endPing()
{
    if( ! _ping.isReady() )
        throw std::logic_error("WebSocket ping is not ready");

    _ping.finish();
}


void WebSocketChannel::beginShutdown(unsigned code, const std::string& reason)
{
    requireOpen();

    if( ! _shutdown.isIdle() )
        throw std::logic_error("WebSocket shutdown is outstanding");

    if(_closeState != CloseOpen)
        throw std::logic_error("WebSocket already closing");

    if(code == 1005 || code == 1006 || code == 1015)
        throw std::invalid_argument("WebSocket close code");

    if(reason.size() > 123 || ! isValidUtf8(reason.data(), reason.size()))
        throw std::invalid_argument("WebSocket close reason");

    if(_outputFailure)
        std::rethrow_exception(_outputFailure);

    _shutdown.begin();

    setCloseCode(code, reason);
    _closeState = CloseSent;
    enqueueClose(code, reason);
    requestInput();
}


void WebSocketChannel::endShutdown()
{
    if( ! _shutdown.isReady() )
        throw std::logic_error("WebSocket shutdown is not ready");

    _shutdown.finish();
}


// The first code decides; later ones only describe the consequence.
void WebSocketChannel::setCloseCode(unsigned code, const std::string& reason)
{
    if(_closeCode != 0)
        return;

    _closeCode = code;
    _closeReason = reason;
}


unsigned WebSocketChannel::echoableCloseCode(unsigned code) const
{
    if(code == 1005 || code == 1006 || code == 1015 || code == 0)
        return 1000;

    return code;
}


void WebSocketChannel::discardOpenMessage()
{
    _incoming.discard();
}


void WebSocketChannel::onCloseFinished()
{
    if(_closeState != CloseFinished)
        return;

    if( ! _outputQueue.empty() || _outputPending )
        return;

    _shutdown.setReady();

    if( isOpen() )
        Channel::close();
}


void WebSocketChannel::onPassiveClose()
{
    if( ! isOpen() )
        return;

    if(_closeCode == 0)
        setCloseCode(1006, std::string());

    _receive.setClosed();
    _shutdown.setClosed();
    Channel::close();
}


void WebSocketChannel::onClose()
{
    if(_closeState != CloseFinished)
        setCloseCode(1006, std::string());

    resetState();
}


void WebSocketChannel::setTimeout(std::size_t timeout)
{
    _timeout = timeout;

    if( Stream* stream = this->stream() )
        stream->setTimeout(timeout);
}


void WebSocketChannel::setIdleTimeout(std::size_t ms)
{
    _idleTimeout = ms;
    armIdleTimer();
}


void WebSocketChannel::armIdleTimer()
{
    if(_idleTimeout == 0)
    {
        _idleTimer.stop();
        return;
    }

    Stream* stream = this->stream();
    System::EventLoop* loop = stream ? stream->loop() : 0;
    if( ! loop )
        return;

    if( ! _idleTimer.loop() )
        _idleTimer.setActive(*loop);

    _idleTimer.start(_idleTimeout);
}


void WebSocketChannel::restartIdleTimer()
{
    if(_idleTimeout != 0)
        _idleTimer.start(_idleTimeout);
}


void WebSocketChannel::onIdleTimeout()
{
    onPassiveClose();
}


void WebSocketChannel::enqueueData(bool isBegin, bool finished)
{
    // One fragment per send, so a queued control frame waits at most this long.
    PendingFrame frame;
    frame.fin = finished && _outgoing.sendSize() <= MaxFramePayload;
    frame.kind = PendingFrame::Data;
    frame.size = _outgoing.sendSize();
    if(frame.size > MaxFramePayload)
        frame.size = MaxFramePayload;
    frame.opcode = isBegin
                 ? ((_outgoing.type() == WebSocketMessage::Text) ? 0x01 : 0x02)
                 : 0;

    _outputQueue.push_back(frame);
    requestOutput();
}


void WebSocketChannel::enqueueControl(unsigned opcode, PendingFrame::Kind kind,
                                      const char* payload, std::size_t n)
{
    PendingFrame frame;
    frame.opcode = opcode;
    frame.fin = true;
    frame.kind = kind;
    frame.size = n;
    if(n != 0 && payload)
        std::memcpy(frame.payload, payload, n);

    _outputQueue.push_back(frame);
    requestOutput();
}


void WebSocketChannel::enqueueClose(unsigned code, const std::string& reason)
{
    char payload[125];
    Pt::uint16_t be = Pt::hostToBe( static_cast<Pt::uint16_t>(code) );
    std::memcpy(payload, &be, 2);
    std::size_t n = 2;
    if( ! reason.empty() )
    {
        std::memcpy(payload + 2, reason.data(), reason.size());
        n += reason.size();
    }

    enqueueControl(0x08, PendingFrame::Close, payload, n);
}


void WebSocketChannel::requestInput()
{
    if( ! isOpen() || _inputPending )
        return;

    if( ! wantsInput() )
        return;

    Stream* stream = this->stream();
    if( ! stream || ! stream->isValid() )
        return;

    if( ! stream->loop() )
        return;

    stream->beginInput();
    _inputPending = true;
}


void WebSocketChannel::onInput()
{
    Stream* stream = this->stream();
    if( ! stream )
        return;

    try
    {
        std::size_t n = stream->endInput();
        if(n == 0)
        {
            _inputPending = false;
            onPassiveClose();
            return;
        }

        for(;;)
        {
            const Frame* frame = _wsbuf.import();
            if( ! frame )
            {
                _inputPending = false;
                requestInput();
                return;
            }

            if( onFrame(*frame) == NextFrame )
                continue;

            if( wantsInput() && _wsbuf.in_avail() == 0 )
                continue;

            if( wantsInput() && ! _receive.isPending() )
            {
                _wsbuf.discard();
                continue;
            }

            _inputPending = false;
            return;
        }
    }
    catch(const WebSocketError& e)
    {
        _inputPending = false;
        onProtocolError(std::current_exception(), e.code());
    }
    catch(...)
    {
        _inputPending = false;
        onInputError( std::current_exception() );
    }
}


WebSocketChannel::InputAction WebSocketChannel::onFrame(const Frame& frame)
{
    switch( frame.opcode() )
    {
        case 0x09: return onPing(frame);
        case 0x0A: return onPong(frame);
        case 0x08: return onPeerClose(frame);
        case 0x01:
        case 0x02:
        case 0x00: return onData(frame);
        default:   throw WebSocketError(1002);
    }
}


WebSocketChannel::InputAction WebSocketChannel::onPing(const Frame& frame)
{
    restartIdleTimer();

    if(_closeState == CloseOpen)
        enqueueControl(0x0A, PendingFrame::Pong, frame.data(), frame.size());

    return NextFrame;
}


WebSocketChannel::InputAction WebSocketChannel::onPong(const Frame& frame)
{
    const char* data = frame.data();
    _pongPayload.assign(data ? data : "", frame.size());
    restartIdleTimer();
    _pong.send(_pongPayload);
    return NextFrame;
}


void WebSocketChannel::takeClosePayload(const Frame& frame)
{
    const char* data = frame.data();
    std::size_t n = frame.size();

    if(n == 1)
        throw WebSocketError(1002);

    if(n >= 2)
    {
        Pt::uint16_t be = 0;
        std::memcpy(&be, data, 2);
        unsigned code = Pt::beToHost(be);
        std::string reason(data + 2, n - 2);

        if( ! isValidUtf8(reason.data(), reason.size()) )
            throw WebSocketError(1002);

        setCloseCode(code, reason);
        return;
    }

    setCloseCode(1005, std::string());
}


WebSocketChannel::InputAction WebSocketChannel::onPeerClose(const Frame& frame)
{
    takeClosePayload(frame);
    discardOpenMessage();

    if(_closeState == CloseOpen)
        enqueueClose( echoableCloseCode(_closeCode), std::string() );

    _closeState = CloseFinished;
    _receive.setClosed();
    onCloseFinished();
    return Stop;
}


WebSocketChannel::InputAction WebSocketChannel::onData(const Frame& frame)
{
    if( ! _receive.isPending() )
    {
        _wsbuf.discard();
        return NextFrame;
    }

    unsigned opcode = frame.opcode();
    if(opcode == 0x01 || opcode == 0x02)
    {
        WebSocketMessage::Type type = (opcode == 0x01)
                                    ? WebSocketMessage::Text
                                    : WebSocketMessage::Binary;
        _incoming.setTypeFromEngine(type);
        _receiveProgress.setHeader();
    }

    if( frame.fin() && frame.remaining() == 0 )
        _receiveProgress.setFinished();

    if(_wsbuf.in_avail() > 0)
        _receiveProgress.setBody();

    if( _receive.setReady() )
        restartIdleTimer();

    return Stop;
}


void WebSocketChannel::onProtocolError(std::exception_ptr error, unsigned code)
{
    if( ! isOpen() || _closeState != CloseOpen )
        return;

    setCloseCode(code, std::string());
    _inputFailure = error;
    _receive.setFailed(error);

    _closeState = CloseFinished;
    enqueueClose(code, std::string());
}


void WebSocketChannel::onInputError(std::exception_ptr error)
{
    if( ! isOpen() )
        return;

    _inputFailure = error;
    _receive.setFailed(error);
    _shutdown.setFailed(error);
}


void WebSocketChannel::requestOutput()
{
    if( ! isOpen() || _outputPending || _outputFailure )
        return;

    Stream* stream = this->stream();
    if( ! stream )
        return;

    if(_outputQueue.empty())
        return;

    const PendingFrame& frame = _outputQueue.front();
    const char* payload = frame.kind == PendingFrame::Data
                        ? (frame.size != 0 ? _outgoing.sendData() : 0)
                        : frame.payload;
    writeFrame(frame.opcode, frame.fin, payload, frame.size);

    _outputPending = true;
    stream->beginOutput();
}


void WebSocketChannel::onOutput()
{
    Stream* stream = this->stream();
    if( ! stream )
        return;

    try
    {
        stream->endOutput();
        _outputPending = false;

        if( ! isOpen() )
            return;

        if( _outputQueue.empty() )
            return;

        PendingFrame frame = _outputQueue.front();
        _outputQueue.pop_front();
        onFrameWritten(frame);

        requestOutput();
        onCloseFinished();
    }
    catch(...)
    {
        onOutputError( std::current_exception() );
    }
}


void WebSocketChannel::onFrameWritten(const PendingFrame& frame)
{
    switch(frame.kind)
    {
        case PendingFrame::Data:
            _outgoing.consume(frame.size);
            _sendProgress.setBody();
            if(frame.fin && _outgoing.sendSize() == 0)
                _sendProgress.setFinished();
            if( _send.setReady() )
                restartIdleTimer();
            break;

        case PendingFrame::Ping:
            _ping.setReady();
            break;

        case PendingFrame::Pong:
            break;

        case PendingFrame::Close:
             break;
    }
}


void WebSocketChannel::onOutputError(std::exception_ptr error)
{
    if( ! isOpen() )
        return;

    _outputFailure = error;
    _outputPending = false;
    setCloseCode(1006, std::string());

    if( _outputQueue.empty() )
        return;

    PendingFrame frame = _outputQueue.front();
    _outputQueue.pop_front();

    switch(frame.kind)
    {
        case PendingFrame::Data:
            _send.setFailed(error);
            break;

        case PendingFrame::Ping:
            _ping.setFailed(error);
            break;

        case PendingFrame::Close:
            _shutdown.setFailed(error);
            break;

        case PendingFrame::Pong:
            break;
    }
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
        _maskSeed = _maskSeed * 1664525u + 1013904223u;
        Pt::uint32_t mask = _maskSeed;
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

} // namespace Http

} // namespace Pt
