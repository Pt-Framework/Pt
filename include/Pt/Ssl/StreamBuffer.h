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
#include <Pt/Ssl/CipherList.h>
#include <Pt/Signal.h>
#include <streambuf>
#include <string>

namespace Pt {

namespace Ssl {

class Connection;

/** @brief SSL stream buffer.
*/
class PT_SSL_API StreamBuffer : public std::streambuf
{
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

         /** @brief Return the currently used cipher.. 
        */
        const char* currentCipher() const;

        //void discard();

        /** @brief Check if this SSL stream buffer has been connected to the SSL stream buffer at the other end. 
        */
        bool isConnected() const;

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
         
            If isShutdown() returned false, the shutdown message is written
            to the output. If isShutdown() returned true, or a previous call
            of shutdown() returned false, more data is required to read the 
            shutdown reply. True is returned if the shutown was completed.
        */
        bool shutdown();

        /** @brief Returns true if the shutown notify has to be completed.
        */
        bool isShutdown() const;

        /** @brief Returns true if the connection is closed.
        */
        bool isClosed() const;

        /** @brief Reads user message from the underlying stream.
            
            Call isShutdown() to find out if a shutdown notify was received
            and isClosed() if the connection was prematurely closed.
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
