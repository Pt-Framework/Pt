/*
 * Copyright (C) 2013 by Marc Boris Duerner
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

#ifndef PT_SSL_IOSTREAM_H
#define PT_SSL_IOSTREAM_H

#include <Pt/Ssl/Api.h>
#include <Pt/Ssl/StreamBuffer.h>
#include <Pt/NonCopyable.h>
#include <Pt/IOStream.h>

namespace Pt {

namespace Ssl {

/** @brief SSL iostream.

    %IOStream is the iostream facade for a secure connection. It
    wraps an underlying iostream with a %Context and an %OpenMode,
    and it is the type most callers construct. The handshake,
    encrypted I/O, and shutdown are the stream operations the group
    described; this type forwards them to its %StreamBuffer.

    A default-constructed stream is closed. The constructor that
    takes a context, an iostream, and a mode opens immediately, as
    does %open(). %isConnected() is true after the handshake
    completes. %readHandshake() and %writeHandshake() are the
    non-blocking handshake steps. %import() decrypts available
    input. %sslBuffer() is the buffer when the caller needs
    %StreamBuffer::setPeerName() or the streambuf itself.
    %currentCipher() is the negotiated cipher. %close() drops the
    connection without a TLS shutdown.

    Writes use the inherited iostream API. %flush() pushes encrypted
    bytes to the underlying stream. Premature EOF on the transport
    is handled by the code that owns that stream.

    The example imports available ciphertext and reads the decrypted
    bytes, then writes a message and flushes it so the encrypted
    output reaches the underlying stream.

    @code
    Pt::Ssl::IOStream& ssl = ...;

    for(;;)
    {
        ssl.import();

        std::streamsize avail = ssl.sslBuffer().in_avail();
        if(avail <= 0)
            break;

        do
        {
            char buf[255];
            std::streamsize n = ssl.readsome(buf, sizeof(buf));
            std::cout.write(buf, n);
        }
        while( ssl.sslBuffer().in_avail() > 0 );
    }

    if( ssl.isShutdown() )
        ssl.shutdown();

    ssl << "pi is: " << 3.1415;
    ssl.flush();
    @endcode

    @ingroup Pt-Ssl-Streams
*/
class IOStream : public BasicIOStream<char>
               , private NonCopyable
{
    public:
        /** @brief Creates a closed SSL stream.
        */
        IOStream(std::size_t bufferSize = 1024)
        : BasicIOStream<char>(0)
        , _sb(bufferSize)
        {
            setBuffer(&_sb);
        }

        /** @brief Creates an SSL stream and opens it.
        */
        IOStream(Context& ctx, std::iostream& ios, OpenMode mode, std::size_t bufferSize = 1024)
        : BasicIOStream<char>(0)
        , _sb(ctx, ios, mode, bufferSize)
        {
            setBuffer(&_sb);        
        }

        /** @brief Destructor.
        */
        virtual ~IOStream()
        {}

        /** @brief Opens the SSL stream on @a ios using @a ctx and @a mode.
        */
        void open(Context& ctx, std::iostream& ios, OpenMode mode)
        { _sb.open( ctx, ios, mode); }

        /** @brief Returns the cipher in use, or a placeholder if not connected.
        */
        const char* currentCipher() const
        { return _sb.currentCipher(); }

        /** @brief Closes the stream without a TLS shutdown.
        */
        void close()
        { _sb.close(); }

        /** @brief Returns true if the handshake has completed.
        */
        bool isConnected() const
        { return _sb.isConnected(); }

        /** @brief Writes a handshake message to the underlying stream.

            Returns true if handshake data was written, false otherwise.

            @throw %HandshakeFailed if the handshake cannot complete.
            @throw %SslError if the stream is not open.
        */
        bool writeHandshake()
        { return _sb.writeHandshake(); }

        /** @brief Reads a handshake message from the underlying stream.

            Returns true if more handshake data needs to be read, false
            otherwise.

            @throw %HandshakeFailed if the handshake cannot complete.
            @throw %SslError if the stream is not open.
        */
        bool readHandshake()
        { return _sb.readHandshake(); }

        /** @brief Completes or starts the TLS shutdown.

            Returns true if the shutdown completed.

            @throw %SslError if the shutdown fails.
        */
        bool shutdown()
        { return _sb.shutdown(); }

        /** @brief Returns true if a shutdown alert was received.
        */
        bool isShutdown() const
        { return _sb.isShutdown(); }

        /** @brief Returns true if the connection was closed prematurely.
        */
        bool isClosed() const
        { return _sb.isClosed(); }

        /** @brief Decrypts available data from the underlying stream.

            Returns the number of bytes consumed from the underlying
            stream. Call %isShutdown() to find out if a shutdown alert
            was received, and %isClosed() if the connection was
            closed prematurely.
        */
        std::streamsize import(std::streamsize maxImport = 0)
        { return _sb.import(maxImport); }

        /** @brief Returns the SSL stream buffer.
        */
        StreamBuffer& sslBuffer()
        { return _sb; }

    private:
        StreamBuffer _sb;
};

} // namespace Ssl

} // namespace Pt

#endif // PT_SSL_IOSTREAM_H
