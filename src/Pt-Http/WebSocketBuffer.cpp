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

} // namespace


WebSocketBuffer::Header::Header()
: _size(2)
, _avail(0)
{
    std::memset(_bytes, 0, sizeof(_bytes));
}


void WebSocketBuffer::Header::clear()
{
    _size = 2;
    _avail = 0;
}


void WebSocketBuffer::Header::resize(std::size_t n)
{
    _size = n;
}


bool WebSocketBuffer::Header::fin() const
{
    return (_bytes[0] & 0x80) != 0;
}


unsigned WebSocketBuffer::Header::opcode() const
{
    return static_cast<unsigned char>(_bytes[0]) & 0x0F;
}


bool WebSocketBuffer::Header::isControl() const
{
    return isControl( opcode() );
}


unsigned WebSocketBuffer::Header::rsv() const
{
    return static_cast<unsigned char>(_bytes[0]) & 0x70;
}


bool WebSocketBuffer::Header::masked() const
{
    return (_bytes[1] & 0x80) != 0;
}


unsigned WebSocketBuffer::Header::len7() const
{
    return static_cast<unsigned char>(_bytes[1]) & 0x7F;
}


unsigned WebSocketBuffer::Header::payloadLength(std::size_t& n) const
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


void WebSocketBuffer::Header::mask(char out[4]) const
{
    std::memcpy(out, _bytes + _size - 4, 4);
}


bool WebSocketBuffer::Header::isControl(unsigned opcode)
{
    return opcode == 0x08 || opcode == 0x09 || opcode == 0x0A;
}


WebSocketBuffer::Control::Control()
: _size(0)
, _got(0)
, _opcode(0)
, _complete(false)
{
}


void WebSocketBuffer::Control::clear()
{
    _size = 0;
    _got = 0;
    _opcode = 0;
    _complete = false;
}


void WebSocketBuffer::Control::begin(unsigned opcode, std::size_t n)
{
    _opcode = opcode;
    _size = n;
    _got = 0;
    _complete = false;
}


WebSocketBuffer::Frame::Frame()
: _fin(false)
, _masked(false)
, _opcode(0)
, _payloadSize(0)
, _payloadGot(0)
{
    std::memset(_mask, 0, sizeof(_mask));
}


void WebSocketBuffer::Frame::clear()
{
    _fin = false;
    _masked = false;
    _opcode = 0;
    _payloadSize = 0;
    _payloadGot = 0;
    std::memset(_mask, 0, sizeof(_mask));
}


void WebSocketBuffer::Frame::begin(const Header& header, std::size_t payload)
{
    _fin = header.fin();
    _masked = header.masked();
    _opcode = header.opcode();
    _payloadSize = payload;
    _payloadGot = 0;

    if(_masked)
        header.mask(_mask);
    else
        std::memset(_mask, 0, sizeof(_mask));
}


void WebSocketBuffer::Frame::unmask(char* p, std::size_t n) const
{
    if( ! _masked || n == 0)
        return;

    for(std::size_t i = 0; i < n; ++i)
        p[i] = static_cast<char>(p[i] ^ _mask[(_payloadGot + i) % 4]);
}


WebSocketBuffer::WebSocketBuffer()
: _sb(0)
, _state(NeedHeader)
, _masking(true)
, _maxMessageSize(1024 * 1024)
, _maxRead(0)
, _bytesRead(0)
, _headerSeen(false)
, _opcode(0)
, _finished(false)
, _failCode(0)
, _messageOpen(false)
, _messageSize(0)
, _utf8Need(0)
, _utf8Lead(0)
, _utf8Error(false)
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
    _maxRead = 0;
    _bytesRead = 0;
    _hdr.clear();
    _control.clear();
    _frame.clear();
    _headerSeen = false;
    _opcode = 0;
    _finished = false;
    _failCode = 0;
    _messageOpen = false;
    _messageSize = 0;
    _utf8Need = 0;
    _utf8Lead = 0;
    _utf8Error = false;
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
    _control.clear();
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

        if(_control.complete())
            break;

        if(_state == NeedPayload && ! _frame.isControl() && unused() == 0)
            break;

        std::streamsize avail = _sb->in_avail();
        if(avail < 0)
        {
            bool started = _state != NeedHeader || _hdr.avail() > 0;
            if(started)
                fail(1002);

            break;
        }

        if(avail == 0)
            break;

        if(_finished && _state == NeedHeader && _hdr.avail() == 0)
        {
            int ch = _sb->sgetc();
            if(ch < 0)
                break;

            unsigned opcode = static_cast<unsigned>(ch) & 0x0F;
            if( ! Header::isControl(opcode) )
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
    std::size_t n = take(_hdr.remaining());
    if(n == 0)
        return;

    std::size_t got = 0;
    if( ! readBytes(_hdr.at(_hdr.avail()), n, got) )
        return;

    _hdr.setAvail(_hdr.avail() + got);
    if(_hdr.avail() < _hdr.size())
        return;

    if(_hdr.rsv() != 0)
    {
        fail(1002);
        return;
    }

    unsigned opcode = _hdr.opcode();
    bool control = _hdr.isControl();

    if(control && ! _hdr.fin() )
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

    if(_hdr.masked() != _masking)
    {
        fail(1002);
        return;
    }

    unsigned len7 = _hdr.len7();
    if(control && len7 > 125)
    {
        fail(1002);
        return;
    }

    if(len7 == 126)
    {
        _hdr.resize(4);
        _state = NeedLength;
        return;
    }

    if(len7 == 127)
    {
        _hdr.resize(10);
        _state = NeedLength;
        return;
    }

    if(_hdr.masked())
    {
        _hdr.resize(6);
        _state = NeedMask;
        return;
    }

    beginPayload();
}


