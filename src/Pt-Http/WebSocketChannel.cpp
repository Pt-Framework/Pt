/* Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include "WebSocketChannel.h"
#include "Sha1.h"
#include <Pt/Http/Stream.h>
#include <Pt/Byteorder.h>
#include <Pt/Utf8.h>
#include <Pt/Base64Codec.h>
#include <Pt/TextStream.h>
#include <Pt/System/EventLoop.h>
#include <cstring>
#include <exception>
#include <sstream>
#include <stdexcept>

namespace Pt {

namespace Http {

namespace {

const std::size_t MaxFramePayload = 4096;

} // namespace

///////////////////////////////////////////////////////////////////////
// WebSocketChannel::Sentry
///////////////////////////////////////////////////////////////////////

class WebSocketChannel::Sentry
{
    public:
        Sentry(Sentry*& sentry)
        : _deleted(false)
        , _sentry(sentry)
        {
            sentry = this;
        }

        ~Sentry()
        {
            if( ! _deleted )
                this->detach();
        }

        bool operator!() const
        { return _deleted; }

        void detach()
        {
            _sentry = 0;
            _deleted = true;
        }

        bool _deleted;
        Sentry*& _sentry;
};

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
, _sentry(0)
, _clientMask(false)
, _maxMessageSize(1024 * 1024)
, _closeTimeout(10000)
, _inputPending(false)
, _outputPending(false)
, _isPingPending(false)
, _state(Closed)
, _closeCode(0)
{
    _timeouts.add(_idleTimeout);
    _timeouts.add(_pingDelay);
    _timeouts.add(_pingTimeout);
    _timeouts.add(_closeTimeout);

    _idleTimeout.timeout() += Pt::slot(*this, &WebSocketChannel::onIdleTimeout);
    _pingDelay.timeout() += Pt::slot(*this, &WebSocketChannel::onPingDelay);
    _pingTimeout.timeout() += Pt::slot(*this, &WebSocketChannel::onPingTimeout);
    _closeTimeout.timeout() += Pt::slot(*this, &WebSocketChannel::onCloseTimeout);
    _ping.ready() += Pt::slot(*this, &WebSocketChannel::onPingReady);
    _closeOut.ready() += Pt::slot(*this, &WebSocketChannel::onProtocolDone);
}


WebSocketChannel::~WebSocketChannel()
{
    if(_sentry)
        _sentry->detach();

    close();
}


std::string WebSocketChannel::getAcceptKey(const std::string& key)
{
    std::string accept(key);
    accept += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

    Sha1 sha1(accept.data(), accept.size());
    const std::vector<uint8_t>& digest = sha1.calc();

    std::ostringstream encoded;
    Pt::BasicTextOStream<char, char> base64(encoded, new Pt::Base64Codec());
    base64.write(reinterpret_cast<const char*>(digest.data()),
                 static_cast<std::streamsize>(digest.size()));
    base64.flush();
    return encoded.str();
}


std::string WebSocketChannel::createKey()
{
    Pt::uint8_t nonce[16];
    _random.get(nonce, sizeof(nonce));

    std::ostringstream encoded;
    Pt::BasicTextOStream<char, char> base64(encoded, new Pt::Base64Codec());
    base64.write(reinterpret_cast<const char*>(nonce),
                 static_cast<std::streamsize>(sizeof(nonce)));
    base64.flush();
    return encoded.str();
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

    if( System::EventLoop* loop = stream.loop() )
        _timeouts.setActive(*loop);

    _idleTimeout.begin();
    _pingDelay.begin();
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

    const bool opened = _sendProgress.header()
                        || _sendProgress.continuation();

    if( ! opened )
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

    unsigned msgCode = _send.message().type() == WebSocketMessage::Text ? 0x01
                                                                        : 0x02;
    unsigned opcode = opened ? 0 : msgCode;
    bool fin = finished && _send.message().pending() <= MaxFramePayload;

    _sendProgress = WebSocketProgress();
    if(opened)
        _sendProgress.setContinuation();
    else
        _sendProgress.setHeader();

    _send.set(opcode, fin);
    enqueue(_send);
    onBeginOutput();
}


WebSocketProgress WebSocketChannel::endSend()
{
    if( ! _send.isReady() )
        throw std::logic_error("WebSocket send is not ready");

    _send.end();

    _sendProgress.setBody();
    if( _send.fin() && _send.message().pending() == 0 )
        _sendProgress.setFinished();

    WebSocketProgress progress = _sendProgress;
    if( progress.finished() )
        _sendProgress = WebSocketProgress();

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
    _receiveProgress = WebSocketProgress();
    onBeginInput();
}


WebSocketProgress WebSocketChannel::endReceive()
{
    if( ! _receive.isReady() )
        throw std::logic_error("WebSocket receive is not ready");

    _receive.end();
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

    _isPingPending = true;
    _ping.begin();
    _ping.set(0x09, payload, n);
    enqueue(_ping);
    onBeginOutput();
    _pingTimeout.begin();
}


void WebSocketChannel::endPing()
{
    if( ! _ping.isReady() )
        throw std::logic_error("WebSocket ping is not ready");

    _isPingPending = false;
    _ping.end();
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

    // A failure from before the shutdown must not decide its outcome.
    _inputFailure = std::exception_ptr();

    _shutdown.begin();

    setCloseCode(code, reason);
    _state = CloseSent;

    _timeouts.cancel();
    _closeTimeout.begin();

    enqueueClose(code, reason);
    onBeginOutput();
    onBeginInput();
}


void WebSocketChannel::endShutdown()
{
    if( ! _shutdown.isReady() )
        throw std::logic_error("WebSocket shutdown is not ready");

    _shutdown.end();
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

    _timeouts.cancel();

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
        if(_inputFailure)
            _shutdown.setFailed(_inputFailure);
        else
            _shutdown.setFinished();

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
        _shutdown.setReady();
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
    _receiveProgress = WebSocketProgress();
    _inputFailure = std::exception_ptr();
    _receive.close();

    _outputPending = false;
    _outputQueue.clear();
    _outputFailure = std::exception_ptr();
    _sendProgress = WebSocketProgress();
    _send.close();
    _ping.close();
    _pong.close();
    _isPingPending = false;
    _closeOut.close();
    _shutdown.close();

    _timeouts.detach();
    _incoming.clear();
    _state = Closed;
}


void WebSocketChannel::setIdleTimeout(std::size_t ms)
{
    _idleTimeout.setDuration(ms);

    if(_state == Open)
        _idleTimeout.begin();
}


void WebSocketChannel::setPingInterval(std::size_t ms)
{
    _pingDelay.setDuration(ms);

    if(_state == Open)
        _pingDelay.begin();
}


void WebSocketChannel::setPingTimeout(std::size_t ms)
{
    _pingTimeout.setDuration(ms);
}


void WebSocketChannel::setCloseTimeout(std::size_t ms)
{
    _closeTimeout.setDuration(ms);

    if(_state == CloseSent)
        _closeTimeout.begin();
}


void WebSocketChannel::onIdleTimeout()
{
    onProtocolAborted();
}


void WebSocketChannel::onPingDelay()
{
    Sentry sentry(_sentry);

    if(_state != Open)
        return;

    if( ! _receive.isPending() )
        return;

    if(_pingTimeout.isStarted() || ! _ping.isIdle())
        return;

    _ping.reset();
    _ping.begin();
    _ping.set(0x09, true);
    enqueue(_ping);
    _pingTimeout.begin();

    if(_pingTimeout.duration() != 0)
        _pingDelay.end();

    try
    {
        onBeginOutput();
    }
    catch(const System::IOError&)
    {
        onOutputError( std::current_exception(), sentry );
    }
}


void WebSocketChannel::onPingReady()
{
    if(_isPingPending)
    {
        _pingReady.send();
        return;
    }

    _ping.reset();
}


void WebSocketChannel::onPingTimeout()
{
    onProtocolAborted();
}


void WebSocketChannel::onCloseTimeout()
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

    Sentry sentry(_sentry);

    std::size_t n = 0;
    try
    {
        n = stream->endInput();
    }
    catch(const System::IOError&)
    {
        if( ! sentry )
            return;

        onInputError( std::current_exception() );
        return;
    }

    if( ! sentry )
        return;

    if(_state >= Finish)
    {
        _inputPending = false;
        return;
    }

    if(n == 0)
    {
        std::streambuf* buf = stream->buffer();
        if( ! buf || buf->in_avail() <= 0 )
        {
            _inputPending = false;
            onProtocolAborted();
            return;
        }
    }

    const bool wasReady = _receive.isReady();

    for(;;)
    {
        bool want = (_state == CloseSent) ||
                    (_state == Open && _receive.isPending());
        if( ! want )
        {
            _inputPending = false;
            return;
        }

        const Frame* frame = 0;
        try
        {
            frame = _wsbuf.import();
            if( ! frame )
            {
                _inputPending = false;
                onBeginInput();
                return;
            }
        }
        catch(const WebSocketError& e)
        {
            if( ! sentry )
                return;

            onProtocolError( std::current_exception(), e.code(), sentry );
            return;
        }
        catch(const System::IOError&)
        {
            if( ! sentry )
                return;

            onInputError( std::current_exception() );
            return;
        }

        try
        {
            onFrame(*frame);
        }
        catch(const WebSocketError& e)
        {
            if( ! sentry )
                return;

            onProtocolError( std::current_exception(), e.code(), sentry );
            return;
        }

        if( ! sentry )
            return;

        try
        {
            onBeginOutput();
        }
        catch(const System::IOError&)
        {
            onOutputError( std::current_exception(), sentry );
        }

        if( ! sentry )
            return;

        if( ! _receive.isReady() )
            continue;

        if(wasReady)
            continue;

        _inputPending = false;
        _receive.setReady();
        if( ! sentry )
            return;

        if(_state == CloseSent)
        {
            try
            {
                onBeginInput();
            }
            catch(const System::IOError&)
            {
                if( ! sentry )
                    return;

                onInputError( std::current_exception() );
            }
        }

        return;
    }
}


void WebSocketChannel::onFrame(const Frame& frame)
{
    unsigned opcode = frame.opcode();

    if(opcode != 0x08 && _state == Open)
    {
        _idleTimeout.begin();
        _pingDelay.begin();
    }

    switch(opcode)
    {
        case 0x09: onPing(frame); break;
        case 0x0A: onPong(frame); break;
        case 0x08: onPeerClose(frame); break;
        case 0x01:
        case 0x02:
        case 0x00: onData(frame); break;
        default:   throw WebSocketError(1002);
    }
}


void WebSocketChannel::onPing(const Frame& frame)
{
    if(_state != Open)
        return;

    if( _pong.isPending() )
    {
        if(_outputPending && ! _outputQueue.empty() && _outputQueue.front() == &_pong)
            return;

        _pong.set(0x0A, frame.data(), frame.size());
        return;
    }

    _pong.reset();
    _pong.begin();
    _pong.set(0x0A, frame.data(), frame.size());
    enqueue(_pong);
}


void WebSocketChannel::onPong(const Frame& frame)
{
    const char* data = frame.data();
    _pongData.assign(data ? data : "", frame.size());

    _pingTimeout.end();

    _pongReceived.send(_pongData);
}


void WebSocketChannel::onPeerClose(const Frame& frame)
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

    if( ! _receive.isReady() )
        _incoming.discard();

    const bool reply = (_state == Open);
    onProtocolFinished();

    if(reply)
        enqueueClose(_closeCode, std::string());
    else
        enqueueClose(_closeCode, _closeReason);

    onProtocolDone();
}


void WebSocketChannel::onData(const Frame& frame)
{
    if( ! _receive.isPending() )
    {
        if(_state != CloseSent)
            return;

        _wsbuf.discard();
        return;
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
    else if(opcode == 0)
        _receiveProgress.setContinuation();

    if( frame.fin() && frame.remaining() == 0 )
        _receiveProgress.setFinished();

    if(_wsbuf.in_avail() > 0)
        _receiveProgress.setBody();

    _receive.setFinished();
}


void WebSocketChannel::onProtocolError(std::exception_ptr error, unsigned code, Sentry& sentry)
{
    if(_state == CloseSent)
    {
        _inputPending = false;
        _inputFailure = error;
        onProtocolFinished();
        enqueueClose(_closeCode, _closeReason);

        try
        {
            onBeginOutput();
        }
        catch(const System::IOError&)
        {
            onOutputError( std::current_exception(), sentry );
        }

        if( ! sentry )
            return;

        onProtocolDone();
        return;
    }

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
    }

    try
    {
        onBeginOutput();
    }
    catch(const System::IOError&)
    {
        onOutputError( std::current_exception(), sentry );
    }

    if( ! sentry )
        return;

    if(receivePending)
        _receive.setReady();
    else
        onProtocolDone();
}


void WebSocketChannel::onInputError(std::exception_ptr error)
{
    if(_state == Closed)
        return;

    _inputFailure = error;
    _inputPending = false;

    if(_state == CloseSent)
    {
        onProtocolFinished();
        _state = Done;
        _shutdown.setFailed(error);
        _shutdown.setReady();
        return;
    }

    if( _receive.isPending() )
    {
        _receive.setFailed(error);
        _receive.setReady();
    }
}


void WebSocketChannel::onBeginOutput()
{
    if(_outputPending || _outputQueue.empty() || _outputFailure)
        return;

    Output& out = *_outputQueue.front();

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
        _random.get(reinterpret_cast<Pt::uint8_t*>(maskBytes), 4);
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

    try
    {
        stream->beginOutput();
    }
    catch(const System::IOError&)
    {
        _outputFailure = std::current_exception();
        throw;
    }

    _outputPending = true;
}


void WebSocketChannel::onOutput()
{
    Stream* stream = this->stream();
    if( ! stream )
        return;

    Sentry sentry(_sentry);
    Output* out = 0;

    try
    {
        stream->endOutput();
        _outputPending = false;

        if(_state == Closed)
            return;

        if( _outputQueue.empty() )
            return;

        out = _outputQueue.front();
        _outputQueue.pop_front();
        out->discard();
        out->setFinished();
    }
    catch(const System::IOError&)
    {
        if( ! sentry )
            return;

        onOutputError( std::current_exception(), sentry );
        return;
    }

    if( ! sentry || ! out )
        return;

    out->setReady();
    if( ! sentry )
        return;

    try
    {
        onBeginOutput();
    }
    catch(const System::IOError&)
    {
        onOutputError( std::current_exception(), sentry );
    }
}


void WebSocketChannel::onOutputError(std::exception_ptr error, Sentry& sentry)
{
    if(_state == Closed)
        return;

    _outputFailure = error;
    _outputPending = false;
    setCloseCode(1006, std::string());

    if( _outputQueue.empty() )
        return;

    Output* out = _outputQueue.front();
    _outputQueue.clear();
    out->setFailed(error);

    if(out == &_closeOut && _shutdown.isPending())
    {
        onProtocolFinished();
        _state = Done;
        _shutdown.setFailed(error);
        _shutdown.setReady();
        return;
    }

    if(out == &_pong || (out == &_ping && ! _isPingPending))
    {
        if(_state < Finish)
            onProtocolAborted();
        else
            onProtocolDone();

        return;
    }

    out->setReady();
    if( ! sentry )
        return;

    if(_state < Finish)
        onProtocolAborted();
    else
        onProtocolDone();
}

} // namespace Http

} // namespace Pt
