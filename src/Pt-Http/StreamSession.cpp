/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/StreamSession.h>
#include <Pt/Http/Stream.h>

#include <stdexcept>

namespace Pt {

namespace Http {

StreamSession::StreamSession()
: _stream(0)
{
}


StreamSession::StreamSession(Stream& stream)
: _stream(0)
{
    open(stream);
}


StreamSession::~StreamSession()
{
    close();
}


void StreamSession::open(Stream& stream)
{
    if(_stream)
        throw std::logic_error("HTTP stream session is already open");

    stream.openSession(*this);
    _stream = &stream;
}


void StreamSession::close()
{
    if( ! _stream )
        return;

    Stream* stream = _stream;
    _stream = 0;

    stream->closeSession(*this);
    stream->close();
}


void StreamSession::closeStream(Stream& stream)
{
    _stream = 0;
    onCloseStream(stream);
}


void StreamSession::onCloseStream(Stream& stream)
{
}

} // namespace Http

} // namespace Pt