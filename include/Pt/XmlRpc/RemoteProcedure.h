/*
 * Copyright (C) 2009 by Dr. Marc Boris Duerner
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
#ifndef Pt_XmlRpc_RemoteProcedure_h
#define Pt_XmlRpc_RemoteProcedure_h

#include <Pt/XmlRpc/Api.h>
#include <Pt/XmlRpc/Client.h>
#include <Pt/XmlRpc/Fault.h>
#include <Pt/XmlRpc/Result.h>
#include <Pt/SerializationContext.h>
#include <Pt/Deserializer.h>
#include <Pt/Serializer.h>
#include <Pt/Signal.h>
#include <string>

namespace Pt {

namespace XmlRpc {

class Fault;

class PT_XMLRPC_API IRemoteProcedure
{
    friend class ClientImpl;

    public:
        IRemoteProcedure(Client& client, const String& name)
        : _client(&client)
        , _name(name)
        { }

        IRemoteProcedure(Client& client, const std::string& name)
        : _client(&client)
        , _name(String::widen(name))
        { }

        IRemoteProcedure(Client& client, const char* name)
        : _client(&client)
        , _name(String::widen(name))
        { }

        virtual ~IRemoteProcedure()
        { cancel(); }

        Client& client()
        { return *_client; }

        const String& name() const
        { return _name; }

        virtual void setFault(int rc, const std::string& msg) = 0;

        virtual bool failed() const = 0;

        void cancel()
        {
            if (_client && _client->activeProcedure() == this)
                _client->cancel();
        }

    protected:
        virtual void onFinished() = 0;

    private:
        Client* _client;
        String _name;
};


template <typename R>
class RemoteProcedureBase : public IRemoteProcedure
{
    public:
        RemoteProcedureBase(Client& client, const std::string& name)
        : IRemoteProcedure(client, name),
          _result(client),
          _r( & client.context() )
        { }

        void setFault(int rc, const std::string& msg)
        {
            _result.setFault(rc, msg);
        }

        const R& result()
        {
            return _result.get();
        }

        virtual bool failed() const
        {
            return _result.failed();
        }

        Signal< const Result<R> & > finished;

    protected:
        void onFinished()
        { finished.send(_result); }

        /// TODO
        //SerializationContext _context;
        Result<R> _result;
        Composer<R> _r;
};


}

}

#include <Pt/XmlRpc/RemoteProcedure.tpp>

#endif
