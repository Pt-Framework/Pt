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

/** @brief HTTP service responder.
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

        void setReady(bool isFinished);

        void setFinished(bool isFinished);

    private:
        Service&  _service;
        Acceptor* _acceptor;
};

} // namespace Http

} // namespace Pt

#endif // Pt_Http_Responder_h
