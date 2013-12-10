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

class PT_HTTP_API Authorization : private Pt::NonCopyable
{
    public:
        virtual ~Authorization();

        void beginAuthorize(const Request& req, Reply& reply);

        bool endAuthorize();

        Signal<Authorization&>& finished();

    protected:
        Authorization();

        void setReady();

        virtual void onBeginAuthorize(const Request& req, Reply& reply) = 0;

        virtual bool onEndAuthorize() = 0;

    private:
        Signal<Authorization&> _finished;
};


class PT_HTTP_API Authorizer : private Pt::NonCopyable
{
    public:
        Authorizer(const std::string& realm);

        Authorizer(const char* realm);

        virtual ~Authorizer();
       
        const std::string& realm() const;

        Authorization* beginAuthorize(const Request& req, Reply& reply, bool& granted);

        bool endAuthorization(Authorization* auth);

        void cancelAuthorization(Authorization* auth);

    protected:
        virtual Authorization* onBeginAuthorize(const Request& req, Reply& reply, bool& granted) = 0;

        virtual void onReleaseAuthorization(Authorization* auth) = 0;

    private:
        atomic_t _useCount;
        std::string _realm;
};


class PT_HTTP_API BasicAuthorizer : public Authorizer
{
    public:
        BasicAuthorizer(const std::string& realm);

        BasicAuthorizer(const char* realm);

        ~BasicAuthorizer();

    protected:
        virtual Authorization* onBeginAuthorize(const Request& req, Reply& reply, bool& granted);

        virtual Authorization* onAuthorizeCredentials(const Credential& cred, bool& granted) = 0;
};


class PT_HTTP_API BasicUserListAuthorizer : public BasicAuthorizer
{
    public:
        BasicUserListAuthorizer(const std::string& realm);

        BasicUserListAuthorizer(const char* realm);

        ~BasicUserListAuthorizer();

        void setUser(const Credential& cred);

        void removeUser(const std::string& user);

        void removeUser(const char* user);

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
