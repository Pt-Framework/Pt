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
, _opened(false)
, _ended(false)
, _timeout(30000)
, _maxMessageSize(1024 * 1024)
, _idleTimeout(0)
, _maskSeed( static_cast<Pt::uint32_t>(std::time(0)) )
, _actions(NoAction)
, _inputState(InputIdle)
, _inputProcessing(false)
, _discardingInput(false)
, _messageOpen(false)
, _outputState(OutputIdle)
, _outputOpcode(0)
, _sendStarted(false)
, _firstFragment(true)
, _dataFinWritten(false)
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
    _clientMask = clientMask;
    _opened = true;
    Channel::open(stream);
    stream.setTimeout(_timeout);
    stream.inputReady() += Pt::slot(*this, &WebSocketChannel::onInput);
    stream.outputReady() += Pt::slot(*this, &WebSocketChannel::onOutput);

    if( std::streambuf* sb = stream.buffer() )
        _wsbuf.reset(*sb);

    _wsbuf.setMasking( ! clientMask );
    _wsbuf.setMaxMessageSize(_maxMessageSize);
    _incoming.setBuffer(_wsbuf);
    requestInput();
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


bool WebSocketChannel::isActionPending(Action action) const
{
    return (_actions & static_cast<unsigned>(action)) != 0;
}


void WebSocketChannel::setActionPending(Action action)
{
    _actions |= static_cast<unsigned>(action);
}


void WebSocketChannel::clearAction(Action action)
{
    _actions &= ~static_cast<unsigned>(action);
}


void WebSocketChannel::setCloseSent()
{
    if(_closeState == CloseOpen)
        _closeState = CloseSent;
    else if(_closeState == CloseReceived)
        _closeState = CloseExchanged;
}


void WebSocketChannel::setCloseReceived()
{
    if(_closeState == CloseOpen)
        _closeState = CloseReceived;
    else if(_closeState == CloseSent)
        _closeState = CloseExchanged;
}


void WebSocketChannel::beginSend()
{
    requireNotEnded();
    requireOpen();

    if( isActionPending(SendAction) )
        throw std::logic_error("WebSocket send is outstanding");

    if( isActionPending(CloseAction) )
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

    setActionPending(SendAction);
    _outputError = std::exception_ptr();
    _sendProgress = MessageProgress();
    _sendProgress.setHeader();
    pumpOutput();
}


MessageProgress WebSocketChannel::endSend()
{
    if( ! isActionPending(SendAction) )
    {
        requireNotEnded();
        throw std::logic_error("WebSocket send is not outstanding");
    }

    clearAction(SendAction);
    if(_outputState == OnEndSend)
        _outputState = OutputIdle;

    if(_outputError)
        std::rethrow_exception(_outputError);

    requireNotEnded();
    restartIdleTimer();

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

    if( isActionPending(ReceiveAction) )
        throw std::logic_error("WebSocket receive is outstanding");

    setActionPending(ReceiveAction);
    _receiveProgress = MessageProgress();

    if( ! _messageOpen )
    {
        _incoming.setTypeFromEngine(WebSocketMessage::Unknown);
        _incoming.discard();
    }

    if( _incoming.type() != WebSocketMessage::Unknown )
        _receiveProgress.setHeader();

    if(_inputError)
        _inputReady.send();
    else
        processInput();
}


