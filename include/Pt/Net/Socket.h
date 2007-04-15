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

namespace Pt
{

namespace Net
{

    /** @brief Returns a 32-bit value from host to TCP/IP network
        byte order (which is big endian).
        
        \param hostlong 32-bit value in host byte order. 
        
        \return 32-bit value in TCP/IP's network byte order.
     */
    unsigned long fromHost(unsigned long hostlong);
    
    /** @brief Returns a 32-bit value from TCP/IP network order to
        host byte order (which is little-endian on Intel processors).
        If the netlong parameter was already in host byte order, then
        no operation is performed.
        
        \param netlong 32-bit value in TCP/IP network byte order.
        
        \return 32-bit value in host byte order.
     */
    unsigned long toHost(unsigned long netlong);
    
    /** @brief Returns a 32-bit value from host to TCP/IP network
        byte order (which is big endian).
        
        \param hostint 32-bit value in host byte order. 
        
        \return 32-bit value in TCP/IP's network byte order.
     */
    unsigned int fromHost(unsigned int hostint);
    
    /** @brief Returns a 32-bit value from TCP/IP network order to
        host byte order (which is little-endian on Intel processors).
        If the netlong parameter was already in host byte order, then
        no operation is performed.
        
        \param netint 32-bit value in TCP/IP network byte order.
        
        \return 32-bit value in host byte order.
     */
    unsigned int toHost(unsigned int netint);
    
   /** @brief Returns a 16-bit value from host to TCP/IP network
        byte order (which is big endian).
        
        \param hostlong 16-bit value in host byte order. 
        
        \return 16-bit value in TCP/IP's network byte order.
     */
    unsigned short fromHost(unsigned short hostshort);
    
   /** @brief Returns a 16-bit value from TCP/IP network byte order to
        host byte order (which is little-endian on Intel processors).
        If the netshort parameter was already in host byte order, then
        no operation is performed.
        
        \param netshort 16-bit value in network byte order.
        
        \return 16-bit value in host byte order.
     */
    unsigned short toHost(unsigned short netshort);

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
