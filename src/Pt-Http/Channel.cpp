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

    stream.attachChannel(*this);
    _stream = &stream;
}


void Channel::close()
{
    if( ! _stream )
        return;

    _stream->close();
    _stream->detachChannel(*this);
    _stream = 0;
}


void Channel::closeStream(Stream& stream)
{
    onCloseStream(stream);
}


void Channel::onCloseStream(Stream& stream)
{
}

} // namespace Http

} // namespace Pt