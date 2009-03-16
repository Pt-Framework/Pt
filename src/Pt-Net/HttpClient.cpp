/*
 * Copyright (C) 2009 by Marc Boris Duerner, Tommi Maekitalo
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <Pt/Net/HttpClient.h>

namespace Pt {

namespace Net {

HttpReply::HttpReply()
: _request(0)
, _readHeader(true)
, _contentSize(0)
, _requestReady(false)
, _executed(false)
{
    _stream.attachDevice(_socket);
    connect(_socket.connected, *this, &HttpReply::onConnect);
    connect(_stream.buffer().outputReady, *this, &HttpReply::onOutput);
    connect(_stream.buffer().inputReady, *this, &HttpReply::onInput);
}


void HttpReply::setSelector(System::SelectorBase& selector)
{
    selector.add(_socket);
}


void HttpReply::beginExecute(HttpRequest& request)
{
    _requestReady = false;
    _socket.beginConnect( request.server(), request.port() );
    _request = &request;
}


void HttpReply::wait(std::size_t msecs)
{
    _socket.wait(msecs);
}


void HttpReply::onConnect(TcpSocket& socket)
{
    _stream << _request->method() << ' '
            << _request->url() << " HTTP/1.1\r\n";

    for (HttpRequest::Headers::const_iterator it = _request->_headers.begin();
        it != _request->_headers.end(); ++it)
    {
        _stream << it->first << ": " << it->second;
    }

    _stream << "\r\n\r\n";

    _stream.buffer().beginWrite();
}


void HttpReply::onOutput(System::StreamBuffer& sb)
{
    if( sb.out_avail() > 0 )
    {
        sb.beginWrite();
    }
    else
    {
        sb.beginRead();
    }
}


void HttpReply::onInput(System::StreamBuffer& sb)
{
    _readHeader = false; // TODO: parse header first

    if (_readHeader)
    {
        // TODO parse reply
        while( sb.in_avail() > 0 )
            sb.sbumpc();

        bool ready = true;
        if( ready )
        {
            _contentSize = 0;
            headerReceived(*this);
            _readHeader = false;

            if( sb.in_avail() > 0 )
                replyReceived(*this);
        }
    }
    else
    {
        _contentSize -= replyReceived(*this);
        if( _contentSize <= 0 )
            _requestReady = true;
    }
}

} // namespace Net

} // namespace Pt
