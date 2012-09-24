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

#include <Pt/Http/Authentication.h>
#include <Pt/Http/Reply.h>
#include <Pt/Http/Request.h>
#include <Pt/TextStream.h>
#include <Pt/Base64Codec.h>
#include <sstream>

namespace Pt {

namespace Http {

Challenge::~Challenge() 
{ 
}


Signal<Challenge&>& Challenge::finished()
{ 
    return _finished; 
}
    
       
void Challenge::setReady()
{ 
    _finished.send(*this); 
}


bool Challenge::getResult()
{ 
    return onGetResult(); 
}


bool Authentication::authenticate(const Request& req, Reply& reply) 
{
    bool granted = this->onAuthenticate(req, reply);
    return granted;
}


Challenge* Authentication::beginChallenge(const Request& req, Reply& reply) 
{
    return this->onBeginChallenge(req, reply);
}


bool Authentication::endChallenge(Challenge* challenge, const Request& req, Reply& reply) 
{
    bool granted = this->onEndChallenge(challenge, req, reply);

    this->onDestroyChallenge(challenge);

    return granted;
}


void Authentication::cancelChallenge(Challenge* challenge) 
{
    this->onDestroyChallenge(challenge);
}


bool BasicAuthentication::onAuthenticate(const Request& req, Reply& reply)
{
    bool granted = false;
    std::string user, passwd, token;

    const char* auth = req.header().get("Authorization");
    if( auth )
    {
        std::istringstream iss(auth);
        iss >> token;

        for(std::string::size_type n = 0; n < token.size(); ++n)
            token[n] = std::tolower(token[n]);

        if(token == "basic")
        {
            iss >> std::skipws >> token;
            iss.str(token);

            BasicTextIStream<char, char> b64conv(iss, new Base64Codec());
            std::getline(b64conv, user, ':');
            b64conv >> passwd;

            std::map<std::string, std::string>::iterator it = _passwd.find(user);
            granted = (it != _passwd.end() && it->second == passwd);
        }
    }

    return granted;
}


Challenge* BasicAuthentication::onBeginChallenge(const Request& req, Reply& reply)
{
    reply.setStatus(401, "Authorization Required");
    reply.header().set("WWW-Authenticate", ("Basic realm=\"" + realm() + '"').c_str());
    reply.finish();
    return 0;
}


bool BasicAuthentication::onEndChallenge(Challenge* challenge, const Request& req, Reply& reply)
{
    if( ! challenge )
        return false;

    bool granted = challenge->getResult();
    if( ! granted )
    {
        reply.setStatus(401, "Authorization Required");
        reply.header().set("WWW-Authenticate", ("Basic realm=\"" + realm() + '"').c_str());
        reply.finish();
    }

    return granted;
}

void BasicAuthentication::onDestroyChallenge(Challenge* challenge)
{
    throw std::logic_error("BasicAuthentication::onDestroyChallenge not implemented");
}

}

}
