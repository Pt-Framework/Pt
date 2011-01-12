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
#ifndef PT_SSL_SSLSTREAMBUFFER_CLIENT_H
#define PT_SSL_SSLSTREAMBUFFER_CLIENT_H

#include "SSLStreamBuffer.h"

namespace Pt {
namespace Ssl {

//!
//! \brief SSL connector.
class PT_SSL_API SSLStreamBufferClient : public SSLStreamBuffer {
    public:
        //! \brief Construct an SSL connector client that uses the given IO device and context.
        SSLStreamBufferClient(System::IODevice& ioDevice, SSLContext& sslContext, const char* sessionID);

        //! \brief Construct an SSL connector client that uses the given stream buffer and context.
        SSLStreamBufferClient(System::StreamBuffer& streamBuffer, SSLContext& sslContext, const char* sessionID);

        //! \brief Standard dtor.
        virtual ~SSLStreamBufferClient();

        //! \brief Activate this SSL connector as an SSL connector client and initiate a connection to an SSL connector server.
        void connect();

        //! \brief Disconnect the connection.
        void disconnect();

        //! \brief Get the peer CN (Common Name).
        const std::string getPeerCN() const;
};


class PT_SSL_API SSLStreamBuffer2 : public Connectable, public std::streambuf
{
    public:
        SSLStreamBuffer2(std::iostream& ios, SSLContext& ctx, const char* sessionID);

        virtual ~SSLStreamBuffer2();

        /** @brief Initiates the handshake communication.

            Writes the complete first handshake message to the underlying stream.
        */
        void initHandshake();
        void initServerHandshake();

        /** @brief Advances the handshake communication.

            If the underlying stream has input available, the available data is consumed,
            otherwise this method blocks until data becomes available. If the received
            handshake message is complete, the handshake reply is written to the underlying
            stream.

        */
        void handshake();

        void startClientHandshake();

        void doHandshake();

        bool writeHandshake();

        std::streamsize out_avail();

        bool readHandshake();

        bool connected() const;

        void disconnect();

        std::string getPeerCN() const;

    private:
        std::iostream* _ios;
        BIO*           _in;
        BIO*           _out;
        SSL*           _ssl;
};

} // namespace Pt

} // namespace Ssl

#endif
