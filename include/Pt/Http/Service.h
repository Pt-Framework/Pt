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
#include <Pt/Signal.h>
#include <vector>
#include <memory>
#include <string>

#include <Pt/Http/Reply.h>
#include <Pt/Http/Request.h>
#include <Pt/TextStream.h>
#include <Pt/Base64Codec.h>
#include <map>
#include <sstream>

namespace Pt {

namespace Http {

class Responder;
class RequestHeader;
class ReplyHeader;

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
        System::Mutex _mutex;
        std::vector<Server*> _servers;
        bool _shutdown;
        unsigned _responderCount;
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


class Challenge
{
    public:
        Challenge()
        {}

        virtual ~Challenge() 
        { }

        Signal<Challenge&>& finished()
        { return _finished; }

        virtual void beginVerify() = 0;
        
        virtual bool endVerify()= 0;

    protected:
        void setReady()
        { _finished.send(*this); }

    private:
        Signal<Challenge&> _finished;
};


class Authentication
{
    public:
        Authentication(const std::string realm)
        : _realm(realm)
        { }

        virtual ~Authentication() 
        { }
        
        const std::string& realm() const
        { return _realm; }

        virtual Challenge* beginAuthenticate(const Request& req, Reply& reply) = 0;

        virtual bool endAuthenticate(Challenge* challenge, const Request& req, Reply& reply) = 0;

        virtual void cancelAuthenticate(Challenge* challenge) = 0;

    private:
        std::string _realm;
};


class BasicAuthentication : public Authentication
{
    class FailedChallenge : public Challenge
    {
        public:
            FailedChallenge()
            {}
            
            virtual void beginVerify()
            { setReady(); }

            virtual bool endVerify()
            { return false; }
    };

    public:
        BasicAuthentication(const std::string realm)
        : Authentication(realm)
        { }

        ~BasicAuthentication()
        { }

        void setUser(const std::string& user, const std::string& passwd)
        { _passwd[user] = passwd; }

        void removeUser(const std::string& user)
        { _passwd.erase(user); }

        void clear()
        { _passwd.clear(); }

        virtual Challenge* beginAuthenticate(const Request& req, Reply& reply)
        {
            std::string user, passwd;

            const char* auth = req.header().getHeader("Authorization");
            if( auth )
            {
                std::istringstream iss(auth);

                std::string type;
                iss >> type;

                for(std::string::size_type n = 0; n < type.size(); ++n)
                    type[n] = std::tolower(type[n]);

                if(type == "basic")
                {
                    iss >> std::skipws;

                    BasicTextIStream<char, char> b64conv(iss, new Base64Codec());
                    std::getline(b64conv, user, ':');
                    b64conv >> passwd;

                    std::map<std::string, std::string>::iterator it = _passwd.find(user);
                    if(it != _passwd.end() && it->second == passwd)
                    {
                        return 0;
                    }
                }
            }

            Challenge* challenge = new FailedChallenge;
            return challenge;
        }

        virtual bool endAuthenticate(Challenge* challenge, const Request& req, Reply& reply)
        {
            if( ! challenge )
                return false;

            bool granted = challenge->endVerify();
            cancelAuthenticate(challenge);

            if( ! granted )
            {
                reply.header().httpReturn(401, "Not Authorized");
                reply.header().setHeader("WWW-Authenticate", ("Basic realm=\"" + realm() + '"').c_str());
                reply.finish();
            }

            return granted;
        }

        virtual void cancelAuthenticate(Challenge* challenge)
        {
            delete challenge;
        }

    private:
        std::map<std::string, std::string> _passwd;
};

} // namespace Http

} // namespace Pt

#endif
