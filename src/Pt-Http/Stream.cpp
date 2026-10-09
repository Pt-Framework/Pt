/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/Stream.h>
#include <Pt/Http/Channel.h>
#include "Connection.h"

#include <stdexcept>

namespace Pt {

namespace Http {

Stream::Stream(Connection& connection, const std::string& protocol)
: _connection(&connection)
, _channel(0)
, _protocolName(protocol)
{
}


Stream::~Stream()
{
    close();
}


void Stream::attachChannel(Channel& channel)
{
    if(_channel)
        throw std::logic_error("HTTP stream already has a channel");

    _channel = &channel;
}


void Stream::detachChannel(Channel& channel)
{
    if(_channel == &channel)
        _channel = 0;
}


void Stream::close()
{
    cancel();

    if(_connection)
    {
        _connection->closeStream(*this);
        _connection = 0;

        if(_channel)
            _channel->closeStream(*this);
    }
}


std::streambuf* Stream::buffer()
{
    if( ! _connection )
        throw std::logic_error("HTTP stream has no connection");

    return _connection->streamBuffer();
}


void Stream::beginInput()
{
    if( ! _connection )
        throw std::logic_error("HTTP stream has no connection");

    _connection->beginInput();
}


std::size_t Stream::endInput()
{
    if( ! _connection )
        throw std::logic_error("HTTP stream has no connection");

    return _connection->endInput();
}


void Stream::beginOutput()
{
    if( ! _connection )
        throw std::logic_error("HTTP stream has no connection");

    _connection->beginOutput();
}


std::size_t Stream::endOutput()
{
    if( ! _connection )
        throw std::logic_error("HTTP stream has no connection");

    return _connection->endOutput();
}


void Stream::cancel()
{
    if(_connection)
        _connection->cancelStream(*this);
}


void Stream::setTimeout(std::size_t ms)
{
    if( ! _connection )
        throw std::logic_error("HTTP stream has no connection");

    _connection->setStreamTimeout(ms);
}


System::EventLoop* Stream::loop() const
{
    if( ! _connection )
        return 0;

    return _connection->loop();
}

} // namespace Http

} // namespace Pt
