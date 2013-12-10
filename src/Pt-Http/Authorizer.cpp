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

#include <Pt/Http/Authorizer.h>
#include <Pt/Http/Reply.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/HttpError.h>
#include <Pt/TextStream.h>
#include <Pt/Base64Codec.h>
#include <Pt/StreamBuffer.h>
#include <sstream>

namespace Pt {

namespace Http {

// Authorization

Authorization::Authorization()
{}


Authorization::~Authorization() 
{ 
}


void Authorization::beginAuthorize(const Request& req, Reply& reply)
{ 
    onBeginAuthorize(req, reply); 
}


bool Authorization::endAuthorize()
{ 
    return onEndAuthorize(); 
}


Signal<Authorization&>& Authorization::finished()
{ 
    return _finished; 
}
    
       
void Authorization::setReady()
{ 
    _finished.send(*this); 
}


// Authorizer

Authorizer::Authorizer(const std::string& realm)
: _useCount(0)
, _realm(realm)
{ }


Authorizer::Authorizer(const char* realm)
: _useCount(0)
, _realm(realm)
{ }


Authorizer::~Authorizer() 
{ }


const std::string& Authorizer::realm() const
{ 
    return _realm; 
}


Authorization* Authorizer::beginAuthorize(const Request& req, Reply& reply, bool& granted) 
{
    granted = false;
    
    Authorization* auth = this->onBeginAuthorize(req, reply, granted);
    if(auth)
        atomicIncrement(_useCount);
    
    return auth;
}


bool Authorizer::endAuthorization(Authorization* auth) 
{
    bool granted = auth->endAuthorize();
    
    this->onReleaseAuthorization(auth);
    atomicDecrement(_useCount);
    
    return granted;
}


void Authorizer::cancelAuthorization(Authorization* auth) 
{
    this->onReleaseAuthorization(auth);
    atomicDecrement(_useCount);
}


// BasicAuthorizer

BasicAuthorizer::BasicAuthorizer(const std::string& realm)
: Authorizer(realm)
{ 
}


BasicAuthorizer::BasicAuthorizer(const char* realm)
: Authorizer(realm)
{ 
}


BasicAuthorizer::~BasicAuthorizer()
{ 
}


Authorization* BasicAuthorizer::onBeginAuthorize(const Request& req, Reply& reply, bool& granted)
{
    std::string token;
    Credential cred;
    Authorization* author = 0;
    granted = false;

    const char* auth = req.header().get("Authorization");
    if(auth)
    {
        std::istringstream iss(auth);
        iss >> token;

        for(std::string::size_type n = 0; n < token.size(); ++n)
            token[n] = std::tolower(token[n]);

        if(token != "basic")
            throw HttpError("invalid HTTP message");

        iss >> std::skipws >> token;
        iss.str(token);

        BasicTextIStream<char, char> b64conv(iss, new Base64Codec());
        std::getline(b64conv, cred.user(), ':');
        b64conv >> cred.password();

        author = onAuthorizeCredentials(cred, granted);
    }

    if( ! author && ! granted )
    {
        reply.setStatus(Reply::Unauthorized, "Authorization Required");
        reply.header().set("WWW-Authenticate", ("Basic realm=\"" + realm() + '"').c_str());
    }

    return author;
}


// BasicUserListAuthorizer

BasicUserListAuthorizer::BasicUserListAuthorizer(const std::string& realm)
: BasicAuthorizer(realm)
{ 
}


BasicUserListAuthorizer::BasicUserListAuthorizer(const char* realm)
: BasicAuthorizer(realm)
{ 
}


BasicUserListAuthorizer::~BasicUserListAuthorizer()
{ 
    clear(); 
}


void BasicUserListAuthorizer::setUser(const Credential& cred)
{ 
    System::MutexLock lock(_mutex);
    _passwd[cred.user()] = cred.password(); 
}


void BasicUserListAuthorizer::removeUser(const std::string& user)
{ 
    System::MutexLock lock(_mutex);
    _passwd.erase(user); 
}


void BasicUserListAuthorizer::removeUser(const char* user)
{ 
    System::MutexLock lock(_mutex);
    _passwd.erase(user); 
}


void BasicUserListAuthorizer::clear()
{ 
    System::MutexLock lock(_mutex);
    _passwd.clear(); 
}


Authorization* BasicUserListAuthorizer::onAuthorizeCredentials(const Credential& cred, bool& granted)
{
    std::map<std::string, std::string>::iterator it = _passwd.find(cred.user());
    granted = (it != _passwd.end() && it->second == cred.password());
    return 0;
}


void BasicUserListAuthorizer::onReleaseAuthorization(Authorization* auth)
{
    throw std::logic_error("BasicUserListAuthorizer::onReleaseAuthorization");
}

} // namespace Http

} // namespace Pt
