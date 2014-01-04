/*
 * Copyright (C) 2013 Marc Boris Duerner
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
#include "PipeImpl.h"
#include <cstring>
#include <algorithm>

namespace Pt {

namespace System {

PipeIODevice::PipeIODevice(Mode m)
: _mode(m)
, _timeout(Pt::System::EventLoop::WaitInfinite)
{
}


PipeIODevice::~PipeIODevice()
{
    try
    {
        IODevice::close();
    }
    catch(...)
    {
    }
}


void PipeIODevice::init(PipeImpl* pipe)
{
    _pipe = pipe;
}


void PipeIODevice::onSetTimeout(std::size_t timeout)
{
    _timeout = timeout;
}


void PipeIODevice::onClose()
{
	_pipe->close();
}


void PipeIODevice::onCancel()
{
    if( this->isReading() )
        _pipe->cancelRead();

    if( this->isWriting() )
        _pipe->cancelWrite();

    IODevice::onCancel();
}


bool PipeIODevice::onRun()
{
    if( this->isReading() )
    {
        if( ravail() || isEof() || _pipe->readAvail() )
        {
            inputReady().send(*this);
            return true;
        }
    }

    if( this->isWriting() )
    {
        if( wavail() || _pipe->writeAvail() )
        {
            outputReady().send(*this);
            return true;
        }
    }

    return false;
}


std::size_t PipeIODevice::onBeginRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    if(_mode != Read)
    {
        throw IOError("I/O device not readable");
    }

    return _pipe->beginRead(loop, buffer, n, eof);
}


std::size_t PipeIODevice::onEndRead(EventLoop&, char* buffer, std::size_t n, bool& eof)
{
    return _pipe->endRead(buffer, n, eof);
}


std::size_t PipeIODevice::onRead(char* buffer, std::size_t count, bool& eof)
{
    throw IOError("blocking I/O not supported");
    return 0;
}


std::size_t PipeIODevice::onBeginWrite(EventLoop& loop, const char* buffer, std::size_t n)
{
    if(_mode != Write)
    {
        throw IOError("I/O device not writable");
    }

    return _pipe->beginWrite(loop, buffer, n);
}


std::size_t PipeIODevice::onEndWrite(EventLoop&, const char* buffer, std::size_t n)
{
    return _pipe->endWrite(buffer, n);
}


std::size_t PipeIODevice::onWrite(const char* buffer, std::size_t count)
{
    throw IOError("blocking I/O not supported");
    return 0;
}


void PipeIODevice::onSync() const
{
}




static const std::size_t MaxBufferSize = 16384;


PipeImpl::PipeImpl()
: _eof(false)
, _readLoop(0)
, _writeLoop(0)
, _in(PipeIODevice::Write)
, _out(PipeIODevice::Read)
{
    _in.init(this);
    _out.init(this);
}


PipeImpl::~PipeImpl()
{

}


void PipeImpl::close()
{
	_eof = true;

	if (_readLoop)
	{
		_readLoop->setReady(_out);
		_readLoop = 0;
	}
}


std::size_t PipeImpl::beginRead(EventLoop& loop, char* buf, std::size_t n, bool&)
{
    MutexLock lock(_mtx);
    
    _readLoop = &loop;
    
    if( ! _buffer.empty() || _eof )
    {
        _readLoop->setReady(_out);
        _readLoop = 0;
    }

    return 0;
}


bool PipeImpl::readAvail()
{
    MutexLock lock(_mtx);
    return _buffer.size() > 0 || _eof;
}


std::size_t PipeImpl::endRead(char* buf, std::size_t n, bool& eof)
{
    MutexLock lock(_mtx);

    _readLoop = 0;

    std::size_t readSize = std::min<std::size_t>( n, _buffer.size() );
	if (readSize == 0)
	{
		eof = _eof;
	}
	else
	{
		std::memcpy(buf, &_buffer[0], readSize);
		_buffer.erase(_buffer.begin(), _buffer.begin() + readSize);
	}

    if(_writeLoop)
    {
        _writeLoop->setReady(_in);
        _writeLoop = 0;
    }

    return readSize;
}


std::size_t PipeImpl::beginWrite(EventLoop& loop, const char* buf, std::size_t n)
{
    MutexLock lock(_mtx);
    
    _writeLoop = &loop;
    
    if( _buffer.size() < MaxBufferSize )
    {
        _writeLoop->setReady(_in);
        _writeLoop = 0;
    }

    return 0;
}


bool PipeImpl::writeAvail()
{
    MutexLock lock(_mtx);
    return _buffer.size() < MaxBufferSize;
}


std::size_t PipeImpl::endWrite(const char* buf, std::size_t n)
{
    MutexLock lock(_mtx);

    _writeLoop = 0;

    std::size_t writeSize = std::min<std::size_t>( n, MaxBufferSize - _buffer.size() );
    _buffer.insert(_buffer.end(), buf, buf + writeSize);

    if(_readLoop)
    {
        _readLoop->setReady(_out);
        _readLoop = 0;
    }
    
    return writeSize;
}


void PipeImpl::cancelRead()
{
    MutexLock lock(_mtx);
    _readLoop = 0;
}


void PipeImpl::cancelWrite()
{
    MutexLock lock(_mtx);
    _writeLoop = 0;
}


PipeIODevice& PipeImpl::out()
{
    return _out;
}


PipeIODevice& PipeImpl::in()
{
    return _in;
}

} // namespace System

} // namespace Pt
