/*
 * Copyright (C) 2006-2009 by Marc Boris Duerner, Tommi Maekitalo
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
#include "AddrInfoImpl.h"
#include "Pt/SourceInfo.h"
#include "Pt/System/IOError.h"
#include "Pt/System/SystemError.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string.h>

namespace Pt {

namespace Net {

AddrInfoImpl::AddrInfoImpl(const std::string& ipaddr, unsigned short port, bool listen)
: ai(0)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));

    if (listen)
        hints.ai_flags |= AI_PASSIVE;

    init(ipaddr, port, hints);
}


AddrInfoImpl::~AddrInfoImpl()
{
    if (ai)
        ::freeaddrinfo(ai);
}


void AddrInfoImpl::init(const std::string& ipaddr, unsigned short port, const addrinfo& hints)
{
    std::ostringstream p;
    p << port;
    
    _host = ipaddr;
    _port = port;

    if (0 != ::getaddrinfo(ipaddr.c_str(), p.str().c_str(), &hints, &ai))
         throw System::AccessFailed(_host + ':' + p.str());

    /*std::vector<std::string> ips;
    hostAddresses(ips);

    struct addrinfo hints2;
    memset(&hints2, 0, sizeof(hints2));

    ::addrinfo* ai2;
    ::getaddrinfo("", "", &hints2, &ai2);

    for(::addrinfo* current = ai2; current; current = current->ai_next)
    {
        sockaddr* saddr = current->ai_addr;

        DWORD len = 64;
        TCHAR adr[64];
        INT ret = WSAAddressToString(saddr, current->ai_addrlen, NULL, adr, &len);
        if(ret == 0)
        {
            std::string address;
            for(unsigned n = 0; n < len; n++)
            {
                if(adr[n] != 0)
                    address.push_back( int(adr[n]) );
            }
        
            std::cout << "YYYY: " << address << std::endl;
        }

    }

    ::freeaddrinfo(ai2);*/
}


void AddrInfoImpl::hostAddresses(std::vector<std::string>& ips)
{
    SOCKET sock;
    DWORD bytesRet;
    int ret;
    
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (SOCKET_ERROR == sock)
    {
        std::cerr << "sockerr" << WSAGetLastError() << std::endl;
        return;
    }
    
    char buffer[4096];
    ret = WSAIoctl(sock, SIO_ADDRESS_LIST_QUERY, NULL, 0, buffer, sizeof(buffer), &bytesRet, NULL, NULL);
    if (SOCKET_ERROR == ret)
    {
        closesocket(sock);
        std::cerr << "WSAIoctl error "<< WSAGetLastError() << std::endl;
        return;
    }
       
    SOCKET_ADDRESS_LIST* slist = reinterpret_cast<SOCKET_ADDRESS_LIST*>(buffer);
    closesocket(sock);

    for (int i = 0; i <= (slist->iAddressCount-1); i++)
    {
        SOCKADDR* saddr = slist->Address[i].lpSockaddr;
        DWORD len = 64;
        TCHAR adr[64];
        WSAAddressToString(saddr, sizeof(SOCKADDR), NULL, adr, &len);
        
        std::string address;
        for(unsigned n = 0; n < len; n++)
        {
            address.push_back( int(adr[n]) );
        }
    
        std::cout << "XXXXX: " << address << std::endl;
        ips.push_back(address);
    }

    // TODO: IPv6
}


