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
        { return _hasControl; }

        unsigned controlOpcode() const
        { return _controlOpcode; }

        const char* controlData() const
        { return _controlSize == 0 ? 0 : _control; }

        std::size_t controlSize() const
        { return _controlSize; }

        void consumeControl();

    protected:
        virtual int_type underflow();

        virtual std::streamsize showmanyc();

    private:
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

        bool isControl(unsigned opcode) const;

        unsigned headerOpcode() const;

        bool headerFin() const;

        bool headerMasked() const;

        unsigned headerRsv() const;

        std::size_t headerLen7() const;

        std::size_t payloadLength();

    private:
        static const std::size_t WindowSize = 4096;

        std::streambuf* _sb;
        State _state;
        bool _masking;
        std::size_t _maxMessageSize;
        std::size_t _maxRead;
        std::size_t _bytesRead;

        char _hdr[14];
        std::size_t _hdrSize;
        std::size_t _hdrAvail;

        char _window[WindowSize];

        char _control[125];
        std::size_t _controlSize;
        std::size_t _controlGot;
        bool _hasControl;
        unsigned _controlOpcode;

        bool _headerSeen;
        unsigned _opcode;
        bool _finished;
        unsigned _failCode;

        bool _messageOpen;
        std::size_t _messageSize;
        unsigned _utf8Need;
        unsigned char _utf8Lead;
        bool _utf8Error;

        bool _frameFin;
        bool _frameMasked;
        unsigned _frameOpcode;
        char _mask[4];
        std::size_t _payloadSize;
        std::size_t _payloadGot;
};

} // namespace Http

} // namespace Pt

#endif
