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

#ifndef PT_NET_TcpServerImpl_H
#define PT_NET_TcpServerImpl_H

#include <Pt/Net/TcpServer.h>
#include <Pt/Net/Endpoint.h>
#include <Pt/System/EventLoop.h>
#include <string>
#include <vector>

namespace Pt {

namespace Net {

class TcpServerImpl
{
    public:
        TcpServerImpl(TcpServer& server);

        ~TcpServerImpl();

        void close();

        void cancel(System::EventLoop& loop);

        void beginAccept(System::EventLoop& loop);

        Windows::Networking::Sockets::StreamSocket^ accept();

        void listen(const std::string& ipaddr, unsigned short int port,
                    const TcpServerOptions& options);

        void listen(const Endpoint& e, const TcpServerOptions& options);

        bool run(System::EventLoop& loop);

        void setTimeout(std::size_t msecs)
        { }

        std::size_t timeout() const
        { return 0; }

    private:
        void onConnectionReceived(Windows::Networking::Sockets::StreamSocketListener^ listener, 
                                  Windows::Networking::Sockets::StreamSocketListenerConnectionReceivedEventArgs^ args);

    private:
        TcpServer& _server;
        System::EventLoop* _loop;
        Windows::Networking::Sockets::StreamSocketListener^ _listener;
        Windows::Foundation::IAsyncAction^ _bindOp;
        System::Mutex _mtx;
        std::vector<Windows::Networking::Sockets::StreamSocket^> _backlog;
};

} // namespace Net

} // namespace Pt

#endif
