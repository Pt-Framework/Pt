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

#ifndef Pt_Http_Authentication_h
#define Pt_Http_Authentication_h

#include <Pt/Http/Api.h>
#include <Pt/Signal.h>
#include <string>
#include <map>

namespace Pt {

namespace Http {

class Request;
class Reply;

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


class PT_HTTP_API Authentication
{
    public:
        Authentication(const std::string realm)
        : _realm(realm)
        { }

        virtual ~Authentication() 
        { }
        
        const std::string& realm() const
        { return _realm; }

        virtual Challenge* beginAuthenticate(const Request& req, Reply& reply);

        virtual bool endAuthenticate(Challenge* challenge, const Request& req, Reply& reply);

        virtual void cancelAuthenticate(Challenge* challenge);

    protected:
        virtual bool onAuthenticate(const Request& req, Reply& reply) = 0;

        virtual Challenge* onBeginChallenge(const Request& req, Reply& reply) = 0;

        virtual bool onEndChallenge(Challenge* challenge, const Request& req, Reply& reply) = 0;

        virtual void onDestroyChallenge(Challenge* challenge) = 0;

    private:
        std::string _realm;
};


class PT_HTTP_API BasicAuthentication : public Authentication
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

    protected:
        virtual bool onAuthenticate(const Request& req, Reply& reply);

        virtual Challenge* onBeginChallenge(const Request& req, Reply& reply);

        virtual bool onEndChallenge(Challenge* challenge, const Request& req, Reply& reply);

        virtual void onDestroyChallenge(Challenge* challenge);

    private:
        std::map<std::string, std::string> _passwd;
};

} // namespace Http

} // namespace Pt

#endif
