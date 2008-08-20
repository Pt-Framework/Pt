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

#include "IODeviceImpl.h"
#include "Pt/System/IOError.h"
#include "Pt/System/SerialDevice.h"
#include <string>
#include <windows.h>

namespace Pt{

namespace System{

class SerialDeviceImpl : public IODeviceImpl
{
    public:
        SerialDeviceImpl();
        
        ~SerialDeviceImpl();

        void open( const std::string& file, std::ios_base::openmode mode, bool isAsync );

        void close();

        void attach(SelectorBase& s);

        void detach(SelectorBase& s);

        bool wait(unsigned int msecs);
        
        bool setWaitHandle(HANDLE h, std::set<Selectable*>* actives);
        
        bool checkEvent();
        
        void beginRead(char* buffer, size_t n, bool& eof);

		size_t endRead(bool& eof);

        void beginWrite(const char* buffer, size_t n);

        size_t endWrite();		
       
        size_t read( char* buffer, size_t count, bool& eof );        
        
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
        
        void setTimeout( size_t msec );
        
        size_t timeout() const;    
        
    private:
        void writeCommState( DCB& commState );
        
        void readCommState( DCB& commState ) const;

    private:
        HANDLE _waitHandle;
        OVERLAPPED _readOv;
        char* _rbuf;
        size_t _rbuflen;
        OVERLAPPED _writeOv;
        const char* _wbuf;
        size_t _wbuflen;
        DCB _orgCommState;
};

}//namespace System

}//namespaec Pt

#endif
