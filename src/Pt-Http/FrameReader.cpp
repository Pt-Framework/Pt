/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include "FrameReader.h"
#include <Pt/Byteorder.h>
#include <Pt/IOError.h>
#include <cstring>
#include <stdexcept>

namespace Pt {

namespace Http {

namespace {

const std::size_t MaxChunk = 4096;

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


Frame::Frame()
: _type(None)
, _avail(0)
{
}


unsigned Frame::opcode() const
{
    return _header.opcode();
}


bool Frame::fin() const
{
    return _header.fin();
}


void Frame::clear()
{
    _type = None;
    _avail = 0;
    _bytes.clear();
    _header.clear();
}


void Frame::reset(Type type, std::size_t n)
{
    _type = type;
    _avail = 0;
    _bytes.resize(n);
}


char* Frame::at(std::size_t pos)
{
    return &_bytes[pos];
}


void Frame::unmask()
{
    if(_avail == 0 || ! _header.masked())
        return;

    Pt::uint32_t key = _header.maskKey();
    char mask[4];
    std::memcpy(mask, &key, 4);

    std::size_t i = 0;
    while(i + 4 <= _avail)
    {
        Pt::uint32_t word = 0;
        std::memcpy(&word, &_bytes[i], 4);
        word ^= key;
        std::memcpy(&_bytes[i], &word, 4);
        i += 4;
    }

    while(i < _avail)
    {
        _bytes[i] = static_cast<char>(_bytes[i] ^ mask[i % 4]);
        ++i;
    }
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


char* Frame::Header::at(std::size_t pos)
{
    return _bytes + pos;
}


bool Frame::Header::fin() const
{
    return (_bytes[0] & 0x80) != 0;
}


unsigned Frame::Header::opcode() const
{
    return static_cast<unsigned char>(_bytes[0]) & 0x0F;
}


unsigned Frame::Header::rsv() const
{
    return static_cast<unsigned char>(_bytes[0]) & 0x70;
}


bool Frame::Header::masked() const
{
    return (_bytes[1] & 0x80) != 0;
}


std::size_t Frame::Header::payloadLength() const
{
    unsigned len7 = static_cast<unsigned char>(_bytes[1]) & 0x7F;
    if(len7 < 126)
        return len7;

    if(len7 == 126)
    {
        Pt::uint16_t size = 0;
        std::memcpy(&size, _bytes + 2, 2);
        size = Pt::beToHost(size);
        if(size < 126)
            throw IOError("WebSocket length");

        return size;
    }

    Pt::uint64_t size = 0;
    std::memcpy(&size, _bytes + 2, 8);
    size = Pt::beToHost(size);
    if( (size & 0x8000000000000000ull) != 0 || size < 65536 )
        throw IOError("WebSocket length");

    if(sizeof(std::size_t) < 8 &&
       size > static_cast<Pt::uint64_t>(static_cast<std::size_t>(-1)) )
        throw IOError("WebSocket message too large");

    return static_cast<std::size_t>(size);
}


Pt::uint32_t Frame::Header::maskKey() const
{
    Pt::uint32_t key = 0;
    std::memcpy(&key, _bytes + _size - 4, 4);
    return key;
}


FrameReader::FrameReader()
: _sb(0)
, _state(OnHeader)
, _masking(true)
, _maxMessageSize(1024 * 1024)
, _maxRead(0)
, _bytesRead(0)
, _current(false)
, _messageOpen(false)
, _messageOpcode(0)
, _messageSize(0)
, _utf8Need(0)
, _utf8Lead(0)
, _utf8Error(false)
{
}


FrameReader::FrameReader(std::streambuf& sb)
: _sb(&sb)
, _state(OnHeader)
, _masking(true)
, _maxMessageSize(1024 * 1024)
, _maxRead(0)
, _bytesRead(0)
, _current(false)
, _messageOpen(false)
, _messageOpcode(0)
, _messageSize(0)
, _utf8Need(0)
, _utf8Lead(0)
, _utf8Error(false)
{
}


FrameReader::~FrameReader()
{
}


void FrameReader::attach(std::streambuf& sb)
{
    _sb = &sb;
}


void FrameReader::reset()
{
    _frame.clear();
    _sb = 0;
    _state = OnHeader;
    _current = false;
    _messageOpen = false;
    _messageOpcode = 0;
    _messageSize = 0;
    _utf8Need = 0;
    _utf8Lead = 0;
    _utf8Error = false;
}


void FrameReader::reset(std::streambuf& sb)
{
    reset();
    _sb = &sb;
}


void FrameReader::setMaxMessageSize(std::size_t n)
{
    _maxMessageSize = n;
}


void FrameReader::setMasking(bool masked)
{
    _masking = masked;
}


Frame& FrameReader::get()
{
    if( ! _current )
        return next();

    return _frame;
}


Frame& FrameReader::next()
{
    if( ! _sb )
        throw std::logic_error("no input");

    while( ! advance(0) )
        ;

    return _frame;
}


Frame* FrameReader::advance(std::size_t n)
{
    if( ! _sb )
        return 0;

    if(_current)
    {
        _current = false;
        _frame.clear();
        _state = OnHeader;
    }

    _maxRead = n;
    _bytesRead = 0;

    for(;;)
    {
        if(_maxRead != 0 && _bytesRead >= _maxRead)
            break;

        std::streamsize avail = _sb->in_avail();
        if(avail < 0)
        {
            bool started = _state != OnHeader || _frame.header().avail() > 0;
            if(started)
                throw IOError("WebSocket frame incomplete");

            break;
        }

        if(avail == 0)
            break;

        switch(_state)
        {
            case OnHeader:
                onHeader();
                break;

            case OnLength:
                onLength();
                break;

            case OnMask:
                onMask();
                break;

            case OnPayload:
                onPayload();
                break;
        }

        if(_current)
            break;
    }

    if( ! _current )
        return 0;

    return &_frame;
}


void FrameReader::onHeader()
{
    Frame::Header& header = _frame.header();
    std::size_t want = header.size() - header.avail();
    std::size_t n = want;

    if(_maxRead != 0 && n > _maxRead - _bytesRead)
        n = _maxRead - _bytesRead;

    std::streamsize avail = _sb->in_avail();
    if(n > static_cast<std::size_t>(avail))
        n = static_cast<std::size_t>(avail);

    if(n > MaxChunk)
        n = MaxChunk;

    std::streamsize got = _sb->sgetn(header.at(header.avail()),
                                     static_cast<std::streamsize>(n));
    if(got <= 0)
        throw IOError("WebSocket frame incomplete");

    header.setAvail(header.avail() + static_cast<std::size_t>(got));
    _bytesRead += static_cast<std::size_t>(got);

    if(header.avail() < header.size())
        return;

    if(header.rsv() != 0)
        throw IOError("WebSocket RSV");

    unsigned opcode = header.opcode();
    bool control = opcode == 0x08 || opcode == 0x09 || opcode == 0x0A;

    if(control && ! header.fin())
        throw IOError("WebSocket control frame fragmented");

    if(opcode == 0x00)
    {
        if( ! _messageOpen )
            throw IOError("WebSocket continuation");
    }
    else if(opcode == 0x01 || opcode == 0x02)
    {
        if(_messageOpen)
            throw IOError("WebSocket data opcode");
    }
    else if( ! control )
    {
        throw IOError("WebSocket opcode");
    }

    if(header.masked() != _masking)
        throw IOError("WebSocket mask");

    unsigned len7 = static_cast<unsigned char>(header.data()[1]) & 0x7F;
    if(control && len7 > 125)
        throw IOError("WebSocket control frame too large");

    if(len7 == 126)
    {
        header.resize(4);
        _state = OnLength;
        return;
    }

    if(len7 == 127)
    {
        header.resize(10);
        _state = OnLength;
        return;
    }

    if(header.masked())
    {
        header.resize(6);
        _state = OnMask;
        return;
    }

    if( ! control )
    {
        if(_maxMessageSize != 0 &&
           (len7 > _maxMessageSize ||
            _messageSize > _maxMessageSize - len7))
            throw IOError("WebSocket message too large");

        _messageSize += len7;
    }

    Frame::Type type = control ? Frame::Control : Frame::Data;
    _frame.reset(type, len7);
    _state = OnPayload;
}


void FrameReader::onLength()
{
    Frame::Header& header = _frame.header();
    std::size_t want = header.size() - header.avail();
    std::size_t n = want;

    if(_maxRead != 0 && n > _maxRead - _bytesRead)
        n = _maxRead - _bytesRead;

    std::streamsize avail = _sb->in_avail();
    if(n > static_cast<std::size_t>(avail))
        n = static_cast<std::size_t>(avail);

    std::streamsize got = _sb->sgetn(header.at(header.avail()),
                                     static_cast<std::streamsize>(n));
    if(got <= 0)
        throw IOError("WebSocket frame incomplete");

    header.setAvail(header.avail() + static_cast<std::size_t>(got));
    _bytesRead += static_cast<std::size_t>(got);

    if(header.avail() < header.size())
        return;

    std::size_t payload = header.payloadLength();
    unsigned opcode = header.opcode();
    bool control = opcode == 0x08 || opcode == 0x09 || opcode == 0x0A;

    if(control && payload > 125)
        throw IOError("WebSocket control frame too large");

    if( ! control )
    {
        if(_maxMessageSize != 0 &&
           (payload > _maxMessageSize ||
            _messageSize > _maxMessageSize - payload))
            throw IOError("WebSocket message too large");

        _messageSize += payload;
    }

    if(header.masked())
    {
        header.resize(header.size() + 4);
        _state = OnMask;
        return;
    }

    Frame::Type type = control ? Frame::Control : Frame::Data;
    _frame.reset(type, payload);
    _state = OnPayload;
}


void FrameReader::onMask()
{
    Frame::Header& header = _frame.header();
    std::size_t want = header.size() - header.avail();
    std::size_t n = want;

    if(_maxRead != 0 && n > _maxRead - _bytesRead)
        n = _maxRead - _bytesRead;

    std::streamsize avail = _sb->in_avail();
    if(n > static_cast<std::size_t>(avail))
        n = static_cast<std::size_t>(avail);

    std::streamsize got = _sb->sgetn(header.at(header.avail()),
                                     static_cast<std::streamsize>(n));
    if(got <= 0)
        throw IOError("WebSocket frame incomplete");

    header.setAvail(header.avail() + static_cast<std::size_t>(got));
    _bytesRead += static_cast<std::size_t>(got);

    if(header.avail() < header.size())
        return;

    std::size_t payload = header.payloadLength();
    unsigned opcode = header.opcode();
    bool control = opcode == 0x08 || opcode == 0x09 || opcode == 0x0A;
    Frame::Type type = control ? Frame::Control : Frame::Data;
    _frame.reset(type, payload);
    _state = OnPayload;
}


void FrameReader::onPayload()
{
    if(_frame.size() > 0)
    {
        std::size_t want = _frame.size() - _frame.avail();
        std::size_t n = want;

        if(_maxRead != 0 && n > _maxRead - _bytesRead)
            n = _maxRead - _bytesRead;

        std::streamsize avail = _sb->in_avail();
        if(n > static_cast<std::size_t>(avail))
            n = static_cast<std::size_t>(avail);

        if(n > MaxChunk)
            n = MaxChunk;

        std::streamsize got = _sb->sgetn(_frame.at(_frame.avail()),
                                         static_cast<std::streamsize>(n));
        if(got <= 0)
            throw IOError("WebSocket frame incomplete");

        _frame.setAvail(_frame.avail() + static_cast<std::size_t>(got));
        _bytesRead += static_cast<std::size_t>(got);

        if( ! _frame.full() )
            return;
    }

    _frame.unmask();

    unsigned opcode = _frame.opcode();
    bool text = opcode == 0x01 || (opcode == 0x00 && _messageOpcode == 0x01);
    if(text && ! feedUtf8(_utf8Need, _utf8Lead, _utf8Error,
                          _frame.data(), _frame.avail()) )
        throw IOError("WebSocket text is not UTF-8");

    if(opcode == 0x08 && _frame.avail() > 0)
    {
        if(_frame.avail() == 1)
            throw IOError("WebSocket close");

        unsigned need = 0;
        unsigned char lead = 0;
        bool error = false;
        const char* reason = _frame.data() + 2;
        if( ! feedUtf8(need, lead, error, reason, _frame.avail() - 2) ||
            ! finishUtf8(need, error) )
            throw IOError("WebSocket close");
    }

    if(opcode == 0x01 || opcode == 0x02)
    {
        _messageOpen = true;
        _messageOpcode = opcode;
    }

    bool control = opcode == 0x08 || opcode == 0x09 || opcode == 0x0A;
    if(_frame.fin() && ! control)
    {
        if(_messageOpcode == 0x01 && ! finishUtf8(_utf8Need, _utf8Error) )
            throw IOError("WebSocket text is not UTF-8");

        _messageOpen = false;
        _messageOpcode = 0;
        _messageSize = 0;
        _utf8Need = 0;
        _utf8Lead = 0;
        _utf8Error = false;
    }

    _current = true;
    _state = OnHeader;
}

} // namespace Http

} // namespace Pt
