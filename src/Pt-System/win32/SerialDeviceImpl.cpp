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
#include "win32.h"
#include "SerialDeviceImpl.h"

namespace Pt{
namespace System{

SerialDeviceImpl::SerialDeviceImpl( const std::string& port_, std::ios_base::openmode mode ) throw(IO::IOError)
{    
    std::basic_string<TCHAR> port = win32::fromMultiByte( port_.c_str() );
    
    _handle = CreateFile( port.c_str() , GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
       
    if ( _handle == 0  || _handle == INVALID_HANDLE_VALUE )
        throw IO::IOError("Could not open port" , PT_SOURCEINFO);
        
    COMMTIMEOUTS comTimeOut;                   
    comTimeOut.ReadIntervalTimeout          = MAXDWORD;  
    comTimeOut.ReadTotalTimeoutMultiplier   = 0;  
    comTimeOut.ReadTotalTimeoutConstant     = 0;    
    comTimeOut.WriteTotalTimeoutMultiplier  = 10;  
    comTimeOut.WriteTotalTimeoutConstant    = 1000;  
    
    if( !SetCommTimeouts( _handle, &comTimeOut ) )
        throw IO::IOError("Set port time outs failed" , PT_SOURCEINFO);
            
    if( !GetCommState( _handle, &_commState ) )
        throw IO::IOError("Get port state failed" , PT_SOURCEINFO);        
}

SerialDeviceImpl::~SerialDeviceImpl()
{ }
        
void SerialDeviceImpl::close()
{
    if( _handle != 0 )
        CloseHandle( _handle );

    _handle = 0;        
}

size_t SerialDeviceImpl::read( char* buffer, size_t count, bool& eof )
{
    DWORD length;
  
    if( ! ReadFile( _handle, buffer, count, &length, 0 ) )
        throw IO::IOError("Read port failed" , PT_SOURCEINFO);
        
    return length;     
}

size_t SerialDeviceImpl::write( const char* buffer, size_t count )
{ 
    DWORD noOfBytesWritten = 0;

    if( !WriteFile(  _handle,  buffer,  count, &noOfBytesWritten, 0 ) )
        throw IO::IOError("Write port failed" , PT_SOURCEINFO);

    return noOfBytesWritten;
}

void SerialDeviceImpl::updateCommState()
{
    if( !SetCommState( _handle, &_commState ) )
        throw IO::IOError("Changing port state failed" , PT_SOURCEINFO);        
}

void SerialDeviceImpl::setBaudRate( SerialDevice::BaudRate rate )
{
    _commState.BaudRate = static_cast<DWORD>( rate );
    updateCommState();
}

SerialDevice::BaudRate SerialDeviceImpl::baudRate() const
{
    return static_cast<SerialDevice::BaudRate>( _commState.BaudRate );
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
              
void SerialDeviceImpl::setStopBits( SerialDevice::StopBits bits )
{
    switch( bits )
    {
        case SerialDevice::OneStopBit: 
            _commState.StopBits  = ONESTOPBIT;
        break;
        
        case SerialDevice::One5StopBits:
            _commState.StopBits  = ONE5STOPBITS;
        break;
        
        case SerialDevice::TwoStopBits:
            _commState.StopBits  = TWOSTOPBITS;
        break;
    }
    
    updateCommState();
}

SerialDevice::StopBits SerialDeviceImpl::stopBits() const
{
    switch( _commState.StopBits )
    {
        case ONESTOPBIT: 
            return SerialDevice::OneStopBit;
        break;

        case ONE5STOPBITS:
            return SerialDevice::One5StopBits;
        break;

        case TWOSTOPBITS:
            return SerialDevice::TwoStopBits;
        break;
    }
    
    throw std::runtime_error( "Unknown stop bits" + PT_SOURCEINFO);
    
    return SerialDevice::OneStopBit;
}
                
void SerialDeviceImpl::setParity( SerialDevice::Parity parity )
{
    switch( parity )
    {
        case SerialDevice::ParityEven:
            _commState.Parity = EVENPARITY;
        break;
        
        case SerialDevice::ParityOdd:
            _commState.Parity = ODDPARITY; 
        break;
        
        case SerialDevice::ParityNone:
            _commState.Parity = NOPARITY; 
        break;            
    }
    
    updateCommState();
}

SerialDevice::Parity SerialDeviceImpl::parity() const
{
    switch( _commState.Parity )
    {
        case EVENPARITY:
            return SerialDevice::ParityEven;
        break;
        
        case ODDPARITY:
            return SerialDevice::ParityOdd; 
        break;
        
        case NOPARITY :
            return SerialDevice::ParityNone; 
        break;            
    }
     
    throw std::runtime_error( "Invalid parity" + PT_SOURCEINFO);
    return SerialDevice::ParityEven;    
}
        
void SerialDeviceImpl::setFlowControl( SerialDevice::FlowControl flowControl )            
{
    if(  flowControl  == SerialDevice::FlowControlHard)
        _commState.fOutxCtsFlow  = TRUE;
    else            
        _commState.fOutxCtsFlow  = FALSE;
    
    updateCommState();
}

SerialDevice::FlowControl SerialDeviceImpl::flowControl() const
{    
    if( _commState.fOutxCtsFlow )
        return SerialDevice::FlowControlHard;
    else
        return SerialDevice::FlowControlSoft;            
}

}//namespace System
}//namespace Pt
