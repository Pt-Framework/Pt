/***************************************************************************
 *   Copyright (C) 2006 by Marc Boris Duerner, Tommi Maekitalo             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "AddrInfo.h"
#include "Pt/SourceInfo.h"
#include <stdexcept>

namespace Pt {

namespace Net {

AddrInfo::AddrInfo(const std::string& ipaddr, unsigned short port, const addrinfo& hints)
: ai(0)
{
    std::ostringstream p;
    p << port;

    if (0 != ::getaddrinfo(ipaddr.c_str(), p.str().c_str(), &hints, &ai))
      throw std::runtime_error("invalid ipaddress " + ipaddr + ":" + p.str() + PT_SOURCEINFO); // TODO specify errortype

    if (ai == 0)
      throw std::runtime_error("unknown error in getaddrinfo" + PT_SOURCEINFO); // TODO specify errortype
}

AddrInfo::~AddrInfo()
{
    if (ai)
        ::freeaddrinfo(ai);
}

}

}

