/***************************************************************************
 *   Copyright (C) 2007 Marc Boris Drner                                  *
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "SerialDeviceImpl.h"

#include <cerrno>


namespace Pt {

namespace System {


SerialDeviceImpl::SerialDeviceImpl( const std::string& port, std::ios_base::openmode mode )
: _fd(-1)
{
    this->open(port.c_str(), mode);
}


SerialDeviceImpl::~SerialDeviceImpl()
{ }


void SerialDeviceImpl::open(const char* path, std::ios_base::openmode mode)
{
    int flags = O_RDONLY;

    if( (mode & std::ios_base::in ) && (mode & std::ios_base::out) ) {
        flags |= O_RDWR;
        flags |= O_CREAT;
    }
    else if(mode & std::ios_base::out) {
        flags |= O_WRONLY;
        flags |= O_CREAT;
    }
    else if(mode & std::ios_base::in  ) {
        flags |= O_RDONLY;
    }

    //if(mode & IODevice::NonBlock) {
    //    flags |= O_NONBLOCK;
    //}

    if(mode & std::ios::trunc)
        flags |= O_TRUNC;

    _fd = ::open(path, flags, 0644);
    if(_fd == -1) {
        throw IO::IOError("Could not open file handle", PT_SOURCEINFO);
    }

/*
    try {
        if(mode & std::ios::ate)
            this->seek(0, IODevice::SeekEnd);
    }
    catch(...) {
        this->close();
        throw;
    }
*/
}


void SerialDeviceImpl::close()
{
    if(_fd != -1)
    {
        if( ::close(_fd) != 0 )
            throw IO::IOError("Could not close file handle", PT_SOURCEINFO);

        _fd = -1;
    }
}


size_t SerialDeviceImpl::read( char* buffer, size_t count, bool& eof )
{
    eof = false;

    retry:

    ssize_t ret = ::read(_fd, (void*)buffer, count);
    if(ret == -1) 
    {
        if(errno == EINTR) // signal interrupt
            goto retry;

        if(errno == EAGAIN) // non-blocking and no data yet
            return 0;

        throw IO::IOError("Could not read from file handle", PT_SOURCEINFO);
    }

    if(ret == 0)
        eof = true;

    return ret;
}


size_t SerialDeviceImpl::write( const char* buffer, size_t count )
{ 
    retry:

    ssize_t ret = ::write(_fd, (const void*)buffer, count);
    if(ret == -1) {
        if(errno == EINTR) // signal interrupt
            goto retry;

        if(errno == EAGAIN) // non-blocking and no data yet
            return 0;

        throw IO::IOError("Could not write to file handle", PT_SOURCEINFO);
    }

    return ret;
}


void SerialDeviceImpl::setBaudRate( SerialDevice::BaudRate br )
{
    struct termios ios;

    if( ::tcgetattr(_fd, &ios) == -1  )
    {
        throw Pt::IO::IOError( "Could not set baud rate", PT_SOURCEINFO);
    }

    speed_t rate = B0;

    switch(br)
    {
        case SerialDevice::BaudRate0 : rate = B0; break;
        case SerialDevice::BaudRate50: rate = B50; break;
        case SerialDevice::BaudRate75: rate = B75; break;
        case SerialDevice::BaudRate110: rate = B110; break;
        case SerialDevice::BaudRate134: rate = B134; break;
        case SerialDevice::BaudRate150: rate = B150; break;
        case SerialDevice::BaudRate200: rate = B200; break;
        case SerialDevice::BaudRate300: rate = B300; break;
        case SerialDevice::BaudRate600: rate = B600; break;
        case SerialDevice::BaudRate1200: rate = B1200; break;
        case SerialDevice::BaudRate1800: rate = B1800; break;
        case SerialDevice::BaudRate2400: rate = B2400; break;
        case SerialDevice::BaudRate4800: rate = B4800; break;
        case SerialDevice::BaudRate9600: rate = B9600; break;
        case SerialDevice::BaudRate19200: rate = B19200; break;
        case SerialDevice::BaudRate38400: rate = B38400; break;
        case SerialDevice::BaudRate57600: rate = B57600; break;
        case SerialDevice::BaudRate115200: rate = B115200; break;
        case SerialDevice::BaudRate230400: rate = B230400; break;
    }

    ::cfsetispeed( &ios, rate );
    ::cfsetospeed( &ios, rate );

    if( ::tcsetattr(_fd, TCSANOW, &ios) == -1  )
    {
        throw Pt::IO::IOError("Could not set baud rate", PT_SOURCEINFO);
    } 
}


SerialDevice::BaudRate SerialDeviceImpl::baudRate() const
{
    struct termios ios;
    if( ::tcgetattr(_fd, &ios) == -1 )
    {
        throw Pt::IO::IOError("Could not get baud rate", PT_SOURCEINFO);
    }

    speed_t rate = ::cfgetispeed( &ios ) ;
    switch(rate)
    {
        case B0:      return SerialDevice::BaudRate0;
        case B50:     return SerialDevice::BaudRate50;
        case B75:     return SerialDevice::BaudRate75;
        case B110:    return SerialDevice::BaudRate110;
        case B134:    return SerialDevice::BaudRate134;
        case B150:    return SerialDevice::BaudRate150;
        case B200:    return SerialDevice::BaudRate200;
        case B300:    return SerialDevice::BaudRate300;
        case B600:    return SerialDevice::BaudRate600;
        case B1200:   return SerialDevice::BaudRate1200;
        case B1800:   return SerialDevice::BaudRate1800;
        case B2400:   return SerialDevice::BaudRate2400;
        case B4800:   return SerialDevice::BaudRate4800;
        case B9600:   return SerialDevice::BaudRate9600;
        case B19200:  return SerialDevice::BaudRate19200;
        case B38400:  return SerialDevice::BaudRate38400;
        case B57600:  return SerialDevice::BaudRate57600;
        case B115200: return SerialDevice::BaudRate115200;
        case B230400: return SerialDevice::BaudRate230400;
    }

    return SerialDevice::BaudRate0;
}


