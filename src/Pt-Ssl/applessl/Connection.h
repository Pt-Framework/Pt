/*
 * Copyright (C) 2010-2012 by Marc Boris Duerner
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
#ifndef PT_SSL_Connection_H
#define PT_SSL_Connection_H

#include <Pt/Ssl/Api.h>
#include <Pt/Ssl/Context.h>
#include <ios>

#include <Security/Security.h>

namespace Pt {

namespace Ssl {

class Connection
{
    public:
        Connection(Context& ctx, std::ios& ios, OpenMode omode);

        ~Connection();

        bool connected() const
        { return _connected; }

        const char* currentCipher() const;

        bool writeHandshake();

        bool readHandshake();

        bool shutdown();

        bool isShutdown() const;

        bool isClosed() const;

        std::streamsize write(const char* buf, size_t n);

        std::streamsize read(char* buf, size_t n, std::streamsize isize);

        OSStatus sslRead(void* data, size_t* n);

        OSStatus sslWrite(const void* data, size_t* n);

        static OSStatus sslWriteCallback(SSLConnectionRef connection, const void* data, size_t* n);

        static OSStatus sslReadCallback(SSLConnectionRef connection, void* data, size_t* n);

    private:
        const char* toCipherName(SSLCipherSuite cipher) const;

    private:
        Context* _ctx;
        SSLContextRef   _context;
        std::ios* _ios;
        std::streamsize _maxImport;
        std::streamsize _iocount;
        bool _connected;
        bool _wantRead;
        bool _isReading;
        bool _isWriting;
        bool _receivedShutdown;
        bool _sentShutdown;
};

} // namespace Ssl

} // namespace Pt

#endif // PT_SSL_StreamBuffer_H
