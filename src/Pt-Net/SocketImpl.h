/***************************************************************************
 *   Copyright (C) 2006 by Marc Boris Duerner, Tommi Maekitalo             *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef Pt_Net_SocketImpl_h
#define Pt_Net_SocketImpl_h

#if defined(WIN32) || defined(_WIN32)
	#include <winsock2.h>
	#define SHUT_RD   1
	#define SHUT_RDWR 2
	#define PT_INVALID_SOCKET INVALID_SOCKET
	#define PT_EINTR WSAEINTR
	#define PT_ECONNRESET WSAECONNRESET
	#define PT_EAGAIN WSAEWOULDBLOCK
#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/poll.h>
	#include <cerrno>
	typedef int SOCKET;
	#define PT_INVALID_SOCKET -1
	#define PT_EINTR EINTR
	#define PT_ECONNRESET ECONNRESET
	#define PT_EAGAIN EAGAIN
#endif

#include <Pt/Exception.h>


namespace Pt {

namespace Net {

    class SocketImpl
    {
        public:
            enum WaitMode
            {
                WaitInput = 0x1, WaitOutput = 0x2
            };

        public:
            SocketImpl()
            : _sd(PT_INVALID_SOCKET)
            { }
        
            explicit SocketImpl(SOCKET sd)
            : _sd(sd)
            { }
        
            ~SocketImpl()
            {
                if (_sd != PT_INVALID_SOCKET)
                    this->close();
            }
        
            static int lastError()
            {
                #ifdef WIN32
                    return WSAGetLastError();
                #else
                    return errno;
                #endif
            }
			
            void create(int domain, int type, int protocol)
            {
                
                #ifdef WIN32
                {   // TODO: concurrency
                    WSADATA wsadata;
                    WSAStartup(MAKEWORD(2,0), &wsadata);
                }
                #endif
            
                _sd = ::socket(domain, type, protocol);
                if (_sd < 0)
                    throw RuntimeError("cannot create socket", PT_SOURCEINFO);
                    // TODO change exceptiontype
            }
        
            void close()
            {
                ::shutdown(_sd, SHUT_RDWR);
                
                #ifdef WIN32
                ::closesocket(_sd);
                #else
                ::close(_sd);
                #endif
                
                _sd = PT_INVALID_SOCKET;
            }


            bool wait(SocketImpl::WaitMode mode, int msec) const
            {
                fd_set rfds;
                FD_ZERO(&rfds);
                FD_SET(_sd, &rfds);
        
                struct timeval tv;
                tv.tv_sec = msec / 1000;
                tv.tv_usec = (msec % 1000) * 1000;
        
                _select:
                int ret = -1;
        
                switch(mode)
                {
                    case SocketImpl::WaitInput:
                        ret = select(_sd + 1, &rfds, 0, 0, &tv);
                        break;
            
                    case SocketImpl::WaitOutput:
                        ret = select(_sd + 1, 0, &rfds, 0, &tv);
                        break;
                }
        
                // error
                if(ret == -1)
                {
                    if(this->lastError() == PT_EINTR)
                        goto _select;

                 throw Exception("Could not select on socket", PT_SOURCEINFO); //TODO
                }
        
                // data available
                if(ret == 1)
                    return true;
        
                // no data available
                return false;
            }

        protected:
            SOCKET handle() const
            { return _sd; }

        private:
            SOCKET _sd;
    };

} // namespace Net

} //namespace Pt

#endif
