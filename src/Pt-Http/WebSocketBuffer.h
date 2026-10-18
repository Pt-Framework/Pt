/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_WEBSOCKETBUFFER_H
#define PT_HTTP_WEBSOCKETBUFFER_H

#include <Pt/Http/Api.h>
#include <Pt/Types.h>
#include <streambuf>
#include <cstddef>

namespace Pt {

namespace Http {

/** @internal Parses WebSocket frames as the body of one data message.

    Data payload is the get area. Control frames are a side channel.
*/
class PT_HTTP_API WebSocketBuffer : public std::streambuf
{
    public:
        WebSocketBuffer();

        explicit WebSocketBuffer(std::streambuf& sb);

        ~WebSocketBuffer();

        WebSocketBuffer(const WebSocketBuffer&) = delete;

        WebSocketBuffer& operator=(const WebSocketBuffer&) = delete;

        void attach(std::streambuf& sb);

        void reset();

        void reset(std::streambuf& sb);

        void setMaxMessageSize(std::size_t n);

        void setMasking(bool masked);

        std::streamsize import(std::streamsize n = 0);

        bool header() const
        { return _headerSeen; }

        unsigned opcode() const
        { return _opcode; }

        bool finished() const
        { return _finished; }

        unsigned failCode() const
        { return _failCode; }

        bool hasControl() const
        { return _control.complete(); }

        unsigned controlOpcode() const
        { return _control.opcode(); }

        const char* controlData() const
        { return _control.data(); }

        std::size_t controlSize() const
        { return _control.size(); }

        void consumeControl();

    protected:
        virtual int_type underflow();

        virtual std::streamsize showmanyc();

    private:
        class Header
        {
            public:
                Header();

                void clear();

                void resize(std::size_t n);

                char* at(std::size_t pos)
                { return _bytes + pos; }

                std::size_t size() const
                { return _size; }

                std::size_t avail() const
                { return _avail; }

                void setAvail(std::size_t n)
                { _avail = n; }

                std::size_t remaining() const
                { return _size - _avail; }

                bool fin() const;

                unsigned opcode() const;

                bool isControl() const;

                unsigned rsv() const;

                bool masked() const;

                unsigned len7() const;

                unsigned payloadLength(std::size_t& n) const;

                void mask(char out[4]) const;

                static bool isControl(unsigned opcode);

            private:
                char _bytes[14];
                std::size_t _size;
                std::size_t _avail;
        };


        class Control
        {
            public:
                Control();

                void clear();

                void begin(unsigned opcode, std::size_t n);

                bool complete() const
                { return _complete; }

                void setComplete()
                { _complete = true; }

                unsigned opcode() const
                { return _opcode; }

                char* at(std::size_t pos)
                { return _bytes + pos; }

                const char* data() const
                { return _size == 0 ? 0 : _bytes; }

                std::size_t size() const
                { return _size; }

                std::size_t got() const
                { return _got; }

                void addGot(std::size_t n)
                { _got += n; }

            private:
                char _bytes[125];
                std::size_t _size;
                std::size_t _got;
                unsigned _opcode;
                bool _complete;
        };


        class Frame
        {
            public:
                Frame();

                void clear();

                void begin(const Header& header, std::size_t payload);

                bool fin() const
                { return _fin; }

                unsigned opcode() const
                { return _opcode; }

                bool isControl() const
                { return Header::isControl(_opcode); }

                bool masked() const
                { return _masked; }

                std::size_t payloadSize() const
                { return _payloadSize; }

                std::size_t payloadGot() const
                { return _payloadGot; }

                std::size_t remaining() const
                { return _payloadSize - _payloadGot; }

                void addGot(std::size_t n)
                { _payloadGot += n; }

                void unmask(char* p, std::size_t n) const;

            private:
                bool _fin;
                bool _masked;
                unsigned _opcode;
                char _mask[4];
                std::size_t _payloadSize;
                std::size_t _payloadGot;
        };

        enum State
        {
            NeedHeader,
            NeedLength,
            NeedMask,
            NeedPayload
        };

        void fail(unsigned code);

        void beginNextMessage();

        void compact();

        std::size_t unused() const;

        std::size_t take(std::size_t want);

        bool readBytes(char* dst, std::size_t n, std::size_t& got);

        void onHeader();

        void onLength();

        void onMask();

        void beginPayload();

        void onPayload();

        void finishFrame();

    private:
        static const std::size_t WindowSize = 4096;

        std::streambuf* _sb;
        State _state;
        bool _masking;
        std::size_t _maxMessageSize;
        std::size_t _maxRead;
        std::size_t _bytesRead;

        Header _hdr;
        Control _control;
        Frame _frame;
        char _window[WindowSize];

        bool _headerSeen;
        unsigned _opcode;
        bool _finished;
        unsigned _failCode;

        bool _messageOpen;
        std::size_t _messageSize;
        unsigned _utf8Need;
        unsigned char _utf8Lead;
        bool _utf8Error;
};

} // namespace Http

} // namespace Pt

#endif
