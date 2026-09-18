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

#ifndef PT_SSL_StreamBuffer_H
#define PT_SSL_StreamBuffer_H

#include <Pt/Ssl/Api.h>
#include <Pt/Ssl/Context.h>
#include <Pt/StreamBuffer.h>
#include <ios>
#include <cstddef>

namespace Pt {

namespace Ssl {

class Connection;

/** @brief SSL stream buffer.

    %StreamBuffer is the streambuf that implements the secure
    connection the group described. %IOStream holds one and forwards
    handshake, import, and shutdown to it. Construct this type
    directly when the caller already works with a %std::streambuf,
    or when the expected peer name must be set.

    %setPeerName() is the name expected in the peer certificate. It
    may be called before or after %open(); an open connection
    receives the name immediately. %import() returns the number of
    bytes consumed from the underlying stream. %readHandshake()
    and %writeHandshake() are the same non-blocking handshake as on
    %IOStream. %currentCipher() is the cipher in use, or a
    placeholder when no connection is open.

    The example opens a client buffer and sets the name that must
    appear in the peer certificate.

    @code
    std::iostream& ios = ...;
    Pt::Ssl::StreamBuffer sb(ctx, ios, Pt::Ssl::Connect);
    sb.setPeerName("example.com");
    @endcode

    @ingroup Pt-Ssl-Streams
*/
class PT_SSL_API StreamBuffer : public BasicStreamBuffer<char>
{
    public:
        /** @brief Creates a closed SSL stream buffer.
        */
        StreamBuffer(std::size_t bufferSize = 1024);

        /** @brief Creates an SSL stream buffer and opens it.
        */
        StreamBuffer(Context& ctx, std::ios& ios, OpenMode mode, std::size_t bufferSize = 1024);

        /** @brief Destructor.
        */
        virtual ~StreamBuffer();

        /** @brief Opens the stream buffer on @a ios using @a ctx and @a mode.
        */
        void open(Context& ctx, std::ios& ios, OpenMode mode);
        
        /** @brief Sets the name expected in the peer certificate.
        */
        void setPeerName(const std::string& peerName);

        /** @brief Returns the cipher in use, or a placeholder if not connected.
        */
        const char* currentCipher() const;

        /** @brief Closes the stream buffer without a TLS shutdown.
        */
        void close();

        /** @brief Returns true if the handshake has completed.
        */
        bool isConnected() const;

        /** @brief Writes a handshake message to the underlying stream.

            Returns true if handshake data was written, false otherwise.

            @throw %HandshakeFailed if the handshake cannot complete.
            @throw %SslError if the stream is not open.
        */
        bool writeHandshake();

        /** @brief Reads a handshake message from the underlying stream.

            Returns true if more handshake data needs to be read, false
            otherwise.

            @throw %HandshakeFailed if the handshake cannot complete.
            @throw %SslError if the stream is not open.
        */
        bool readHandshake(std::streamsize maxRead = 0);

        /** @brief Completes or starts the TLS shutdown.

            Returns true if the shutdown completed.

            @throw %SslError if the shutdown fails.
        */
        bool shutdown();

        /** @brief Returns true if a shutdown alert was received.
        */
        bool isShutdown() const;

        /** @brief Returns true if the connection was closed prematurely.
        */
        bool isClosed() const;

        /** @brief Decrypts available data from the underlying stream.

            Returns the number of bytes consumed from the underlying
            stream. Call %isShutdown() to find out if a shutdown alert
            was received.
        */
        std::streamsize import(std::streamsize maxImport = 0);

    protected:
        // inheritdoc
        virtual std::streamsize showmanyc();

        // inheritdoc
        virtual std::streamsize showfull();

        // inheritdoc
        virtual int sync();
        
        // inheritdoc
        virtual int_type underflow();
        
        // inheritdoc
        virtual int_type overflow(int_type ch);

    private:
        Connection*  _connection;
        std::string  _peerName;
        std::size_t  _ibufferSize;
        char*        _ibuffer;
        std::size_t  _obufferSize;
        char*        _obuffer;

        static const int _pbmax = 4;
};

} // namespace Ssl

} // namespace Pt

#endif // PT_SSL_StreamBuffer_H
