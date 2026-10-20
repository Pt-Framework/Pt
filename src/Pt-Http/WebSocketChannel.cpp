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
, _closeState(CloseOpen)
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
    _outputQueue.clear();
    _pings.clear();
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
}


void WebSocketChannel::requireOpen() const
{
    if( ! isOpen() )
        throw std::logic_error("WebSocket is not open");
}


void WebSocketChannel::setCloseSent()
{
    if(_closeState == CloseQueued)
        _closeState = CloseSent;
    else if(_closeState == CloseReplyQueued)
        _closeState = CloseExchanged;
}


void WebSocketChannel::setCloseReceived()
{
    if(_closeState == CloseOpen)
        _closeState = CloseReceived;
    else if(_closeState == CloseQueued)
        _closeState = CloseReplyQueued;
    else if(_closeState == CloseSent)
        _closeState = CloseExchanged;
}


bool WebSocketChannel::closeNeedsInput() const
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
    pumpOutput();
}


MessageProgress WebSocketChannel::endSend()
{
    if(_sendState != Ready && _sendState != Failed)
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

    pumpOutput();
    return progress;
}


void WebSocketChannel::beginReceive()
{
    requireOpen();

    if(_receiveState != Idle)
        throw std::logic_error("WebSocket receive is outstanding");

    _receiveState = Pending;
    _receiveProgress = MessageProgress();

    if( ! _messageOpen )
    {
        _incoming.setTypeFromEngine(WebSocketMessage::Unknown);
        _incoming.discard();
    }

    if( _incoming.type() != WebSocketMessage::Unknown )
        _receiveProgress.setHeader();

    if(_inputError)
    {
        _receiveState = Failed;
        _inputReady.send();
    }
    else
        processInput();
}


