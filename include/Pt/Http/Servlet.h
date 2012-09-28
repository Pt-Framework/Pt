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
#include <cstddef>

namespace Pt {

namespace Http {

class Authentication;
class Challenge;
class Request;
class Reply;
class Responder;
class Server;
class Service;

class Servlet2 : private NonCopyable
{
    // @internal
    friend class Server;

    public:
        // ServiceDispatch, ServiceRoute
        class Mapping
        {
            public:
                virtual ~Mapping()
                {}

                virtual bool map(const Request& request) = 0;
        };

        template <typename PredicateT>
        class MapIf : public Mapping
        {
            public:
                MapIf(PredicateT p)
                : _p(p)
                {}

                bool map(const Request& request)
                { return _p(request); }

            private:
                PredicateT _p;
        };

    public:
        template <typename PredicateT>
        Servlet2(const PredicateT& map, Service& s, Authentication& a)
        : _server(0)
        , _mapping(0)
        , _service(0)
        , _auth(0)
        {
            _mapping = new MapIf<PredicateT>(map);
        }
        
        Servlet2(const std::string& url, Service& s);

        ~Servlet2();

        void setShutdown(bool shutdown = true);

        bool isIdle();

        void detach();

        bool isMapped(const Request& request) const
        { return _mapping->map(request); }

        Service* service()
        { return _service; }

        Authentication* authentication()
        { return _auth; }

    protected:
        // @internal
        void registerServer(Server& server);
        
        // @internal
        void unregisterServer(Server& server);

    private:
        Server* _server;
        std::size_t _useCount;
        Mapping* _mapping;
        Service* _service;
        Authentication* _auth;
};

} // namespace Http

} // namespace Pt

#endif

