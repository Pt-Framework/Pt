/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/Channel.h>
#include <Pt/Http/Stream.h>

#include <stdexcept>

namespace Pt {

namespace Http {

Channel::Channel()
: _stream(0)
{
}


Channel::Channel(Stream& stream)
: _stream(0)
{
    open(stream);
}


Channel::~Channel()
{
    close();
}


void Channel::open(Stream& stream)
{
    if(_stream)
        throw std::logic_error("HTTP channel is already open");

    stream.openChannel(*this);
    _stream = &stream;
}


void Channel::close()
{
    if( ! _stream )
        return;

    Stream* stream = _stream;
    _stream = 0;

    stream->closeChannel(*this);
    stream->close();
}


void Channel::closeStream(Stream& stream)
{
    _stream = 0;
    onCloseStream(stream);
}


void Channel::onCloseStream(Stream& stream)
{
}

} // namespace Http

} // namespace Pt