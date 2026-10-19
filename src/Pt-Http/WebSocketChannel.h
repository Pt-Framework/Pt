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

        void open(Stream& stream, bool clientMask);

        WebSocketMessage& incoming()
        { return _incoming; }

        WebSocketMessage& outgoing()
        { return _outgoing; }

        void beginSend();

        MessageProgress endSend();

        void beginReceive();

        MessageProgress endReceive();

        void ping(const char* payload, std::size_t n);

        void shutdown(unsigned code, const std::string& reason);

        unsigned closeCode() const
        { return _closeCode; }

        const std::string& closeReason() const
        { return _closeReason; }

        Signal<>& inputReady()
        { return _inputReady; }

        Signal<>& outputReady()
        { return _outputReady; }

        Signal<>& closed()
        { return _closed; }

        void setTimeout(std::size_t timeout);

        void setMaxMessageSize(std::size_t maxSize);

        void setIdleTimeout(std::size_t ms);

    protected:
        virtual void onClose();

    private:
        struct PendingFrame
        {
            unsigned opcode;
            bool fin;
            bool data;
            char payload[125];
            std::size_t size;
        };

        enum OperationState
        {
            Idle,
            Pending,
            Ready,
            Failed
        };

        enum CloseState
        {
            CloseOpen,
            CloseQueued,
            CloseSent,
            CloseReceived,
            CloseReplyQueued,
            CloseExchanged
        };

        void requireOpen() const;

        void requireNotEnded() const;

        void setCloseSent();

        void setCloseReceived();

        bool closeNeedsInput() const;

        void enqueueControl(unsigned opcode, const char* payload, std::size_t n);

        void enqueueClose(unsigned code, const std::string& reason);

        void protocolFail(unsigned code);

        void requestInput();

        void processInput();

        void setInputError(std::exception_ptr error);

        void pumpOutput();

        void writeFrame(unsigned opcode, bool fin, const char* payload, std::size_t n);

        Pt::uint32_t nextMask();

        bool onControlFrame(const Frame& frame);

        void restartIdleTimer();

        void onInput();

        void onOutput();

        void onIdleTimeout();

        void passiveClose();

    private:
        bool _clientMask;
        bool _opened;
        bool _ended;
        std::size_t _timeout;
        std::size_t _maxMessageSize;
        std::size_t _idleTimeout;
        System::Timer _idleTimer;
        Pt::uint32_t _maskSeed;

        WebSocketMessage _incoming;
        WebSocketMessage _outgoing;
        WebSocketBuffer _wsbuf;

        OperationState _inputState;
        OperationState _receiveState;
        bool _messageOpen;
        MessageProgress _receiveProgress;

        OperationState _outputState;
        OperationState _sendState;
        bool _sendStarted;
        bool _firstFragment;
        bool _dataFinWritten;
        MessageProgress _sendProgress;

        std::deque<PendingFrame> _outputQueue;
        std::deque<std::string> _unansweredPings;
        CloseState _closeState;
        unsigned _closeCode;
        std::string _closeReason;
        std::exception_ptr _inputError;
        std::exception_ptr _outputError;

        Signal<> _inputReady;
        Signal<> _outputReady;
        Signal<> _closed;
};

} // namespace Http

} // namespace Pt

#endif
