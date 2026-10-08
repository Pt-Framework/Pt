/* Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_WEBSOCKETCHANNEL_H
#define PT_HTTP_WEBSOCKETCHANNEL_H

#include <Pt/Http/Channel.h>
#include <Pt/Http/WebSocketMessage.h>
#include <Pt/Http/Message.h>
#include <Pt/Connectable.h>
#include <Pt/Signal.h>
#include <Pt/System/Timer.h>
#include <deque>
#include <string>
#include <vector>
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

        void detach();

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
        virtual void onCloseStream(Stream& stream);

    private:
        struct ControlFrame
        {
            unsigned opcode;
            char payload[125];
            std::size_t size;
        };

        enum InputState
        {
            InputIdle,
            InputHeader,
            InputLength,
            InputMask,
            InputPayload
        };

        enum OutputState
        {
            OutputIdle,
            OutputWriting
        };

        enum OutputKind
        {
            OutputNone,
            OutputData,
            OutputControl
        };

        void requireOpen() const;

        void requireNotEnded() const;

        void enqueueControl(unsigned opcode, const char* payload, std::size_t n);

        void enqueueClose(unsigned code, const std::string& reason);

        void protocolFail(unsigned code);

        void beginInputPump();

        void pumpOutput();

        void writeFrame(unsigned opcode, bool fin, const char* payload, std::size_t n);

        Pt::uint32_t nextMask();

        bool parseAvailable();

        bool onDataFrameComplete();

        bool onControlFrame();

        void restartIdleTimer();

        void onInput();

        void onOutput();

        void onIdleTimeout();

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

        InputState _inputState;
        bool _receiveOutstanding;
        bool _messageOpen;
        bool _frameFin;
        bool _frameMasked;
        unsigned _frameOpcode;
        unsigned _messageOpcode;
        Pt::uint32_t _frameMask;
        std::size_t _payloadSize;
        std::size_t _payloadGot;
        std::size_t _headerNeed;
        std::size_t _messageSize;
        std::vector<char> _header;
        std::vector<char> _controlPayload;
        unsigned _utf8Need;
        unsigned char _utf8Lead;
        bool _utf8Error;
        MessageProgress _receiveProgress;

        OutputState _outputState;
        OutputKind _outputKind;
        bool _sendOutstanding;
        bool _sendStarted;
        bool _firstFragment;
        bool _dataFinWritten;
        bool _awaitingEndSend;
        MessageProgress _sendProgress;

        std::deque<ControlFrame> _controlQueue;
        std::deque<std::string> _unansweredPings;
        bool _closeQueued;
        bool _closeSent;
        bool _closeReceived;
        unsigned _closeCode;
        std::string _closeReason;

        Signal<> _inputReady;
        Signal<> _outputReady;
        Signal<> _closed;
};

} // namespace Http

} // namespace Pt

#endif
