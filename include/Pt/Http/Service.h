/*
 * Copyright (C) 2009 by Marc Boris Duerner, Tommi Maekitalo
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

#ifndef Pt_Http_Service_h
#define Pt_Http_Service_h

#include <Pt/Http/Api.h>
#include <Pt/Http/Responder.h>
#include <Pt/Http/Server.h>
#include <Pt/Allocator.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/Condition.h>
#include <vector>
#include <memory>
#include <string>

namespace Pt {

namespace Http {

class Responder;
class RequestHeader;
class ReplyHeader;

class Authenticator
{
    public:
        virtual ~Authenticator() 
        { }

        void challengeReply(Request& req, Reply& rep)
        {
            //this->challenge(rep.header());
            //rep.finish();
            //rep.beginSend();
        }
        
        virtual bool authenticate(const RequestHeader&) const = 0;

        virtual void challenge(const RequestHeader&, ReplyHeader&)
        { }
};


class PT_HTTP_API Service
{
    friend class Server;

    public:
        Service();

        virtual ~Service();

        Responder* getResponder(const RequestHeader&);
        
        void releaseResponder(Responder*);

        void setShutdown(bool shutdown = true);

        bool isIdle();

        void detach();

        bool checkAuth(const RequestHeader& request);

        void setRealm(const std::string& realm, const std::string& content = std::string() )
        { 
            _realm = realm; 
            _authContent = content; 
        }

        const std::string& realm() const        
        { return _realm; }
        
        const std::string& authContent() const  
        { return _authContent; }

        void addAuthenticator(const Authenticator* auth)
        { _authenticators.push_back(auth); }

    protected:
        /** @brief Creates a responder to handle request received by a server.
            
            The implementer of this method must also make sure that no 
            responders exists anymore when the derived class is destructed.
            The easiest way to ensure this is to call Service::detach in
            the derived class's destructor.
        */
        virtual Responder* createResponder(const RequestHeader&) = 0;
        
        /** @brief Destroys a responder created by a server.
            
            The implementer of this method must also make sure that no 
            responders exists anymore when the derived class is destructed.
            The easiest way to ensure this is to call Service::detach in
            the derived class's destructor.
        */
        virtual void destroyResponder(Responder*) = 0;

    private:
        // @internal
        void registerServer(Server& server);

        // @internal
        void unregisterServer(Server& server);

    private:
        std::vector<Server*> _servers;
        bool _shutdown;
        unsigned _responderCount;
        std::vector<const Authenticator*> _authenticators;
        std::string _realm;
        std::string _authContent;
        System::Mutex _mutex;
};


template <typename R, typename Alloc = Allocator>
class BasicService : public Service
{
    public:
        BasicService()
        { }

        ~BasicService()
        {
            detach();
        }

    protected:
        virtual Responder* createResponder(const RequestHeader&)
        {
            void* r = _alloc.allocate( sizeof(R) );
            return new(r) R(*this);
        }

        virtual void destroyResponder(Responder* r)
        {
            r->~Responder();
            _alloc.deallocate( r, sizeof(R) );
        }

    private:
        Alloc _alloc;
};

} // namespace Http

} // namespace Pt

#endif
