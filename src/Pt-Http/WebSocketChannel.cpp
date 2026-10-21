/* Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include "WebSocketChannel.h"
#include <Pt/Http/Stream.h>
#include <Pt/Byteorder.h>
#include <Pt/System/EventLoop.h>
#include <Pt/System/IOError.h>
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
, _inputState(Idle)
, _receiveState(Idle)
, _messageOpen(false)
, _outputState(Idle)
, _sendState(Idle)
, _sendFragment(0)
, _pingState(Idle)
, _shutdownState(Idle)
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
    _inputState = Idle;
    _receiveState = Idle;
    _messageOpen = false;
    _receiveProgress = MessageProgress();
    _outputState = Idle;
    _sendState = Idle;
    _sendFragment = 0;
    _sendProgress = MessageProgress();
    _pingState = Idle;
    _pingError = std::exception_ptr();
    _outputQueue.clear();
    _shutdownState = Idle;
    _shutdownError = std::exception_ptr();
    _closeState = CloseOpen;
    _closeCode = 0;
    _closeReason.clear();
    _inputError = std::exception_ptr();
    _outputError = std::exception_ptr();
    _incoming.clear();
    _outgoing.clear();

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


bool WebSocketChannel::awaitingCloseReply() const
{
    return _closeState == CloseQueued || _closeState == CloseSent;
}


void WebSocketChannel::beginSend()
{
    requireOpen();

    if(_sendState != Idle)
        throw std::logic_error("WebSocket send is outstanding");

    if(_closeState != CloseOpen)
        throw std::logic_error("WebSocket is closing");

    if(_sendFragment == 0)
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

    }

    _sendState = Pending;
    _outputError = std::exception_ptr();
    _sendProgress = MessageProgress();
    _sendProgress.setHeader();
    enqueueData();
    requestOutput();
}


MessageProgress WebSocketChannel::endSend()
{
    if(_sendState != Ready)
    {
        requireOpen();
        throw std::logic_error("WebSocket send is not ready");
    }

    if(_outputError)
    {
        _sendState = Idle;
        std::rethrow_exception(_outputError);
    }

    requireOpen();
    restartIdleTimer();

    MessageProgress progress = _sendProgress;
    _sendState = Idle;
    if(progress.finished())
        _sendFragment = 0;

    return progress;
}


void WebSocketChannel::beginReceive()
{
    requireOpen();

    if(_receiveState != Idle)
        throw std::logic_error("WebSocket receive is outstanding");

    if(_shutdownState != Idle)
        throw std::logic_error("WebSocket shutdown is outstanding");

    if(_inputError)
    {
        std::exception_ptr error = _inputError;
        _inputError = std::exception_ptr();
        std::rethrow_exception(error);
    }

    _receiveState = Pending;
    _receiveProgress = MessageProgress();

    if( ! _messageOpen )
    {
        _incoming.setTypeFromEngine(WebSocketMessage::Unknown);
        _incoming.discard();
    }

    if( _incoming.type() != WebSocketMessage::Unknown )
        _receiveProgress.setHeader();

    requestInput();
}


MessageProgress WebSocketChannel::endReceive()
{
    if(_receiveState != Ready)
    {
        requireOpen();
        throw std::logic_error("WebSocket receive is not ready");
    }

    if(_inputError)
    {
        std::exception_ptr error = _inputError;
        _inputError = std::exception_ptr();
        _receiveState = Idle;
        std::rethrow_exception(error);
    }

    requireOpen();
    restartIdleTimer();

    MessageProgress progress = _receiveProgress;
    _receiveState = Idle;
    if(progress.finished())
        _messageOpen = false;

    return progress;
}


void WebSocketChannel::beginPing(const char* payload, std::size_t n)
{
    requireOpen();

    if(_pingState != Idle)
        throw std::logic_error("WebSocket ping is outstanding");

    if(_closeState != CloseOpen)
        throw std::logic_error("WebSocket is closing");

    if(n > 125 || (n != 0 && ! payload))
        throw std::invalid_argument("WebSocket ping payload");

    _pingState = Pending;
    _pingError = std::exception_ptr();
    enqueueControl(0x09, PendingFrame::Ping, payload, n);
    requestOutput();
}


void WebSocketChannel::endPing()
{
    if(_pingState != Ready)
        throw std::logic_error("WebSocket ping is not ready");

    if(_pingError)
    {
        _pingState = Idle;
        std::rethrow_exception(_pingError);
    }

    _pingState = Idle;
}


void WebSocketChannel::beginShutdown(unsigned code, const std::string& reason)
{
    requireOpen();

    if(_shutdownState != Idle)
        throw std::logic_error("WebSocket shutdown is outstanding");

    if(_receiveState != Idle)
        throw std::logic_error("WebSocket receive is outstanding");

    if(_closeState != CloseOpen)
        throw std::logic_error("WebSocket already closing");

    if(code == 1005 || code == 1006 || code == 1015)
        throw std::invalid_argument("WebSocket close code");

    if(reason.size() > 123 || ! isValidUtf8(reason.data(), reason.size()))
        throw std::invalid_argument("WebSocket close reason");

    _shutdownState = Pending;
    _shutdownError = std::exception_ptr();
    _closeCode = code;
    _closeReason = reason;
    enqueueClose(code, reason, PendingFrame::Shutdown);
    requestOutput();
    requestInput();
}


void WebSocketChannel::endShutdown()
{
    if(_shutdownState != Ready)
        throw std::logic_error("WebSocket shutdown is not ready");

    if(_shutdownError)
    {
        _shutdownState = Idle;
        std::rethrow_exception(_shutdownError);
    }

    _shutdownState = Idle;
}


void WebSocketChannel::finishShutdown()
{
    if(_shutdownState != Pending)
        return;

    _shutdownState = Ready;

    if(isOpen())
        Channel::close();

    _shutdownReady.send();
    _closed.send();
}


void WebSocketChannel::passiveClose()
{
    if( ! isOpen() )
        return;

    Channel::close();
    _closed.send();
}


void WebSocketChannel::onClose()
{
    _inputState = Idle;
    _receiveState = Idle;
    _outputState = Idle;
    _sendState = Idle;
    _sendFragment = 0;
    _inputError = std::exception_ptr();
    _outputError = std::exception_ptr();
    _idleTimer.stop();
    _incoming.clear();
    _outgoing.clear();

    if(_closeCode == 0 && _closeState != CloseExchanged)
        _closeCode = 1006;
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
    passiveClose();
}


void WebSocketChannel::enqueueData()
{
    PendingFrame frame;
    frame.fin = _outgoing.sendSize() <= MaxFramePayload;
    frame.kind = PendingFrame::Data;
    frame.size = _outgoing.sendSize();
    if(frame.size > MaxFramePayload)
        frame.size = MaxFramePayload;
    frame.opcode = _sendFragment == 0
                 ? ((_outgoing.type() == WebSocketMessage::Text) ? 0x01 : 0x02)
                 : 0;

    _outputQueue.push_back(frame);
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
}


void WebSocketChannel::enqueueClose(unsigned code, const std::string& reason,
                                    PendingFrame::Kind kind, bool reply)
{
    if(_closeState != CloseOpen)
        return;

    _closeState = reply ? CloseReplyQueued : CloseQueued;

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

    enqueueControl(0x08, kind, payload, n);
}


void WebSocketChannel::protocolFail(unsigned code)
{
    if( ! isOpen() )
        return;

    if(_closeCode == 0)
        _closeCode = code;

    enqueueClose(code, std::string());
    requestOutput();
}


Pt::uint32_t WebSocketChannel::nextMask()
{
    _maskSeed = _maskSeed * 1664525u + 1013904223u;
    return _maskSeed;
}


void WebSocketChannel::requestInput()
{
    if( ! isOpen() || _inputState != Idle || _inputError )
        return;

    if( _receiveState == Pending || awaitingCloseReply() )
    {
        Stream* stream = this->stream();
        if( ! stream || ! stream->isValid() )
            return;

        if( ! stream->loop() )
            return;

        stream->beginInput();
        _inputState = Pending;
    }
}


void WebSocketChannel::onInput()
{
    Stream* stream = this->stream();
    if( ! stream )
        return;

    try
    {
        _inputState = Idle;

        std::size_t n = stream->endInput();
        if(n == 0)
        {
            passiveClose();
            return;
        }

        _inputState = Ready;

        while( onProcessInput() )
            ;

        if(_inputState == Ready)
            _inputState = Idle;
    }
    catch(...)
    {
        onInputFailed( std::current_exception() );
    }
}


void WebSocketChannel::onInputFailed(std::exception_ptr error)
{
    if( ! isOpen() )
        return;

    if(_closeCode == 0)
        _closeCode = 1006;

    _inputState = Ready;

    if(_shutdownState == Pending)
    {
        _shutdownError = error;
        finishShutdown();
        return;
    }

    if(_receiveState == Pending)
    {
        _inputError = error;
        _receiveState = Ready;
        _inputReady.send();
        return;
    }

    passiveClose();
}


bool WebSocketChannel::onProcessInput()
{
    try
    {
        const Frame* frame = _wsbuf.import();
        if( ! frame )
        {
            _inputState = Idle;
            requestInput();
            return false;
        }

        switch( frame->kind() )
        {
            case Frame::Control:
                return onProcessControl(*frame);

            case Frame::Data:
                return onProcessData(*frame);
        }

        return false;
    }
    catch(const WebSocketError& e)
    {
        protocolFail( e.code() );
        _inputState = Idle;
        requestInput();
        return false;
    }
}


bool WebSocketChannel::onProcessControl(const Frame& frame)
{
    unsigned opcode = frame.opcode();
    const char* data = frame.data();
    std::size_t n = frame.size();

    if(opcode == 0x09)
    {
        restartIdleTimer();

        if(_closeState == CloseOpen)
        {
            enqueueControl(0x0A, PendingFrame::Pong, data, n);
            requestOutput();
        }

        return true;
    }

    if(opcode == 0x0A)
    {
        _pongPayload.assign(data ? data : "", n);
        restartIdleTimer();
        _pong.send(_pongPayload);
        return true;
    }

    if(n == 1)
    {
        protocolFail(1002);
        return true;
    }

    if(n >= 2)
    {
        Pt::uint16_t be = 0;
        std::memcpy(&be, data, 2);
        unsigned code = Pt::beToHost(be);
        std::string reason(data + 2, n - 2);

        if( ! isValidUtf8(reason.data(), reason.size()) )
        {
            protocolFail(1002);
            return true;
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

    bool replyNeeded = _closeState == CloseOpen;
    if(_closeState == CloseQueued)
        _closeState = CloseReplyQueued;
    else if(_closeState == CloseSent)
        _closeState = CloseExchanged;

    if(_messageOpen)
    {
        _incoming.discard();
        _messageOpen = false;
    }

    unsigned reply = 1000;
    if(_closeCode != 1005 && _closeCode != 1006 && _closeCode != 1015 &&
       _closeCode != 0)
        reply = _closeCode;

    if(replyNeeded)
        enqueueClose(reply, std::string(), PendingFrame::Close, true);

    requestOutput();

    if(_closeState == CloseExchanged && _outputState == Idle)
    {
        if(_shutdownState == Pending)
            finishShutdown();
        else
            passiveClose();
    }

    return false;
}


bool WebSocketChannel::onProcessData(const Frame& frame)
{
    bool finished = frame.fin() && frame.remaining() == 0;
    if(_shutdownState == Pending || _receiveState != Pending)
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
        _messageOpen = true;
    }

    _receiveProgress.setHeader();

    if(finished)
    {
        _receiveProgress.setFinished();
        _messageOpen = false;
    }

    if(_wsbuf.in_avail() > 0)
        _receiveProgress.setBody();

    _receiveState = Ready;
    _inputReady.send();

    if(_receiveState != Idle)
        return _wsbuf.in_avail() == 0;

    if( ! awaitingCloseReply())
        return false;

    if(_wsbuf.in_avail() != 0)
        _wsbuf.discard();

    return true;
}


void WebSocketChannel::requestOutput()
{
    if( ! isOpen() || _outputState != Idle)
        return;

    Stream* stream = this->stream();
    if( ! stream )
        return;

    if(_outputQueue.empty())
        return;

    const PendingFrame& frame = _outputQueue.front();
    const char* payload = frame.kind == PendingFrame::Data && frame.size != 0
                        ? _outgoing.sendData()
                        : frame.payload;
    writeFrame(frame.opcode, frame.fin, payload, frame.size);
    _outputState = Pending;
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
        onProcessOutput();
        requestOutput();
    }
    catch(...)
    {
        onOutputFailed( std::current_exception() );
    }
}


void WebSocketChannel::onOutputFailed(std::exception_ptr error)
{
    if( ! isOpen() )
        return;

    if(_closeCode == 0)
        _closeCode = 1006;

    _outputError = error;
    _outputState = Ready;

    if(_outputQueue.empty())
    {
        passiveClose();
        return;
    }

    const PendingFrame& frame = _outputQueue.front();

    switch(frame.kind)
    {
        case PendingFrame::Data:
        {
            if(_sendState == Pending)
            {
                _sendState = Ready;
                _outputReady.send();
                return;
            }

            break;
        }

        case PendingFrame::Ping:
        {
            if(_pingState == Pending)
            {
                _pingError = _outputError;
                _pingState = Ready;
                _pingReady.send();
                return;
            }

            break;
        }

        case PendingFrame::Shutdown:
        {
            if(_shutdownState == Pending)
            {
                _shutdownError = _outputError;
                finishShutdown();
                return;
            }

            break;
        }

        case PendingFrame::Close:
        case PendingFrame::Pong:
            break;
    }

    passiveClose();
}


void WebSocketChannel::onProcessOutput()
{
    if( ! isOpen() || _outputState == Ready || _outputError)
        return;

    if( _outputQueue.empty() )
    {
        _outputState = Idle;
        return;
    }

    PendingFrame frame = _outputQueue.front();
    _outputQueue.pop_front();
    _outputState = Idle;

    switch(frame.kind)
    {
        case PendingFrame::Data:
        {
            _outgoing.consume(frame.size);
            ++_sendFragment;
            if(_sendState == Pending)
            {
                _sendProgress.setBody();
                if(frame.fin && _outgoing.sendSize() == 0)
                    _sendProgress.setFinished();

                _sendState = Ready;
                _outputReady.send();
            }

            return;
        }

        case PendingFrame::Ping:
        {
            if(_pingState == Pending)
            {
                _pingState = Ready;
                _pingReady.send();
            }

            return;
        }

        case PendingFrame::Close:
        case PendingFrame::Shutdown:
        {
            if(_closeState == CloseQueued)
                _closeState = CloseSent;
            else if(_closeState == CloseReplyQueued)
                _closeState = CloseExchanged;

            if(_closeState == CloseExchanged)
            {
                if(_shutdownState == Pending)
                    finishShutdown();
                else
                    passiveClose();
            }

            return;
        }

        case PendingFrame::Pong:
            return;
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

} // namespace Http

} // namespace Pt
