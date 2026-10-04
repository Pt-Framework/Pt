/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/WebSocketFrame.h>
#include <stdexcept>
#include <cstring>

namespace Pt {

namespace Http {

class WebSocketFrame::PayloadBuffer : public std::streambuf
{
    public:
        explicit PayloadBuffer(std::vector<char>& payload)
        : _payload(&payload)
        {
        }

        void reset()
        {
            _payload->clear();
            setp(0, 0);
            setg(0, 0, 0);
        }

        void prepareGet()
        {
            if( _payload->empty() )
            {
                setg(0, 0, 0);
                setp(0, 0);
                return;
            }

            setg(&(*_payload)[0], &(*_payload)[0], &(*_payload)[0] + _payload->size());
            setp(0, 0);
        }

    protected:
        virtual int_type overflow(int_type ch)
        {
            if( ch == traits_type::eof() )
                return traits_type::not_eof(ch);

            std::size_t off = pptr() ? static_cast<std::size_t>(pptr() - pbase())
                                     : _payload->size();
            _payload->push_back( static_cast<char>(ch) );
            setp(&(*_payload)[0], &(*_payload)[0] + _payload->size());
            pbump( static_cast<int>(off + 1) );
            return ch;
        }

        virtual std::streamsize xsputn(const char* data, std::streamsize n)
        {
            std::size_t off = pptr() ? static_cast<std::size_t>(pptr() - pbase())
                                     : _payload->size();
            _payload->insert(_payload->end(), data, data + n);
            setp(&(*_payload)[0], &(*_payload)[0] + _payload->size());
            pbump( static_cast<int>(off + static_cast<std::size_t>(n)) );
            return n;
        }

    private:
        std::vector<char>* _payload;
};


WebSocketFrame::WebSocketFrame()
: _type(Text)
, _fin(true)
, _busy(false)
, _closeSet(false)
, _closeCode(1000)
, _buffer(new PayloadBuffer(_payload))
, _body(_buffer)
{
}


WebSocketFrame::~WebSocketFrame()
{
    _body.rdbuf(0);
    delete _buffer;
}


void WebSocketFrame::setType(Type type)
{
    ensureIdle();
    _type = type;
}


void WebSocketFrame::setFin(bool fin)
{
    ensureIdle();
    _fin = fin;
}


std::size_t WebSocketFrame::available() const
{
    std::streambuf* sb = _body.rdbuf();
    if( ! sb )
        return 0;

    std::streamsize n = sb->in_avail();
    return n > 0 ? static_cast<std::size_t>(n) : 0;
}


std::size_t WebSocketFrame::pending() const
{
    return _payload.size();
}


void WebSocketFrame::discard()
{
    ensureIdle();
    _buffer->reset();
    _body.clear();
}


void WebSocketFrame::clear()
{
    ensureIdle();
    _type = Text;
    _fin = true;
    _closeSet = false;
    _closeCode = 1000;
    _closeReason.clear();
    _buffer->reset();
    _body.clear();
}


unsigned short WebSocketFrame::closeCode() const
{
    if(_type != Close)
        return 0;

    return _closeSet ? _closeCode : 1000;
}


void WebSocketFrame::setCloseCode(unsigned short code)
{
    ensureIdle();
    _closeCode = code;
    _closeSet = true;
}


const std::string& WebSocketFrame::closeReason() const
{
    static const std::string empty;
    if(_type != Close)
        return empty;

    return _closeReason;
}


void WebSocketFrame::setCloseReason(const std::string& reason)
{
    ensureIdle();
    _closeReason = reason;
}


void WebSocketFrame::setBusy(bool busy)
{
    _busy = busy;
}


void WebSocketFrame::composeClosePayload()
{
    unsigned short code = closeCode();
    if(code == 0)
        code = 1000;

    _closeCode = code;
    _closeSet = true;

    std::string reason = _closeReason;
    _payload.clear();
    _payload.push_back( static_cast<char>(code >> 8) );
    _payload.push_back( static_cast<char>(code & 0xff) );
    _payload.insert(_payload.end(), reason.begin(), reason.end());
    _buffer->prepareGet();
    _body.clear();
}


void WebSocketFrame::assign(Type type, bool fin, const char* data, std::size_t n)
{
    _type = type;
    _fin = fin;
    _payload.assign(data, data + n);
    _closeSet = false;
    _closeCode = 1000;
    _closeReason.clear();

    if(type == Close)
    {
        if(n >= 2)
        {
            _closeCode = static_cast<unsigned short>(
                (static_cast<unsigned char>(data[0]) << 8) |
                 static_cast<unsigned char>(data[1]) );
            _closeSet = true;
            _closeReason.assign(data + 2, data + n);
        }
        else if(n == 0)
        {
            _closeCode = 1000;
            _closeSet = true;
        }
    }

    _buffer->prepareGet();
    _body.clear();
}


const char* WebSocketFrame::data() const
{
    return _payload.empty() ? 0 : &_payload[0];
}


void WebSocketFrame::ensureIdle() const
{
    if(_busy)
        throw std::logic_error("WebSocket frame transfer is active");
}

} // namespace Http

} // namespace Pt
