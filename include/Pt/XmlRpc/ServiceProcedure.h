/*
 * Copyright (C) 2009-2013 by Marc Duerner
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

#ifndef PT_XMLRPC_SERVICEPROCEDURE_H
#define PT_XMLRPC_SERVICEPROCEDURE_H

#include <Pt/XmlRpc/Api.h>
#include <Pt/System/EventLoop.h>
#include <Pt/Decomposer.h>
#include <Pt/Composer.h>

namespace Pt {

namespace XmlRpc {

class Responder;

/** @brief XML-RPC service procedure.
*/
class ServiceProcedure
{
    public:
        /** @brief Destructor.
        */
        virtual ~ServiceProcedure()
        {}

        /** @brief Indicates that the procedure has finished.
        */
        void setReady()
        { _responder->endCall(); }

        //! @internal
        Composer** beginArgs()
        { return onBeginArgs(); }

        //! @internal
        void beginCall(System::EventLoop& loop)
        { onBeginCall(loop); }

        //! @internal
        Decomposer* endCall()
        { return onEndCall(); }

    protected:
        /** @brief Constructor.
        */
        ServiceProcedure(Responder& r)
        : _responder( &r )
        {}

        //! @internal
        virtual Composer** onBeginArgs() = 0;

        //! @internal
        virtual void onBeginCall(System::EventLoop& loop) = 0;

        //! @internal
        virtual Decomposer* onEndCall() = 0;  

    private:
        Responder* _responder;
};


class ServiceProcedureDef
{
    public:
        virtual ~ServiceProcedureDef()
        {}

        ServiceProcedure* createProcedure(Responder& r) const
        { return this->onCreateProcedure(r); }

    protected:
        ServiceProcedureDef()
        {}

        virtual ServiceProcedure* onCreateProcedure(Responder& r) const = 0;
};

} // namespace XmlRpc

} // namespace Pt

#endif // PT_XMLRPC_SERVICEPROCEDURE_H

#include <Pt/XmlRpc/BasicProcedure.h>
#include <Pt/XmlRpc/ActiveProcedure.h>
