/* Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_NET_API_H
#define PT_NET_API_H

#include <Pt/Api.h>

#define PT_NET_VERSION_MAJOR PT_VERSION_MAJOR
#define PT_NET_VERSION_MINOR PT_VERSION_MINOR
#define PT_NET_VERSION_REVISION PT_VERSION_REVISION
#define PT_NET_VERSION_PRERELEASE PT_VERSION_PRERELEASE
 
#if defined(PT_NET_API_EXPORT)
#    define PT_NET_API PT_EXPORT
#  else
#    define PT_NET_API PT_IMPORT
#  endif

namespace Pt {

/** @namespace Pt::Net
    @brief TCP and UDP network sockets.

    Portable TCP and UDP sockets for IPv4 and IPv6, including unicast,
    broadcast and multicast UDP.
*/
namespace Net {

class AddressInUse;
class Endpoint;
class TcpServer;
class TcpServerOptions;
class TcpSocket;
class TcpSocketOptions;
class UdpSocket;
class UdpSocketOptions;

} // namespace Net

} // namespace Pt
 
#endif // PT_NET_API_H
