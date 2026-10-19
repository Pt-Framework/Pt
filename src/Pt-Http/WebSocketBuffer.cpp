/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include "WebSocketBuffer.h"
#include <Pt/Byteorder.h>
#include <cstring>
#include <limits>

namespace Pt {

namespace Http {

namespace {

bool feedUtf8(unsigned& need, unsigned char& lead, const char* data, std::size_t n)
{
    const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
    for(std::size_t i = 0; i < n; ++i)
    {
        unsigned char c = p[i];
        if(need == 0)
        {
            lead = c;
            if(c <= 0x7F)
                continue;

            if(c >= 0xC2 && c <= 0xDF)
            {
                need = 1;
                continue;
            }

            if(c >= 0xE0 && c <= 0xEF)
            {
                need = 2;
                continue;
            }

            if(c >= 0xF0 && c <= 0xF4)
            {
                need = 3;
                continue;
            }

            return false;
        }

        if( (c & 0xC0) != 0x80 )
        {
            return false;
        }

        if(need == 2 && lead == 0xE0 && c < 0xA0)
        {
            return false;
        }

        if(need == 2 && lead == 0xED && c > 0x9F)
        {
            return false;
        }

        if(need == 3 && lead == 0xF0 && c < 0x90)
        {
            return false;
        }

        if(need == 3 && lead == 0xF4 && c > 0x8F)
        {
            return false;
        }

        --need;
    }

    return true;
}


bool finishUtf8(unsigned need)
{
    return need == 0;
}

} // namespace


Frame::Frame()
: _opcode(0)
, _fin(false)
, _data(0)
, _size(0)
, _avail(0)
, _pos(0)
, _kind(Data)
{
}


WebSocketError::WebSocketError(unsigned code)
: std::runtime_error("WebSocket protocol error")
, _code(code)
{
}


Frame::Header::Header()
: _size(2)
, _avail(0)
{
    std::memset(_bytes, 0, sizeof(_bytes));
}


void Frame::Header::clear()
{
    _size = 2;
    _avail = 0;
}


void Frame::Header::resize(std::size_t n)
{
    _size = n;
}


bool Frame::Header::fin() const
{
    return (_bytes[0] & 0x80) != 0;
}


unsigned Frame::Header::opcode() const
{
    return static_cast<unsigned char>(_bytes[0]) & 0x0F;
}


bool Frame::Header::isControl() const
{
    return isControl( opcode() );
}


unsigned Frame::Header::rsv() const
{
    return static_cast<unsigned char>(_bytes[0]) & 0x70;
}


bool Frame::Header::masked() const
{
    return (_bytes[1] & 0x80) != 0;
}


unsigned Frame::Header::len7() const
{
    return static_cast<unsigned char>(_bytes[1]) & 0x7F;
}


unsigned Frame::Header::payloadLength(std::size_t& n) const
{
    unsigned len7 = static_cast<unsigned char>(_bytes[1]) & 0x7F;
    if(len7 < 126)
    {
        n = len7;
        return 0;
    }

    if(len7 == 126)
    {
        Pt::uint16_t size = 0;
        std::memcpy(&size, _bytes + 2, 2);
        size = Pt::beToHost(size);
        if(size < 126)
            return 1002;

        n = size;
        return 0;
    }

    Pt::uint64_t size = 0;
    std::memcpy(&size, _bytes + 2, 8);
    size = Pt::beToHost(size);
    if( (size & 0x8000000000000000ull) != 0 || size < 65536 )
        return 1002;

    if(sizeof(std::size_t) < 8 &&
       size > static_cast<Pt::uint64_t>(static_cast<std::size_t>(-1)) )
        return 1009;

    n = static_cast<std::size_t>(size);
    return 0;
}


void Frame::Header::mask(char out[4]) const
{
    std::memcpy(out, _bytes + _size - 4, 4);
}


bool Frame::Header::isControl(unsigned opcode)
{
    return opcode == 0x08 || opcode == 0x09 || opcode == 0x0A;
}


WebSocketBuffer::WebSocketBuffer()
: _sb(0)
, _state(NeedHeader)
, _masking(true)
, _maxMessageSize(1024 * 1024)
, _importRemaining(0)
, _frameReady(false)
, _messageOpcode(0)
, _failCode(0)
, _messageSize(0)
, _utf8Need(0)
, _utf8Lead(0)
{
    setg(0, 0, 0);
}


WebSocketBuffer::WebSocketBuffer(std::streambuf& sb)
: WebSocketBuffer()
{
    _sb = &sb;
}


WebSocketBuffer::~WebSocketBuffer()
{
}


void WebSocketBuffer::attach(std::streambuf& sb)
{
    _sb = &sb;
}


void WebSocketBuffer::reset()
{
    _sb = 0;
    _state = NeedHeader;
    _importRemaining = 0;
    _frame.header().clear();
    _frameReady = false;
    _messageOpcode = 0;
    _failCode = 0;
    _messageSize = 0;
    _utf8Need = 0;
    _utf8Lead = 0;
    setg(0, 0, 0);
}


void WebSocketBuffer::reset(std::streambuf& sb)
{
    reset();
    _sb = &sb;
}


void WebSocketBuffer::setMaxMessageSize(std::size_t n)
{
    _maxMessageSize = n;
}


void WebSocketBuffer::setMasking(bool masked)
{
    _masking = masked;
}


const Frame* WebSocketBuffer::import(std::streamsize n)
{
    if(_failCode != 0)
        throw WebSocketError(_failCode);

    if( ! _sb )
        return 0;

    _importRemaining = n > 0
                     ? static_cast<std::size_t>(n)
                     : std::numeric_limits<std::size_t>::max();
    _frameReady = false;

    while( ! _frameReady )
    {
        State state = _state;
        switch(state)
        {
            case NeedHeader:
                _state = onHeader();
                break;

            case NeedLength:
                _state = onLength();
                break;

            case NeedMask:
                _state = onMask();
                break;

            case NeedPayload:
                _state = onPayload();
                break;

            case NeedDataPayload:
                _state = onDataPayload();
                break;

            case NeedControlPayload:
                _state = onControlPayload();
                break;
        }

        if(_state == state)
            break;
    }

    return _frameReady ? &_frame : 0;
}


WebSocketBuffer::int_type WebSocketBuffer::underflow()
{
    if(gptr() && gptr() < egptr())
        return traits_type::to_int_type(*gptr());

    return traits_type::eof();
}


std::streamsize WebSocketBuffer::showmanyc()
{
    if(gptr() && egptr() && gptr() < egptr())
        return egptr() - gptr();

    return 0;
}


void WebSocketBuffer::failThrow(unsigned code)
{
    if(_failCode == 0)
        _failCode = code;

    throw WebSocketError(_failCode);
}


void WebSocketBuffer::beginMessage(unsigned opcode)
{
    _messageOpcode = opcode;
    _messageSize = 0;
    _utf8Need = 0;
    _utf8Lead = 0;
}


std::size_t WebSocketBuffer::readBytes(char* dst, std::size_t n)
{
    if(n > _importRemaining)
        n = _importRemaining;

    std::streamsize avail = _sb->in_avail();
    if(avail < 0)
    {
        bool started = _state != NeedHeader || _frame.header().avail() > 0;
        if(started)
            failThrow(1002);

        return 0;
    }

    if(n > static_cast<std::size_t>(avail))
        n = static_cast<std::size_t>(avail);

    if(n == 0)
        return 0;

    std::streamsize r = _sb->sgetn(dst, static_cast<std::streamsize>(n));
    if(r <= 0)
        failThrow(1002);

    std::size_t got = static_cast<std::size_t>(r);
    _importRemaining -= got;
    return got;
}


WebSocketBuffer::State WebSocketBuffer::onHeader()
{
    Frame::Header& header = _frame.header();
    std::size_t got = readBytes(header.at(header.avail()), header.remaining());
    if(got == 0)
        return NeedHeader;

    header.setAvail(header.avail() + got);
    if(header.avail() < header.size())
        return NeedHeader;

    if(header.rsv() != 0)
        failThrow(1002);

    unsigned opcode = header.opcode();
    bool control = header.isControl();

    if(control && ! header.fin() )
        failThrow(1002);

    if(opcode == 0x00)
    {
        if(_messageOpcode == 0)
            failThrow(1002);
    }
    else if(opcode == 0x01 || opcode == 0x02)
    {
        if(_messageOpcode != 0)
            failThrow(1002);
    }
    else if( ! control )
    {
        failThrow(1002);
    }

    if(header.masked() != _masking)
        failThrow(1002);

    unsigned len7 = header.len7();
    if(control && len7 > 125)
        failThrow(1002);

    if(len7 == 126)
    {
        header.resize(4);
        return NeedLength;
    }

    if(len7 == 127)
    {
        header.resize(10);
        return NeedLength;
    }

    if(header.masked())
    {
        header.resize(6);
        return NeedMask;
    }

    return NeedPayload;
}


WebSocketBuffer::State WebSocketBuffer::onLength()
{
    Frame::Header& header = _frame.header();
    std::size_t got = readBytes(header.at(header.avail()), header.remaining());
    if(got == 0)
        return NeedLength;

    header.setAvail(header.avail() + got);
    if(header.avail() < header.size())
        return NeedLength;

    std::size_t payload = 0;
    unsigned err = header.payloadLength(payload);
    if(err != 0)
        failThrow(err);

    if(header.isControl() && payload > 125)
        failThrow(1002);

    if(header.masked())
    {
        header.resize(header.size() + 4);
        return NeedMask;
    }

    return NeedPayload;
}


WebSocketBuffer::State WebSocketBuffer::onMask()
{
    Frame::Header& header = _frame.header();
    std::size_t got = readBytes(header.at(header.avail()), header.remaining());
    if(got == 0)
        return NeedMask;

    header.setAvail(header.avail() + got);
    if(header.avail() < header.size())
        return NeedMask;

    return NeedPayload;
}


WebSocketBuffer::State WebSocketBuffer::onPayload()
{
    std::size_t payload = 0;
    Frame::Header& header = _frame.header();
    unsigned err = header.payloadLength(payload);
    if(err != 0)
        failThrow(err);

    unsigned opcode = header.opcode();
    bool control = header.isControl();

    if(control)
    {
        _frame.setKind(Frame::Control);
        _frame.setOpcode(opcode);
        _frame.setSize(payload);
    }
    else
    {
        if(opcode == 0x01 || opcode == 0x02)
            beginMessage(opcode);

        if(_maxMessageSize != 0 &&
           (payload > _maxMessageSize ||
            _messageSize > _maxMessageSize - payload))
        {
            failThrow(1009);
        }

        _messageSize += payload;
        _frame.setKind(Frame::Data);
        _frame.setSize(payload);
        _frame.setOpcode(opcode);
        _frame.setFin(header.fin());
        _frame.setAvail(_frame.buffer(), 0);

    }

    if(payload == 0)
    {
        if(control)
            return NeedControlPayload;

        _frameReady = true;
        return NeedDataPayload;
    }

    return control ? NeedControlPayload : NeedDataPayload;
}


void WebSocketBuffer::unmaskPayload(char* p, std::size_t n, std::size_t pos)
{
    Frame::Header& header = _frame.header();
    if( ! header.masked() || n == 0)
        return;

    char mask[4];
    header.mask(mask);
    for(std::size_t i = 0; i < n; ++i)
        p[i] = static_cast<char>(p[i] ^ mask[(pos + i) % 4]);
}


WebSocketBuffer::State WebSocketBuffer::onDataPayload()
{
    if(_frame.remaining() == 0)
    {
        if(showmanyc() == 0)
        {
            if(_frame.header().fin())
            {
                if(_messageOpcode == 0x01 && ! finishUtf8(_utf8Need) )
                    failThrow(1007);

                _messageOpcode = 0;
                _messageSize = 0;
                _utf8Need = 0;
                _utf8Lead = 0;
            }

            _frame.header().clear();
            return NeedHeader;
        }
    }
    else
    {
        char* buffer = _frame.buffer();
        std::size_t avail = static_cast<std::size_t>(showmanyc());
        if(avail != 0 && gptr() != buffer)
            std::memmove(buffer, gptr(), avail);

        setg(buffer, buffer, buffer + avail);

        std::size_t space = Frame::BufferSize - avail;
        if(space != 0)
        {
            char* dst = egptr();
            std::size_t got = readBytes(dst, std::min(_frame.remaining(), space));
            if(got != 0)
            {
                unmaskPayload(dst, got, _frame.pos());

                bool text = _messageOpcode == 0x01;
                if(text && ! feedUtf8(_utf8Need, _utf8Lead, dst, got) )
                    failThrow(1007);

                setg(eback(), gptr(), egptr() + static_cast<std::streamsize>(got));
                _frame.advance(got);
            }
        }
    }

    std::size_t avail = static_cast<std::size_t>(showmanyc());
    _frame.setAvail(gptr(), avail);
    if(avail != 0)
        _frameReady = true;

    return NeedDataPayload;
}


WebSocketBuffer::State WebSocketBuffer::onControlPayload()
{
    if(_frame.remaining() != 0)
    {
        std::size_t pos = _frame.pos();
        char* dst = _frame.at(pos);
        std::size_t got = readBytes(dst, _frame.remaining());
        if(got == 0)
            return NeedControlPayload;
        unmaskPayload(dst, got, pos);

        _frame.advance(got);
        if(_frame.remaining() != 0)
            return NeedControlPayload;
    }

    std::size_t size = _frame.size();
    if(_frame.header().opcode() == 0x08 && size > 0)
    {
        if(size == 1)
            failThrow(1002);

        unsigned need = 0;
        unsigned char lead = 0;
        const char* reason = _frame.at(2);
        if( ! feedUtf8(need, lead, reason, size - 2) || ! finishUtf8(need) )
        {
            failThrow(1002);
        }
    }

    _frame.setFin(true);
    _frame.setAvail(_frame.at(0), size);
    _frame.header().clear();
    _frameReady = true;
    return NeedHeader;
}

} // namespace Http

} // namespace Pt
