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

#include <Pt/Http/Authenticator.h>
#include <Pt/Http/Reply.h>
#include <Pt/Http/Request.h>
#include <Pt/TextStream.h>
#include <Pt/Base64Codec.h>
#include <Pt/StreamBuffer.h>
#include <sstream>

namespace Pt {

namespace Http {

bool BasicAuthentication::authenticate(Credentials& credentials, Request& request, const Reply& reply) const
{ 
    const char* auth = reply.header().get("WWW-Authenticate");
    if( ! auth)
        return true;

    std::string token;
    std::istringstream iss(auth);
    iss >> token;

    for(std::string::size_type n = 0; n < token.size(); ++n)
            token[n] = std::tolower(token[n]);

    if(token != "basic")
        return false;

    getline(iss, token, '"');
    getline(iss, token, '"');

    Credentials::iterator it = credentials.find(token);
    if( it == credentials.end() )
        return false;

    std::ostringstream oss;
    oss << "Basic ";
               
    BasicTextOStream<char, char> b64(oss, new Base64Codec());
    b64 << it->second.user() << ':' << it->second.password();
    b64.terminate();

    //log_debug("set Authorization to " << oss.str());
    request.header().set("Authorization", oss.str().c_str());
    return true; 
}


Authenticator::Authenticator()
{
    addAuthentication(_basicAuth);
}
     

Authenticator::~Authenticator()
{
}


void Authenticator::addAuthentication(const Authentication& auth)
{
    _auths.push_back(&auth);
}


void Authenticator::setCredential(const std::string& realm, const Credential& cred)
{ 
    _credentials[realm] = cred; 
}


bool Authenticator::authenticate(Request& request, const Reply& reply)
{ 
    const char* auth = reply.header().get("WWW-Authenticate");
    if( ! auth)
        return true;

    std::string authType;
    std::istringstream iss(auth);
    iss >> authType;

    for(std::string::size_type n = 0; n < authType.size(); ++n)
        authType[n] = std::tolower(authType[n]);

    std::vector<const Authentication*>::const_iterator it;
    for(it = _auths.begin(); it != _auths.end(); it++)
    {
        if(authType == (*it)->name())
        {
            return (*it)->authenticate(_credentials, request, reply);
        }
    }
    
    return false;
}

} // namespace Http

} // namespace Pt
