/*
 * Copyright (C) 2011 by Marc Boris Duerner
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

#ifndef Pt_Http_Servlet_h
#define Pt_Http_Servlet_h

#include <Pt/Http/Api.h>
#include <Pt/NonCopyable.h>
#include <string>

namespace Pt {

namespace Http {

class Authorizer;
class Request;
class Reply;
class Server;
class Service;

class PT_HTTP_API Servlet : private NonCopyable
{
    // @internal
    friend class Server;

    public:
        Servlet(Service& s);

        Servlet(Service& s, Authorizer& a);

        virtual ~Servlet();

        void setShutdown(bool shutdown = true);

        bool isIdle();

        void detach();

        bool isMapped(const Request& request) const
        { return this->onRequest(request); }

        Service* service()
        { return _service; }

        Authorizer* authorizer()
        { return _auth; }

    protected:
        /** @brief Returns true if the servlet should process the request.
        */
        virtual bool onRequest(const Request& request) const = 0;

    private:
        // @internal
        void registerServer(Server& server);
        
        // @internal
        void unregisterServer(Server& server);

    private:
        Server* _server;
        Service* _service;
        Authorizer* _auth;
};


class PT_HTTP_API MapUrl : public Servlet
{
    public:
        MapUrl(const std::string& url, Service& s)
        : Servlet(s)
        , _url(url)
        {}

        MapUrl(const std::string& url, Service& s, Authorizer& a)
        : Servlet(s, a)
        , _url(url)
        {}

    protected:
        bool onRequest(const Request& request) const;

    private:
        std::string _url;
};


class PT_HTTP_API MapAny : public Servlet
{
    public:
        MapAny(Service& s)
        : Servlet(s)
        {}

        MapAny(Service& s, Authorizer& a)
        : Servlet(s, a)
        {}

    protected:
        bool onRequest(const Request& request) const;
};

} // namespace Http

} // namespace Pt

#endif // Pt_Http_Servlet_h
