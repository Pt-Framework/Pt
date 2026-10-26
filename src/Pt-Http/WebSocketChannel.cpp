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

///////////////////////////////////////////////////////////////////////
// WebSocketChannel::Output
///////////////////////////////////////////////////////////////////////

WebSocketChannel::Output::Output()
: _opcode(0)
, _fin(true)
, _size(0)
{
}


void WebSocketChannel::Output::set(unsigned opcode, bool fin)
{
    std::size_t n = _message.pending();
    if(n > MaxFramePayload)
        n = MaxFramePayload;

    _opcode = opcode;
    _fin = fin;
    _size = n;
}


void WebSocketChannel::Output::set(unsigned opcode, const char* data, std::size_t n)
{
    _message.discard();
    if(n != 0 && data)
        _message.body().write(data, n);

    set(opcode, true);
}


void WebSocketChannel::Output::discard()
{
    _message.discard(_size);
}

///////////////////////////////////////////////////////////////////////
// WebSocketChannel
///////////////////////////////////////////////////////////////////////

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


void WebSocketChannel::open(Stream& stream, bool clientMask)
{
    Channel::open(stream);

    _clientMask = clientMask;
    _closeState = CloseOpen;
    _closeCode = 0;
    _closeReason.clear();

    _receive.open();
    _send.open();
    _ping.open();
    _pongOut.open();
    _closeOut.open();
    _shutdown.open();

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
        WebSocketMessage& msg = _send.message();
        WebSocketMessage::Type type = msg.type();
        if(type != WebSocketMessage::Text && type != WebSocketMessage::Binary)
            throw std::invalid_argument("WebSocket message type");

        std::size_t n = msg.pending();
        if(_maxMessageSize != 0 && n > _maxMessageSize)
            throw std::invalid_argument("WebSocket message too large");

        if(finished && type == WebSocketMessage::Text)
        {
            const char* data = msg.buffer().data();
            if( ! isValidUtf8(data, n) )
                throw std::invalid_argument("WebSocket text is not UTF-8");
        }
    }

    _send.begin();
    _sendProgress = MessageProgress();
    if(isBegin)
        _sendProgress.setHeader();

    unsigned opcode = isBegin
                    ? ((_send.message().type() == WebSocketMessage::Text) ? 0x01 : 0x02)
                    : 0;
    bool fin = finished && _send.message().pending() <= MaxFramePayload;

    _send.set(opcode, fin);
    enqueue(&_send);
}


MessageProgress WebSocketChannel::endSend()
{
    if( ! _send.isReady() )
        throw std::logic_error("WebSocket send is not ready");

    _send.finish();

    _sendProgress.setBody();
    if( _send.fin() && _send.message().pending() == 0 )
        _sendProgress.setFinished();

    restartIdleTimer();

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
    if( _receive.isClosed() )
        throw IOError("WebSocket closed");

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
    _ping.set(0x09, payload, n);
    enqueue(&_ping);
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
    if( _shutdown.isClosed() )
        throw IOError("WebSocket closed");

    if( ! _shutdown.isReady() )
        throw std::logic_error("WebSocket shutdown is not ready");

    _shutdown.finish();
}


void WebSocketChannel::setCloseCode(unsigned code, const std::string& reason)
{
    // The first code decides; later ones only describe the consequence.

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


void WebSocketChannel::onCloseFinished()
{
    // Close handshake done and the close frame written.
    // Shutdown succeeds, then the stream detaches.

    if(_closeState != CloseFinished)
        return;

    if( ! _outputQueue.empty() || _outputPending )
        return;

    const bool receivePending = _receive.isPending();

    _shutdown.setReady();

    if( isOpen() )
        Channel::close();

    notifyClosed(_receive, receivePending);
}


void WebSocketChannel::onPassiveClose()
{
    // Unexpected stream end. Tear down first, then complete
    // outstanding receive/shutdown.

    if( ! isOpen() )
        return;

    if(_closeCode == 0)
        setCloseCode(1006, std::string());

    const bool receivePending = _receive.isPending();
    const bool shutdownPending = _shutdown.isPending();

    Channel::close();

    notifyClosed(_receive, receivePending);
    notifyClosed(_shutdown, shutdownPending);
}


void WebSocketChannel::notifyClosed(Operation& op, bool pending)
{
    if(pending)
        op.notify();
}


void WebSocketChannel::onClose()
{
    // Stream is gone. Every operation becomes Closed. No signals.

    if(_closeState != CloseFinished)
        setCloseCode(1006, std::string());

    _inputPending = false;
    _receiveProgress = MessageProgress();
    _inputFailure = std::exception_ptr();
    _receive.close();

    _outputPending = false;
    _outputQueue.clear();
    _outputFailure = std::exception_ptr();
    _sendProgress = MessageProgress();
    _send.close();
    _ping.close();
    _pongOut.close();
    _closeOut.close();
    _shutdown.close();

    _idleTimer.stop();
    _incoming.clear();
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


void WebSocketChannel::enqueue(Output* out)
{
    _outputQueue.push_back(out);
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

    if( _closeOut.isPending() )
    {
        _closeOut.set(0x08, payload, n);
        return;
    }

    _closeOut.reset();
    _closeOut.begin();
    _closeOut.set(0x08, payload, n);
    enqueue(&_closeOut);
}


void WebSocketChannel::requestInput()
{
    if( ! isOpen() || _inputPending )
        return;

    if(_closeState != CloseSent
       && (_closeState != CloseOpen || ! _receive.isPending()))
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

            bool nextFrame = onFrame(*frame);
            if( ! nextFrame )
            {
                _inputPending = false;

                if(_wsbuf.in_avail() == 0)
                    requestInput();

                return;
            }
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


bool WebSocketChannel::onFrame(const Frame& frame)
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


bool WebSocketChannel::onPing(const Frame& frame)
{
    restartIdleTimer();

    if(_closeState != CloseOpen)
        return true;

    if( _pongOut.isPending() )
    {
        if(_outputPending && ! _outputQueue.empty() && _outputQueue.front() == &_pongOut)
            return true;

        _pongOut.set(0x0A, frame.data(), frame.size());
        return true;
    }

    _pongOut.reset();
    _pongOut.begin();
    _pongOut.set(0x0A, frame.data(), frame.size());
    enqueue(&_pongOut);
    return true;
}


bool WebSocketChannel::onPong(const Frame& frame)
{
    const char* data = frame.data();
    _pongPayload.assign(data ? data : "", frame.size());
    restartIdleTimer();
    _pong.send(_pongPayload);
    return true;
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


bool WebSocketChannel::onPeerClose(const Frame& frame)
{
    // Peer close frame. Echo a close if we have not sent one.

    takeClosePayload(frame);

    _incoming.discard();

    if(_closeState == CloseOpen)
        enqueueClose( echoableCloseCode(_closeCode), std::string() );

    _closeState = CloseFinished;
    onCloseFinished();
    return false;
}


bool WebSocketChannel::onData(const Frame& frame)
{
    if( ! _receive.isPending() )
    {
        _wsbuf.discard();
        return true;
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

    return ! _receive.isReady();
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

    Output* out = _outputQueue.front();
    writeFrame(out->opcode(), out->fin(),
               out->message().buffer().data(), out->size());

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

        Output* out = _outputQueue.front();
        _outputQueue.pop_front();
        out->discard();
        out->setReady();

        requestOutput();
        onCloseFinished();
    }
    catch(...)
    {
        onOutputError( std::current_exception() );
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

    Output* out = _outputQueue.front();
    _outputQueue.pop_front();
    out->setFailed(error);
    _shutdown.setFailed(error);
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
