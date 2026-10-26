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
#include <Pt/IOError.h>
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
        { return _send.message(); }

        void beginSend(bool finished);

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
        { return _receive.ready(); }

        Signal<>& outputReady()
        { return _send.ready(); }

        Signal<const std::string&>& pong()
        { return _pong; }

        Signal<>& pingReady()
        { return _ping.ready(); }

        Signal<>& shutdownReady()
        { return _shutdown.ready(); }

    protected:
        virtual void onClose();

    private:
        class Operation
        {
            public:
                Operation()
                : _state(Closed)
                { }

                bool isIdle() const
                { return _state == Idle; }

                bool isPending() const
                { return _state == Pending; }

                bool isReady() const
                { return _state == Ready; }

                bool isClosed() const
                { return _state == Closed; }

                Signal<>& ready()
                { return _ready; }

                void open()
                {
                    _state = Idle;
                    _error = std::exception_ptr();
                }

                void reset()
                {
                    _state = Idle;
                    _error = std::exception_ptr();
                }

                void close()
                {
                    _state = Closed;
                    _error = std::exception_ptr();
                }

                void notify()
                {
                    _ready.send();
                }

                void begin()
                {
                    _state = Pending;
                    _error = std::exception_ptr();
                }

                bool setReady()
                {
                    if(_state != Pending)
                        return false;

                    _state = Ready;
                    _ready.send();
                    return true;
                }

                bool setFailed(std::exception_ptr error)
                {
                    if(_state != Pending)
                        return false;

                    _error = error;
                    return setReady();
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

            private:
                enum State
                {
                    Closed,
                    Idle,
                    Pending,
                    Ready
                };

                Signal<>           _ready;
                State              _state;
                std::exception_ptr _error;
        };

        class Output : public Operation
        {
            public:
                Output();

                WebSocketMessage& message()
                { return _message; }

                unsigned opcode() const
                { return _opcode; }

                bool fin() const
                { return _fin; }

                std::size_t size() const
                { return _size; }

                void set(unsigned opcode, bool fin);

                void set(unsigned opcode, const char* data, std::size_t n);

                void discard();

                void reset()
                {
                    Operation::reset();
                    _message.clear();
                    _opcode = 0;
                    _fin = true;
                    _size = 0;
                }

                void close()
                {
                    Operation::close();
                    _message.clear();
                    _opcode = 0;
                    _fin = true;
                    _size = 0;
                }

            private:
                WebSocketMessage _message;
                unsigned         _opcode;
                bool             _fin;
                std::size_t      _size;
        };

        enum CloseState
        {
            CloseOpen,
            CloseSent,
            CloseFinished
        };

    private:
        void requestInput();

        void onInput();

        bool onFrame(const Frame& frame);

        bool onPing(const Frame& frame);

        bool onPong(const Frame& frame);

        bool onPeerClose(const Frame& frame);

        bool onData(const Frame& frame);

        void onProtocolError(std::exception_ptr error, unsigned code);

        void onInputError(std::exception_ptr error);

    private:
        void enqueue(Output* out);

        void enqueueClose(unsigned code, const std::string& reason);

        void requestOutput();

        void onOutput();

        void onOutputError(std::exception_ptr error);

        void writeFrame(unsigned opcode, bool fin,
                        const char* payload, std::size_t n);

    private:
        void takeClosePayload(const Frame& frame);

        unsigned echoableCloseCode(unsigned code) const;

        void onCloseFinished();

        void onPassiveClose();

        void notifyClosed(Operation& op, bool pending);

        void setCloseCode(unsigned code, const std::string& reason);

    private:
        void armIdleTimer();

        void restartIdleTimer();

        void onIdleTimeout();

        void requireOpen() const;

    private:
        bool          _clientMask;
        std::size_t   _timeout;
        std::size_t   _maxMessageSize;
        std::size_t   _idleTimeout;
        System::Timer _idleTimer;
        Pt::uint32_t  _maskSeed;

        WebSocketMessage _incoming;
        WebSocketBuffer  _wsbuf;

        std::deque<Output*> _outputQueue;

        bool               _inputPending;
        Operation          _receive;
        MessageProgress    _receiveProgress;
        std::exception_ptr _inputFailure;

        bool               _outputPending;
        Output             _send;
        MessageProgress    _sendProgress;
        std::exception_ptr _outputFailure;

        Output             _ping;

        Output                     _pongOut;
        std::string                _pongPayload;
        Signal<const std::string&> _pong;

        Output    _closeOut;
        Operation _shutdown;

        CloseState  _closeState;
        unsigned    _closeCode;
        std::string _closeReason;
};

} // namespace Http

} // namespace Pt

#endif
