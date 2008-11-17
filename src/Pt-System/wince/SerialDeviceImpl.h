/***************************************************************************
 *   Copyright (C) 2007 Marc Boris D�rner                                  *
 *   Copyright (C) 2007 Laurentiu-Gheorghe Crisan                          *
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
#ifndef PT_SYSTEM_SERIALDEVICEIMPL_H
#define PT_SYSTEM_SERIALDEVICEIMPL_H

#include "IODeviceImpl.h"
#include "Pt/System/IODevice.h"
#include "Pt/System/IOError.h"
#include "Pt/System/SerialDevice.h"
#include "Pt/System/Thread.h"
#include <string>
#include <windows.h>

namespace Pt{

namespace System{

class SerialDeviceImpl : public Pt::System::IODeviceImpl
{
    public:
        SerialDeviceImpl(SerialDevice& device);

        ~SerialDeviceImpl();

        void open( const std::string& file, IODevice::OpenMode mode);

        //! @brief Closes the I/O device
        void close();

        void attach(SelectorBase& mon);

        void detach(SelectorBase& mon);

        bool wait(std::size_t msecs);

        bool setWaitHandle(HANDLE h, bool& avail);
		
        void getWaitHandles(HandleMap& handles, bool& avail);
		
        bool checkEvent();

        size_t beginRead(char* buffer, size_t n, bool& eof);    

        size_t endRead( bool& eof);

        size_t beginWrite(const char* buffer, size_t n);

        size_t endWrite();

        //! @brief Read bytes from device
        size_t read( char* buffer, size_t count, bool& eof );

        //! @brief Write bytes to device
        size_t write( const char* buffer, size_t count );

        void flush();

        void setBaudRate( SerialDevice::BaudRate rate );
        SerialDevice::BaudRate baudRate() const;

        void setCharSize( int size );
        int charSize() const;

        void setStopBits( SerialDevice::StopBits bits );
        SerialDevice::StopBits stopBits() const;    

        void setParity( SerialDevice::Parity parity );
        SerialDevice::Parity parity() const;

        void setFlowControl( SerialDevice::FlowControl flowControl );
        SerialDevice::FlowControl flowControl() const;      
        
        void setTimeout( size_t timeout );       
        size_t timeout() const;  
        
    private:
        SerialDeviceImpl* self()
        { return this; }
        
        void writeCommState( DCB& commState );

        void readCommState( DCB& commState ) const;
        
        void run();       
        
    public:
		SerialDevice& _device;
        HANDLE _ioReady;
        HANDLE _beginWait;
        DCB _orgCommState;        
        AttachedThread* _thread;  
        bool _terminateThread;
        DWORD _rlen;
        size_t _wlen;
        DWORD _event;
};

}//namespace System
}//namespaec Pt

#endif
