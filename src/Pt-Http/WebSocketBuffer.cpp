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


Frame::Frame(Kind kind)
: _opcode(0)
, _fin(false)
, _data(0)
, _size(0)
, _avail(0)
, _pos(0)
, _kind(kind)
{
}


void Frame::set(unsigned opcode, bool fin, const char* data, std::size_t avail)
{
    setOpcode(opcode);
    setFin(fin);
    setAvail(data, avail);
}


DataFrame::DataFrame()
: Frame(Data)
{
}


ControlFrame::ControlFrame()
: Frame(Control)
{
}


WebSocketError::WebSocketError(unsigned code)
: std::runtime_error("WebSocket protocol error")
, _code(code)
{
}


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


WebSocketBuffer::WebSocketBuffer()
: _sb(0)
, _state(NeedHeader)
, _masking(true)
, _maxMessageSize(1024 * 1024)
, _maxRead(0)
, _bytesRead(0)
, _dataProgress(false)
, _controlReady(false)
, _messageOpcode(0)
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
    _controlReady = false;
    _dataProgress = false;
    _messageOpcode = 0;
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


const Frame* WebSocketBuffer::import(std::streamsize n)
{
    if(_failCode != 0)
        throw WebSocketError(_failCode);

    if( ! _sb )
        return 0;

    if(_finished)
        beginNextMessage();

    compact();

    _dataProgress = false;
    _controlReady = false;
    _maxRead = static_cast<std::size_t>(n < 0 ? 0 : n);
    _bytesRead = 0;

    for(;;)
    {
        if(_controlReady)
            break;

        // Stop right at a finished message; a trailing control is picked up
        // by the next import() instead of being folded into this result.
        if(_finished)
            break;

        if(_state == NeedPayload && ! _hdr.isControl() && unused() == 0)
            break;

        std::streamsize avail = _sb->in_avail();
        if(avail < 0)
        {
            bool started = _state != NeedHeader || _hdr.avail() > 0;
            if(started)
                failThrow(1002);

            break;
        }

        if(avail == 0)
            break;

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

    if(_controlReady)
        return &_controlResult;

    if(_dataProgress)
    {
        std::size_t chunk = static_cast<std::size_t>(egptr() - eback());
        _dataResult.set(_messageOpcode, _finished, eback(), chunk);
        return &_dataResult;
    }

    return 0;
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


void WebSocketBuffer::beginNextMessage()
{
    _finished = false;
    _messageOpcode = 0;
    _messageOpen = false;
    _messageSize = 0;
    _utf8Need = 0;
    _utf8Lead = 0;
    _utf8Error = false;
}


void WebSocketBuffer::compact()
{
    char* buffer = _dataResult.buffer();

    if( ! gptr() )
    {
        setg(buffer, buffer, buffer);
        return;
    }

    std::size_t leftover = static_cast<std::size_t>(egptr() - gptr());
    if(leftover != 0 && gptr() != buffer)
        std::memmove(buffer, gptr(), leftover);

    setg(buffer, buffer, buffer + leftover);
}


std::size_t WebSocketBuffer::unused() const
{
    if( ! eback() )
        return DataFrame::BufferSize;

    return DataFrame::BufferSize - static_cast<std::size_t>(egptr() - eback());
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


std::size_t WebSocketBuffer::readBytes(char* dst, std::size_t n)
{
    std::streamsize r = _sb->sgetn(dst, static_cast<std::streamsize>(n));
    if(r <= 0)
        failThrow(1002);

    std::size_t got = static_cast<std::size_t>(r);
    _bytesRead += got;
    return got;
}


void WebSocketBuffer::onHeader()
{
    std::size_t n = take(_hdr.remaining());
    if(n == 0)
        return;

    std::size_t got = readBytes(_hdr.at(_hdr.avail()), n);

    _hdr.setAvail(_hdr.avail() + got);
    if(_hdr.avail() < _hdr.size())
        return;

    if(_hdr.rsv() != 0)
        failThrow(1002);

    unsigned opcode = _hdr.opcode();
    bool control = _hdr.isControl();

    if(control && ! _hdr.fin() )
        failThrow(1002);

    if(opcode == 0x00)
    {
        if( ! _messageOpen )
            failThrow(1002);
    }
    else if(opcode == 0x01 || opcode == 0x02)
    {
        if(_messageOpen)
            failThrow(1002);
    }
    else if( ! control )
    {
        failThrow(1002);
    }

    if(_hdr.masked() != _masking)
        failThrow(1002);

    unsigned len7 = _hdr.len7();
    if(control && len7 > 125)
        failThrow(1002);

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

    std::size_t got = readBytes(_hdr.at(_hdr.avail()), n);

    _hdr.setAvail(_hdr.avail() + got);
    if(_hdr.avail() < _hdr.size())
        return;

    std::size_t payload = 0;
    unsigned err = _hdr.payloadLength(payload);
    if(err != 0)
        failThrow(err);

    if(_hdr.isControl() && payload > 125)
        failThrow(1002);

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

    std::size_t got = readBytes(_hdr.at(_hdr.avail()), n);

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
        failThrow(err);

    unsigned opcode = _hdr.opcode();
    bool control = _hdr.isControl();

    if(control)
    {
        _controlResult.setOpcode(opcode);
        _controlResult.setSize(payload);
    }
    else
    {
        if(_maxMessageSize != 0 &&
           (payload > _maxMessageSize ||
            _messageSize > _maxMessageSize - payload))
        {
            failThrow(1009);
        }

        _messageSize += payload;
        _dataResult.setSize(payload);

        if(opcode == 0x01 || opcode == 0x02)
        {
            _messageOpen = true;
            _messageOpcode = opcode;
            _dataProgress = true;
        }
    }

    _state = NeedPayload;

    if(payload == 0)
        finishFrame();
}


void WebSocketBuffer::unmaskPayload(char* p, std::size_t n, std::size_t pos)
{
    if( ! _hdr.masked() || n == 0)
        return;

    char mask[4];
    _hdr.mask(mask);
    for(std::size_t i = 0; i < n; ++i)
        p[i] = static_cast<char>(p[i] ^ mask[(pos + i) % 4]);
}


void WebSocketBuffer::onPayload()
{
    bool control = _hdr.isControl();
    std::size_t priorPos = control ? _controlResult.pos() : _dataResult.pos();
    std::size_t want = control ? _controlResult.remaining() : _dataResult.remaining();

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

    char* dst = control ? _controlResult.at(priorPos) : egptr();

    std::size_t got = readBytes(dst, n);

    unmaskPayload(dst, got, priorPos);

    if(control)
    {
        _controlResult.advance(got);
        if(_controlResult.remaining() != 0)
            return;
    }
    else
    {
        bool text = _hdr.opcode() == 0x01 ||
                    (_hdr.opcode() == 0x00 && _messageOpcode == 0x01);
        if(text && ! feedUtf8(_utf8Need, _utf8Lead, _utf8Error, dst, got) )
            failThrow(1007);

        setg(eback(), gptr(), egptr() + static_cast<std::streamsize>(got));
        _dataProgress = true;

        _dataResult.advance(got);
        if(_dataResult.remaining() != 0)
            return;
    }

    finishFrame();
}


void WebSocketBuffer::finishFrame()
{
    if(_hdr.isControl())
    {
        std::size_t size = _controlResult.size();
        if(_hdr.opcode() == 0x08 && size > 0)
        {
            if(size == 1)
                failThrow(1002);

            unsigned need = 0;
            unsigned char lead = 0;
            bool error = false;
            const char* reason = _controlResult.at(2);
            if( ! feedUtf8(need, lead, error, reason, size - 2) ||
                ! finishUtf8(need, error) )
                failThrow(1002);
        }

        _controlResult.setFin(true);
        _controlResult.setAvail(_controlResult.at(0), size);
        _controlReady = true;
        _hdr.clear();
        _state = NeedHeader;
        return;
    }

    if(_hdr.fin())
    {
        if(_messageOpcode == 0x01 && ! finishUtf8(_utf8Need, _utf8Error) )
            failThrow(1007);

        _finished = true;
        _messageOpen = false;
        _dataProgress = true;
    }

    _hdr.clear();
    _state = NeedHeader;
}

} // namespace Http

} // namespace Pt
