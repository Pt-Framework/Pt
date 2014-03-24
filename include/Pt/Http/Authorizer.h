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

#ifndef Pt_Http_Authorizer_h
#define Pt_Http_Authorizer_h

#include <Pt/Http/Api.h>
#include <Pt/Http/Credentials.h>
#include <Pt/System/Mutex.h>
#include <Pt/Atomicity.h>
#include <Pt/Signal.h>
#include <string>
#include <map>

namespace Pt {

namespace Http {

class Request;
class Reply;

/** @brief HTTP authorization operation.
*/
class PT_HTTP_API Authorization : private Pt::NonCopyable
{
    public:
        /** @brief Destructor.
        */
        virtual ~Authorization();

        /** @brief Begin authorization for a reply.
        */
        void beginAuthorize(const Request& req, Reply& reply);

        /** @brief End authorization for a reply.
        */
        bool endAuthorize();

        /** @brief Notifies that authorization is finished.
        */
        Signal<Authorization&>& finished();

    protected:
        /** @brief Default constructor.
        */
        Authorization();

        /** @brief Set authorization to ready state.
        */
        void setReady();

        /** @brief Begin authorization for a reply.
        */
        virtual void onBeginAuthorize(const Request& req, Reply& reply) = 0;

        /** @brief End authorization for a reply.
        */
        virtual bool onEndAuthorize() = 0;

    private:
        Signal<Authorization&> _finished;
};

/** @brief %Server side authorization.
*/
class PT_HTTP_API Authorizer : private Pt::NonCopyable
{
    public:
        /** @brief Construct for a realm.
        */
        Authorizer(const std::string& realm);

        /** @brief Construct for a realm.
        */
        Authorizer(const char* realm);

        /** @brief Destructor.
        */
        virtual ~Authorizer();
       
       /** @brief Returns the realm.
        */
        const std::string& realm() const;

        /** @brief Begin authorization for a reply.
        */
        Authorization* beginAuthorize(const Request& req, Reply& reply, bool& granted);

        /** @brief End authorization.
        */
        bool endAuthorization(Authorization* auth);

        /** @brief Cancel a running authorization.
        */
        void cancelAuthorization(Authorization* auth);

    protected:
        /** @brief Begin authorization for a reply.
        */
        virtual Authorization* onBeginAuthorize(const Request& req, Reply& reply, bool& granted) = 0;

        /** @brief Release authorization operation.
        */
        virtual void onReleaseAuthorization(Authorization* auth) = 0;

    private:
        atomic_t _useCount;
        std::string _realm;
};

/** @brief %Server side basic HTTP authorization.
*/
class PT_HTTP_API BasicAuthorizer : public Authorizer
{
    public:
        /** @brief Construct for a realm.
        */
        BasicAuthorizer(const std::string& realm);
        
        /** @brief Construct for a realm.
        */
        BasicAuthorizer(const char* realm);

        /** @brief Destructor.
        */
        ~BasicAuthorizer();

    protected:
        virtual Authorization* onBeginAuthorize(const Request& req, Reply& reply, bool& granted);

        /** @brief Begin authorization using client credentials.
        */
        virtual Authorization* onAuthorizeCredentials(const Credential& cred, bool& granted) = 0;
};


/** @brief %Server side basic HTTP authorization.
*/
class PT_HTTP_API BasicUserListAuthorizer : public BasicAuthorizer
{
    public:
        /** @brief Construct for a realm.
        */
        BasicUserListAuthorizer(const std::string& realm);

        /** @brief Construct for a realm.
        */
        BasicUserListAuthorizer(const char* realm);

        /** @brief Destructor.
        */
        ~BasicUserListAuthorizer();

        /** @brief Set user credential.
        */
        void setUser(const Credential& cred);

        /** @brief Remove user from list.
        */
        void removeUser(const std::string& user);

        /** @brief Remove user from list.
        */
        void removeUser(const char* user);

        /** @brief Clears all content.
        */
        void clear();

    protected:
        virtual Authorization* onAuthorizeCredentials(const Credential& cred, bool& granted);

        virtual void onReleaseAuthorization(Authorization* auth);

    private:
        System::Mutex _mutex;
        std::map<std::string, std::string> _passwd;
};

} // namespace Http

} // namespace Pt

#endif // Pt_Http_Authorizer_h
