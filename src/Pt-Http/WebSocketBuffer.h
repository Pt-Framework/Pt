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

    `import` returns a pointer to a member it owns, valid only until the next
    call. `opcode` and `fin` are the current wire frame's values. `size` is
    its total payload length, `pos` is the imported payload length, and
    `avail` is the current get area's length starting at `data`.
*/
class PT_HTTP_API Frame
{
    public:
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

    public:
        enum Kind { Data, Control };

        Frame();

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

        void setKind(Kind kind)
        { _kind = kind; }

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

        char* buffer()
        { return _buffer; }

        char* at(std::size_t pos)
        { return _buffer + pos; }

        static const std::size_t BufferSize = 4096;

        Header& header()
        { return _header; }

    private:
        unsigned _opcode;
        bool _fin;
        const char* _data;
        std::size_t _size;
        std::size_t _avail;
        std::size_t _pos;
        Kind _kind;
        char _buffer[BufferSize];
        Header _header;
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

/** @internal Pull parser for WebSocket frames.

    A data result exposes the current frame's payload through the get area. A
    control result is returned only after its complete payload is buffered.
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
        enum State
        {
            OnHeader,
            OnLength,
            OnMask,
            OnPayload,
            OnDataPayload,
            OnControlPayload
        };

        State onHeader();

        State onLength();

        State onMask();

        State onPayload();

        State onDataPayload();

        State onControlPayload();

    private:
        void fail(unsigned code);

        std::size_t readBytes(char* dst, std::size_t n);

        void unmaskPayload(char* p, std::size_t n, std::size_t pos);

    private:
        std::streambuf* _sb;
        State           _state;
        bool            _masking;
        std::size_t     _maxMessageSize;
        std::size_t     _importRemaining;

        Frame         _frame;
        bool          _frameReady;

        unsigned      _messageOpcode;
        unsigned      _failCode;
        std::size_t   _messageSize;

        unsigned      _utf8Need;
        unsigned char _utf8Lead;
};

} // namespace

} // namespace

#endif
