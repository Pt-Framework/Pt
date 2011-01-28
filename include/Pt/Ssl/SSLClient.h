/*
 * Copyright (C) 2010-2010 by Marc Boris Duerner
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
#ifndef PT_SSL_CLIENT_H
#define PT_SSL_CLIENT_H

#include <iostream>

#include <Pt/Ssl/SSLStreamBuf.h>
#include <Pt/System/IOStream.h>

namespace Pt {
namespace Ssl {

/**
 * \brief SSL stream buffer client.
 */
class PT_SSL_API SSLClient : public std::iostream, public Pt::Connectable
{
    public:
        /** \brief Construct an SSL stream buffer client that uses the given IO stream and SSL context. */
        SSLClient(Pt::System::IOStream& ios, SSLContext& ctx, const char* sessionID = 0);

        /** \brief Standard dtor. */
        virtual ~SSLClient();

        /** \brief Return the internal SSLStreamBuf instance. */
        inline SSLStreamBuf& buffer()
        { return _sslbuf; }

        /** \brief Return the internal SSLStreamBuf instance. */
        inline const SSLStreamBuf& buffer() const
        { return _sslbuf; }

        /** @brief Starts the client handshake
            After this method has been called, the first handshake message
            can be written to the server.
        */
        void beginHandshake();

        /** @brief This signal will be fired if the SLL system has finished the handshake */
        Pt::Signal<SSLClient&> handshakeFinished;

    private:
        void onWriteHandshake(Pt::System::StreamBuffer& sb);
        void onReadHandshake(Pt::System::StreamBuffer& sb);

    private:
        System::IOStream* _ios;
        SSLStreamBuf      _sslbuf;
};

} // namespace Pt
} // namespace Ssl

#endif
