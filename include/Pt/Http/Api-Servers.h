/*
 * Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
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

#ifndef PT_HTTP_API_SERVERS_H
#define PT_HTTP_API_SERVERS_H

/** @addtogroup Pt-Http-Servers

    @brief Listen, map requests and send replies.

    %Server is the listening HTTP service. It binds a local %Endpoint
    and accepts connections, but it does not implement a resource.
    Incoming requests are mapped by servlets. Each %Servlet combines a
    mapping rule, a %Service, and an optional %Authorizer.
    %addServlet() registers the servlet, and the first servlet that
    maps the request handles it.

    A service is a factory: %onGetResponder() creates a %Responder for
    the request, and %onReleaseResponder() destroys it after the reply
    has been sent or the exchange has failed. %BasicService is that
    factory for a responder type, with an allocator. Use a custom
    %Service when the responder type depends on the request, or when
    responders are pooled.

    A responder handles one exchange, and the server calls it in
    order. %onBeginRequest() runs when the request header is
    available, %onReadRequest() runs for each chunk of the request
    body, %onBeginReply() runs when the request is complete and the
    reply should start, and %onWriteReply() runs when a previous
    %beginSend(false) needs another chunk of the reply body. The
    responder finishes the reply with %Reply::beginSend() and the
    completion flag set to true. It may send that finished reply from
    an earlier callback, in which case remaining callbacks are skipped
    and the rest of the request is ignored.

    All server I/O is asynchronous, so the server needs an %EventLoop,
    passed to a constructor or to %setActive(). %listen() binds the
    local endpoint, and a second listen replaces the previous binding.
    %setSecure() restricts the server to HTTPS. %setMaxThreads()
    bounds worker threads, and timeouts together with
    %setMaxRequestSize() bound idle connections and request size.

    %MapUrl maps one exact URL, and %MapAny maps every request. A
    custom servlet implements %onRequest() and returns true when its
    service should run. Several servlets may share one service, so the
    same resource can appear under more than one name.

    %Authorizer is the server-side access check. It is attached to a
    servlet, not to the server as a whole, and the same authorizer may
    be shared. %BasicAuthorizer implements HTTP Basic authentication
    and asks a derived class whether the credentials are granted.
    Authorization may complete immediately or through an asynchronous
    %Authorization object.

    The example is the listening server and a URL mapping. The service
    is a %BasicService for a responder type that the next sections
    implement, and the loop runs the server.

    @code
    Pt::System::MainLoop loop;
    Pt::Http::Server server(loop, Pt::Net::Endpoint::ip4Any(80));

    HelloService hello;
    Pt::Http::MapUrl mapHello("/hello", hello);
    server.addServlet(mapHello);

    loop.run();
    @endcode
*/

#endif