MessageProgress WebSocketChannel::endReceive()
{
    if( ! isActionPending(ReceiveAction) )
    {
        requireNotEnded();
        throw std::logic_error("WebSocket receive is not outstanding");
    }

    clearAction(ReceiveAction);

    if(_inputError)
    {
        std::exception_ptr error = _inputError;
        _inputError = std::exception_ptr();
        std::rethrow_exception(error);
    }

    requireNotEnded();
    restartIdleTimer();

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

    if( isActionPending(CloseAction) )
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


void WebSocketChannel::onClose()
{
    _ended = true;
    _opened = false;
    _actions = NoAction;
    _inputState = InputIdle;
    _inputProcessing = false;
    _discardingInput = false;
    _outputState = OutputIdle;
    _outputOpcode = 0;
    _sendStarted = false;
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
    if(_ended)
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
    PendingControl frame;
    frame.opcode = opcode;
    frame.size = n;
    if(n != 0 && payload)
        std::memcpy(frame.payload, payload, n);

    _controlQueue.push_back(frame);
}


void WebSocketChannel::enqueueClose(unsigned code, const std::string& reason)
{
    if( isActionPending(CloseAction) )
        return;

    setActionPending(CloseAction);

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


void WebSocketChannel::requestInput()
{
    if(_ended || _inputState != InputIdle || _inputError)
        return;

    Stream* stream = this->stream();
    if( ! stream || ! stream->isValid() )
        return;

    if( ! stream->loop() )
        return;

    try
    {
        _inputState = OnInput;
        stream->beginInput();
    }
    catch(...)
    {
        _inputState = InputIdle;
        setInputError( std::current_exception() );
    }
}


void WebSocketChannel::setInputError(std::exception_ptr error)
{
    if(_ended || _inputError)
        return;

    _inputError = error;

    if( isActionPending(ReceiveAction) )
        _inputReady.send();
}


void WebSocketChannel::pumpOutput()
{
    if(_ended || _outputState != OutputIdle)
        return;

    Stream* stream = this->stream();
    if( ! stream )
        return;

    if( ! _controlQueue.empty() )
    {
        const PendingControl& frame = _controlQueue.front();
        writeFrame(frame.opcode, true, frame.payload, frame.size);

        if(frame.opcode == 0x09)
            _unansweredPings.push_back( std::string(frame.payload, frame.size) );

        _controlQueue.pop_front();
        _outputOpcode = frame.opcode;
        _outputState = OnControlOutput;
        stream->beginOutput();
        return;
    }

    if( ! isActionPending(SendAction) || _dataFinWritten )
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
    _outputState = OnDataOutput;
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
    if(_ended || _inputProcessing || _inputError)
        return;

    _inputProcessing = true;
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
            if( ! isActionPending(ReceiveAction) || _discardingInput )
            {
                _wsbuf.discard();
                _discardingInput = ! finished;
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

            _inputReady.send();

            if( isActionPending(ReceiveAction) && _wsbuf.in_avail() != 0)
                break;

            if( isActionPending(ReceiveAction) )
                continue;

            if(_wsbuf.in_avail() != 0)
            {
                _wsbuf.discard();
                _discardingInput = ! finished;
            }
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

    _inputProcessing = false;

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
        if( ! _unansweredPings.empty() && _unansweredPings.front() == payload )
            _unansweredPings.pop_front();

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

    if(_closeState == CloseExchanged && _outputState == OutputIdle)
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
    if( ! stream || _ended )
        return;

    try
    {
        _inputState = InputIdle;

        if( stream->endInput() == 0 )
        {
            passiveClose();
            return;
        }

        processInput();
    }
    catch(...)
    {
        if(_ended)
            return;

        _inputState = InputIdle;

        if(_closeCode == 0)
            _closeCode = 1006;

        setInputError( std::current_exception() );
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

        if(_outputState == OnControlOutput)
        {
            unsigned opcode = _outputOpcode;
            _outputState = OutputIdle;
            _outputOpcode = 0;

            if(opcode == 0x08)
                setCloseSent();

            if(_closeState == CloseExchanged)
            {
                passiveClose();
                return;
            }

            pumpOutput();
            return;
        }

        if(_outputState == OnDataOutput)
        {
            _sendProgress.setBody();
            if(_dataFinWritten && _outgoing.sendSize() == 0)
                _sendProgress.setFinished();

            _outputState = OnEndSend;
            _outputReady.send();
            return;
        }

        _outputState = OutputIdle;
        pumpOutput();
    }
    catch(...)
    {
        if(_ended)
            return;

        if(_closeCode == 0)
            _closeCode = 1006;

        _outputError = std::current_exception();

        if( isActionPending(SendAction) )
            _outputReady.send();
    }
}

} // namespace Http

} // namespace Pt
