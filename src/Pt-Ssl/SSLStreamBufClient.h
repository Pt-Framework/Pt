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
#ifndef PT_SSL_SSLSTREAMBUF_CLIENT_H
#define PT_SSL_SSLSTREAMBUF_CLIENT_H

#include "SSLStreamBuf.h"
#include <Pt/System/IOStream.h>
#include <iostream>

namespace Pt {

namespace Ssl {

/**
 * \brief SSL stream buffer client.
 */
class PT_SSL_API SSLStreamBufClient : public SSLStreamBuf
{
    public:
        /** \brief Construct an SSL stream buffer client that uses the given IO stream and SSL context. */
        SSLStreamBufClient(std::iostream& ios, SSLContext& ctx, const char* sessionID = 0);

        /** \brief Standard dtor. */
        virtual ~SSLStreamBufClient();

        /** @brief Starts the client handshake
            After this method has been called, the first handshake message
            can be written to the server.
        */
        void startClientHandshake();

        /** \brief Disconnect the connection. */
        void disconnect();

        /** \brief Get the peer CN (Common Name). */
        const std::string getPeerCN() const;
};


/**
 * \brief SSL stream buffer client.
 */
class PT_SSL_API SslClient : public std::iostream, public Pt::Connectable
{
    public:
        /** \brief Construct an SSL stream buffer client that uses the given IO stream and SSL context. */
        SslClient(Pt::System::IOStream& ios, SSLContext& ctx, const char* sessionID = 0);

        /** \brief Standard dtor. */
        virtual ~SslClient();

        /** @brief Starts the client handshake
            After this method has been called, the first handshake message
            can be written to the server.
        */
        void startHandshake();

        SSLStreamBuf& buffer()
        { return _sslbuf; }

        void onWriteHandshake(Pt::System::StreamBuffer& sb)
        {
            _ios->buffer().endWrite();
            std::cerr << "[@@ TestApp @@]" << "out_avail = " << _ios->buffer().out_avail() << std::endl;

            if( _sslbuf.writeHandshake() || _ios->buffer().out_avail() > 0 )
            {
                std::cerr << "[@@ TestApp @@]"  << "Begin write" << std::endl;
                _ios->buffer().beginWrite();
                return;
            }

            if(_sslbuf.connected())
            {
                std::cerr << "[@@ TestApp @@]" << "Successfully connected to the client" << std::endl;
                _ios->buffer().outputReady -= Pt::slot(*this, &SslClient::onWriteHandshake);
                _ios->buffer().inputReady  -= Pt::slot(*this, &SslClient::onReadHandshake);
                return;
            }

            std::cerr << "[@@ TestApp @@]" << "Begin read" << std::endl;
            _ios->buffer().beginRead();
        }

        void onReadHandshake(Pt::System::StreamBuffer& sb)
        {
            _ios->buffer().endRead();
            std::cerr << "[@@ TestApp @@]" << "in_avail = " << _ios->buffer().in_avail() << std::endl;

            if(_sslbuf.readHandshake())
            {
                std::cerr << "[@@ TestApp @@]" << "Read more handshake bytes" << std::endl;
                _ios->buffer().beginRead();
                return;
            }

            _sslbuf.writeHandshake();

            std::cerr << "[@@ TestApp @@]" << "Begin write" << std::endl;
            _ios->buffer().beginWrite();
        }

    private:
        System::IOStream* _ios;
        SSLStreamBuf _sslbuf;
};

} // namespace Pt

} // namespace Ssl

#endif
