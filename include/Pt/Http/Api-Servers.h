/* Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
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
