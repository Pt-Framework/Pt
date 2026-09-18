/*
 * Copyright (C) 2012 by Marc Boris Duerner
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

#ifndef Pt_Http_Responder_h
#define Pt_Http_Responder_h

#include <Pt/Http/Api.h>

namespace Pt {

namespace System {
class EventLoop;
}

namespace Http {

class Request;
class Reply;
class Service;
class Acceptor;

/** @brief Handles one HTTP request and writes the reply.

    %Responder is the per-exchange handler in the server model. A
    %Service creates it for a mapped request and releases it when the
    reply has been sent. Derive from it and implement the four
    callbacks, which the server calls in order.

    %onBeginRequest() runs when the request header is available, so
    the responder can inspect fields and prepare the reply.
    %onReadRequest() runs for each chunk of the request body, and may
    be called more than once. %onBeginReply() runs when the request is
    complete and the reply should start. %onWriteReply() runs when a
    previous %Reply::beginSend(false) needs another chunk of the reply
    body, which is how a large reply is written in chunked encoding.

    Finish the reply with %Reply::beginSend() and the completion flag
    set to true. That call may be made from an earlier callback, in
    which case the remaining callbacks are skipped and the rest of the
    request is ignored. %service() is the service that created this
    responder.

    The example is a responder that ignores the request body and
    writes a fixed reply. %beginSend(true) completes the reply in
    %onBeginReply(), so %onWriteReply() stays empty.

    @code
    class HelloResponder : public Pt::Http::Responder
    {
        public:
            explicit HelloResponder(Pt::Http::Service& s)
            : Pt::Http::Responder(s)
            {}

        protected:
            virtual void onBeginRequest(Pt::Http::Request& request,
                                        Pt::Http::Reply& reply,
                                        Pt::System::EventLoop& loop)
            {}

            virtual void onReadRequest(Pt::Http::Request& request,
                                       Pt::Http::Reply& reply,
                                       Pt::System::EventLoop& loop)
            {}

            virtual void onBeginReply(const Pt::Http::Request& request,
                                      Pt::Http::Reply& reply,
                                      Pt::System::EventLoop& loop)
            {
                reply.body() << "Hello World!";
                reply.beginSend(true);
            }

            virtual void onWriteReply(const Pt::Http::Request& request,
                                      Pt::Http::Reply& reply,
                                      Pt::System::EventLoop& loop)
            {}
    };
    @endcode

    @ingroup Pt-Http-Servers
*/
class PT_HTTP_API Responder
{
    public:
        /** @brief Construct with service.
        */
        explicit Responder(Service& s);

        /** @brief Destructor.
        */
        virtual ~Responder();

        /** @brief Returns the service for which to respond.
        */
        Service& service()
        { return _service; }

        /** @brief Returns the service for which to respond.
        */
        const Service& service() const
        { return _service; }

        //! @internal
        void setAcceptor(Acceptor& a)
        { _acceptor = &a; }

        /** @brief Called when the request header was received.
        */
        void beginRequest(Request& request, Reply& reply, 
                          System::EventLoop& loop);
        
        /** @brief Called when request body data was received.
        */
        void readRequest(Request& request, Reply& reply, 
                         System::EventLoop& loop);

        /** @brief Called when request is complete.
        */
        void beginReply(const Request& request, Reply& reply, 
                        System::EventLoop& loop);

        /** @brief Write responding reply.
        */
        void writeReply(const Request& request, Reply& reply, 
                        System::EventLoop& loop);

    protected:
        /** @brief Called when the request header was received.
        */
        virtual void onBeginRequest(Request& request, Reply& reply, 
                                    System::EventLoop& loop) = 0;
        
        /** @brief Called for each chunk of the request body.
        */
        virtual void onReadRequest(Request& request, Reply& reply, 
                                   System::EventLoop& loop) = 0;

        /** @brief Called when request is complete.
        */
        virtual void onBeginReply(const Request& request, Reply& reply, 
                                  System::EventLoop& loop) = 0;

        /** @brief Write responding reply.
        */
        virtual void onWriteReply(const Request& request, Reply& reply, 
                                  System::EventLoop& loop) = 0;

        // TODO: setFinished() -> setReady()

        /** @brief Sets whether the current step is complete.
        */
        void setReady(bool isFinished);

        /** @brief Sets whether the exchange is finished.
        */
        void setFinished(bool isFinished);

    private:
        Service&  _service;
        Acceptor* _acceptor;
};

} // namespace Http

} // namespace Pt

#endif // Pt_Http_Responder_h