void SerialDeviceImpl::setCharSize( int size )
{
    struct termios ios;
    if( ::tcgetattr(_fd, &ios) == -1 )
        throw Pt::IO::IOError("Could not set char size", PT_SOURCEINFO);

    ios.c_cflag &= ~CSIZE;

    switch(size)
    {
        case 5:
            ios.c_cflag |= CS5;
            break;
        case 6:
            ios.c_cflag |= CS6;
            break;
        case 7:
            ios.c_cflag |= CS7;
            break;
        case 8:
            ios.c_cflag |= CS8;
            break;
        default:
            throw Pt::IO::IOError("Invalid char size", PT_SOURCEINFO);
    }

    tcsetattr(_fd, TCSANOW, &ios);
}


int SerialDeviceImpl::charSize() const
{
    struct termios ios;

    if( ::tcgetattr(_fd, &ios) == -1 )
        throw Pt::IO::IOError("Could not get char size", PT_SOURCEINFO);

    int size = ios.c_cflag & CSIZE;
    switch(size)
    {
        case CS5: return 5;
        case CS6: return 6;
        case CS7: return 7;
        case CS8: return 8;
        default:
            throw Pt::IO::IOError("Invalid char size", PT_SOURCEINFO);
    }

    return 0;
}


void SerialDeviceImpl::setStopBits( SerialDevice::StopBits bits )
{
    struct termios ios;

    if( ::tcgetattr(_fd, &ios) == -1 )
        throw Pt::IO::IOError("Could not get stop bits", PT_SOURCEINFO);

    ios.c_cflag &= ~CSTOPB;

    switch(bits)
    {
        case SerialDevice::OneStopBit:
            ios.c_cflag &= ~CSTOPB;
            break;
        case SerialDevice::TwoStopBits:
            ios.c_cflag |= CSTOPB;
            break;
        default:
            throw Pt::IO::IOError("Invalid stop bits", PT_SOURCEINFO);
    }

    tcsetattr(_fd, TCSANOW, &ios);
}


SerialDevice::StopBits SerialDeviceImpl::stopBits() const
{
   struct termios ios;

    if( ::tcgetattr(_fd, &ios) == -1 )
        throw Pt::IO::IOError("Could not get stop bits", PT_SOURCEINFO);

    if( ios.c_cflag & CSTOPB )
    {
        return SerialDevice::TwoStopBits;
    } else
    {
        return SerialDevice::OneStopBit;
    }

   throw Pt::IO::IOError("Invalid stop bits", PT_SOURCEINFO);
   return SerialDevice::OneStopBit;
}


void SerialDeviceImpl::setParity( SerialDevice::Parity parity )
{
   struct termios ios;

    if( ::tcgetattr(_fd, &ios) == -1 )
        throw Pt::IO::IOError("Could not get parity", PT_SOURCEINFO);

    ios.c_cflag &= ~(PARENB | PARODD);

    switch(parity)
    {
        case SerialDevice::ParityEven:
            ios.c_cflag |= PARENB;
            break;
        case SerialDevice::ParityOdd:
            ios.c_cflag |= (PARENB | PARODD);
            break;
        case SerialDevice::ParityNone:
            break;
        default:
            throw Pt::IO::IOError("Invalid parity", PT_SOURCEINFO);
    }

    tcsetattr(_fd, TCSANOW, &ios);
}


SerialDevice::Parity SerialDeviceImpl::parity() const
{
   struct termios ios;

    if( ::tcgetattr(_fd, &ios) == -1 )
        throw Pt::IO::IOError("Could not get parity", PT_SOURCEINFO);

    if( ios.c_cflag & PARENB )
    {
        if( ios.c_cflag & PARODD )
        {
            return SerialDevice::ParityOdd ;
        }
        else
        {
            return SerialDevice::ParityEven ;
        }
    }

    return SerialDevice::ParityNone;
}


void SerialDeviceImpl::setFlowControl( SerialDevice::FlowControl flowControl )
{
   struct termios ios;

    if( ::tcgetattr(_fd, &ios) == -1 )
        throw Pt::IO::IOError("Could not set flow control", PT_SOURCEINFO);

    ios.c_cflag &= ~CRTSCTS;
    ios.c_iflag &= ~(IXON | IXANY | IXOFF);

    switch(flowControl)
    {
    case SerialDevice::FlowControlSoft:
        ios.c_iflag |= (IXON | IXANY | IXOFF);
        break;
    //case flowBoth:
    //    ios.c_iflag |= (IXON | IXANY | IXOFF);
    case SerialDevice::FlowControlHard:
        ios.c_cflag |= CRTSCTS;
        break;
    //case flowNone:
    //    break;
    default:
        throw Pt::IO::IOError("Invalid flow control", PT_SOURCEINFO);
    }

    tcsetattr(_fd, TCSANOW, &ios);
}


SerialDevice::FlowControl SerialDeviceImpl::flowControl() const
{

}

} //namespace System

} //namespace Pt
