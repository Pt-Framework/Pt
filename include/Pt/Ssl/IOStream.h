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

/** @brief SSL stream.
*/
class IOStream : public BasicIOStream<char>
               , private NonCopyable
{
    public:
        /** @brief Construct an SSL stream. 
        */
        IOStream(std::size_t bufferSize = 1024)
        : BasicIOStream<char>(0)
        , _sb(bufferSize)
        {
            init(&_sb);
        }

        /** @brief Constructs an open SSL stream. 
        */
        IOStream(Context& ctx, std::iostream& ios, OpenMode mode, std::size_t bufferSize = 1024)
        : BasicIOStream<char>(0)
        , _sb(ctx, ios, mode, bufferSize)
        {
            init(&_sb);        
        }

        /** @brief Destructor. 
        */
        virtual ~IOStream()
        {}

        /** @brief Opens the SSL stream. 
        */
        void open(Context& ctx, std::iostream& ios, OpenMode mode)
        { _sb.open( ctx, ios, mode); }

        /** @brief Returns the currently used cipher.
        */
        const char* currentCipher() const
        { return _sb.currentCipher(); }

        /** @brief Closes the stream.
        */
        void close()
        { _sb.close(); }

        /** @brief Returns true if connected to peer. 
        */
        bool isConnected() const
        { return _sb.isConnected(); }

        /** @brief Writes a handshake message to the underlying stream
            
            Returns true if handshake data was written, false if not.
        */
        bool writeHandshake()
        { return _sb.writeHandshake(); }

        /** @brief Reads handshake message from the underlying stream
            
            Returns true if more handshake data needs to be read, false
            if not.
        */
        bool readHandshake()
        { return _sb.readHandshake(); }

        /** @brief Shutdown the SSL connection. 
         
            If isShutdown() returned false, the shutdown message is written
            to the output. If isShutdown() returned true, or a previous call
            of shutdown() returned false, more data is required to read the 
            shutdown reply. True is returned if the shutown was completed.
        */
        bool shutdown()
        { return _sb.shutdown(); }

        /** @brief Returns true if the shutown notify has to be completed.
        */
        bool isShutdown() const
        { return _sb.isShutdown(); }

        /** @brief Returns true if the connection is closed.
        */
        bool isClosed() const
        { return _sb.isClosed(); }

        /** @brief Reads user message from the underlying stream.
            
            Call isShutdown() to find out if a shutdown notify was received
            and isClosed() if the connection was prematurely closed.
        */
        void import(std::streamsize maxImport = 0)
        { _sb.import(maxImport); }

        /** @brief Returns the ssl buffer.
        */
        StreamBuffer& sslBuffer()
        { return _sb; }

    private:
        StreamBuffer _sb;
};

} // namespace Ssl

} // namespace Pt

#endif // PT_SSL_IOSTREAM_H
