/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include "WebSocketBuffer.h"
#include <Pt/Byteorder.h>
#include <cstring>

namespace Pt {

namespace Http {

namespace {

bool feedUtf8(unsigned& need, unsigned char& lead, bool& error,
              const char* data, std::size_t n)
{
    if(error)
        return false;

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

            error = true;
            return false;
        }

        if( (c & 0xC0) != 0x80 )
        {
            error = true;
            return false;
        }

        if(need == 2 && lead == 0xE0 && c < 0xA0)
        {
            error = true;
            return false;
        }

        if(need == 2 && lead == 0xED && c > 0x9F)
        {
            error = true;
            return false;
        }

        if(need == 3 && lead == 0xF0 && c < 0x90)
        {
            error = true;
            return false;
        }

        if(need == 3 && lead == 0xF4 && c > 0x8F)
        {
            error = true;
            return false;
        }

        --need;
    }

    return true;
}


bool finishUtf8(unsigned need, bool error)
{
    return ! error && need == 0;
}


void xorMask(char* p, std::size_t n, std::size_t off, const char* mask)
{
    for(std::size_t i = 0; i < n; ++i)
        p[i] = static_cast<char>(p[i] ^ mask[(off + i) % 4]);
}

} // namespace


WebSocketBuffer::WebSocketBuffer()
: _sb(0)
, _state(NeedHeader)
, _masking(true)
, _maxMessageSize(1024 * 1024)
, _maxRead(0)
, _bytesRead(0)
, _hdrSize(2)
, _hdrAvail(0)
, _controlSize(0)
, _controlGot(0)
, _hasControl(false)
, _controlOpcode(0)
, _headerSeen(false)
, _opcode(0)
, _finished(false)
, _failCode(0)
, _messageOpen(false)
, _messageSize(0)
, _utf8Need(0)
, _utf8Lead(0)
, _utf8Error(false)
, _frameFin(false)
, _frameMasked(false)
, _frameOpcode(0)
, _payloadSize(0)
, _payloadGot(0)
{
    std::memset(_hdr, 0, sizeof(_hdr));
    std::memset(_mask, 0, sizeof(_mask));
    setg(0, 0, 0);
}


