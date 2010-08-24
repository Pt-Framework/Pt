/*
 * Copyright (C) 2006-2009 by Marc Boris Duerner, Tommi Maekitalo
 *                            Laurentiu-Gheorghe Crisan
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

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "Pt/Net/Api.h"
#include "Pt/Signal.h"
#include "Pt/Net/AddrInfo.h"
#include "SelectableImpl.h"
#include "AddrInfoImpl.h"
#include <string>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

namespace Pt {

namespace System {
    class EventLoop;
}

namespace Net {

class AddrInfo;
class TcpServer;
class TcpSocket;

class TcpSocketImpl : public System::SelectableImpl
{
    struct DestructionSentry
    {
        DestructionSentry(DestructionSentry*& sentry)
        : _deleted(false)
        , _sentry(sentry)
        {
           sentry = this;
        }

        ~DestructionSentry()
        {
            if( ! _deleted )
                this->detach();
        }

        bool operator!() const
        { return _deleted; }

        void detach()
        {
            _sentry = 0;
            _deleted = true;
        }

        bool _deleted;
        DestructionSentry*& _sentry;
    };

	private:
		AddrInfo _addrInfo;
		const char* _connectResult;
		AddrInfoImpl::const_iterator _addrInfoPtr;
        DestructionSentry* _sentry;
        SOCKET	      _fd;
		SOCKADDR      _peeraddr;
		TcpSocket&    _socket;
		WSAEVENT      _waitEvent;
		std::size_t	  _timeout;
//		WSAOVERLAPPED _connectOverlapped;
		WSAOVERLAPPED _sendOverlapped;
//		WSAOVERLAPPED _receiveOverlapped;
		WSABUF        _sendBuffer;
		HANDLE		  _currentEventHandle;
		WSABUF		  _receiveBuffer;
		bool          _isConnected;
		long		  _eventFlags;
		size_t		  _dataSends;

		int checkConnect();
        void attachEvent(HANDLE ev, long events);
		size_t checkReceiveResult(bool& eof);
		size_t checkSendResult();
		const char* tryConnect();
		void checkPendingError();
    public:
        TcpSocketImpl(TcpSocket& socket);

        ~TcpSocketImpl();

        void close();

        bool isConnected() const
        { return _isConnected; }

        void accept(const TcpServer& server, bool closeOnExec);

        void connect(const AddrInfo& addrinfo);

        bool beginConnect(const AddrInfo& addrinfo);

        void endConnect();

		size_t beginRead(char* buffer, size_t n, bool& eof);

		size_t read(char* buffer, size_t count, bool& eof);

		size_t endRead(bool& eof);

		size_t beginWrite(const char* buffer, size_t n);

		size_t endWrite();

		size_t write(const char* buffer, size_t count);

		std::string getSockAddr() const;

		std::string getPeerAddr() const;

		void setTimeout(std::size_t msecs)
		{
			_timeout = msecs;
		}

		std::size_t timeout() const
		{
			return _timeout;
		}

        bool wait(std::size_t msecs);

        void attach(System::EventLoop& sb);

        void detach(System::EventLoop& sb);

		bool setWaitHandle(HANDLE h, bool& avail);

		void getWaitHandles(System::HandleMap& handles, bool& avail);

		bool checkEvent();

        void cancel();
};

} // namespace Net

} // namespace Pt

#endif

