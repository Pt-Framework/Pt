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
#include <Pt/NonCopyable.h>
#include <string>
#include <vector>

namespace Pt {

namespace Http {

class Request;
class Reply;

/** @brief HTTP authentication for clients.
*/
class Authentication
{
    public:
        //! @brief Credentials for realms.
        typedef std::map<std::string, Credential> Credentials;

    public:
        /** @brief Construct with type name.

            The @a name is the type name of the authentication 
            method, such as "basic".
        */
        Authentication(const std::string& name)
        : _name(name) 
        {}

        /** @brief Construct with type name.

            The @a name is the type name of the authentication 
            method, such as "basic".
        */
        Authentication(const char* name)
        : _name(name) 
        {}

        /** @brief Destructor.
        */
        virtual ~Authentication()
        {}

        /** @brief Returns the type name of the authentication.
        */
        const std::string& name() const
        { return _name; }

        /** @brief Authenticate a request in response to a reply.

            Authenticates the @a request using the @a credentials in response
            to the corresponding @a reply.
        */
        virtual bool authenticate(const Credentials& credentials, Request& request, const Reply& reply) = 0;

    private:
        std::string _name;
};

/** @brief Basic HTTP authentication for clients.
*/
class PT_HTTP_API BasicAuthentication : public Authentication
{
    public:
        /** @brief Default Constructor.
        */
        BasicAuthentication()
        : Authentication("basic")
        {}

        /** @brief Destructor.
        */
        virtual ~BasicAuthentication()
        {}

        void preAuthenticate(const Credential& credential, Request& request);

        // inheric docs
        virtual bool authenticate(const Credentials& credentials, Request& request, const Reply& reply);
};

/** @brief %Client side authentication.
*/
class PT_HTTP_API Authenticator : private NonCopyable
{
    public:
        //! @brief Credentials for realms.
        typedef std::map<std::string, Credential> Credentials;

    public:
        /** @brief Default Constructor.
        */
        Authenticator();

        /** @brief Destructor.
        */
        ~Authenticator();

        /** @brief Adds an authentication method.
        */
        void addAuthentication(Authentication& auth);
        
        /** @brief Set credential for a realm.
        */
        void setCredential(const std::string& realm, const Credential& cred);

        /** @brief Authenticate a request in response to a reply.
        */
        bool authenticate(Request& request, const Reply& reply);

    private:
        Credentials _credentials;
        std::vector<Authentication*> _auths;
        BasicAuthentication _basicAuth;
};

} // namespace Http

} // namespace Pt

#endif // Pt_Http_Authenticator_h
