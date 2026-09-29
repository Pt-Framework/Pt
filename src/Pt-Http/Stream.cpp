/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/Stream.h>
#include <Pt/Http/StreamSession.h>
#include "Connection.h"

#include <stdexcept>

namespace Pt {

namespace Http {

Stream::Stream(Connection& connection, const std::string& protocol)
: _connection(&connection)
, _session(0)
, _protocolName(protocol)
{
}


Stream::~Stream()
{
    StreamSession* session = _session;

    if(session)
        session->closeStream(*this);
}


void Stream::openSession(StreamSession& session)
{
    if(_session)
        throw std::logic_error("HTTP stream already has a session");

    if( ! _connection )
        throw std::logic_error("HTTP stream has no connection");

    _session = &session;
}


void Stream::closeSession(StreamSession& session)
{
    if(_session == &session)
        _session = 0;
}


void Stream::close()
{
    StreamSession* session = _session;
    _session = 0;

    Connection* connection = _connection;
    _connection = 0;

    if(session)
        session->closeStream(*this);

    if(connection)
        connection->closeStream(*this);
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

} // namespace Http

} // namespace Pt
