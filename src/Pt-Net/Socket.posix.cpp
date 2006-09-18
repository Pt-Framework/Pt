/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Drner                               *
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
#ifndef _Pt_Net_SocketImpl_h
#define _Pt_Net_SocketImpl_h

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "Pt/Exception.h"
using namespace Pt;

#include "Pt/Net/Socket.h"
using namespace Pt::Net;

#include <iostream>
using namespace std;


class PT_EXPORT SocketImpl {
	public:
		SocketImpl()
		: _handle(0),
		  _timeout(-1)
		{ }

		virtual ~SocketImpl()
		{ }

		bool wait(Socket::WaitMode mode, unsigned int msec) throw(IO::IOError)
		{
			//cerr << "Select: " << _handle << endl;
			fd_set rfds;
			FD_ZERO(&rfds);
			FD_SET(_handle, &rfds);

			struct timeval tv;
			tv.tv_sec = msec / 1000;
			tv.tv_usec = (msec % 1000) * 1000;

			_select:
			int ret = -1;

			switch(mode) {
				case Socket::WaitInput:
					ret = select(_handle + 1, &rfds, 0, 0, &tv);
					break;

				case Socket::WaitOutput:
					ret = select(_handle + 1, 0, &rfds, 0, &tv);
					break;

				case Socket::WaitError:
					ret = select(_handle + 1, 0, 0, &rfds, &tv);
					break;
			}

			if(ret == -1) {
				if(errno == EINTR)
					goto _select;
				throw IO::IOError(errno, "Could not select on socket", PT_SOURCEINFO);
			}
		
			if(ret == 1)
				return true;
		
			return false;
		}

		void setTimeOut(int msec) throw(IO::IOError)
		{
			//cerr << "Timeout: " << msec  << endl;
			_timeout = msec;

			int flags = ::fcntl(_handle, F_GETFL);
			if(flags == -1)
				throw IO::IOError(errno, "Could not set timeout", PT_SOURCEINFO);

			if(_timeout < 0)
				flags &= ~O_NONBLOCK;
			else
				flags |= O_NONBLOCK;

			int ret = ::fcntl(_handle, F_SETFL, flags);
			if(ret == -1)
				throw IO::IOError(errno, "Could not set timeout", PT_SOURCEINFO);
		}

		void close()
		{
			//printf("Closed: %i\n", _handle);
			int ret = ::close(_handle);
			if(ret == -1)
				throw IO::IOError(errno, "Could not close socket", PT_SOURCEINFO);
		}

	protected:
		long _handle;
		int _timeout;
};


#endif

