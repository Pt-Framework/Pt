/*
 * Copyright (C) 2006 by Marc Boris Duerner, Tommi Maekitalo
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

#ifndef Pt_Net_TcpSocket_h
#define Pt_Net_TcpSocket_h

#include <Pt/Net/Api.h>
#include <Pt/Net/Socket.h>
#include <Pt/System/IODevice.h>

namespace Pt {

namespace Net {

    class TcpSocketImpl;
    class TcpServerSocket;

    class PT_NET_API TcpSocket : public Socket, public System::IODevice
    {
        public:
            TcpSocket()
            : _impl(0)
            { }

            TcpSocket(const std::string& ipaddr, unsigned short int port)
            : _impl(0)
            { connect(ipaddr, port); }

            TcpSocket(const TcpServerSocket& server)
            : _impl(0)
            { accept(server); }

            ~TcpSocket();

            void setTimeout(ssize_t msec);

            void connect(const std::string& ipaddr, unsigned short int port);
            void accept(const TcpServerSocket& server);

            unsigned long availableBytes(void);

            virtual System::IODeviceImpl& ioimpl()
            { System::IODeviceImpl* impl = 0; return *impl; }

            virtual System::SelectableImpl& simpl()
            { System::SelectableImpl* impl = 0; return *impl; }
            
        protected:
            virtual bool onWait(size_t msecs)
            { return false; }

            virtual void onAttach(System::SelectorBase&)
            {}

            virtual void onDetach(System::SelectorBase&)
            {}

            size_t onBeginRead(char* buffer, size_t n, bool& eof)
            { return 0; }

            size_t onEndRead( bool& eof)
            { return 0; }

            size_t onBeginWrite(const char* buffer, size_t n)
            { return 0; }

            size_t onEndWrite()
            { return 0; }

            size_t onRead(char* buffer, size_t count, bool& eof);

            size_t onWrite(const char* buffer, size_t count);

            bool _wait(Socket::WaitMode, unsigned int);

            void onClose();

            bool _waitable() const
            { return true; }

        private:
            TcpSocketImpl* _impl;
    };

} // !namespace Net

} // !namespace Pt

#endif
