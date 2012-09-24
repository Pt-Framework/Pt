/*
 * Copyright (C) 2012 Marc Boris Duerner
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

#include <Pt/Http/Authorization.h>
#include <Pt/Http/Reply.h>
#include <Pt/Http/Request.h>
#include <Pt/TextStream.h>
#include <Pt/Base64Codec.h>
#include <sstream>

namespace Pt {

namespace Http {

Authorization::~Authorization()
{
}


void Authorization::authorize(Request& request, const Reply& reply)
{ 
    onAuthorize(request, reply); 
}


BasicAuthorization::BasicAuthorization(const std::string& user, const std::string& passwd)
: _username(user)
, _password(passwd)
{}



void BasicAuthorization::set(const std::string& user, const std::string& passwd)
{
    _username = user;
    _password = passwd;
}


BasicAuthorization::~BasicAuthorization()
{
}


void BasicAuthorization::onAuthorize(Request& request, const Reply& reply)
{
    std::ostringstream oss;
    oss << "Basic ";
               
    BasicTextOStream<char, char> b64(oss, new Base64Codec());
    b64 <<_username<< ':' << _password;
    b64.terminate();

    //log_debug("set Authorization to " << oss.str());
    request.header().set("Authorization", oss.str().c_str());
}

} // namespace Http

} // namespace Pt
