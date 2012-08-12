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

#include <Pt/Http/Responder.h>
#include <Pt/Http/Reply.h>
#include <Pt/System/Logger.h>

log_define("Pt.Http.Responder")

namespace Pt {

namespace Http {

Responder::Responder(Service& service)
: _service(service)
{ }


Responder::~Responder() 
{ }


void Responder::beginRequest(std::istream& in, RequestHeader& request)
{
}


std::size_t Responder::readBody(std::istream& in, Reply& reply)
{
    std::streambuf* sb = in.rdbuf();

    std::size_t ret = 0;
    while (sb->in_avail() > 0)
    {
        sb->sbumpc();
        ++ret;
    }

    return ret;
}

// TODO: remove this method, its only here for backwards compatibility
void Responder::reply(std::ostream& os, RequestHeader&, Reply& reply)
{ 
    replyError(os, reply); 
}


void Responder::beginReply(std::ostream& os, RequestHeader& request, Http::Reply& reply)
{
    this->reply(os, request, reply);
    reply.finish();
}

// TODO: remove this method, responder specific errors are handled in 
//       beginReply, otherwise http server knows better what to reply
//       as error e.g. corrupt header, bad alloc, system error...
//       This method was only neccessary because exceptions from beginReply
//       where catched in the server and then forwarded to this method.
void Responder::replyError(std::ostream& out, Reply& reply)
{
    reply.httpReturn(500, "internal server error");
    reply.setHeader("Content-Type", "text/plain");
    reply.setHeader("Connection", "close");
    out << "Error 500";
}


void Responder::release()     
{ 
    _service.doReleaseResponder(this); 
}

} // namespace Http

} // namespace Pt
