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
#include "Pt/System/Runnable.h"
#include "Pt/System/Thread.h"
#include "IODeviceImpl.h"

namespace Pt{
namespace System{

class IOResultSerial : public IOResultImpl
{
protected:
    IOResultSerial()
    : _writtenBytes(0)
    , _bufferSize(0)
    , _buffer(0)
    {}
public:    
    virtual ~IOResultSerial()
    {}

    void attach(char* buffer, size_t size)
    {
        _writtenBytes = 0;
        _buffer = buffer;
        _bufferSize = size;
    }

    void setWrittenBytes(size_t bytes)
    {
        _writtenBytes = bytes;
    }

    size_t writtenBytes() const
    {
        return _writtenBytes;
    }

    char* buffer() const
    { return _buffer; }

    size_t bufferSize() const
    { return _bufferSize; }    

private:
    char*   _buffer;
    size_t  _bufferSize;
    size_t  _writtenBytes;
};

class ReadResultSerial : public IOResultSerial
{
public:
    virtual void onComplete()
    {
        this->device()->inputReady(*this);        
    }
};

class WriteResultSerial : public IOResultSerial
{
public:
    virtual void onComplete()
    {
        this->device()->outputReady(*this);        
    }
};

class SerialDeviceImpl :  public Pt::System::IODeviceImpl , public Pt::System::Runnable
{
    public:
        SerialDeviceImpl();
        ~SerialDeviceImpl();

        void open( const std::string& file, std::ios_base::openmode mode, bool isAsync);         

        //! @brief Closes the I/O device
        void close();

        IOResult& beginRead(char* buffer, size_t n, bool& eof);    

        size_t endRead(IOResult& result, bool& eof);

        IOResult& beginWrite(const char* buffer, size_t n);

        size_t endWrite(IOResult& result);

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
       
        HANDLE deviceHandle() const
        { return _handle; }   
        
    private:
        SerialDeviceImpl* self()
        { return this; }
        
        void writeCommState( DCB& commState );
        void readCommState( DCB& commState ) const;
        
        void run();       
       
        enum { CharReceived, EventCharReceived, SendComplete } _commEventType;
        
public:
        HANDLE                 _handle;        
        HANDLE                 _comEvent;
        HANDLE                 _waitForComEvent;
        DCB                    _orgCommState;        
        Thread                 _eventThread;  
        bool                   _terminateThread;
        ReadResultSerial       _readResult;
        WriteResultSerial      _writeResult;
        
};

}//namespace System
}//namespaec Pt

#endif
