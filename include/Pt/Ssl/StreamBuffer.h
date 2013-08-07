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
#include <Pt/Ssl/Session.h>
#include <Pt/Ssl/CipherList.h>
#include <Pt/Signal.h>
#include <streambuf>
#include <string>

#ifdef __APPLE__
#import <Security/Security.h>
#import <CoreFoundation/CoreFoundation.h>
#import <CoreFoundation/CFDictionary.h>
#endif

namespace Pt {

namespace Ssl {

#ifdef __APPLE__

class PT_SSL_API Connection
{
    public:
        Connection(Context& ctx, std::streambuf& ios, int mode);

        ~Connection();

        bool connected() const
        { return _connected; }

        bool writeHandshake();

        bool readHandshake();

        // TODO: writeShutdown / readShutdown
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
        Context* _ctx;
        SSLContextRef   _context;
        std::streambuf* _ios;
        std::streamsize _iocount;
        bool _connected;
        bool _wantRead;
        bool _isReading;
        bool _isWritingHandshake;
        bool _isShutdown;
};

#else

class PT_SSL_API Connection
{
    public:
        Connection(Context& ctx, std::streambuf& ios, int mode);

        ~Connection();

        bool connected() const
        { return _connected; }

        bool writeHandshake();

        bool readHandshake();

        void shutdown();

        bool isShutdown() const;

        bool isClosed() const;

        std::streamsize write(const char* buf, size_t n);

        std::streamsize read(char* buf, size_t n, std::streamsize isize);

    private:
        std::streambuf* _ios;
        bool _connected;
        bio_st* _in;
        bio_st* _out;
        ssl_st* _ssl;
};

#endif

/** @brief SSL stream buffer.
*/
class PT_SSL_API StreamBuffer : public std::streambuf
{
    public:
        enum OpenMode
        {
            Connect = 3,
            Accept = 4
        };

    public:
        /** @brief Construct an SSL stream buffer that uses the given IO stream. 
        */
        StreamBuffer(size_t bufferSize = 1024);

        /** @brief Construct an SSL stream buffer that uses the given IO stream and SSL context. 
        */
        StreamBuffer(Context& ctx, std::streambuf& sb, OpenMode mode, size_t bufferSize = 1024);

        /** @brief Standard dtor. 
        */
        virtual ~StreamBuffer();

        /** @brief Initializes the SSL stream to use the given context. 
        */
        void open(Context& ctx, std::streambuf& sb, OpenMode mode);

        //void discard();

        /** @brief Return a list of available ciphers for the current protocol. 
        */
        //CipherList ciphers() const;

        /** @brief Return the currently used cipher (the cipher that are actually used to form the SSL channel). 
        */
        //Cipher currentCipher() const;

        /** @brief Check if this SSL stream buffer has been connected to the SSL stream buffer at the other end. 
        */
        bool isConnected() const;

        /** @brief Get the peer CN (Common Name). 
        */
        //std::string peerName() const;

        /** @brief Get the current session data.
            
            The value returned by this function is only meaningful after a
            successful handshake.
        */
        //Session session() const;

        /** @brief Set the current session data.
            
            It is only meaningful to call this function before starting any handshake.
        */
        //void setSession(const Session& sess);

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

        /** @brief Shutdown the SSL connection. 
         
            If isShutdown() returns false, the shutdown message is written
            to the output. If isShutdown returned true, or subsequent calls
            return false if more data is required to read the shutdown reply.
        */
        bool shutdown();

        bool isShutdown() const;

        bool isClosed() const;

        /** @brief Reads user message from the underlying stream
            
            Call isShutdown() to find out if peer is shutting down the connection.
        */
        void import(std::streamsize maxImport = 0);

    protected:
        virtual int sync();
        
        virtual int_type underflow();
        
        virtual int_type overflow(int_type ch);

    private:
        std::streamsize sslRead(char* buf, size_t n, std::streamsize isize);

        std::streamsize sslWrite(char* buf, size_t n);

        /** @brief Get the current status string of this SSL stream buffer. 
        */
        //const char* getStatus() const;

    private:
        Connection*  _connection;
        size_t       _ibufferSize;
        char*        _ibuffer;
        std::size_t  _obufferSize;
        char*        _obuffer;
        const size_t _pbmax;
};

} // namespace Ssl

} // namespace Pt

#endif // PT_SSL_StreamBuffer_H