MessageProgress WebSocketChannel::endReceive()
{
    if(_receiveState != Ready && _receiveState != Failed)
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


void WebSocketChannel::ping(const char* payload, std::size_t n)
{
    requireOpen();

    if(n > 125)
        throw std::invalid_argument("WebSocket ping payload");

    enqueueControl(0x09, payload, n);
    pumpOutput();
}


void WebSocketChannel::shutdown(unsigned code, const std::string& reason)
{
    requireOpen();

    if(_closeState != CloseOpen)
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
    processInput();
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

    if(_closeCode == 0 && _closeState != CloseReceived &&
       _closeState != CloseExchanged)
        _closeCode = 1006;
}


void WebSocketChannel::passiveClose()
{
    if( ! isOpen() )
        return;

    Channel::close();
    _closed.send();
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
    _wsbuf.setMaxMessageSize(maxSize);
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
    passiveClose();
}


void WebSocketChannel::enqueueControl(unsigned opcode, const char* payload, std::size_t n)
{
    PendingFrame frame;
    frame.opcode = opcode;
    frame.fin = true;
    frame.data = false;
    frame.size = n;
    if(n != 0 && payload)
        std::memcpy(frame.payload, payload, n);

    _outputQueue.push_back(frame);
}


void WebSocketChannel::enqueueClose(unsigned code, const std::string& reason)
{
    if(_closeState != CloseOpen && _closeState != CloseReceived)
        return;

    if(_closeState == CloseOpen)
        _closeState = CloseQueued;
    else
        _closeState = CloseReplyQueued;

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
    if( ! isOpen() )
        return;

    if(_closeCode == 0)
        _closeCode = code;

    enqueueClose(code, std::string());
    pumpOutput();
}


void WebSocketChannel::requestInput()
{
    if( ! isOpen() || _inputState != Idle || _inputError)
        return;

    if(_receiveState != Pending && ! closeNeedsInput())
        return;

    Stream* stream = this->stream();
    if( ! stream || ! stream->isValid() )
        return;

    if( ! stream->loop() )
        return;

    try
    {
        _inputState = Pending;
        stream->beginInput();
    }
    catch(...)
    {
        _inputState = Idle;
        setInputError( std::current_exception() );
    }
}


void WebSocketChannel::setInputError(std::exception_ptr error)
{
    if( ! isOpen() || _inputError)
        return;

    _inputError = error;

    _inputState = Failed;

    if(_receiveState == Pending)
    {
        _receiveState = Failed;
        _inputReady.send();
    }
    else if( closeNeedsInput() )
    {
        passiveClose();
    }
}


void WebSocketChannel::pumpOutput()
{
    if( ! isOpen() || _outputState != Idle)
        return;

    Stream* stream = this->stream();
    if( ! stream )
        return;

    if(_outputQueue.empty() && _sendState == Pending)
    {
        PendingFrame frame;
        frame.fin = _outgoing.sendSize() <= MaxFramePayload;
        frame.data = true;
        frame.size = _outgoing.sendSize();
        if(frame.size > MaxFramePayload)
            frame.size = MaxFramePayload;
        frame.opcode = _sendFragment == 0
                     ? ((_outgoing.type() == WebSocketMessage::Text) ? 0x01 : 0x02)
                     : 0;
        _outputQueue.push_back(frame);
    }

    if(_outputQueue.empty())
        return;

    const PendingFrame& frame = _outputQueue.front();
    const char* payload = frame.data && frame.size != 0 ? _outgoing.sendData() : frame.payload;
    writeFrame(frame.opcode, frame.fin, payload, frame.size);
    _outputState = Pending;
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


void WebSocketChannel::processInput()
{
    if( ! isOpen() || _inputState == Ready || _inputState == Failed || _inputError)
        return;

    if(_receiveState != Pending && ! closeNeedsInput())
        return;

    _inputState = Ready;
    bool request = false;

    try
    {
        for(;;)
        {
            const Frame* frame = _wsbuf.import();
            if( ! frame )
            {
                request = true;
                break;
            }

            if(frame->kind() == Frame::Control)
            {
                if( onControlFrame(*frame) )
                    break;

                continue;
            }

            bool finished = frame->fin() && frame->remaining() == 0;
            if(_receiveState != Pending)
            {
                _wsbuf.discard();
                continue;
            }

            unsigned opcode = frame->opcode();
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

            if((_receiveState == Ready || _receiveState == Pending) && _wsbuf.in_avail() != 0)
                break;

            if(_receiveState == Ready || _receiveState == Pending)
                continue;

            if(closeNeedsInput())
            {
                if(_wsbuf.in_avail() != 0)
                    _wsbuf.discard();

                continue;
            }

            break;
        }
    }
    catch(const WebSocketError& e)
    {
        protocolFail( e.code() );
        request = true;
    }
    catch(...)
    {
        setInputError( std::current_exception() );
    }

    if(_inputState == Ready)
        _inputState = Idle;

    if(request)
        requestInput();
}


bool WebSocketChannel::onControlFrame(const Frame& frame)
{
    unsigned opcode = frame.opcode();
    const char* data = frame.data();
    std::size_t n = frame.size();

    if(opcode == 0x09)
    {
        enqueueControl(0x0A, data, n);
        restartIdleTimer();
        pumpOutput();
        return false;
    }

    if(opcode == 0x0A)
    {
        std::string payload(data ? data : "", n);
        if( ! _pings.empty() && _pings.front() == payload )
            _pings.pop_front();

        restartIdleTimer();
        return false;
    }

    setCloseReceived();

    if(n == 1)
    {
        protocolFail(1002);
        return false;
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

    if(_closeState == CloseExchanged && _outputState == Idle)
        passiveClose();

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
    if( ! stream )
        return;

    try
    {
        _inputState = Idle;

        if( stream->endInput() == 0 )
        {
            passiveClose();
            return;
        }

        processInput();
    }
    catch(...)
    {
        if( ! isOpen() )
            return;

        _inputState = Idle;

        if(_closeCode == 0)
            _closeCode = 1006;

        setInputError( std::current_exception() );
    }
}


void WebSocketChannel::onOutput()
{
    Stream* stream = this->stream();
    if( ! stream )
        return;

    try
    {
        stream->endOutput();

        if(_outputQueue.empty())
        {
            _outputState = Idle;
            pumpOutput();
            return;
        }

        PendingFrame frame = _outputQueue.front();
        _outputQueue.pop_front();
        _outputState = Idle;

        if( ! frame.data )
        {
            if(frame.opcode == 0x09)
                _pings.push_back( std::string(frame.payload, frame.size) );

            if(frame.opcode == 0x08)
                setCloseSent();

            if(_closeState == CloseExchanged)
            {
                passiveClose();
                return;
            }

            pumpOutput();
            return;
        }

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
    catch(...)
    {
        if( ! isOpen() )
            return;

        if(_closeCode == 0)
            _closeCode = 1006;

        _outputError = std::current_exception();
        _outputState = Failed;

        if(_sendState == Pending)
        {
            _sendState = Failed;
            _outputReady.send();
        }
    }
}

} // namespace Http

} // namespace Pt
