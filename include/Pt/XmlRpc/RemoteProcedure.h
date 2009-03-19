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
#include <Pt/XmlRpc/TypeHandler.h>
#include <Pt/Signal.h>
#include <string>

namespace Pt {

namespace XmlRpc {

class IRemoteProcedure
{
    friend class Client;

    public:
        IRemoteProcedure(const std::string& name)
        :_name(name)
        { }

        virtual ~IRemoteProcedure()
        { }

        const std::string& name() const
        { return _name; }

    protected:
        virtual void onFinished() = 0;

    private:
        std::string _name;
};


template <typename R,
          typename A1,
          typename A2 >
class RemoteProcedure : public IRemoteProcedure
{
    public:
        RemoteProcedure(Client& service, const std::string& name)
        : IRemoteProcedure(name)
        , _service(&service)
        { }

        ~RemoteProcedure()
        {}

        void begin(const A1& a1, const A2& a2)
        {
            _a1handler.begin(a1);
            _a2handler.begin(a2);
            _rhandler.begin(_result);
            _service->beginCall(_rhandler, *this, _a1handler, _a2handler);
        }

        const R& call(const A1& a1, const A2& a2)
        {
            _service->call(_rhandler, *this, _a1handler, _a2handler);
            return _result;
        }

        const R& operator()(const A1& a1, const A2& a2)
        {
            return this->call(a1, a2);
        }

        const R& result()
        { return _result; }

        Signal<const R&> finished;

    protected:
        void onFinished()
        { finished.send(_result); }

    private:
        Client* _service;
        R _result;
        TypeHandler<R> _rhandler;
        TypeHandler<A1> _a1handler;
        TypeHandler<A2> _a2handler;
};

}

}

#endif
