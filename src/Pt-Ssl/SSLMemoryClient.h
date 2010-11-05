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
#ifndef PT_SSL_SSLMEMORY_CLIENT_H
#define PT_SSL_SSLMEMORY_CLIENT_H

#include "SSLMemoryServer.h"

namespace Pt {
namespace Ssl {

// Just for testing: memory-based SSL client
class SSLMemoryClient : public SSLMemoryConnector {
    public:
        // Construct a memory-based SSL client that uses the given context
        SSLMemoryClient(SSLContext& sslContext, const char* sessionID);

        // Standard dtor
        virtual ~SSLMemoryClient();

        // Override the connect() method
        virtual void connect(SSLMemoryServer& server);

        // Override the disconnect() method
        virtual void disconnect();

        // Override the write() method
        virtual int write(const char* buff, int len);

    private:
        SSLMemoryServer* _server;
};

} // namespace Pt
} // namespace Ssl

#endif
