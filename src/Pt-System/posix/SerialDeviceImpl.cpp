/*
 * Copyright (C) 2007 Marc Boris Drner
 * Copyright (C) 2007 Laurentiu-Gheorghe Crisan
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "SerialDeviceImpl.h"

#include <cerrno>
#include <iostream>

#include <sys/ioctl.h>

#if defined(__QNX__)
#define CRTSCTS (IHFLOW | OHFLOW)
#endif

#if defined(_THR_UNIXWARE) || defined(__hpux) || defined(_AIX)
#include <sys/termiox.h>
#define CRTSCTS (CTSXON | RTSXOFF)
#endif

#if defined(__ANDROID__) && ! defined(_POSIX_VDISABLE)
    #define _POSIX_VDISABLE _PC_VDISABLE
#endif

#if defined(__LSB_VERSION__) && ! defined(CRTSCTS)
    #define CRTSCTS 020000000000
#endif

namespace Pt {

namespace System {

SerialDeviceImpl::SerialDeviceImpl(IODevice& device)
: IODeviceImpl(device)
{
}


SerialDeviceImpl::~SerialDeviceImpl()
{
}


void SerialDeviceImpl::open(const std::string& path, std::ios::openmode mode)
{
    int flags = O_RDONLY;

    if( (mode & std::ios::in ) && (mode & std::ios::out) )
    {
        flags |= O_RDWR;
    }
    else if(mode & std::ios::out)
    {
        flags |= O_WRONLY;
    }
    else if(mode & std::ios::in  )
    {
        flags |= O_RDONLY;
    }

    flags |= O_NONBLOCK;

    if(mode & std::ios::trunc)
        flags |= O_TRUNC;

    flags |=  O_NOCTTY;

    int fd = ::open( path.c_str(), flags );
    if( -1 == fd )
    {
        throw AccessFailed(path);
    }

    // TODO: exception safety
    IODeviceImpl::open(fd, false);

    struct termios ios;
    if( ::tcgetattr( IODeviceImpl::fd(), &ios) == -1 )
        throw IOError( PT_ERROR_MSG("tcgetattr failed") );

    if( ::tcgetattr( IODeviceImpl::fd(), &_prevIos) == -1 )
        throw IOError( PT_ERROR_MSG("tcgetattr failed") );

    // Disable canonical
    //::cfmakeraw(&ios);
    ios.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
                    | INLCR | IGNCR | ICRNL | IXON);
    ios.c_oflag &= ~OPOST;
    ios.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    ios.c_cflag &= ~(CSIZE | PARENB);
    ios.c_cflag |= CS8;

    if( ::tcsetattr( IODeviceImpl::fd(), TCSANOW, &ios) == -1  )
        throw IOError( PT_ERROR_MSG("tcsetattr failed") );
}


void SerialDeviceImpl::close()
{
    if( IODeviceImpl::fd() != -1)
    {
        ::tcsetattr( IODeviceImpl::fd(), TCSANOW, &_prevIos );

        IODeviceImpl::close();
    }
}


void SerialDeviceImpl::setBaudRate( unsigned br )
{
    struct termios ios;

    if( ::tcgetattr( IODeviceImpl::fd(), &ios ) == -1  )
    {
        throw IOError( PT_ERROR_MSG("tcgetattr failed") );
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
        #ifdef B57600
            case SerialDevice::BaudRate57600: rate = B57600; break;
        #endif
        #ifdef B115200
            case SerialDevice::BaudRate115200: rate = B115200; break;
        #endif
        #ifdef B230400
            case SerialDevice::BaudRate230400: rate = B230400; break;
        #endif
		default:
			throw IOError( PT_ERROR_MSG("no such baud rate") );
    }

    ::cfsetispeed( &ios, rate );
    ::cfsetospeed( &ios, rate );

    if( ::tcsetattr(IODeviceImpl::fd(), TCSANOW, &ios) == -1  )
    {
        throw IOError( PT_ERROR_MSG("tcsetattr failed") );
    }
}


unsigned SerialDeviceImpl::baudRate() const
{
    struct termios ios;
    if( ::tcgetattr(IODeviceImpl::fd(), &ios) == -1 )
    {
        throw IOError( PT_ERROR_MSG("tcgetattr failed") );
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
        
        #ifdef B57600
            case B57600:  return SerialDevice::BaudRate57600;
        #endif

        #ifdef B115200
            case B115200: return SerialDevice::BaudRate115200;
        #endif
        
        #ifdef B230400
            case B230400: return SerialDevice::BaudRate230400;
        #endif
    }

    return SerialDevice::BaudRate0;
}


void SerialDeviceImpl::setCharSize( int size )
{
    struct termios ios;
    if( ::tcgetattr(IODeviceImpl::fd(), &ios) == -1 )
        throw IOError( PT_ERROR_MSG("tcgetattr failed") );

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
            throw IOError( PT_ERROR_MSG("no such char size") );
    }

    tcsetattr(IODeviceImpl::fd(), TCSANOW, &ios);
}


int SerialDeviceImpl::charSize() const
{
    struct termios ios;

    if( ::tcgetattr(IODeviceImpl::fd(), &ios) == -1 )
        throw IOError( PT_ERROR_MSG("tcgetattr failed") );

    int size = ios.c_cflag & CSIZE;
    switch(size)
    {
        case CS5: return 5;
        case CS6: return 6;
        case CS7: return 7;
        case CS8: return 8;
        default:
            throw IOError( PT_ERROR_MSG("invalid char size") );
    }

    return 0;
}


void SerialDeviceImpl::setStopBits( SerialDevice::StopBits bits )
{
    struct termios ios;

    if( ::tcgetattr(IODeviceImpl::fd(), &ios) == -1 )
        throw IOError( PT_ERROR_MSG("tcgetattr failed") );

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
            throw IOError( PT_ERROR_MSG("no such stop bits") );
    }

    tcsetattr(IODeviceImpl::fd(), TCSANOW, &ios);
}


SerialDevice::StopBits SerialDeviceImpl::stopBits() const
{
   struct termios ios;

    if( ::tcgetattr(IODeviceImpl::fd(), &ios) == -1 )
        throw IOError( PT_ERROR_MSG("tcgetattr failed") );

    if( ios.c_cflag & CSTOPB )
    {
        return SerialDevice::TwoStopBits;
    } else
    {
        return SerialDevice::OneStopBit;
    }

   throw IOError( PT_ERROR_MSG("no such stop bits") );
   return SerialDevice::OneStopBit;
}


void SerialDeviceImpl::setParity( SerialDevice::Parity parity )
{
   struct termios ios;

    if( ::tcgetattr(IODeviceImpl::fd(), &ios) == -1 )
        throw IOError( PT_ERROR_MSG("tcgetattr failed") );

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
            throw IOError( PT_ERROR_MSG("invalid parity") );
    }

    tcsetattr(IODeviceImpl::fd(), TCSANOW, &ios);
}


SerialDevice::Parity SerialDeviceImpl::parity() const
{
   struct termios ios;

    if( ::tcgetattr(IODeviceImpl::fd(), &ios) == -1 )
        throw IOError( PT_ERROR_MSG("tcgetattr failed") );

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
    static const int CTRL_Q = 0x11;
    static const int CTRL_S = 0x13;

   struct termios ios;

    if( ::tcgetattr(IODeviceImpl::fd(), &ios) == -1 )
        throw IOError( PT_ERROR_MSG("tcgetattr failed") );

    ios.c_cflag &= ~CRTSCTS;
    ios.c_iflag &= ~(IXON | IXANY | IXOFF);

    switch(flowControl)
    {
        case SerialDevice::FlowControlSoft:
            ios.c_iflag |= (IXON | IXANY | IXOFF);
            ios.c_cc[VSTART] = CTRL_Q ;
            ios.c_cc[VSTOP]  = CTRL_S ;
            break;

        case SerialDevice::FlowControlHard:
            ios.c_cflag |= CRTSCTS;
            ios.c_cc[VSTART] = _POSIX_VDISABLE;
            ios.c_cc[VSTOP] = _POSIX_VDISABLE;
            break;

        case SerialDevice::FlowControlNone:
            break;
    }

    tcsetattr(IODeviceImpl::fd(), TCSANOW, &ios);
    _flowControl = flowControl;
}


SerialDevice::FlowControl SerialDeviceImpl::flowControl() const
{
    return _flowControl;
}


void SerialDeviceImpl::setRts(bool on)
{
    int value = TIOCM_RTS;
    unsigned long cmd = on ? TIOCMBIS : TIOCMBIC;
    ioctl(IODeviceImpl::fd(), cmd, &value);
}


bool SerialDeviceImpl::isCts() const
{
	int flags = 0;
	ioctl(IODeviceImpl::fd(), TIOCMGET, &flags);
    return flags & TIOCM_CTS;
}

	
void SerialDeviceImpl::setDtr(bool on)
{
    int value = TIOCM_DTR;
    unsigned long cmd = on ? TIOCMBIS : TIOCMBIC;
    ioctl(IODeviceImpl::fd(), cmd, &value);
}


bool SerialDeviceImpl::isDsr() const
{
	int flags = 0;
	ioctl(IODeviceImpl::fd(), TIOCMGET, &flags);
    return flags & TIOCM_DSR;
}


void SerialDeviceImpl::setBreak(bool on)
{	
    unsigned long cmd = on ? TIOCSBRK : TIOCCBRK;
    ioctl(IODeviceImpl::fd(), cmd, 0);	  
}


void SerialDeviceImpl::sendBreak(int duration)
{	
    tcsendbreak(IODeviceImpl::fd(), duration);	  
}


/*bool SerialDeviceImpl::setSignal(SerialDevice::Signal signal)
{
	struct termios ios;

	if( ::tcgetattr(IODeviceImpl::fd(), &ios) == -1 )
		throw IOError("tcgetattr failed");

#ifdef TIOCMGET
	int flags = 0;
	ioctl(IODeviceImpl::fd(), TIOCMGET, &flags);
#endif

	switch(signal)
    {	
#ifdef TIOCSBRK
        case SerialDevice::SetBreak:
			ioctl(IODeviceImpl::fd(), TIOCSBRK, 0);
		break;

		case SerialDevice::ClearBreak:            
			ioctl(IODeviceImpl::fd(), TIOCCBRK, 0);			  
        break;
#endif

#ifdef TIOCM_DTR
        case SerialDevice::SetDtr:		
			flags |= TIOCM_DTR;
        break;

        case SerialDevice::ClearDtr:
           flags &= ~TIOCM_DTR;
        break;

		case SerialDevice::SetRts:
			flags |= TIOCM_RTS;
        break;

		case SerialDevice::ClearRts:
			flags &= ~TIOCM_RTS;
        break;
#endif
        case SerialDevice::SetXOn:
			ios.c_iflag |= IXON;
        break;

        case SerialDevice::SetXOff:
			ios.c_iflag |= IXOFF;
        break;
    }

#ifdef TIOCMSET	
	ioctl(IODeviceImpl::fd(), TIOCMSET, &flags);
#endif 

	tcsetattr(IODeviceImpl::fd(), TCSANOW, &ios);

    return false;
}
*/

void SerialDeviceImpl::clear()
{
  ::tcflush(IODeviceImpl::fd(), TCIOFLUSH);
}


void SerialDeviceImpl::sync() const
{
    tcdrain( IODeviceImpl::fd() );
}

} //namespace System

} //namespace Pt
