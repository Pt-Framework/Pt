/*
 * Copyright (C) 2010-2012 by Marc Boris Duerner
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

#ifndef PT_SSL_Connection_H
#define PT_SSL_Connection_H

#include <Pt/Ssl/Api.h>
#include <Pt/Ssl/Context.h>
#include <mbedtls/ssl.h>
#include <ios>
#include <cstddef>

namespace Pt {

namespace Ssl {

class Connection
{
    public:
        Connection(Context& ctx, std::ios& ios, OpenMode omode);

        ~Connection();

        void setPeerName(const std::string& peerName);

        bool connected() const
        { return _connected; }

        const char* currentCipher() const;

        bool writeHandshake();

        bool readHandshake();

        bool shutdown();

        bool isShutdown() const;

        bool isClosed() const;

        std::streamsize write(const char* buf, std::size_t n);

        std::streamsize read(char* buf, std::size_t n, std::streamsize isize);

    protected:
        static int bio_send(void* ctx, const unsigned char* buf, std::size_t len);

        static int bio_recv(void* ctx, unsigned char* buf, std::size_t len);

    protected:
        int bioWrite(const unsigned char* buf, std::size_t len);

        int bioRead(unsigned char* buf, std::size_t len);

        void verifyPeerName();

    private:
        enum PendingDirection
        {
            NoneWanted,
            WantRead,
            WantWrite
        };

        Context*             _ctx;
        std::ios*            _ios;
        mbedtls_ssl_context   _ssl;
        std::string           _peerName;
        bool                  _connected;
        bool                  _isWriting;
        bool                  _isReading;
        std::streamsize       _maxImport;
        PendingDirection      _pending;
        bool                  _shutdownSent;
        bool                  _shutdownReceived;
};

} // namespace Ssl

} // namespace Pt

#endif // PT_SSL_StreamBuffer_H
