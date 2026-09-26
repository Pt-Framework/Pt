/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/Stream.h>
#include "Connection.h"

#include <stdexcept>
#include <streambuf>

namespace Pt {

namespace Http {

Stream::Stream()
: _state(0)
{
}


Stream::Stream(const Stream& other)
: _state(other._state)
{
    attach();
}


Stream::Stream(StreamState* state)
: _state(state)
{
    if( ! _state )
        throw std::logic_error("HTTP stream has no connection");

    attach();
}


Stream::~Stream()
{
    detach();
    release();
}


Stream& Stream::operator=(const Stream& other)
{
    if( this == &other )
        return *this;

    detach();
    release();
    _state = other._state;
    attach();
    return *this;
}


const std::string& Stream::protocol() const
{
    if( ! _state )
        throw std::logic_error("HTTP stream has no connection");

    return _state->protocol;
}


void Stream::retain()
{
    if( ! _state || ! _state->connection )
        throw std::logic_error("HTTP stream has no connection");

    _state->retained = true;
}


bool Stream::isRetained() const
{
    return _state && _state->retained;
}


void Stream::close()
{
    if( ! _state || ! _state->connection )
        return;

    Http::Connection* conn = _state->connection;
    _state->connection = 0;
    detach();

    _closed.send();
    conn->closeStream();
}


std::streambuf* Stream::buffer()
{
    if( ! _state || ! _state->connection )
        throw std::logic_error("HTTP stream has no connection");

    return _state->connection->streamBuffer();
}


void Stream::beginInput()
{
    if( ! _state || ! _state->connection )
        throw std::logic_error("HTTP stream has no connection");

    _state->connection->beginInput();
}


std::size_t Stream::endInput()
{
    if( ! _state || ! _state->connection )
        throw std::logic_error("HTTP stream has no connection");

    return _state->connection->endInput();
}


void Stream::beginOutput()
{
    if( ! _state || ! _state->connection )
        throw std::logic_error("HTTP stream has no connection");

    _state->connection->beginOutput();
}


std::size_t Stream::endOutput()
{
    if( ! _state || ! _state->connection )
        throw std::logic_error("HTTP stream has no connection");

    return _state->connection->endOutput();
}


void Stream::cancel()
{
    if(_state && _state->connection)
        _state->connection->cancel();
}


void Stream::setTimeout(std::size_t ms)
{
    if( ! _state || ! _state->connection )
        throw std::logic_error("HTTP stream has no connection");

    _state->connection->setStreamTimeout(ms);
}


Signal<>& Stream::inputReady()
{
    return _inputReady;
}


Signal<>& Stream::outputReady()
{
    return _outputReady;
}


Signal<>& Stream::closed()
{
    return _closed;
}


void Stream::attach()
{
    if( ! _state )
        return;

    ++_state->handles;

    if( ! _state->connection )
        return;

    _state->connection->inputReady() += Pt::slot(*this, &Stream::onInput);
    _state->connection->outputReady() += Pt::slot(*this, &Stream::onOutput);
}


void Stream::detach()
{
    if( ! _state || ! _state->connection )
        return;

    _state->connection->inputReady() -= Pt::slot(*this, &Stream::onInput);
    _state->connection->outputReady() -= Pt::slot(*this, &Stream::onOutput);
}


void Stream::release()
{
    if( ! _state )
        return;

    StreamState* state = _state;
    _state = 0;

    if(state->handles > 0)
        --state->handles;

    if(state->handles == 0 && state->connection == 0)
        delete state;
}


void Stream::onInput(Pt::System::IOBuffer&)
{
    _inputReady.send();
}


void Stream::onOutput(Pt::System::IOBuffer&)
{
    _outputReady.send();
}

} // namespace Http

} // namespace Pt
