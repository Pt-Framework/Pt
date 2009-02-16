/*
 * Copyright (C) 2006-2009 by Marc Boris Duerner, Tommi Maekitalo
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

#ifndef PT_NET_TcpSocketImpl_H
#define PT_NET_TcpSocketImpl_H

#include "Pt/Net/Api.h"
#include "Pt/Signal.h"
#include "SelectableImpl.h"
#include <string>

namespace Pt {

namespace System {
    class SelectorBase;
}

namespace Net {

class TcpServer;
class TcpSocket;

class TcpSocketImpl : public System::SelectableImpl
{
    private:
        bool _isConnected;
        std::size_t _timeout;

    public:
        TcpSocketImpl(TcpSocket& socket);

        ~TcpSocketImpl();

        std::string getSockAddr() const;

        void close();

        void setTimeout(std::size_t msecs)
        { _timeout = msecs; }

        std::size_t timeout() const
        { return _timeout; }

        void accept(TcpServer& server);

        bool isConnected() const
        { return _isConnected; }

        void connect(const std::string& ipaddr, unsigned short int port);

        bool beginConnect(const std::string& ipaddr, unsigned short int port)
        {return false;}

        void endConnect()
        {}

        size_t beginRead(char* buffer, size_t n, bool& eof);

        size_t endRead(bool& eof);

        size_t read(char* buffer, size_t count, bool& eof);

        size_t beginWrite(const char* buffer, size_t n);

        size_t endWrite();

        size_t write(const char* buffer, size_t count);

        bool wait(std::size_t msecs)
        { return false; }

        void attach(System::SelectorBase& sb)
        { }

        void detach(System::SelectorBase& sb)
        { }
};

} // namespace Net

} // namespace Pt

#endif
