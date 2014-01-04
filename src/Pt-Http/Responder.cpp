/*
 * Copyright (C) 2012 by Marc Boris Duerner
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
#include <Pt/Http/Request.h>
#include <Pt/System/Logger.h>

log_define("Pt.Http.Server.Responder")

namespace Pt {

namespace Http {

Responder::Responder(Service& s)
: _service(s)
{ }


Responder::~Responder() 
{ }


void Responder::beginRequest(Request& request, Reply& reply, System::EventLoop& loop)
{    
    onBeginRequest(request, reply, loop);
}


void Responder::readRequest(Request& request, Reply& reply, System::EventLoop& loop)
{
    onReadRequest(request, reply, loop);

    // ignore everything the responder didn't consume
    std::streambuf* sb = request.body().rdbuf();
    if(sb)
    {
        // cannot use std::streambuf::ignore, because on some implementations
        // undeflow will be called when the last character is extracted
        
        std::streamsize n = sb->in_avail();
        while(n--)
            sb->sbumpc();
    }
}


void Responder::beginReply(const Request& request, Reply& reply, System::EventLoop& loop)
{ 
    onBeginReply(request, reply, loop);
}


void Responder::writeReply(const Request& request, Reply& reply, System::EventLoop& loop)
{ 
    onWriteReply(request, reply, loop); 
}

} // namespace Http

} // namespace Pt
