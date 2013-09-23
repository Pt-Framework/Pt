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

#ifndef PT_NET_UdpSocketImpl_H
#define PT_NET_UdpSocketImpl_H

#include <Pt/Net/Api.h>
#include <Pt/Net/AddrInfo.h>
#include <Pt/System/EventLoop.h>
#include <string>

namespace Pt {

namespace Net {

class UdpSocket;

class UdpSocketImpl
{
    public:
        UdpSocketImpl(UdpSocket& socket);

        ~UdpSocketImpl();

        void close();

        void bind(const std::string& ipaddr, unsigned short int port, const UdpSocket::Options& o);

        void connect(const AddrInfo& addrinfo);

        bool isConnected() const;

        bool isBound() const;

        void setBroadcast();

        void setHopLimit(unsigned int n);

        void joinMulticastGroup(const std::string& ipaddr);

        void dropMulticastGroup(const std::string& ipaddr);

        void cancel(System::EventLoop& loop);

        std::string socketAddress() const;

        std::string peerAddress() const;

        void setTimeout(std::size_t msecs)
        { _timeout = msecs; }

        std::size_t timeout() const
        { return _timeout; }

        void onMessageReceived(DatagramSocket^ socket, 
                               DatagramSocketMessageReceivedEventArgs^ args)

        size_t beginRead(System::EventLoop& loop, char* buffer, size_t n, bool& eof);


        bool runRead(System::EventLoop& loop);

        size_t endRead(System::EventLoop& loop, char* buffer, size_t n, bool& eof);

        size_t read(char* buffer, size_t count, bool& eof);

        size_t beginWrite(System::EventLoop& loop, const char* buffer, size_t n);

        bool runWrite(System::EventLoop& loop);

        size_t endWrite(System::EventLoop& loop, const char* buffer, size_t n);

        size_t write(const char* buffer, size_t n);

    private:
        UdpSocket& _device;
        std::size_t _timeout;
        bool _broadcast;
        bool _isConnected;
        bool _isBound;
        unsigned int _hopLimit;
        Windows::Storage::Streams::DataWriter^ _writer;
        Windows::Storage::Streams::DataWriterStoreOperation ^ _storeOp;
        size_t _storeCount;
};

} // namespace Net

} // namespace Pt

#endif
