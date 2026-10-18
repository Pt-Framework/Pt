/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_FRAMEREADER_H
#define PT_HTTP_FRAMEREADER_H

#include <Pt/Http/Api.h>
#include <Pt/Types.h>
#include <streambuf>
#include <vector>
#include <cstddef>

namespace Pt {

namespace Http {

/** @internal One parsed WebSocket frame.

    The header is part of the frame. size() is the declared payload
    length. avail() is how much of it has been written.
*/
class PT_HTTP_API Frame
{
    public:
        enum Type
        {
            None,
            Data,
            Control
        };

        // At most 14 bytes: 2 base, 0/2/8 length, 0/4 masking key.
        // size() is the target. avail() is how much is in.
        class PT_HTTP_API Header
        {
            public:
                Header();

                void clear();

                void resize(std::size_t n);

                char* at(std::size_t pos);

                const char* data() const
                { return _bytes; }

                std::size_t size() const
                { return _size; }

                std::size_t avail() const
                { return _avail; }

                void setAvail(std::size_t n)
                { _avail = n; }

                bool fin() const;

                unsigned opcode() const;

                unsigned rsv() const;

                bool masked() const;

                std::size_t payloadLength() const;

                Pt::uint32_t maskKey() const;

            private:
                char _bytes[14];
                std::size_t _size;
                std::size_t _avail;
        };

    public:
        Frame();

        Type type() const
        { return _type; }

        unsigned opcode() const;

        bool fin() const;

        const char* data() const
        { return _avail == 0 ? 0 : &_bytes[0]; }

        std::size_t size() const
        { return _bytes.size(); }

        std::size_t avail() const
        { return _avail; }

        bool full() const
        { return _avail >= _bytes.size(); }

        Header& header()
        { return _header; }

        const Header& header() const
        { return _header; }

        void clear();

        void reset(Type type, std::size_t n);

        char* at(std::size_t pos);

        void setAvail(std::size_t n)
        { _avail = n; }

        void unmask();

    private:
        Type _type;
        std::size_t _avail;
        std::vector<char> _bytes;
        Header _header;
};

/** @internal Parses WebSocket frames from a streambuf.

    get() returns the current frame. next() reads until the next
    frame is complete and may block. advance() reads at most @a n
    bytes, and only what in_avail() reports. Zero means no byte limit.
*/
class PT_HTTP_API FrameReader
{
    public:
        FrameReader();

        explicit FrameReader(std::streambuf& sb);

        ~FrameReader();

        FrameReader(const FrameReader&) = delete;

        FrameReader& operator=(const FrameReader&) = delete;

        void attach(std::streambuf& sb);

        void reset();

        void reset(std::streambuf& sb);

        void setMaxMessageSize(std::size_t n);

        void setMasking(bool masked);

        Frame& get();

        Frame& next();

        Frame* advance(std::size_t n = 0);

    private:
        enum State
        {
            OnHeader,
            OnLength,
            OnMask,
            OnPayload
        };

        void onHeader();

        void onLength();

        void onMask();

        void onPayload();

        std::streambuf* _sb;
        State _state;
        bool _masking;
        std::size_t _maxMessageSize;
        std::size_t _maxRead;
        std::size_t _bytesRead;

        bool _current;
        Frame _frame;

        bool _messageOpen;
        unsigned _messageOpcode;
        std::size_t _messageSize;
        unsigned _utf8Need;
        unsigned char _utf8Lead;
        bool _utf8Error;
};

} // namespace Http

} // namespace Pt

#endif
