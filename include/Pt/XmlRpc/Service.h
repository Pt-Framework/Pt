/*
 * Copyright (C) 2009 by Dr. Marc Boris Duerner
 * Copyright (C) 2009 by Tommi Meakitalo
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
#ifndef Pt_XmlRpc_Service_h
#define Pt_XmlRpc_Service_h

#include <Pt/XmlRpc/Api.h>
#include <Pt/XmlRpc/Responder.h>
#include <Pt/Http/Service.h>
#include <Pt/Http/Responder.h>
#include <Pt/System/EventLoop.h>
#include <Pt/System/Mutex.h>
#include <Pt/Decomposer.h>
#include <Pt/Composer.h>
#include <Pt/Void.h>
#include <Pt/TypeTraits.h>
#include <string>
#include <map>

namespace Pt {

namespace XmlRpc {

// TODO: derive Responder from this class or find other way to make ownership
//       more explicit and clearer.
class Context
{
    public:
        Context(SerializationContext& ctx, XmlRpcResponder& r, System::EventLoop& loop)
        : _ctx()
        , _resp(&r)
        , _loop(&loop)
        {}

        SerializationContext& sctx()
        { return *_ctx; }

        System::EventLoop& loop()
        { return *_loop; }

        XmlRpcResponder& responder()
        { return *_resp; }

    private:
        SerializationContext* _ctx;
        XmlRpcResponder* _resp;
        System::EventLoop* _loop;
};

class ServiceProcedure
{
    public:
        virtual ~ServiceProcedure()
        {}

        System::EventLoop& loop()
        { return *_loop; }

        void setReady()
        { if(_resp) _resp->endReply(); }

        virtual IComposer** beginArgs() = 0;

        virtual void beginCall() = 0;

        virtual IDecomposer* endCall() = 0;      

    protected:
        ServiceProcedure(Context& ctx)
        : _resp( &ctx.responder() )
        , _loop( &ctx.loop() )
        {}

    private:
        XmlRpcResponder* _resp;
        System::EventLoop* _loop;
};


class ServiceProcedureDef
{
    public:
        virtual ~ServiceProcedureDef()
        {}

        ServiceProcedure* createProcedure(Context& ctx) const
        { return this->onCreateProcedure(ctx); }

    protected:
        ServiceProcedureDef()
        {}

        virtual ServiceProcedure* onCreateProcedure(Context& ctx) const = 0;
};

} // namespace XmlRpc

} // namespace Pt

#include <Pt/XmlRpc/Service.tpp>

#endif
