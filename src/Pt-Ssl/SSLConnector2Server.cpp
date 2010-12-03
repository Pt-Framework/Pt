/*
 * Copyright (C) 2010-2010 by Aloysius Indrayanto
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

#include <iostream>

#include "SSLConnector2Server.h"

namespace Pt {
namespace Ssl {

static const std::string _getType(const SSLConnector2* ssl, const char* funcName)
{
    char buff[1024];
    sprintf(buff, "%s [%12s] ", (dynamic_cast<const SSLConnector2Server*>(ssl)) ? "(Server)" : "(Client)", funcName);

    return buff;
}


SSLConnector2Server::SSLConnector2Server(System::IODevice& ioDevice, SSLContext& sslContext, const char* sessionID)
: SSLConnector2(ioDevice, sslContext, sessionID)
{ }

SSLConnector2Server::~SSLConnector2Server()
{ }

void SSLConnector2Server::accept()
{
    SSL_set_accept_state(_ssl);

    if(dynamic_cast<SSLConnector2Server*>(this))
        std::cerr << "[SSLConnector2] " << _getType(this, __func__) << " begin read from IO device" << std::endl;
    else
        std::cerr << "[SSLConnector2] (Client) begin read from IO device" << std::endl;

    _iod.beginRead(_iodBuff, sizeof(_iodBuff));
}

} // namespace Pt
} // namespace Ssl
