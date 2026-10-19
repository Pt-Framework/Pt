/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_WEBSOCKETBUFFER_H
#define PT_HTTP_WEBSOCKETBUFFER_H

#include <Pt/Http/Api.h>
#include <Pt/Types.h>
#include <streambuf>
#include <stdexcept>
#include <cstddef>

namespace Pt {

namespace Http {

/** @internal Result of one `WebSocketBuffer::import` step.

    `import` always returns a pointer to one of two members it owns,
    `DataFrame` or `ControlFrame`, valid only until the next call. `size` is
    the current wire frame's total payload length for both kinds; `avail` is
    how many bytes starting at `data` are valid right now, which for an
    in-progress data frame is less than `size`.
*/
class PT_HTTP_API Frame
{
    public:
        enum Kind { Data, Control };

        Kind kind() const
        { return _kind; }

        unsigned opcode() const
        { return _opcode; }

        bool fin() const
        { return _fin; }

        const char* data() const
        { return _data; }

        std::size_t size() const
        { return _size; }

        std::size_t avail() const
        { return _avail; }

        std::size_t pos() const
        { return _pos; }

        std::size_t remaining() const
        { return _size - _pos; }

        void advance(std::size_t n)
        { _pos += n; }

        void setOpcode(unsigned opcode)
        { _opcode = opcode; }

        void setFin(bool fin)
        { _fin = fin; }

        void setSize(std::size_t size)
        {
            _size = size;
            _avail = 0;
            _pos = 0;
        }

        void setAvail(const char* data, std::size_t avail)
        {
            _data = data;
            _avail = avail;
        }

        void set(unsigned opcode, bool fin, const char* data, std::size_t avail);

    protected:
        explicit Frame(Kind kind);

    private:
        unsigned _opcode;
        bool _fin;
        const char* _data;
        std::size_t _size;
        std::size_t _avail;
        std::size_t _pos;
        Kind _kind;
};


/** @internal A data frame; its buffer backs the stream buffer's get area.

    `data`/`avail` are finalized once per `import` to the get area's current
    extent; `size` stays the total declared length of the wire frame.
*/
class PT_HTTP_API DataFrame : public Frame
{
    public:
        DataFrame();

        /** @brief Returns the raw storage later exposed read-only via `data()`/`avail()`.
        */
        char* buffer()
        { return _buffer; }

        static const std::size_t BufferSize = 4096;

    private:
        char _buffer[BufferSize];
};


/** @internal A complete control frame, buffered since it is never streamed.
*/
class PT_HTTP_API ControlFrame : public Frame
{
    public:
        ControlFrame();

        char* at(std::size_t pos)
        { return _bytes + pos; }

    private:
        char _bytes[125];
};


/** @internal A WebSocket protocol violation, carrying the close code to send.
*/
class PT_HTTP_API WebSocketError : public std::runtime_error
{
    public:
        explicit WebSocketError(unsigned code);

        unsigned code() const
        { return _code; }

    private:
        unsigned _code;
};


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

        const Frame* import(std::streamsize n = 0);

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


        enum State
        {
            NeedHeader,
            NeedLength,
            NeedMask,
            NeedPayload
        };

        void failThrow(unsigned code);

        void beginNextMessage();

        void compact();

        std::size_t unused() const;

        std::size_t take(std::size_t want);

        std::size_t readBytes(char* dst, std::size_t n);

        void onHeader();

        void onLength();

        void onMask();

        void beginPayload();

        void unmaskPayload(char* p, std::size_t n, std::size_t pos);

        void onPayload();

        void finishFrame();

    private:
        std::streambuf* _sb;
        State _state;
        bool _masking;
        std::size_t _maxMessageSize;
        std::size_t _maxRead;
        std::size_t _bytesRead;

        Header _hdr;
        DataFrame _dataResult;
        ControlFrame _controlResult;
        bool _dataProgress;
        bool _controlReady;

        unsigned _messageOpcode;
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
