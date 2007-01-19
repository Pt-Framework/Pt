/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
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

#ifndef Pt_Net_Socket_h
#define Pt_Net_Socket_h

#include <Pt/Net/Api.h>
#include <Pt/Types.h>
// #include <Pt/IO/IODevice.h>


namespace Pt
{

namespace Net
{

	class PT_API Socket // : public IO::IODevice
    {
		public:
			virtual ~Socket();

			virtual void setTimeout(size_t msec)
              { _timeout = msec; }

            size_t getTimeout() const
              { return _timeout; }

		protected:
            Socket();

			bool _remote() const;

        private:
            size_t _timeout;
	};

} // !namespace Net

} // !namespace Pt

#endif
