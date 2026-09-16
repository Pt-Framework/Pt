/*
 * Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
#ifndef PT_HTTP_WEBSOCKETSTREAM_H
#define PT_HTTP_WEBSOCKETSTREAM_H

#include <Pt/Http/IOStream.h>
#include <Pt/Net/Endpoint.h>
#include <Pt/Net/TcpSocket.h>
#include <Pt/System/Timer.h>
#include <Pt/Signal.h>

namespace Pt {
namespace Http {

class WebSocketStream : public Pt::Connectable
{
    public:

        WebSocketStream(Pt::Http::IOStream& io);    

        WebSocketStream();

        virtual ~WebSocketStream();

        void beginConnect(const Pt::Net::Endpoint& ep);

        Pt::Signal<>& connected()
        {
            return _connected;
        }

        bool endConnect();


        void write(const char* buffer, size_t size);

        void beginWrite(const char* buffer, size_t size);

        Pt::Signal<>& outputReady()
        {
            return _outputReady;
        }

        size_t endWrite();


        void beginRead(char* buffer, size_t size);

        Pt::Signal<>& inputReady()
        {
            return _inputReady;
        }

        size_t endRead();


        void setActive(Pt::System::EventLoop& loop)
        {
            _socket->setActive(loop);
            _connectionTimer.setActive(loop);
        }

        void cancel();

        void setTimeout(size_t ms)
        {
            _timeout = ms;
        }

        bool isServer() const
        {
            return _httpIoStream != 0;
        }

        bool isClient() const
        {
            return _socket != 0;
        }

    private:
        void onInputS();

        void onOutputS();

        void onConnected(Pt::Net::TcpSocket& socket);

        void onInputC(Pt::System::IODevice& device);

        void onOutputC(Pt::System::IODevice& device);

        void onConnectTimeout();

    private:
        Pt::Signal<>        _connected;
        Pt::Signal<>        _outputReady;
        Pt::Signal<>        _inputReady;
        Pt::Http::IOStream* _httpIoStream;
        char*               _inBuffer;
        size_t              _bufferSize;
        Pt::Net::TcpSocket* _socket;
        bool                _error;
        size_t              _timeout;
        Pt::System::Timer   _connectionTimer;
        bool                _ignoreEvent;
};

}}

#endif
