/* Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include "WebSocketChannel.h"
#include <Pt/Http/Stream.h>
#include <Pt/Byteorder.h>
#include <Pt/Utf8.h>
#include <Pt/System/EventLoop.h>
#include <cstring>
#include <ctime>
#include <exception>
#include <stdexcept>

namespace Pt {

namespace Http {

namespace {

const std::size_t MaxFramePayload = 4096;

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
, _maxMessageSize(1024 * 1024)
, _idleTimeout(0)
, _maskSeed( static_cast<Pt::uint32_t>(std::time(0)) )
, _inputPending(false)
, _outputPending(false)
, _state(Closed)
, _closeCode(0)
{
    _idleTimer.timeout() += Pt::slot(*this, &WebSocketChannel::onIdleTimeout);
    _closeOut.ready() += Pt::slot(*this, &WebSocketChannel::onProtocolDone);
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
    _state = Open;
    _closeCode = 0;
    _closeReason.clear();

    _receive.open();
    _send.open();
    _ping.open();
    _pong.open();
    _closeOut.open();
    _shutdown.open();

    stream.inputReady() += Pt::slot(*this, &WebSocketChannel::onInput);
    stream.outputReady() += Pt::slot(*this, &WebSocketChannel::onOutput);

    if( std::streambuf* sb = stream.buffer() )
        _wsbuf.reset(*sb);

    _wsbuf.setMasking( ! clientMask );
    _wsbuf.setMaxMessageSize(_maxMessageSize);
    _incoming.setBuffer(_wsbuf);

    armIdleTimer();
}


void WebSocketChannel::beginSend(bool finished)
{
    if(_state == Closed)
        throw std::logic_error("WebSocket is not open");

    if( ! _send.isIdle() )
        throw std::logic_error("WebSocket send is outstanding");

    if(_state != Open)
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
    }

    _send.begin();
    _sendProgress = MessageProgress();
    if(isBegin)
        _sendProgress.setHeader();

    unsigned msgCode = _send.message().type() == WebSocketMessage::Text ? 0x01
                                                                        : 0x02;
    unsigned opcode = isBegin ? msgCode : 0;
    bool fin = finished && _send.message().pending() <= MaxFramePayload;

    _send.set(opcode, fin);
    enqueue(_send);
}


MessageProgress WebSocketChannel::endSend()
{
    if( ! _send.isReady() )
        throw std::logic_error("WebSocket send is not ready");

    _send.finish();

    _sendProgress.setBody();
    if( _send.fin() && _send.message().pending() == 0 )
        _sendProgress.setFinished();

    armIdleTimer();

    MessageProgress progress = _sendProgress;
    if( progress.finished() )
        _sendProgress = MessageProgress();

    return progress;
}


void WebSocketChannel::beginReceive()
{
    if(_state == Closed)
        throw std::logic_error("WebSocket is not open");

    if( ! _receive.isIdle() )
        throw std::logic_error("WebSocket receive is outstanding");

    if(_state >= Finish)
        throw std::logic_error("WebSocket is closing");

    if(_inputFailure)
        std::rethrow_exception(_inputFailure);

    _receive.begin();
    _receiveProgress = MessageProgress();
    onBeginInput();
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
    if(_state == Closed)
        throw std::logic_error("WebSocket is not open");

    if( ! _ping.isIdle() )
        throw std::logic_error("WebSocket ping is outstanding");

    if(_state != Open)
        throw std::logic_error("WebSocket is closing");

    if(n > 125 || (n != 0 && ! payload))
        throw std::invalid_argument("WebSocket ping payload");

    if(_outputFailure)
        std::rethrow_exception(_outputFailure);

    _ping.begin();
    _ping.set(0x09, payload, n);
    enqueue(_ping);
}


void WebSocketChannel::endPing()
{
    if( ! _ping.isReady() )
        throw std::logic_error("WebSocket ping is not ready");

    _ping.finish();
}


void WebSocketChannel::beginShutdown(unsigned code, const std::string& reason)
{
    if(_state == Closed)
        throw std::logic_error("WebSocket is not open");

    if( ! _shutdown.isIdle() )
        throw std::logic_error("WebSocket shutdown is outstanding");

    if(_state != Open)
        throw std::logic_error("WebSocket already closing");

    if(code == 1005 || code == 1006 || code == 1015)
        throw std::invalid_argument("WebSocket close code");

    if(reason.size() > 123 || ! isValidUtf8(reason.data(), reason.size()))
        throw std::invalid_argument("WebSocket close reason");

    if(_outputFailure)
        std::rethrow_exception(_outputFailure);

    _shutdown.begin();

    setCloseCode(code, reason);
    _state = CloseSent;
    enqueueClose(code, reason);
    onBeginInput();
}


void WebSocketChannel::endShutdown()
{
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


void WebSocketChannel::onProtocolFinished()
{
    _state = Finish;
    _idleTimer.stop();

    if(_outputQueue.empty())
        return;

    if(_outputPending)
    {
        Output* inFlight = _outputQueue.front();
        _outputQueue.clear();
        _outputQueue.push_back(inFlight);
        return;
    }

    _outputQueue.clear();
}


void WebSocketChannel::onProtocolDone()
{
    if(_state != Finish)
        return;

    if( ! _outputQueue.empty() || _outputPending )
        return;

    _state = Done;

    if( _shutdown.isPending() )
    {
        _shutdown.setReady();
        return;
    }

    _closed.send();
}


void WebSocketChannel::onProtocolAborted()
{
    if(_state >= Finish)
        return;

    if(_closeCode == 0)
        setCloseCode(1006, std::string());

    onProtocolFinished();

    if( _shutdown.isPending() )
    {
        _state = Done;
        _shutdown.setFailed(
            std::make_exception_ptr( IOError("WebSocket closed") ) );
        return;
    }

    onProtocolDone();
}


void WebSocketChannel::onClose()
{
    // Stream is gone. Every operation becomes Closed. No signals.

    if(_state < Finish)
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
    _pong.close();
    _closeOut.close();
    _shutdown.close();

    _idleTimer.stop();
    _incoming.clear();
    _state = Closed;
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


void WebSocketChannel::onIdleTimeout()
{
    onProtocolAborted();
}


void WebSocketChannel::enqueue(Output& out)
{
    if(_outputFailure)
        return;

    std::deque<Output*>::iterator it;
    for(it = _outputQueue.begin(); it != _outputQueue.end(); ++it)
    {
        if(*it == &out)
            return;
    }

    _outputQueue.push_back(&out);

    if( ! _outputPending )
        onBeginOutput( *_outputQueue.front() );
}


void WebSocketChannel::enqueueClose(unsigned code, const std::string& reason)
{
    if(_outputFailure)
        return;

    if( _closeOut.isReady() || _closeOut.isClosed() )
        return;

    if(_outputPending && ! _outputQueue.empty() && _outputQueue.front() == &_closeOut)
        return;

    if(code == 1005 || code == 1006 || code == 1015 || code == 0)
        code = 1000;

    char payload[125];
    Pt::uint16_t be = Pt::hostToBe( static_cast<Pt::uint16_t>(code) );
    std::memcpy(payload, &be, 2);
    std::size_t n = 2;

    std::size_t rlen = reason.size();
    if(rlen > 123)
        rlen = 123;

    if(rlen != 0)
    {
        std::memcpy(payload + 2, reason.data(), rlen);
        n += rlen;
    }

    if( ! _closeOut.isPending() )
    {
        _closeOut.reset();
        _closeOut.begin();
    }

    _closeOut.set(0x08, payload, n);
    enqueue(_closeOut);
}


void WebSocketChannel::onBeginInput()
{
    if(_inputPending)
        return;

    bool want = _state == CloseSent
             || (_state == Open && _receive.isPending());
    if( ! want )
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

        if(_state >= Finish)
        {
            _inputPending = false;
            return;
        }

        if(n == 0)
        {
            onProtocolAborted();
            return;
        }

        for(;;)
        {
            const Frame* frame = _wsbuf.import();
            if( ! frame )
            {
                _inputPending = false;
                onBeginInput();
                return;
            }

            if( ! onFrame(*frame) )
                return;
        }
    }
    catch(const WebSocketError& e)
    {
        onProtocolError(std::current_exception(), e.code());
    }
    catch(...)
    {
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
    armIdleTimer();

    if(_state != Open)
        return true;

    if( _pong.isPending() )
    {
        if(_outputPending && ! _outputQueue.empty() && _outputQueue.front() == &_pong)
            return true;

        _pong.set(0x0A, frame.data(), frame.size());
        return true;
    }

    _pong.reset();
    _pong.begin();
    _pong.set(0x0A, frame.data(), frame.size());
    enqueue(_pong);
    return true;
}


bool WebSocketChannel::onPong(const Frame& frame)
{
    const char* data = frame.data();
    _pongData.assign(data ? data : "", frame.size());
    armIdleTimer();
    _pongReceived.send(_pongData);
    return true;
}


bool WebSocketChannel::onPeerClose(const Frame& frame)
{
    // Peer close frame. Echo a close if we have not sent one.

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

        setCloseCode(code, reason);
    }
    else
    {
        setCloseCode(1005, std::string());
    }

    _incoming.discard();

    const bool reply = (_state == Open);
    onProtocolFinished();

    if(reply)
        enqueueClose(_closeCode, std::string());
    else
        enqueueClose(_closeCode, _closeReason);

    onProtocolDone();
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

    armIdleTimer();
    _receive.setReady();
    return true;
}


void WebSocketChannel::onProtocolError(std::exception_ptr error, unsigned code)
{
    if(_state != Open)
        return;

    setCloseCode(code, std::string());
    _inputFailure = error;

    const bool receivePending = _receive.isPending();
    onProtocolFinished();
    enqueueClose(code, std::string());

    if(receivePending)
    {
        _state = Done;
        _receive.setFailed(error);
        return;
    }

    onProtocolDone();
}


void WebSocketChannel::onInputError(std::exception_ptr error)
{
    if(_state == Closed)
        return;

    _inputFailure = error;

    if( _receive.isPending() )
        _receive.setFailed(error);
    else
        _shutdown.setFailed(error);
}


void WebSocketChannel::onBeginOutput(Output& out)
{
    Stream* stream = this->stream();
    std::streambuf* buf = stream ? stream->buffer() : 0;
    if( ! buf )
        throw std::logic_error("WebSocket has no stream");

    unsigned opcode = out.opcode();
    bool fin = out.fin();
    const char* payload = out.message().buffer().data();
    std::size_t n = out.size();

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

    if(n != 0 && payload)
    {
        if( ! _clientMask )
        {
            buf->sputn(payload, static_cast<std::streamsize>(n));
        }
        else
        {
            for(std::size_t i = 0; i < n; ++i)
            {
                char masked = static_cast<char>(payload[i] ^ maskBytes[i % 4]);
                buf->sputc(masked);
            }
        }
    }

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

        if(_state == Closed)
            return;

        if( _outputQueue.empty() )
            return;

        Output* out = _outputQueue.front();
        _outputQueue.pop_front();
        out->discard();

        if( ! _outputQueue.empty() )
            onBeginOutput( *_outputQueue.front() );

        out->setReady();
    }
    catch(...)
    {
        onOutputError( std::current_exception() );
    }
}


void WebSocketChannel::onOutputError(std::exception_ptr error)
{
    if(_state == Closed)
        return;

    _outputFailure = error;
    _outputPending = false;
    setCloseCode(1006, std::string());

    if( _outputQueue.empty() )
        return;

    Output* out = _outputQueue.front();
    _outputQueue.pop_front();
    out->setFailed(error);
}


} // namespace Http

} // namespace Pt
