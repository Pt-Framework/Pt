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
#include "Pt/System/SerialDevice.h"
#include "SerialDeviceImpl.h"

namespace Pt {

namespace System {

SerialDevice::SerialDevice()
: _impl( 0 )
{
    _impl = new SerialDeviceImpl(*this);
}


SerialDevice::SerialDevice( const std::string& file, std::ios::openmode mode)
: _impl( 0 )
{
    _impl = new SerialDeviceImpl(*this);
    this->open( file, mode);
}


SerialDevice::SerialDevice( const char* file, std::ios::openmode mode)
: _impl( 0 )
{
    _impl = new SerialDeviceImpl(*this);
    this->open( file, mode);
}


SerialDevice::~SerialDevice()
{
    try
    {
        close();
    }
    catch( ... )
    { }

    delete _impl;
}


void SerialDevice::open( const std::string& file, std::ios::openmode mode)
{
    this->close();
    _impl->open( file, mode );
}


void SerialDevice::open( const char* file, std::ios::openmode mode)
{
    this->close();
    _impl->open( file, mode );
}


void SerialDevice::onSetTimeout(std::size_t timeout)
{
    _impl->setTimeout(timeout);
}


std::size_t SerialDevice::onBeginRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    return _impl->beginRead(loop, buffer, n, eof);
}


std::size_t SerialDevice::onEndRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    return _impl->endRead(loop, buffer, n, eof);
}


std::size_t SerialDevice::onBeginWrite(EventLoop& loop, const char* buffer, std::size_t n)
{
    return _impl->beginWrite(loop, buffer, n);
}


std::size_t SerialDevice::onEndWrite(EventLoop& loop, const char* buffer, std::size_t n)
{
    return _impl->endWrite(loop, buffer, n);
}


void SerialDevice::setBaudRate( unsigned rate )
{
    _impl->setBaudRate( rate );
}


unsigned SerialDevice::baudRate() const
{
    return _impl->baudRate();
}


void SerialDevice::setCharSize( int size )
{
    _impl->setCharSize( size );
}


int SerialDevice::charSize() const
{
    return _impl->charSize();
}


void SerialDevice::setStopBits( StopBits bits )
{
    _impl->setStopBits( bits );
}


SerialDevice::StopBits SerialDevice::stopBits() const
{
    return _impl->stopBits();
}


void SerialDevice::setParity( Parity parity )
{
    _impl->setParity( parity );
}


SerialDevice::Parity SerialDevice::parity() const
{
    return _impl->parity();
}


void SerialDevice::setFlowControl(FlowControl flowControl)
{
    _impl->setFlowControl(  flowControl );
}


SerialDevice::FlowControl SerialDevice::flowControl() const
{
    return _impl->flowControl();
}

bool SerialDevice::setSignal(Signal signal)
{
    return _impl->setSignal(signal);
}


void SerialDevice::onClose()
{
    _impl->close();
}


std::size_t SerialDevice::onRead(char* buffer, std::size_t count, bool& eof)
{
    return _impl->read( buffer, count, eof );
}


std::size_t SerialDevice::onWrite(const char* buffer, std::size_t count)
{
    return _impl->write( buffer, count );
}


void SerialDevice::onSync() const
{
    _impl->sync();
}


void SerialDevice::onCancel()
{
    EventLoop* loop = this->loop();
    if( loop )
    {
        _impl->cancel(*loop);
    }

    IODevice::onCancel();
}


bool SerialDevice::onRun()
{
    if( this->isReading() )
    {
        if( _ravail || isEof() || _impl->runRead( *loop() ) )
        {
            inputReady().send(*this);
            return true;
        }
    }

    if( this->isWriting() )
    {
        if( _wavail || _impl->runWrite( *loop() ) )
        {
            outputReady().send(*this);
            return true;
        }
    }

    return false;
}

}//namespace System

}//namespace Pt

