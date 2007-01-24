/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Drner                               *
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

    class PT_NET_API Socket // : public IO::IODevice
    {
        public:
            enum WaitMode
            {
                WaitInput  = 0x1,
                WaitOutput = 0x2
            };

            virtual ~Socket();

            virtual void setTimeout(size_t msec)
            { _timeout = msec; }

            size_t getTimeout() const
            { return _timeout; }

            /** @brief Waits for I/O

                If the socket is in asynchronous mode, this function waits
                for I/O events. It can be waited for input or outut events.

                \param mode WaitInput or WaitOutput.
                \param msec time interval to wait
                \return true if an I/O operatin has occured.
                \throw IOError
            */
            bool wait(WaitMode mode, unsigned int msec)
            { return this->_wait(mode, msec); }

        protected:
            Socket();

            //! @brief Waits until data is available
            virtual bool _wait(WaitMode, unsigned int)
            { return false; }

        private:
            size_t _timeout;
    };

} // !namespace Net

} // !namespace Pt

#endif
