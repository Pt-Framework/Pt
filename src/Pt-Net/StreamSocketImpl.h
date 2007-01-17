/***************************************************************************
 *   Copyright (C) 2006 by Marc Boris Dürner, Tommi Maekitalo              *
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

#ifndef Pt_Net_StreamSocketImpl_h
#define Pt_Net_StreamSocketImpl_h

#include <string>
#include <Pt/Api.h>
#include "AddrInfo.h"
#include "SocketImpl.h"
#include "Pt/Types.h"
#include "Pt/Net/Timeout.h"


namespace Pt
{
namespace Net
{
    class StreamServerSocketImpl;

    class StreamSocketImpl : public SocketImpl
    {
        public:
            StreamSocketImpl()
            { }

            StreamSocketImpl(SOCKET sd, struct sockaddr_storage peeraddr_)
              : SocketImpl(sd),
                peeraddr(peeraddr_)
            { }

            void connect(const std::string& ipaddr, unsigned short int port)
			{
				// give some useful default values to use for getaddrinfo()
				struct addrinfo hints;
				memset(&hints, 0, sizeof(hints));
				hints.ai_socktype = SOCK_STREAM;

				AddrInfo ai(ipaddr, port, hints);

				for (AddrInfo::const_iterator it = ai.begin(); it != ai.end(); ++it)
				{
					SocketImpl::create(it->ai_family, SOCK_STREAM, 0);

					if ( ::connect(handle(), it->ai_addr, it->ai_addrlen) == 0 )
					{
						// save our information
						memmove(&peeraddr, it->ai_addr, it->ai_addrlen);
						return;
					}

					close();
				}

				throw std::runtime_error("connect" + PT_SOURCEINFO);
			}
			
            size_t read(char* buffer, size_t count, bool& eof)
			{
				ssize_t n;

				if (getTimeout() < 0)
				{
					// blocking read
					do
					{
						n = ::recv(handle(), buffer, count, 0);
					} while (n <= 0 && this->lastError() == PT_EINTR);

					if (this->lastError() == PT_ECONNRESET)
						eof = true;
					else if (n < 0)
						throw std::runtime_error("read" + PT_SOURCEINFO); // TODO
				}
				else
				{
					// non-blocking read

					// try reading without timeout
					do
					{
						n = ::recv(handle(), buffer, count, 0);
					} while (n <= 0 && this->lastError() == PT_EINTR);

					if (n <= 0)
					{
						// no data available

						if (this->lastError() == PT_EAGAIN)
						{
							if (getTimeout() == 0)
							  return 0;

							this->wait(WaitInput, getTimeout());

							do
							{
								n = ::recv(handle(), buffer, count, 0);
							} while (n <= 0 && this->lastError() == PT_EINTR);

							if (n < 0)
								throw std::runtime_error("read" + PT_SOURCEINFO); // TODO
						}
						else if (this->lastError() == PT_ECONNRESET)
							eof = true;
						else if (this->lastError() != 0)
							throw std::runtime_error("read" + PT_SOURCEINFO); // TODO
					}

				}

				return n < 0 ? 0 : n;
			}
			
            size_t write(const char* buffer, size_t count)
			{
				ssize_t n = 0;
				size_t s = count;

				while (true)
				{
					do
					{
						n = ::send(handle(), buffer, s, 0);
					} while (n <= 0 && this->lastError() == PT_EINTR);

					if (n <= 0)
					{
						if (this->lastError() == PT_EAGAIN)
							n = 0;
						else
							throw std::runtime_error("write" + PT_SOURCEINFO); // TODO
					}

					buffer += n;
					s -= n;

					if (s <= 0)
						break;

					if (getTimeout() == 0)
						return count - s;

					this->wait(WaitOutput, getTimeout());
				}

				return count;
			}

            void setTimeout(ssize_t msec)
                { timeout = msec; }

            ssize_t getTimeout() const
                { return timeout; }

        private:
            struct sockaddr_storage peeraddr;
            ssize_t timeout;
    };
}
}

#endif
