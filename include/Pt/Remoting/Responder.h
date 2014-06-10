/*
 * Copyright (C) 2009-2014 by Dr. Marc Boris Duerner
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

#ifndef Pt_Remoting_Responder_h
#define Pt_Remoting_Responder_h

#include <Pt/Remoting/Api.h>
#include <Pt/System/EventLoop.h>
#include <Pt/SerializationContext.h>
#include <Pt/Composer.h>
#include <Pt/Decomposer.h>
#include <Pt/NonCopyable.h>
#include <Pt/Types.h>

namespace Pt {

namespace Remoting {

class ServiceDefinition;
class ServiceProcedure;

/** @brief Dispatches requests to a service procedure.
*/
class PT_REMOTING_API Responder : private NonCopyable
{
    public:
        /** @brief Construct with Service.
        */
        explicit Responder(ServiceDefinition& serviceDef);

        /** @brief Destructor.
        */
        virtual ~Responder();

        //! @internal
        SerializationContext& context()
        { return _context; }

        //! @internal
        void setReady()
        { this->onReady(); }

        /** @brief Resets to initial state.
        */
        void cancel();

        /** @brief The currently executing procedure.
        */
        const ServiceProcedure* activeProcedure() const
        { return _proc; }

        virtual bool isFailed() const = 0;

    protected:
        /** @brief Sets the service procedure.
        */
        Pt::Composer** setProcedure(const std::string& name);

        /** @brief Begins the service procedure call.
        */
        void beginCall(System::EventLoop& loop);

        /** @brief Ends the service procedure call.
        */
        Pt::Decomposer* endCall();

    protected:
        /** @brief Cancels all operations.

            Derived responders implement this method to cancel all operations.
        */
        virtual void onCancel() = 0;

        /** @brief The service procedure has finished.

            Derived responders implement this method to format and send the
            XML-RPC result. It is called when the service procedure has
            finished. Use beginResult(), advanceResult() and finishResult()
            to format the XML-RPC result.
        */
        virtual void onReady() = 0;

    private:
        SerializationContext _context;
        ServiceDefinition* _serviceDef;
        ServiceProcedure* _proc;
};

} // namespace Remoting

} // namespace Pt

#endif // Pt_Remoting_Responder_h
