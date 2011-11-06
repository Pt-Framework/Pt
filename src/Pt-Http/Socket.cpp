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

#include "Socket.h"
#include "ServerImpl.h"
#include <cassert>

#define log_define(a)
#define log_trace(a)
#define log_debug(a)
#define log_info(a)
#define log_warn(a)
#define log_error(a)

log_define("pt.http.socket")

namespace Pt {

namespace Http {

void Socket::ParseEvent::onMethod(const std::string& method)
{
    _request.method(method);
}

void Socket::ParseEvent::onUrl(const std::string& url)
{
    _request.url(url);
}

void Socket::ParseEvent::onUrlParam(const std::string& q)
{
    _request.qparams(q);
}

Socket::Socket(ServerImpl& server, Net::TcpServer& tcpServer)
    : inputSlot(slot(*this, &Socket::onInput)),
      _tcpServer(tcpServer),
      _server(server),
      _parseEvent(_request),
      _parser(_parseEvent, false),
      _responder(0),
      _accepted(false)
{
    _stream.attachDevice(*this);
    System::IODevice::inputReady += Pt::slot(*this, &Socket::onIODeviceInput);
    _stream.buffer().outputReady += Pt::slot(*this, &Socket::onOutput);
    _timer.timeout() += Pt::slot(*this, &Socket::onTimeout);
}

Socket::Socket(Socket& socket)
    : inputSlot(slot(*this, &Socket::onInput)),
      _tcpServer(socket._tcpServer),
      _server(socket._server),
      _parseEvent(_request),
      _parser(_parseEvent, false),
      _responder(0),
      _accepted(false)
{
    _stream.attachDevice(*this);
    System::IODevice::inputReady += Pt::slot(*this, &Socket::onIODeviceInput);
    _stream.buffer().outputReady += Pt::slot(*this, &Socket::onOutput);
    _timer.timeout() += Pt::slot(*this, &Socket::onTimeout);
}

Socket::~Socket()
{
    if (_responder)
        _responder->release();
}

void Socket::accept()
{
    Net::TcpSocket::accept(_tcpServer, Net::TcpSocket::DEFER_ACCEPT);

    _accepted = true;

    _stream.buffer().beginRead();

    _timer.start(_server.readTimeout());
}

void Socket::setParent(System::EventLoop* parent)
{
    parent->add(*this);
    parent->add(_timer);

    if( ! parent )
        _timer.setParent(0);
}

void Socket::onIODeviceInput(System::IODevice& iodevice)
{
    log_debug("onIODeviceInput");
    inputReady(*this);
}

void Socket::onInput(System::StreamBuffer& sb)
{
    log_debug("onInput");

    sb.endRead();

    if (sb.in_avail() == 0 || sb.device()->eof())
    {
        close();
        return;
    }

    _timer.start(_server.readTimeout());
    if ( _responder == 0 )
    {
        _parser.advance(sb);

        if (_parser.fail())
        {
            _responder = _server.getDefaultResponder(_request);
            _responder->replyError(_reply.body(), _request, _reply,
                std::runtime_error("invalid http header"));
            _responder->release();
            _responder = 0;

            sendReply();

            onOutput(sb);
            return;
        }

        if (_parser.end())
        {
            _responder = _server.getResponder(_request);
            try
            {
                _responder->beginRequest(_stream, _request);
            }
            catch (const std::exception& e)
            {
                _reply.setHeader("Connection", "close");
                _responder->replyError(_reply.body(), _request, _reply, e);
                _responder->release();
                _responder = 0;
                sendReply();

                onOutput(sb);
                return;
            }

            _contentLength = _request.header().contentLength();
            log_debug("content length of request is " << _contentLength);
            if (_contentLength == 0)
            {
                _timer.stop();
                doReply();
                return;
            }

        }
        else
        {
            sb.beginRead();
        }
    }

    if (_responder)
    {
        if (sb.in_avail() > 0)
        {
            try
            {
                std::size_t s = _responder->readBody(_stream);
                assert(s > 0);
                _contentLength -= s;
            }
            catch (const std::exception& e)
            {
                _reply.setHeader("Connection", "close");
                _responder->replyError(_reply.body(), _request, _reply, e);
                _responder->release();
                _responder = 0;
                sendReply();

                onOutput(sb);
                return;
            }
        }

        if (_contentLength <= 0)
        {
            _timer.stop();
            doReply();
        }
        else
        {
            sb.beginRead();
        }
    }
}

bool Socket::doReply()
{
    log_trace("http::Socket::doReply");
    try
    {
        _responder->reply(_reply.body(), _request, _reply);
    }
    catch (const std::exception& e)
    {
        log_warn("responder reported error: " << e.what());
        _reply.clear();
        _responder->replyError(_reply.body(), _request, _reply, e);
    }

    _responder->release();
    _responder = 0;

    sendReply();

    return onOutput(_stream.buffer());
}

bool Socket::onOutput(System::StreamBuffer& sb)
{
    log_trace("onOutput");

    log_debug("send data to " << getPeerAddr());

    try
    {
        sb.endWrite();

        if ( sb.out_avail() )
        {
            sb.beginWrite();
            _timer.start(_server.writeTimeout());
        }
        else
        {
            bool keepAlive = _request.header().keepAlive()
                          && _reply.header().keepAlive();

            if (keepAlive)
            {
                log_debug("do keep alive");
                _timer.start(_server.keepAliveTimeout());
                _request.clear();
                _reply.clear();
                _parser.reset(false);
                if (sb.in_avail())
                    onInput(sb);
                else
                    _stream.buffer().beginRead();
            }
            else
            {
                log_debug("don't do keep alive");
                close();
                return false;
            }
        }
    }
    catch (const std::exception& e)
    {
        log_warn("exception occured when processing request: " << e.what());
        close();
        timeout(*this);
        return false;
    }

    return true;
}

void Socket::onTimeout()
{
    log_debug("timeout");
    timeout(*this);
}

void Socket::sendReply()
{
    const char* contentLength = "Content-Length";
    const char* server = "Server";
    const char* connection = "Connection";
    const char* date = "Date";

    _stream << "HTTP/"
        << _reply.header().httpVersionMajor() << '.'
        << _reply.header().httpVersionMinor() << ' '
        << _reply.header().httpReturnCode() << ' '
        << _reply.header().httpReturnText() << "\r\n";

    for (ReplyHeader::const_iterator it = _reply.header().begin();
        it != _reply.header().end(); ++it)
    {
        _stream << it->first << ": " << it->second << "\r\n";
    }

    if (!_reply.header().hasHeader(contentLength))
    {
        _stream << "Content-Length: " << _reply.bodySize() << "\r\n";
    }

    if (!_reply.header().hasHeader(server))
    {
        _stream << "Server: Pt-Net-Server\r\n";
    }

    if (!_reply.header().hasHeader(connection))
    {
        _stream << "Connection: "
                << (_request.header().keepAlive() ? "keep-alive" : "close")
                << "\r\n";
    }

    if (!_reply.header().hasHeader(date))
    {
        char buffer[50];
        _stream << "Date: " << MessageHeader::htdateCurrent(buffer) << "\r\n";
    }

    _stream << "\r\n";

    _reply.sendBody(_stream);

}

} // namespace Http

} // namespace Pt
