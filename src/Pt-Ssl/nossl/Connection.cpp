/*
 * Copyright (C) 2010-2012 by Marc Boris Duerner
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

#include "Connection.h"
#include <Pt/Ssl/SslError.h>
#include <Pt/System/Logger.h>
#include <streambuf>

log_define("Pt.Ssl.StreamBuffer")

namespace Pt {

namespace Ssl {

Connection::Connection(Context& ctx, std::ios& ios, OpenMode omode)
: _ctx(&ctx)
, _ios(&ios)
{
}


Connection::~Connection()
{
}


const char* Connection::currentCipher() const
{
    return "NONE";
}


bool Connection::writeHandshake()
{
    log_trace("Connection::writeHandshake");
    throw HandshakeFailed("SSL handshake failed");

    return false;
}


bool Connection::readHandshake()
{
    log_trace("Connection::readHandshake");
    throw HandshakeFailed("SSL handshake failed");
}


bool Connection::shutdown()
{
    log_debug("shutdown failed");
    throw SslError("shutdown failed");
    
    return false;
}


bool Connection::isShutdown() const
{
    return false;
}


bool Connection::isClosed() const
{   
    return true;
}


std::streamsize Connection::write(const char* buf, std::size_t n)
{
    log_trace("Connection::write");
    throw SslError("encoding failed");
}


std::streamsize Connection::read(char* buf, std::size_t n, std::streamsize maxImport)
{
    log_trace("Connection::read");
    throw SslError("decoding failed");
}

} // namespace Ssl

} // namespace Pt
