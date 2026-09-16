/*
 * Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
#include "WebSocketStream.h"

namespace Pt {
namespace Http {

WebSocketStream::WebSocketStream(Pt::Http::IOStream& io)
: _httpIoStream(&io)
, _inBuffer(0)
, _bufferSize(0)
, _socket(0)
, _error(false)
, _timeout(3000)
, _ignoreEvent(false)
{
    _httpIoStream->inputReady() += Pt::slot(*this, &WebSocketStream::onInputS);
    _httpIoStream->outputReady() += Pt::slot(*this, &WebSocketStream::onOutputS);
}

WebSocketStream::WebSocketStream()
: _httpIoStream(0)
, _inBuffer(0)
, _bufferSize(0)
, _socket(new Pt::Net::TcpSocket())
, _error(false)
, _timeout(3000)
, _ignoreEvent(false)
{
    _socket->inputReady() += Pt::slot(*this, &WebSocketStream::onInputC);
    _socket->outputReady() += Pt::slot(*this, &WebSocketStream::onOutputC);
    _socket->connected() += Pt::slot(*this, &WebSocketStream::onConnected);

    _connectionTimer.timeout() += Pt::slot(*this, &WebSocketStream::onConnectTimeout);
}

WebSocketStream::~WebSocketStream()
{
    if (_httpIoStream)
    {
        delete _httpIoStream;
        _httpIoStream = 0;
    }

    if (_socket)
    {
        delete _socket;
        _socket = 0;
    }
}


void WebSocketStream::beginConnect(const Pt::Net::Endpoint& ep)
{
    _error = false;
    _socket->beginConnect(ep);
    _connectionTimer.start(_timeout);
}

void WebSocketStream::onConnected(Pt::Net::TcpSocket& socket)
{
    _connectionTimer.stop();

    try
    {
        _socket->endConnect();
        _connected.send();
    }
    catch (const std::exception&)
    {
        _error = true;
        _connected.send();
        throw;
    }
}

void WebSocketStream::onConnectTimeout()
{
    _connectionTimer.stop();

    try
    {
        cancel();

        throw std::runtime_error("connect timeout");
    }
    catch (const std::exception&)
    {
        _error = true;
        _connected.send();
        throw;
    }
}

bool WebSocketStream::endConnect()
{
    if(_error)
        throw;

    return _socket->isConnected();
}

void WebSocketStream::write(const char* buffer, size_t size)
{
    cancel();

    _error = false;

    if (_socket)
    {
        _socket->write(buffer, size);
        _socket->sync();
    }

    if (_httpIoStream)
    {
        _ignoreEvent = true;
        _httpIoStream->write(buffer, size);
        _httpIoStream->beginOutput();
        _httpIoStream->endOutput();
        _ignoreEvent = false;
    }
}

void WebSocketStream::beginWrite(const char* buffer, size_t size)
{
    _error = false;

    if (_httpIoStream)
    {
        _httpIoStream->write(buffer, size);
        _httpIoStream->beginOutput();
    }

    if (_socket)
        _socket->beginWrite(buffer, size);
}


void WebSocketStream::onOutputS()
{
    if(_ignoreEvent)
        return;

    try
    {
        _bufferSize = _httpIoStream->endOutput();
        _outputReady.send();
    }
    catch (const std::exception&)
    {
        _error = true;
        _outputReady.send();
        throw;
    }
}


void WebSocketStream::onOutputC(Pt::System::IODevice& device)
{
    try
    {
        _bufferSize = device.endWrite();
        _outputReady.send();
    }
    catch (const std::exception&)
    {
        _error = true;
        _outputReady.send();
        throw;
    }
}

size_t WebSocketStream::endWrite()
{
    if(_error)
        throw;

    return _bufferSize;
}


void WebSocketStream::beginRead(char* buffer, size_t size)
{
    _error = false;

    if (_httpIoStream)
    {
        _bufferSize = size;
        _inBuffer = buffer;
        _httpIoStream->beginInput();
    }

    if(_socket)
        _socket->beginRead(buffer, size);
}


void WebSocketStream::onInputS()
{
    try
    {
        _bufferSize = _httpIoStream->endInput();
        _httpIoStream->read(&_inBuffer[0], _bufferSize);
        _inputReady.send();
    }
    catch (const std::exception&)
    {
        _error = true;
        _inputReady.send();
        throw;
    }
}


void WebSocketStream::onInputC(Pt::System::IODevice& device)
{
    try
    {
        _bufferSize = device.endRead();
        _inputReady.send();
    }
    catch (const std::exception&)
    {
        _error = true;
        _inputReady.send();
        throw;
    }
}

size_t WebSocketStream::endRead()
{
    if(_error)
        throw;

    return _bufferSize;
}


void WebSocketStream::cancel()
{
    if (_httpIoStream)
        _httpIoStream->cancel();

    if(_socket)
        _socket->cancel();
}

}}
