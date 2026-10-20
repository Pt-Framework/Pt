/* Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_WEBSOCKETCHANNEL_H
#define PT_HTTP_WEBSOCKETCHANNEL_H

#include <Pt/Http/Channel.h>
#include <Pt/Http/WebSocketMessage.h>
#include <Pt/Http/Message.h>
#include "WebSocketBuffer.h"
#include <Pt/Connectable.h>
#include <Pt/Signal.h>
#include <Pt/System/Timer.h>
#include <deque>
#include <exception>
#include <string>
#include <cstddef>

namespace Pt {

namespace Http {

class Stream;

/** @internal Message engine of one upgraded HTTP stream.
*/
class WebSocketChannel : public Channel
                       , public Connectable
{
    public:
        WebSocketChannel();

        ~WebSocketChannel();

        void setTimeout(std::size_t timeout);

        void setMaxMessageSize(std::size_t maxSize);

        void setIdleTimeout(std::size_t ms);

        void open(Stream& stream, bool clientMask);

        WebSocketMessage& incoming()
        { return _incoming; }

        WebSocketMessage& outgoing()
        { return _outgoing; }

        void beginSend();

        MessageProgress endSend();

        void beginReceive();

        MessageProgress endReceive();

        void beginPing(const char* payload, std::size_t n);

        void endPing();

        void beginShutdown(unsigned code, const std::string& reason);

        void endShutdown();

        unsigned closeCode() const
        { return _closeCode; }

        const std::string& closeReason() const
        { return _closeReason; }

        Signal<>& inputReady()
        { return _inputReady; }

        Signal<>& outputReady()
        { return _outputReady; }

        Signal<const std::string&>& pong()
        { return _pong; }

        Signal<>& pingReady()
        { return _pingReady; }

        Signal<>& shutdownReady()
        { return _shutdownReady; }

        Signal<>& closed()
        { return _closed; }

    protected:
        virtual void onClose();

    private:
        struct PendingFrame
        {
            enum Kind
            {
                Data,
                Ping,
                Pong,
                Close,
                Shutdown
            };

            unsigned opcode;
            bool fin;
            Kind kind;
            char payload[125];
            std::size_t size;
        };

        enum OperationState
        {
            Idle,
            Pending,
            Ready
        };

        enum CloseState
        {
            CloseOpen,
            CloseQueued,
            CloseSent,
            CloseReplyQueued,
            CloseExchanged
        };

    private:
        void requestInput();

        void onInput();

        void onInputFailed(std::exception_ptr error);

        bool onProcessInput();

        bool onProcessControl(const Frame& frame);

        bool onProcessData(const Frame& frame);

    private:
        void requestOutput();

        void onOutput();

        void onOutputFailed(std::exception_ptr error);

        void onProcessOutput();

        void writeFrame(unsigned opcode, bool fin,
                        const char* payload, std::size_t n);

    private:
        void restartIdleTimer();

        void onIdleTimeout();

        void passiveClose();

        void finishShutdown();

    private:
        void enqueueData();

        void enqueueControl(unsigned opcode, PendingFrame::Kind kind,
                            const char* payload, std::size_t n);

        void enqueueClose(unsigned code, const std::string& reason,
                          PendingFrame::Kind kind = PendingFrame::Close,
                          bool reply = false);

        void requireOpen() const;

        bool awaitingCloseReply() const;

        void protocolFail(unsigned code);

        Pt::uint32_t nextMask();

    private:
        bool          _clientMask;
        std::size_t   _timeout;
        std::size_t   _maxMessageSize;
        std::size_t   _idleTimeout;
        System::Timer _idleTimer;
        Pt::uint32_t  _maskSeed;

        WebSocketMessage _incoming;
        WebSocketMessage _outgoing;
        WebSocketBuffer  _wsbuf;

        std::deque<PendingFrame> _outputQueue;

        OperationState     _inputState;
        OperationState     _receiveState;
        bool               _messageOpen;
        MessageProgress    _receiveProgress;
        std::exception_ptr _inputError;
        Signal<>           _inputReady;

        OperationState     _outputState;
        OperationState     _sendState;
        std::size_t        _sendFragment;
        MessageProgress    _sendProgress;
        std::exception_ptr _outputError;
        Signal<>           _outputReady;

        OperationState     _pingState;
        std::exception_ptr _pingError;
        Signal<>           _pingReady;

        std::string _pongPayload;
        Signal<const std::string&> _pong;

        OperationState     _shutdownState;
        std::exception_ptr _shutdownError;
        Signal<>           _shutdownReady;

        CloseState  _closeState;
        unsigned    _closeCode;
        std::string _closeReason;
        Signal<>    _closed;
};

} // namespace Http

} // namespace Pt

#endif