void WebSocketBuffer::onLength()
{
    std::size_t n = take(_hdr.remaining());
    if(n == 0)
        return;

    std::size_t got = 0;
    if( ! readBytes(_hdr.at(_hdr.avail()), n, got) )
        return;

    _hdr.setAvail(_hdr.avail() + got);
    if(_hdr.avail() < _hdr.size())
        return;

    std::size_t payload = 0;
    unsigned err = _hdr.payloadLength(payload);
    if(err != 0)
    {
        fail(err);
        return;
    }

    if(_hdr.isControl() && payload > 125)
    {
        fail(1002);
        return;
    }

    if(_hdr.masked())
    {
        _hdr.resize(_hdr.size() + 4);
        _state = NeedMask;
        return;
    }

    beginPayload();
}


void WebSocketBuffer::onMask()
{
    std::size_t n = take(_hdr.remaining());
    if(n == 0)
        return;

    std::size_t got = 0;
    if( ! readBytes(_hdr.at(_hdr.avail()), n, got) )
        return;

    _hdr.setAvail(_hdr.avail() + got);
    if(_hdr.avail() < _hdr.size())
        return;

    beginPayload();
}


void WebSocketBuffer::beginPayload()
{
    std::size_t payload = 0;
    unsigned err = _hdr.payloadLength(payload);
    if(err != 0)
    {
        fail(err);
        return;
    }

    unsigned opcode = _hdr.opcode();
    bool control = _hdr.isControl();

    if( ! control )
    {
        if(_maxMessageSize != 0 &&
           (payload > _maxMessageSize ||
            _messageSize > _maxMessageSize - payload))
        {
            fail(1009);
            return;
        }

        _messageSize += payload;
    }

    _frame.begin(_hdr, payload);

    if(control)
        _control.begin(opcode, payload);
    else if(opcode == 0x01 || opcode == 0x02)
    {
        _messageOpen = true;
        _opcode = opcode;
        _headerSeen = true;
    }

    _state = NeedPayload;

    if(payload == 0)
        finishFrame();
}


void WebSocketBuffer::onPayload()
{
    bool control = _frame.isControl();
    std::size_t want = _frame.remaining();
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
        dst = _control.at(_control.got());
    else
        dst = egptr();

    std::size_t got = 0;
    if( ! readBytes(dst, n, got) )
        return;

    _frame.unmask(dst, got);

    if( ! control )
    {
        bool text = _frame.opcode() == 0x01 ||
                    (_frame.opcode() == 0x00 && _opcode == 0x01);
        if(text && ! feedUtf8(_utf8Need, _utf8Lead, _utf8Error, dst, got) )
        {
            fail(1007);
            return;
        }

        setg(eback(), gptr(), egptr() + static_cast<std::streamsize>(got));
    }
    else
        _control.addGot(got);

    _frame.addGot(got);
    if(_frame.remaining() != 0)
        return;

    finishFrame();
}


void WebSocketBuffer::finishFrame()
{
    if(_frame.isControl())
    {
        if(_frame.opcode() == 0x08 && _frame.payloadSize() > 0)
        {
            if(_frame.payloadSize() == 1)
            {
                fail(1002);
                return;
            }

            unsigned need = 0;
            unsigned char lead = 0;
            bool error = false;
            const char* reason = _control.data() + 2;
            if( ! feedUtf8(need, lead, error, reason, _frame.payloadSize() - 2) ||
                ! finishUtf8(need, error) )
            {
                fail(1002);
                return;
            }
        }

        _control.setComplete();
        _hdr.clear();
        _state = NeedHeader;
        return;
    }

    if(_frame.fin())
    {
        if(_opcode == 0x01 && ! finishUtf8(_utf8Need, _utf8Error) )
        {
            fail(1007);
            return;
        }

        _finished = true;
        _messageOpen = false;
    }

    _hdr.clear();
    _state = NeedHeader;
}

} // namespace Http

} // namespace Pt