WebSocketBuffer::WebSocketBuffer(std::streambuf& sb)
: _sb(&sb)
, _state(NeedHeader)
, _masking(true)
, _maxMessageSize(1024 * 1024)
, _maxRead(0)
, _bytesRead(0)
, _hdrSize(2)
, _hdrAvail(0)
, _controlSize(0)
, _controlGot(0)
, _hasControl(false)
, _controlOpcode(0)
, _headerSeen(false)
, _opcode(0)
, _finished(false)
, _failCode(0)
, _messageOpen(false)
, _messageSize(0)
, _utf8Need(0)
, _utf8Lead(0)
, _utf8Error(false)
, _frameFin(false)
, _frameMasked(false)
, _frameOpcode(0)
, _payloadSize(0)
, _payloadGot(0)
{
    std::memset(_hdr, 0, sizeof(_hdr));
    std::memset(_mask, 0, sizeof(_mask));
    setg(0, 0, 0);
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
    _hdrSize = 2;
    _hdrAvail = 0;
    _controlSize = 0;
    _controlGot = 0;
    _hasControl = false;
    _controlOpcode = 0;
    _headerSeen = false;
    _opcode = 0;
    _finished = false;
    _failCode = 0;
    _messageOpen = false;
    _messageSize = 0;
    _utf8Need = 0;
    _utf8Lead = 0;
    _utf8Error = false;
    _frameFin = false;
    _frameMasked = false;
    _frameOpcode = 0;
    _payloadSize = 0;
    _payloadGot = 0;
    std::memset(_hdr, 0, sizeof(_hdr));
    std::memset(_mask, 0, sizeof(_mask));
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


void WebSocketBuffer::consumeControl()
{
    _hasControl = false;
    _controlSize = 0;
    _controlGot = 0;
    _controlOpcode = 0;
}


std::streamsize WebSocketBuffer::import(std::streamsize n)
{
    if(_failCode != 0 || ! _sb)
        return 0;

    if(_finished)
        beginNextMessage();

    compact();

    _maxRead = static_cast<std::size_t>(n < 0 ? 0 : n);
    _bytesRead = 0;

    for(;;)
    {
        if(_failCode != 0)
            break;

        if(_hasControl)
            break;

        if(_state == NeedPayload && ! isControl(_frameOpcode) && unused() == 0)
            break;

        std::streamsize avail = _sb->in_avail();
        if(avail < 0)
        {
            bool started = _state != NeedHeader || _hdrAvail > 0;
            if(started)
                fail(1002);

            break;
        }

        if(avail == 0)
            break;

        if(_finished && _state == NeedHeader && _hdrAvail == 0)
        {
            int ch = _sb->sgetc();
            if(ch < 0)
                break;

            unsigned opcode = static_cast<unsigned>(ch) & 0x0F;
            if( ! isControl(opcode) )
                break;
        }

        if(_maxRead != 0 && _bytesRead >= _maxRead)
            break;

        switch(_state)
        {
            case NeedHeader:
                onHeader();
                break;

            case NeedLength:
                onLength();
                break;

            case NeedMask:
                onMask();
                break;

            case NeedPayload:
                onPayload();
                break;
        }
    }

    return static_cast<std::streamsize>(_bytesRead);
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


void WebSocketBuffer::fail(unsigned code)
{
    if(_failCode == 0)
        _failCode = code;
}


void WebSocketBuffer::beginNextMessage()
{
    _finished = false;
    _headerSeen = false;
    _opcode = 0;
    _messageOpen = false;
    _messageSize = 0;
    _utf8Need = 0;
    _utf8Lead = 0;
    _utf8Error = false;
}


void WebSocketBuffer::compact()
{
    if( ! gptr() )
    {
        setg(_window, _window, _window);
        return;
    }

    std::size_t leftover = static_cast<std::size_t>(egptr() - gptr());
    if(leftover != 0 && gptr() != _window)
        std::memmove(_window, gptr(), leftover);

    setg(_window, _window, _window + leftover);
}


std::size_t WebSocketBuffer::unused() const
{
    if( ! eback() )
        return WindowSize;

    return WindowSize - static_cast<std::size_t>(egptr() - eback());
}


std::size_t WebSocketBuffer::take(std::size_t want)
{
    std::size_t n = want;

    if(_maxRead != 0 && n > _maxRead - _bytesRead)
        n = _maxRead - _bytesRead;

    std::streamsize avail = _sb->in_avail();
    if(avail < 0)
        return 0;

    if(n > static_cast<std::size_t>(avail))
        n = static_cast<std::size_t>(avail);

    return n;
}


bool WebSocketBuffer::readBytes(char* dst, std::size_t n, std::size_t& got)
{
    if(n == 0)
    {
        got = 0;
        return true;
    }

    std::streamsize r = _sb->sgetn(dst, static_cast<std::streamsize>(n));
    if(r <= 0)
    {
        fail(1002);
        got = 0;
        return false;
    }

    got = static_cast<std::size_t>(r);
    _bytesRead += got;
    return true;
}


void WebSocketBuffer::onHeader()
{
    std::size_t n = take(_hdrSize - _hdrAvail);
    if(n == 0)
        return;

    std::size_t got = 0;
    if( ! readBytes(_hdr + _hdrAvail, n, got) )
        return;

    _hdrAvail += got;
    if(_hdrAvail < _hdrSize)
        return;

    if(headerRsv() != 0)
    {
        fail(1002);
        return;
    }

    unsigned opcode = headerOpcode();
    bool control = isControl(opcode);

    if(control && ! headerFin() )
    {
        fail(1002);
        return;
    }

    if(opcode == 0x00)
    {
        if( ! _messageOpen )
        {
            fail(1002);
            return;
        }
    }
    else if(opcode == 0x01 || opcode == 0x02)
    {
        if(_messageOpen)
        {
            fail(1002);
            return;
        }
    }
    else if( ! control )
    {
        fail(1002);
        return;
    }

    if(headerMasked() != _masking)
    {
        fail(1002);
        return;
    }

    std::size_t len7 = headerLen7();
    if(control && len7 > 125)
    {
        fail(1002);
        return;
    }

    if(len7 == 126)
    {
        _hdrSize = 4;
        _state = NeedLength;
        return;
    }

    if(len7 == 127)
    {
        _hdrSize = 10;
        _state = NeedLength;
        return;
    }

    if(headerMasked())
    {
        _hdrSize = 6;
        _state = NeedMask;
        return;
    }

    _payloadSize = len7;
    beginPayload();
}


void WebSocketBuffer::onLength()
{
    std::size_t n = take(_hdrSize - _hdrAvail);
    if(n == 0)
        return;

    std::size_t got = 0;
    if( ! readBytes(_hdr + _hdrAvail, n, got) )
        return;

    _hdrAvail += got;
    if(_hdrAvail < _hdrSize)
        return;

    std::size_t payload = payloadLength();
    if(_failCode != 0)
        return;

    unsigned opcode = headerOpcode();
    bool control = isControl(opcode);
    if(control && payload > 125)
    {
        fail(1002);
        return;
    }

    if(headerMasked())
    {
        _hdrSize = _hdrSize + 4;
        _state = NeedMask;
        return;
    }

    _payloadSize = payload;
    beginPayload();
}


void WebSocketBuffer::onMask()
{
    std::size_t n = take(_hdrSize - _hdrAvail);
    if(n == 0)
        return;

    std::size_t got = 0;
    if( ! readBytes(_hdr + _hdrAvail, n, got) )
        return;

    _hdrAvail += got;
    if(_hdrAvail < _hdrSize)
        return;

    std::size_t payload = payloadLength();
    if(_failCode != 0)
        return;

    _payloadSize = payload;
    beginPayload();
}


void WebSocketBuffer::beginPayload()
{
    unsigned opcode = headerOpcode();
    bool control = isControl(opcode);

    if( ! control )
    {
        if(_maxMessageSize != 0 &&
           (_payloadSize > _maxMessageSize ||
            _messageSize > _maxMessageSize - _payloadSize))
        {
            fail(1009);
            return;
        }

        _messageSize += _payloadSize;
    }

    _frameFin = headerFin();
    _frameMasked = headerMasked();
    _frameOpcode = opcode;
    _payloadGot = 0;

    if(_frameMasked)
        std::memcpy(_mask, _hdr + _hdrSize - 4, 4);
    else
        std::memset(_mask, 0, 4);

    if(control)
    {
        _controlOpcode = opcode;
        _controlSize = _payloadSize;
        _controlGot = 0;
        _hasControl = false;
    }
    else if(opcode == 0x01 || opcode == 0x02)
    {
        _messageOpen = true;
        _opcode = opcode;
        _headerSeen = true;
    }

    _state = NeedPayload;

    if(_payloadSize == 0)
        finishFrame();
}


void WebSocketBuffer::onPayload()
{
    bool control = isControl(_frameOpcode);
    std::size_t want = _payloadSize - _payloadGot;
    if( ! control )
    {
        std::size_t space = unused();
        if(space == 0)
            return;

        if(want > space)
            want = space;
    }

    std::size_t n = take(want);
    if(n == 0)
        return;

    char* dst = 0;
    if(control)
        dst = _control + _payloadGot;
    else
        dst = egptr();

    std::size_t got = 0;
    if( ! readBytes(dst, n, got) )
        return;

    if(_frameMasked)
        xorMask(dst, got, _payloadGot, _mask);

    if( ! control )
    {
        bool text = _frameOpcode == 0x01 ||
                    (_frameOpcode == 0x00 && _opcode == 0x01);
        if(text && ! feedUtf8(_utf8Need, _utf8Lead, _utf8Error, dst, got) )
        {
            fail(1007);
            return;
        }

        setg(eback(), gptr(), egptr() + static_cast<std::streamsize>(got));
    }

    _payloadGot += got;
    if(_payloadGot != _payloadSize)
        return;

    finishFrame();
}


void WebSocketBuffer::finishFrame()
{
    bool control = isControl(_frameOpcode);

    if(control)
    {
        if(_frameOpcode == 0x08 && _payloadSize > 0)
        {
            if(_payloadSize == 1)
            {
                fail(1002);
                return;
            }

            unsigned need = 0;
            unsigned char lead = 0;
            bool error = false;
            const char* reason = _control + 2;
            if( ! feedUtf8(need, lead, error, reason, _payloadSize - 2) ||
                ! finishUtf8(need, error) )
            {
                fail(1002);
                return;
            }
        }

        _hasControl = true;
        _controlGot = _payloadSize;
        _hdrSize = 2;
        _hdrAvail = 0;
        _state = NeedHeader;
        return;
    }

    if(_frameFin)
    {
        if(_opcode == 0x01 && ! finishUtf8(_utf8Need, _utf8Error) )
        {
            fail(1007);
            return;
        }

        _finished = true;
        _messageOpen = false;
    }

    _hdrSize = 2;
    _hdrAvail = 0;
    _state = NeedHeader;
}


bool WebSocketBuffer::isControl(unsigned opcode) const
{
    return opcode == 0x08 || opcode == 0x09 || opcode == 0x0A;
}


unsigned WebSocketBuffer::headerOpcode() const
{
    return static_cast<unsigned char>(_hdr[0]) & 0x0F;
}


bool WebSocketBuffer::headerFin() const
{
    return (_hdr[0] & 0x80) != 0;
}


bool WebSocketBuffer::headerMasked() const
{
    return (_hdr[1] & 0x80) != 0;
}


unsigned WebSocketBuffer::headerRsv() const
{
    return static_cast<unsigned char>(_hdr[0]) & 0x70;
}


std::size_t WebSocketBuffer::headerLen7() const
{
    return static_cast<unsigned char>(_hdr[1]) & 0x7F;
}


std::size_t WebSocketBuffer::payloadLength()
{
    unsigned len7 = static_cast<unsigned char>(_hdr[1]) & 0x7F;
    if(len7 < 126)
        return len7;

    if(len7 == 126)
    {
        Pt::uint16_t size = 0;
        std::memcpy(&size, _hdr + 2, 2);
        size = Pt::beToHost(size);
        if(size < 126)
        {
            fail(1002);
            return 0;
        }

        return size;
    }

    Pt::uint64_t size = 0;
    std::memcpy(&size, _hdr + 2, 8);
    size = Pt::beToHost(size);
    if( (size & 0x8000000000000000ull) != 0 || size < 65536 )
    {
        fail(1002);
        return 0;
    }

    if(sizeof(std::size_t) < 8 &&
       size > static_cast<Pt::uint64_t>(static_cast<std::size_t>(-1)) )
    {
        fail(1009);
        return 0;
    }

    return static_cast<std::size_t>(size);
}

} // namespace Http

} // namespace Pt
