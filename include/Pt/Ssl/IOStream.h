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
#ifndef PT_SSL_IOSTREAM_H
#define PT_SSL_IOSTREAM_H

#include <Pt/Ssl/StreamBuffer.h>
#include <Pt/System/IOStream.h>

namespace Pt {

namespace Ssl {

class IOBuffer : public StreamBuffer
{
    public:
        IOBuffer(Context& ctx, Pt::System::IOStream& ios, const char* sessionID = 0, size_t bufsize = 1024);

        /** @brief Standard dtor. 
        */
        virtual ~IOBuffer();

    private:
        Pt::Signal<IOBuffer&> _handshakeFinished;
        Pt::Signal<IOBuffer&> _shutdownFinished;
        Pt::Signal<IOBuffer&> _inputReady;
        Pt::Signal<IOBuffer&> _outputReady;
        int _errorPending;
        bool _reading;
        bool _input;
};

/** @brief SSL I/O stream
 */
class PT_SSL_API IOStream : public std::iostream
                          , public Pt::Connectable
{
    public:
        /** @brief Construct a SSL client that uses the given I/O stream and SSL context. 
        */
        IOStream(Context& ctx, Pt::System::IOStream& ios, const char* sessionID = 0);

        /** @brief Standard dtor. 
        */
        virtual ~IOStream();

        /** @brief Return the internal SSLStreamBuf instance. 
        */
        inline StreamBuffer& buffer()
        { return _sslbuf; }

        /** @brief Return the internal StreamBuffer instance. 
        */
        inline const StreamBuffer& buffer() const
        { return _sslbuf; }

        /** @brief Starts the client handshake
            
            After this method has been called, the first handshake message
            can be written to the server.
        */
        void beginConnectHandshake(bool verifyServerCert);

        void beginAcceptHandshake(bool verifyClientCert, bool requireCertBasedAuth);

        /** @brief Ends the client handshake
            
            This function must be called after the handshake message is complete.
        */
        void endHandshake();

        void beginShutdown();

        void endShutdown();

        void beginRead();

        std::streamsize endRead();

        void beginWrite();

        void endWrite();

        /** @brief This signal will be fired if the SLL system has finished the handshake 
        */
        Pt::Signal<IOStream&>& handshakeFinished()
        { return _handshakeFinished; }

        /** @brief This signal will be fired if the SLL system has finished the shutdown 
        */
        Pt::Signal<IOStream&>& shutdownFinished()
        { return _shutdownFinished; }

        Pt::Signal<IOStream&>& inputReady()
        { return _inputReady; }

        Pt::Signal<IOStream&>& outputReady()
        { return _outputReady; }

    private:
        void onWriteHandshake(Pt::System::StreamBuffer& sb);
        void onReadHandshake(Pt::System::StreamBuffer& sb);

        void onReadServerHandshake(Pt::System::StreamBuffer& sb);
        void onWriteServerHandshake(Pt::System::StreamBuffer& sb);

        void onReadShutdown(Pt::System::StreamBuffer& sb);
        void onWriteShutdown(Pt::System::StreamBuffer& sb);

        void onInput(Pt::System::StreamBuffer& sb);
        void onOutput(Pt::System::StreamBuffer& sb);

    private:
        System::IOStream* _ios;
        IOBuffer _sslbuf;
        Pt::Signal<IOStream&> _handshakeFinished;
        Pt::Signal<IOStream&> _shutdownFinished;
        Pt::Signal<IOStream&> _inputReady;
        Pt::Signal<IOStream&> _outputReady;
        int _errorPending;
        bool _reading;
        bool _input;
};

} // namespace Ssl

} // namespace Pt

#endif
