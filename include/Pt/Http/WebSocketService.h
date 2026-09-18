/*
 * Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
#ifndef PT_HTTP_WEBSOCKETSERVICE_H
#define PT_HTTP_WEBSOCKETSERVICE_H

#include <Pt/Http/Api.h>
#include <Pt/Http/WebSocketResponder.h>
#include <Pt/Http/WebSocket.h>
#include <Pt/Http/Service.h>
#include <Pt/Http/IOStream.h>

namespace Pt {

namespace Http {

/** @brief HTTP service for the WebSocket handshake.

    @ingroup Pt-Http-WebSocket
*/
class PT_HTTP_API WebSocketService : public  Pt::Http::Service
{
    public:
        /** @brief Default constructor.
        */
        WebSocketService();
        
        /** @brief Destructor.
        */
        ~WebSocketService();

    protected:
        virtual Responder* onGetResponder(const Request&);

        virtual void onReleaseResponder(Responder* r);
};

}}

#endif