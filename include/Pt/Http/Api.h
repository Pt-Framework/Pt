/* Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_HTTP_API_H
#define PT_HTTP_API_H

#include <Pt/Api.h>

#define PT_HTTP_VERSION_MAJOR PT_VERSION_MAJOR
#define PT_HTTP_VERSION_MINOR PT_VERSION_MINOR
#define PT_HTTP_VERSION_REVISION PT_VERSION_REVISION
#define PT_HTTP_VERSION_PRERELEASE PT_VERSION_PRERELEASE

#if defined(PT_HTTP_API_EXPORT)
#    define PT_HTTP_API PT_EXPORT
#  else
#    define PT_HTTP_API PT_IMPORT
#  endif

namespace Pt {

/** @namespace Pt::Http
    @brief HTTP clients and servers.

    Request and reply messages, an HTTP client, and an HTTP server that
    maps URLs to services. HTTPS uses %Pt::Ssl. WebSocket is an HTTP
    upgrade of the same connection.
*/
namespace Http {

class Authenticator;
class Authorizer;
class Client;
class Credential;
class Message;
class MessageHeader;
class MessageProgress;
class Reply;
class Request;
class Server;
class Service;
class Servlet;
class Stream;

} // namespace Http

} // namespace Pt

#endif
