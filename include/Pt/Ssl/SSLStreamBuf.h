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
#ifndef PT_SSL_SSLSTREAMBUF_H
#define PT_SSL_SSLSTREAMBUF_H

#include <string>
#include <Pt/Signal.h>
#include <Pt/System/StreamBuffer.h>

#include <Pt/Ssl/SSLContext.h>

namespace Pt {
namespace Ssl {

/**
 * \brief SSL stream buffer.
 */
class PT_SSL_API SSLStreamBuf : public Connectable, public std::streambuf
{
    public:
        typedef std::streambuf::int_type int_type;

        ////// JUST FOR TESTING /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static const std::string _call_info(const char* className, const std::string& funcName);
        ///// JUST FOR TESTING /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    public:
        /** \brief Construct an SSL stream buffer that uses the given IO stream and SSL context. */
        SSLStreamBuf(std::iostream& ios, SSLContext& ctx, const char* sessionID = 0, size_t bufferSize = 1024);

        /** \brief Standard dtor. */
        virtual ~SSLStreamBuf();

        /** \brief Check if this SSL stream buffer has been connected to the SSL stream buffer at the other end. */
        bool connected() const;

        /** \brief Get the current status string of this SSL stream buffer. */
        const char* getStatusString() const;

        /** \brief Get the peer CN (Common Name). */
        const std::string getPeerCN() const;

        /** @brief Starts the server handshake
            After this method has been called, the first handshake message
            can be read from the client.
        */
        void beginServerHandshake();

        /** @brief Starts the client handshake
            After this method has been called, the first handshake message
            can be written to the server.
        */
        void beginClientHandshake();

        /** @brief Writes a handshake message to the underlying stream
            Returns true if at least a part of the handshake message was
            written, false if the handshake message is complete.
        */
        bool writeHandshake();

        /** @brief Reads handshake message from the underlying stream
            Returns true if more handshake data needs to be read, false
            if the handshake message is complete.
        */
        bool readHandshake();

        /** @brief Reads user message from the underlying stream
            Returns the number bytes in the message or -1 if the other peer has shutdown the stream.
        */
        std::streamsize import();

        /** \brief Shutdown this SSL stream buffer. */
        void shutdown();

    protected:
        virtual int sync();
        virtual int_type underflow();
        virtual int_type overflow(int_type ch);
        std::streamsize do_underflow(std::streamsize size);

    protected:
        BIO*           _in;  // Input BIO
        BIO*           _out; // Output BIO
        SSL*           _ssl; // OpenSSL SSL handle
        std::iostream* _ios; // IO

    private:
        size_t       _ibufferSize;
        char*        _ibuffer;
        std::size_t  _obufferSize;
        char*        _obuffer;
        const size_t _pbmax;
        bool         _oextend;
};


} // namespace Pt
} // namespace Ssl

#endif
