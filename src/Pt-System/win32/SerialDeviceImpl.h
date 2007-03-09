/***************************************************************************
 *   Copyright (C) 2007 Marc Boris Dürner                                  *
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

#include <string>
#include <windows.h>

#include "Pt/System/IODevice.h"
#include "Pt/System/IOError.h"
#include "Pt/System/SerialDevice.h"
#include "IODeviceImpl.h"

namespace Pt{
namespace System{

class SerialDeviceImpl : public IODeviceImpl
{
    public:
        SerialDeviceImpl();
        ~SerialDeviceImpl();

        void open( const std::string& file, std::ios_base::openmode mode );

        //! @brief Closes the I/O device
        void close();

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

        bool wait( SerialDevice::WaitMode mode, unsigned int  msec );

        HANDLE handle() const
        { return _handle; }
        
        virtual const IOEvent& waitEvent() const;
        
    private:
        void writeCommState( DCB& commState );
        void readCommState( DCB& commState ) const;

        HANDLE           _handle;
        DCB              _orgCommState;
        OVERLAPPED       _overlapped;
        HANDLE           _terminateEv;

};

}//namespace System
}//namespaec Pt

#endif