/*void AddrInfoImpl::hostAddresses(std::vector<std::string>& ips)
{

    DWORD dwSize = 0;
    DWORD dwRetVal = 0;

    unsigned int i = 0;

    // Set the flags to pass to GetAdaptersAddresses
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;

    // default to unspecified address family (both)
    ULONG family = AF_UNSPEC;

    LPVOID lpMsgBuf = NULL;

    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    ULONG outBufLen = 0;

    PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
    PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
    PIP_ADAPTER_ANYCAST_ADDRESS pAnycast = NULL;
    PIP_ADAPTER_MULTICAST_ADDRESS pMulticast = NULL;
    IP_ADAPTER_DNS_SERVER_ADDRESS *pDnServer = NULL;
    IP_ADAPTER_PREFIX *pPrefix = NULL;

    outBufLen = 4096;

    unsigned tries = 1;
    do 
    {
        pAddresses = (IP_ADAPTER_ADDRESSES *) malloc(outBufLen*tries);
        if (pAddresses == NULL) 
        {
            return;
        }

        dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);

        if (dwRetVal == ERROR_BUFFER_OVERFLOW) 
        {
            free(pAddresses);
            pAddresses = NULL;
        } 
        else 
        {
            break;
        }
    } 
    while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (++tries <= 4));

    if (dwRetVal == NO_ERROR) 
    {
        // If successful, output some information from the data we received
        pCurrAddresses = pAddresses;
        while (pCurrAddresses) 
        {
            printf("\tLength of the IP_ADAPTER_ADDRESS struct: %ld\n",
                   pCurrAddresses->Length);
            printf("\tIfIndex (IPv4 interface): %u\n", pCurrAddresses->IfIndex);
            printf("\tAdapter name: %s\n", pCurrAddresses->AdapterName);

            pUnicast = pCurrAddresses->FirstUnicastAddress;
            if (pUnicast != NULL) {
                for (i = 0; pUnicast != NULL; i++)
                    pUnicast = pUnicast->Next;
                printf("\tNumber of Unicast Addresses: %d\n", i);
            } else
                printf("\tNo Unicast Addresses\n");

            pAnycast = pCurrAddresses->FirstAnycastAddress;
            if (pAnycast) {
                for (i = 0; pAnycast != NULL; i++)
                    pAnycast = pAnycast->Next;
                printf("\tNumber of Anycast Addresses: %d\n", i);
            } else
                printf("\tNo Anycast Addresses\n");

            pMulticast = pCurrAddresses->FirstMulticastAddress;
            if (pMulticast) {
                for (i = 0; pMulticast != NULL; i++)
                    pMulticast = pMulticast->Next;
                printf("\tNumber of Multicast Addresses: %d\n", i);
            } else
                printf("\tNo Multicast Addresses\n");

            pDnServer = pCurrAddresses->FirstDnsServerAddress;
            if (pDnServer) {
                for (i = 0; pDnServer != NULL; i++)
                    pDnServer = pDnServer->Next;
                printf("\tNumber of DNS Server Addresses: %d\n", i);
            } else
                printf("\tNo DNS Server Addresses\n");

            printf("\tDNS Suffix: %wS\n", pCurrAddresses->DnsSuffix);
            printf("\tDescription: %wS\n", pCurrAddresses->Description);
            printf("\tFriendly name: %wS\n", pCurrAddresses->FriendlyName);

            if (pCurrAddresses->PhysicalAddressLength != 0) {
                printf("\tPhysical address: ");
                for (i = 0; i < (int) pCurrAddresses->PhysicalAddressLength;
                     i++) {
                    if (i == (pCurrAddresses->PhysicalAddressLength - 1))
                        printf("%.2X\n",
                               (int) pCurrAddresses->PhysicalAddress[i]);
                    else
                        printf("%.2X-",
                               (int) pCurrAddresses->PhysicalAddress[i]);
                }
            }
            printf("\tFlags: %ld\n", pCurrAddresses->Flags);
            printf("\tMtu: %lu\n", pCurrAddresses->Mtu);
            printf("\tIfType: %ld\n", pCurrAddresses->IfType);
            printf("\tOperStatus: %ld\n", pCurrAddresses->OperStatus);
            printf("\tIpv6IfIndex (IPv6 interface): %u\n",
                   pCurrAddresses->Ipv6IfIndex);
            printf("\tZoneIndices (hex): ");
            for (i = 0; i < 16; i++)
                printf("%lx ", pCurrAddresses->ZoneIndices[i]);
            printf("\n");

            pPrefix = pCurrAddresses->FirstPrefix;
            if (pPrefix) {
                for (i = 0; pPrefix != NULL; i++)
                    pPrefix = pPrefix->Next;
                printf("\tNumber of IP Adapter Prefix entries: %d\n", i);
            } else
                printf("\tNumber of IP Adapter Prefix entries: 0\n");

            printf("\n");

            pCurrAddresses = pCurrAddresses->Next;
        }
    }

    if (pAddresses) {
        free(pAddresses);
    }
}*/

}

}
