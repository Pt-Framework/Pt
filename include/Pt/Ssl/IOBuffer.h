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
#ifndef PT_SSL_IOBUFFER_H
#define PT_SSL_IOBUFFER_H

#include <Pt/Ssl/StreamBuffer.h>
#include <Pt/System/IOBuffer.h>

namespace Pt {

namespace Ssl {

/** @brief SSL I/O stream buffer.
 */
class PT_SSL_API IOBuffer : public StreamBuffer
                          , public Pt::Connectable
{
    public:
        /** @brief Construct a SSL buffer that uses the given I/O buffer. 
        */
        IOBuffer(Pt::System::IOBuffer& sb);
        
        /** @brief Construct a SSL client that uses the given I/O buffer and SSL context. 
        */
        IOBuffer(Context& ctx, Pt::System::IOBuffer& sb);

        /** @brief Standard dtor. 
        */
        virtual ~IOBuffer();

        System::IOBuffer& buffer()
        { return *_sb; }

        void connect();

        void fakeAfterConnect();

        void fakeAfterAccept();

        /** @brief Starts the client connect handshake
        */
        void beginConnect();

        /** @brief Starts the server accept handshake
        */
        void beginAccept();

        /** @brief Ends the client or server handshake
        */
        void endHandshake();

        void beginShutdown();

        void endShutdown();

        void beginRead();

        void endRead();

        void beginWrite();

        void endWrite();

        /** @brief This signal is sent when the handshake has finished. 
        */
        Pt::Signal<IOBuffer&>& handshakeFinished()
        { return _handshakeFinished; }

        /** @brief This signal is sent when the shutdown has finished. 
        */
        Pt::Signal<IOBuffer&>& shutdownFinished()
        { return _shutdownFinished; }

        /** @brief This signal is sent when data is available. 
        */
        Pt::Signal<IOBuffer&>& inputReady()
        { return _inputReady; }

        /** @brief This signal is sent when all data has been sent. 
        */
        Pt::Signal<IOBuffer&>& outputReady()
        { return _outputReady; }

    private:
        void onWriteHandshake(Pt::System::IOBuffer& sb);
        void onReadHandshake(Pt::System::IOBuffer& sb);

        void onReadServerHandshake(Pt::System::IOBuffer& sb);
        void onWriteServerHandshake(Pt::System::IOBuffer& sb);

        void onReadShutdown(Pt::System::IOBuffer& sb);
        void onWriteShutdown(Pt::System::IOBuffer& sb);

        void onInput(Pt::System::IOBuffer& sb);
        void onOutput(Pt::System::IOBuffer& sb);

    private:
        System::IOBuffer* _sb;
        Pt::Signal<IOBuffer&> _handshakeFinished;
        Pt::Signal<IOBuffer&> _shutdownFinished;
        Pt::Signal<IOBuffer&> _inputReady;
        Pt::Signal<IOBuffer&> _outputReady;
        int _errorPending;
        bool _reading;
        bool _input;
};

} // namespace Ssl

} // namespace Pt

#endif
