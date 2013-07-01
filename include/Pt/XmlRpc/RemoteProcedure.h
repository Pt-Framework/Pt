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

class PT_XMLRPC_API RemoteCall
{
    public:
        RemoteCall(Client& client, const String& name)
        : _client(&client)
        , _name(name)
        { }

        RemoteCall(Client& client, const std::string& name)
        : _client(&client)
        , _name(String::widen(name))
        { }

        RemoteCall(Client& client, const char* name)
        : _client(&client)
        , _name(String::widen(name))
        { }

        virtual ~RemoteCall()
        { cancel(); }

        Client& client()
        { return *_client; }

        const String& name() const
        { return _name; }

        bool isFailed() const
        {
            return _client->isFailed();
        }

        void cancel()
        {
            if( _client->activeProcedure() == this )
                _client->cancel();
        }

        void finish()
        {
            this->onFinished();
        }

    protected:
        virtual void onFinished() = 0;

    private:
        Client* _client;
        String _name;
};


template <typename R>
class RemoteProcedureBase : public RemoteCall
{
    public:
        RemoteProcedureBase(Client& client, const std::string& name)
        : RemoteCall(client, name),
          _result(client),
          _r( & client.context() )
        { }

        R& result()
        { 
            return _result.value(); 
        }

        const R& getResult()
        {
            return _result.get();
        }

        Signal< const Result<R>& >& finished()
        { return _finished; }

    protected:
        void onFinished()
        { _finished.send(_result); }

        Signal< const Result<R>& > _finished;
        Result<R> _result;
        Composer<R> _r;
};

}

}

#include <Pt/XmlRpc/RemoteProcedure.tpp>

#endif
