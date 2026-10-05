/* Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_WEBSOCKETCONNECTION_H
#define PT_HTTP_WEBSOCKETCONNECTION_H

#include <Pt/Http/StreamSession.h>
#include <Pt/Connectable.h>
#include <Pt/Signal.h>
#include <Pt/System/Timer.h>
#include <vector>
#include <iostream>
#include <cstddef>

namespace Pt {

namespace Http {

class Stream;

/** @internal Frame engine of one upgraded HTTP stream.
*/
class WebSocketConnection : public StreamSession
                          , public Connectable
{
    public:
        enum Frame
        {
            Unknown,
            Text,
            Binary,
            Ping,
            Pong,
            Close
        };

        WebSocketConnection();

        ~WebSocketConnection();

        void open(Stream& stream, bool clientMask);

        void close();

        std::iostream& body()
        { return _body; }

        std::size_t available() const;

        std::size_t pending() const;

        void discard();

        Frame frame() const
        { return _frame; }

        void beginSend(Frame frame);

        void endSend();

        void beginReceive();

        void endReceive();

        void sendPing();

        void sendPong();

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
        Pt::uint32_t createMask();

        void writeFrame(Frame frame, const char* payload, std::size_t n);

        void beginFrameRead();

        bool parseAvailable();

        void failStream();

        void onInput();

        void onOutput();

        void onIdleTimeout();

    private:
        enum State
        {
            Idle,
            ReceiveHeader,
            ReceiveLength,
            ReceiveMask,
            ReceivePayload,
            Sending
        };

        class PayloadBuffer;

        bool _clientMask;
        std::size_t _timeout;
        std::size_t _maxMessageSize;
        std::size_t _idleTimeout;
        System::Timer _idleTimer;
        bool _error;
        State _state;
        Frame _frame;
        bool _masked;
        Pt::uint32_t _mask;
        std::size_t _payloadSize;
        std::size_t _payloadGot;
        std::size_t _headerNeed;
        std::vector<char> _header;
        std::vector<char> _payload;
        PayloadBuffer* _payloadBuffer;
        std::iostream _body;
        Signal<> _inputReady;
        Signal<> _outputReady;
        Signal<> _closed;
};

} // namespace Http

} // namespace Pt

#endif
