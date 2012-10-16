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
#include <Pt/System/Mutex.h>
#include <Pt/Signal.h>
#include <string>
#include <map>

namespace Pt {

namespace Http {

class Request;
class Reply;

class PT_HTTP_API Challenge : public Pt::NonCopyable
{
    public:
        virtual ~Challenge();

        Signal<Challenge&>& finished();
       
        bool getResult();

    protected:
        Challenge()
        {}

        void setReady();

        virtual bool onGetResult() = 0;

    private:
        Signal<Challenge&> _finished;
};


class PT_HTTP_API Authentication : public Pt::NonCopyable
{
    public:
        Authentication(const std::string& realm);

        virtual ~Authentication();
       
        const std::string& realm() const
        { return _realm; }

        // TODO: isAuthentic
        bool authenticate(const Request& req, Reply& reply);

        // TODO: beginAuthentication
        Challenge* beginChallenge(const Request& req, Reply& reply);

        bool endChallenge(Challenge* challenge, const Request& req, Reply& reply);

        void cancelChallenge(Challenge* challenge);

    protected:
        virtual bool onAuthenticate(const Request& req, Reply& reply) = 0;

        virtual Challenge* onBeginChallenge(const Request& req, Reply& reply) = 0;

        virtual bool onEndChallenge(Challenge* challenge, const Request& req, Reply& reply) = 0;

        virtual void onDestroyChallenge(Challenge* challenge) = 0;

        System::Mutex& mutex()
        { return _mutex; }

    private:
        System::Mutex _mutex;
        std::size_t _useCount;
        std::string _realm;
};


class PT_HTTP_API BasicAuthentication : public Authentication
{
    public:
        BasicAuthentication(const std::string& realm)
        : Authentication(realm)
        { }

        ~BasicAuthentication()
        { clear(); }

        void setUser(const std::string& user, const std::string& passwd)
        { 
            System::MutexLock lock( mutex() );
            _passwd[user] = passwd; 
        }

        void removeUser(const std::string& user)
        { 
            System::MutexLock lock( mutex() );
            _passwd.erase(user); 
        }

        void clear()
        { 
            System::MutexLock lock( mutex() );
            _passwd.clear(); 
        }

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
