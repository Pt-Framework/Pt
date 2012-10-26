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

#ifndef Pt_Http_Authenticator_h
#define Pt_Http_Authenticator_h

#include <Pt/Http/Api.h>
#include <Pt/Http/Credentials.h>
#include <string>
#include <map>

namespace Pt {

namespace Http {

class Request;
class Reply;

typedef std::map<std::string, Credentials> CredentialsMap;

class PT_HTTP_API Authentication
{
    public:
        Authentication(const std::string& name)
        : _name(name) 
        {}

        virtual ~Authentication();

        const std::string& name() const
        { return _name; }

        virtual bool authenticate(CredentialsMap& credentials, Request& request, const Reply& reply) const = 0;

    private:
        std::string _name;
};


class PT_HTTP_API BasicAuthentication : public Authentication
{
    public:
        BasicAuthentication()
        : Authentication("basic")
        {}

        virtual ~BasicAuthentication()
        {}

        virtual bool authenticate(CredentialsMap& credentials, Request& request, const Reply& reply) const;
};

class PT_HTTP_API Authenticator
{
    public:
        Authenticator();

        void addAuthentication(const Authentication& auth);
        
        void setCredentials(const std::string& realm, const Credentials& cred);
        
        bool authenticate(Request& request, const Reply& reply);

    private:
        std::map<std::string, Credentials> _credentials;
        std::map<std::string, const Authentication*> _auths;
        BasicAuthentication _basicAuth;
};

} // namespace Http

} // namespace Pt

#endif // Pt_Http_Authenticator_h
