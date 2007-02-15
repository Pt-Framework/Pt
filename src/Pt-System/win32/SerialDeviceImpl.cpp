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
#include "SerialDeviceImpl.h"

namespace Pt{
namespace System{

SerialDeviceImpl::SerialDeviceImpl( const std::string& file, std::ios_base::openmode mode ) throw(IO::IOError)
{    
    _handle = CreateFile( file.c_str() , GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    
    if ( _handle == 0 )
        throw IO::IOError("Could not open port" , PT_SOURCEINFO);
        
  // instance an object of COMMTIMEOUTS.
    COMMTIMEOUTS comTimeOut;                   
    // Specify time-out between charactor for receiving.
    comTimeOut.ReadIntervalTimeout = 3;
    // Specify value that is multiplied 
    // by the requested number of bytes to be read. 
    comTimeOut.ReadTotalTimeoutMultiplier = 3;
    // Specify value is added to the product of the 
    // ReadTotalTimeoutMultiplier member
    comTimeOut.ReadTotalTimeoutConstant = 2;
    // Specify value that is multiplied 
    // by the requested number of bytes to be sent. 
    comTimeOut.WriteTotalTimeoutMultiplier = 3;
    // Specify value is added to the product of the 
    // WriteTotalTimeoutMultiplier member
    comTimeOut.WriteTotalTimeoutConstant = 2;
    // set the time-out parameter into device control.
    
    if( !SetCommTimeouts( _handle, &comTimeOut ) )
        throw IO::IOError("Set port time outs failed" , PT_SOURCEINFO);
            
    if( !GetCommState( _handle, _commState ) )
        throw IO::IOError("Get port state failed" , PT_SOURCEINFO);        
}

SerialDeviceImpl::~SerialDeviceImpl()
{  }
        
void SerialDeviceImpl::close()
{
    if( _handle != 0 )
        CloseHandle( _handle );

    _handle = 0;        
}

bool SerialDeviceImpl::wait( IO::IODevice::WaitMode mode, unsigned int msec )
{
    return true;
}

size_t SerialDeviceImpl::read( char* buffer, size_t count )
{
    size_t length;
    
     if( ! ReadFile( _handle, buffer, count, sizeBuffer, &length, 0 ) )
        throw IO::IOError("Read port failed" , PT_SOURCEINFO);
        
    return length;     
}

size_t SerialDeviceImpl::write( const char* buffer, size_t count )
{ 
    DWORD noOfBytesWritten = 0;

    if( !WriteFile(  _handler,  buffer,  count, &noOfBytesWritten, 0 )
        throw IO::IOError("Write port failed" , PT_SOURCEINFO);

    return noOfBytesWritten;
}

void SerialDeviceImpl::updateCommState()
{
    if( !SetCommState( _handle, &_commState ) )
        throw IO::IOError("Changing port state failed" , PT_SOURCEINFO);        
}

void SerialDeviceImpl::setBaudRate( BaudRate rate )
{
    _commState.BaudRate = static_cast<DWORD>( rate );
    updateCommState();
}

BaudRate SerialDeviceImpl::baudRate() const
{
    return static_cast<BaudRate>( _commState.BaudRate );
}

void SerialDeviceImpl::setCharSize( int size )
{
    _commState.ByteSize  = size;
    updateCommState();
}

int SerialDeviceImpl::charSize() const
{
    return _commState.ByteSize;
}
              
void SerialDeviceImpl::setStopBits( StopBits bits )
{
    switch( bits )
    {
        case OneStopBit: 
            _commState.StopBits  = ONESTOPBIT;
    }
}
        short stopBits();    
                
        void setParity( Parity Parity );        
        Parity parity() const;
                
        void setFlowControl( FlowControl FlowControl );              
        FlowControl flowControl() const;
        
        void setInputMode( InputMode mode );
        InputMode inputMode() const;
              
        void setControlMode( ControlMode mode );
        ControlMode controlMode() const;
                
        LocalMode localMode() const;
        void setLocalMode( LocalMode mode );
      
        OutputMode outputMode() const;
        void setOutputMode( OutputMode mode );
        
        int vMin()const ;         
        void setVMin( const int chars );
      
        int vTime() const;
        void setVTime(const int secs);


}//namespace System
}//namespace Pt