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

#ifndef Pt_Remoting_Client_h
#define Pt_Remoting_Client_h

#include <Pt/Remoting/Api.h>
#include <Pt/SerializationContext.h>
#include <Pt/Composer.h>
#include <Pt/Decomposer.h>
#include <Pt/NonCopyable.h>
#include <Pt/Types.h>

namespace Pt {

namespace Remoting {

class RemoteCall;

/** @brief A client for remote procedure calls.
*/
class PT_REMOTING_API Client : private NonCopyable
{
    public:
        /** @brief Constructor.
        */
        Client();

        /** @brief Destructor.
        */
        virtual ~Client();

        //! @internal
        SerializationContext& context()
        { return _ctx; }

        //! @internal
        void beginCall(Composer& r, RemoteCall& call, Decomposer** argv, unsigned argc);

        //! @internal
        void endCall();

        //! @internal
        void call(Composer& r, RemoteCall& call, Decomposer** argv, unsigned argc);
        
        //! @internal
        void cancelCall();

        /** @brief The currently executing procedure.
        */
        const RemoteCall* activeProcedure() const
        { return _method; }

        /** @brief Indicates if the procedure has failed.
        */
        virtual bool isFailed() const = 0;

        /** @brief Cancels the currently executing procedure.
        */
        void cancel();

    protected:
        /** @brief Parses the XML-RPC result.

            This method is used by derived Clients after the XML-RPC result 
            has been parsed by parseResult(). The current RemoteProcedure will
            receive completion notification to process the result.
        */
        void setReady();

        virtual void onBeginCall(Composer& r, RemoteCall& method, Decomposer** argv, unsigned argc) = 0;

        virtual void onEndCall() = 0;

        virtual void onCall(Composer& r, RemoteCall& method, Decomposer** argv, unsigned argc) = 0;

        /** @brief Cancels the remote procedure call.

            Derived Clients implement this method to cancel the remote
            procedure call.
        */
        virtual void onCancel() = 0;

    private:
        SerializationContext _ctx;
        RemoteCall* _method;
        Pt::varint_t _r1;
        Pt::varint_t _r2;
};

} // namespace Remoting

} // namespace Pt

#endif // Pt_Remoting_Client_h
