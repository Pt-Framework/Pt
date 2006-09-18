/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
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

#include "Socket.posix.cpp"


class PT_EXPORT TcpSocketImpl : virtual public SocketImpl {
	public:
		TcpSocketImpl()
		{ }

		~TcpSocketImpl()
		{}

		void listen() throw(IO::IOError)
		{
			//cerr << "Listen: " << _handle << endl;

			int ret = ::listen(_handle, 128);
			if(ret == -1)
				throw IO::IOError(errno, "Could not listen on socket", PT_SOURCEINFO);
		}

		size_t read(char* buffer, size_t n)
		{
			ssize_t ret = 0;

			if(_timeout < 0) {
				// blocking read
				ret = ::recv(_handle, (void*)buffer, n, MSG_NOSIGNAL);
				if(ret == -1) {
					throw IO::IOError(errno, "Could not read from socket.", PT_SOURCEINFO);
				}
				return ret;
			}

			// non-blocking read
			ret = ::read(_handle, buffer, n);
			if(ret < 0) {
				// no data available
				if(errno == EAGAIN) {
					// no timeout
					if(_timeout == 0)
						throw IO::IOError(errno, "Could not read from socket.", PT_SOURCEINFO);

					// wait for timeout
					bool avail = SocketImpl::wait(Socket::WaitInput, _timeout);
					if(!avail) {
						throw IO::IOError("Could not read from socket: Timeout.", PT_SOURCEINFO);
					}

					ret = ::read(_handle, buffer, n);
					if (ret < 0)
						throw IO::IOError(errno, "Could not read from socket.", PT_SOURCEINFO);
				}
				else {
					throw IO::IOError(errno, "Could not read from socket.", PT_SOURCEINFO);
				}
			}

			return ret;
		}

		size_t write(const char* buffer, size_t count) throw(IO::IOError)
		{
			size_t left = count;

			while(true) {
				ssize_t ret = ::send(_handle, (const void*)buffer, left, MSG_NOSIGNAL);
				if(ret == -1)
					throw IO::IOError(errno, "Could not write to socket.", PT_SOURCEINFO);

				buffer += ret;
				left -= ret;

				if(left <= 0)
					break;

				if(_timeout >= 0) {
					bool avail = SocketImpl::wait(Socket::WaitOutput, _timeout);
					if(!avail)
						throw IO::IOError("Could not write to socket: Timeout.", PT_SOURCEINFO);
				}
			}

			return count;
		}
};






