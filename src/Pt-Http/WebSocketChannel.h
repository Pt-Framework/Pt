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
                Close
            };

            unsigned opcode;
            bool fin;
            Kind kind;
            char payload[125];
            std::size_t size;
        };

        class Operation
        {
            public:
                Operation()
                : _state(Idle)
                { }

                bool isIdle() const
                { return _state == Idle; }

                bool isPending() const
                { return _state == Pending; }

                bool isReady() const
                { return _state == Ready; }

                void begin()
                {
                    _state = Pending;
                    _error = std::exception_ptr();
                }

                void complete(std::exception_ptr error)
                {
                    _state = Ready;
                    _error = error;
                }

                void finish()
                {
                    _state = Idle;

                    if(_error)
                    {
                        std::exception_ptr error = _error;
                        _error = std::exception_ptr();
                        std::rethrow_exception(error);
                    }
                }

                void reset()
                {
                    _state = Idle;
                    _error = std::exception_ptr();
                }

                // A ready result stays; the application may not have read it yet.
                void cancel()
                {
                    if(_state == Pending)
                        reset();
                }

            private:
                enum State
                {
                    Idle,
                    Pending,
                    Ready
                };

                State              _state;
                std::exception_ptr _error;
        };

        enum IOState
        {
            IOIdle,
            IOPending,
            IOImporting,
            IOFailed
        };

        enum PendingInput
        {
            NoInput           = 0,
            PendingReceive    = 1 << 0,
            PendingCloseReply = 1 << 1
        };

        enum CloseState
        {
            CloseOpen,
            CloseSent,
            CloseReceived,
            CloseFailing,
            CloseExchanged
        };

    private:
        void requestInput();

        void onInput();

        bool onProcessInput(const Frame& frame);

        void onInputFailed(std::exception_ptr error);

        bool onProcessControl(const Frame& frame);

        bool onProcessData(const Frame& frame);

    private:
        void requestOutput();

        void onOutput();

        void onProcessOutput(std::exception_ptr error);

        void onDataWritten(const PendingFrame& frame, std::exception_ptr error);

        void onPingWritten(const PendingFrame& frame, std::exception_ptr error);

        void onCloseWritten(const PendingFrame& frame, std::exception_ptr error);

        void writeFrame(unsigned opcode, bool fin,
                        const char* payload, std::size_t n);

    private:
        void completeSend(std::exception_ptr error);

        void completeReceive(std::exception_ptr error);

        void completePing(std::exception_ptr error);

        void completeShutdown(std::exception_ptr error);

    private:
        void resetState();

        void armIdleTimer();

        void restartIdleTimer();

        void onIdleTimeout();

        void setCloseState(CloseState state);

        void setCloseCode(unsigned code, const std::string& reason);

        void finishClose();

        void passiveClose();

    private:
        void enqueueData();

        void enqueueControl(unsigned opcode, PendingFrame::Kind kind,
                            const char* payload, std::size_t n);

        void requestClose(unsigned code, const std::string& reason);

        void requireOpen() const;

        void addPendingInput(PendingInput pending);

        void removePendingInput(PendingInput pending);

        bool hasPendingInput() const;

        bool hasPendingInput(PendingInput pending) const;

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

        IOState            _inputState;
        unsigned           _pendingInput;
        bool               _messageOpen;
        Operation          _receive;
        MessageProgress    _receiveProgress;
        std::exception_ptr _inputFailure;
        Signal<>           _inputReady;

        IOState            _outputState;
        Operation          _send;
        bool               _sendContinuation;
        MessageProgress    _sendProgress;
        std::exception_ptr _outputFailure;
        Signal<>           _outputReady;

        Operation _ping;
        Signal<>  _pingReady;

        std::string _pongPayload;
        Signal<const std::string&> _pong;

        Operation _shutdown;
        Signal<>  _shutdownReady;

        CloseState  _closeState;
        unsigned    _closeCode;
        std::string _closeReason;
        Signal<>    _closed;
};

} // namespace Http

} // namespace Pt

#endif
