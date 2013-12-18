/*
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2007 Marc Boris Duerner
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

#include "IODeviceImpl.h"
#include "Pt/System/IOError.h"
#include "Pt/System/Logger.h"
#include "Pt/System/EventLoop.h"
#include <cerrno>
#include <cassert>

log_define("Pt.System.IODevice")

namespace Pt {

namespace System {

IODeviceImpl::IODeviceImpl(IODevice& device)
: _ioh(device)
, _timeout(System::EventLoop::WaitInfinite)
, _sentry(0)
, _errorPending(false)
{ 
}


IODeviceImpl::~IODeviceImpl()
{
    // make sure all operations were cancelled
    assert( ! _ioh.isActive() );

    if(_sentry)
        _sentry->detach();
}


bool IODeviceImpl::isOpen() const
{
    return this->fd() != -1;
}


void IODeviceImpl::open(int fd, bool inherit)
{
    log_debug("opening fd:" << fd);

    // TODO: we do not need to enable the i/o handle now, but defer it
    // until we call impl().beginRead or impl().beginWrite on the i/o handle.
    // The EventLoopImpl can check internally...

    _ioh.fd = fd;

    int flags = fcntl(this->fd(), F_GETFL);
    flags |= O_NONBLOCK ;
    int ret = fcntl(this->fd(), F_SETFL, flags);
    if(-1 == ret)
        throw IOError(PT_ERROR_MSG("Could not set fd to non-blocking"));

    if ( ! inherit)
    {
        int flags = fcntl(this->fd(), F_GETFD);
        flags |= FD_CLOEXEC ;
        int ret = fcntl(this->fd(), F_SETFD, flags);
        if(-1 == ret)
            throw IOError(PT_ERROR_MSG("Could not set FD_CLOEXEC"));
    }

    //if( loop )
    //    loop->impl().enable(_ioh);
}


void IODeviceImpl::close()
{
    // TODO: we know cancel is always called before close, so we do not need
    // a loop to disable the i/o handle

    // make sure all operations were cancelled
    assert( ! _ioh.isActive() );

    if( this->isOpen() )
    {
        _errorPending = false;

        //if(loop)
        //    loop->impl().disable(_ioh);

        int fd = _ioh.fd;
        _ioh.fd = -1;

        while ( ::close(fd) != 0 )
        {
            if( errno != EINTR )
                throw IOError( PT_ERROR_MSG("close failed") );
        }

        log_debug("closed fd:" << fd);
    }
}


void IODeviceImpl::cancel(EventLoop& loop)
{
    // disable the handle, we know cancel is always called before close
    // if this is attached to a loop

    if( this->isOpen() )
    {
        loop.selector().cancel(_ioh);
        log_debug("cancelling fd:" << _ioh.fd);
    }
}


std::size_t IODeviceImpl::beginRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    log_debug("begin read on fd:" << _ioh.fd);
    
    for(;;)
    {
        ssize_t ret = ::read( _ioh.fd, (void*)buffer, n);
        if (ret > 0)
        {
            log_debug("read:" << ret << " bytes");
            return static_cast<std::size_t>(ret);
        }

        if(ret == 0 || errno == ECONNRESET)
        {
            eof = true;
            log_debug("read: EOF");
            return 0;
        }

        if(errno == EAGAIN)
            break;

        if(errno != EINTR)
            throw IOError("read failed");
    }

    loop.selector().beginRead( &_ioh );
    return 0;
}


std::size_t IODeviceImpl::endRead(EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    log_debug("end read on fd:" << _ioh.fd);

    loop.selector().endRead( &_ioh );

    if (_errorPending)
    {
        _errorPending = false;
        throw IOError("read error");
    }

    return this->read( buffer, n, eof );
}


std::size_t IODeviceImpl::read( char* buffer, std::size_t count, bool& eof )
{
    ssize_t ret = 0;

    while(true)
    {
        ret = ::read( _ioh.fd, (void*)buffer, count);
        if(ret > 0)
            break;

        if(ret == 0 || errno == ECONNRESET)
        {
            eof = true;
            log_debug("read: EOF");
            return 0;
        }

        if(errno == EINTR)
            continue;

        if(errno != EAGAIN)
            throw IOError("read failed");

        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(this->fd(), &rfds);
        bool ret = this->wait(_timeout, &rfds, 0, 0);
        if(false == ret)
        {
            throw System::IOError("read");
        }
    }

    log_debug("read: " << ret << " bytes");
    return ret;
}


std::size_t IODeviceImpl::beginWrite(EventLoop& loop, const char* buffer, std::size_t n)
{
    log_debug("begin write on fd:" << _ioh.fd);

    for(;;)
    {
        ssize_t ret = ::write(_ioh.fd, (const void*)buffer, n);
        if (ret > 0)
        {
            log_debug("wrote:" << ret << " bytes");
            return static_cast<std::size_t>(ret);
        }

        if (ret == 0 || errno == ECONNRESET || errno == EPIPE)
            throw System::IOError("lost connection to peer");

        if(errno == EAGAIN)
            break;

        if(errno != EINTR)
            throw System::IOError("write failed");
    }
    
    loop.selector().beginWrite( &_ioh );
    return 0;
}


std::size_t IODeviceImpl::endWrite(EventLoop& loop, const char* buffer, std::size_t n)
{
    log_debug("end write on fd:" << _ioh.fd);

    loop.selector().endWrite( &_ioh );

    if (_errorPending)
    {
        _errorPending = false;
        throw IOError("write error");
    }

    return this->write( buffer, n );
}


std::size_t IODeviceImpl::write( const char* buffer, std::size_t count )
{
    ssize_t ret = 0;

    while(true)
    {
        ret = ::write(_ioh.fd, (const void*)buffer, count);
        if(ret > 0)
            break;

        if(ret == 0 || errno == ECONNRESET || errno == EPIPE)
            throw IOError("lost connection to peer");

        if(errno == EINTR)
            continue;

        if(errno != EAGAIN)
            throw IOError("Could not write to file handle");

        fd_set wfds;
        FD_ZERO(&wfds);
        FD_SET(this->fd(), &wfds);
        bool ret = this->wait(_timeout, 0, &wfds, 0);
        if(false == ret)
        {
            throw System::IOError("write");
        }
    }

    log_debug("wrote: " << ret << " bytes");
    return ret;
}


bool IODeviceImpl::wait(std::size_t msecs, fd_set* rfds, fd_set* wfds, fd_set* efds)
{
    struct timeval* timeout = 0;
    struct timeval tv;
    if(msecs != EventLoop::WaitInfinite)
    {
        tv.tv_sec = msecs / 1000;
        tv.tv_usec = (msecs % 1000) * 1000;
        timeout = &tv;
    }

    int ret = -1;
    do
    {
        ret = ::select(FD_SETSIZE, rfds, wfds, efds, timeout);
    } while (ret == -1 && errno == EINTR);

    if (ret == -1)
        throw IOError( PT_ERROR_MSG("select failed") );

    return ret > 0;
}


void IODeviceImpl::sync() const
{
    int ret = fsync(_ioh.fd);
    if(ret != 0)
        throw IOError( PT_ERROR_MSG("sync failed") );
}


bool IODeviceImpl::runRead(EventLoop& loop)
{
    if( ! this->isOpen() )
        return false;

    log_debug("run read on fd:" << _ioh.fd);

    Selector& selector = loop.selector();

    if ( selector.isError(&_ioh) )
    {
        _errorPending = true;
        return true;
    }

    return selector.isReadable(&_ioh);
}


bool IODeviceImpl::runWrite(EventLoop& loop)
{

    if( ! this->isOpen() )
        return false;

    log_debug("run write on fd:" << _ioh.fd);

    Selector& selector = loop.selector();

    if ( selector.isError(&_ioh) )
    {
        _errorPending = true;
        return true;
    }

    return selector.isWritable(&_ioh);
}


/*void IODeviceImpl::open(int fd, bool inherit)
{
    _fd = fd;

    int flags = fcntl(_fd, F_GETFL);
    flags |= O_NONBLOCK ;
    int ret = fcntl(_fd, F_SETFL, flags);
    if(-1 == ret)
        throw IOError(PT_ERROR_MSG("Could not set fd to non-blocking"));

    if ( ! inherit)
    {
        int flags = fcntl(_fd, F_GETFD);
        flags |= FD_CLOEXEC ;
        int ret = fcntl(_fd, F_SETFD, flags);
        if(-1 == ret)
            throw IOError(PT_ERROR_MSG("Could not set FD_CLOEXEC"));
    }

    if( _device.isActive() )
        _iohandle = _device.parent()->impl().enable(_device, this->fd());
}*/


/*void IODeviceImpl::close()
{
    if( this->isOpen() )
    {
        EventLoop* loop = _device.parent();
        if(_iohandle)
        {
            assert(loop);
            loop->impl().disable(_iohandle);
            _iohandle = 0;
        }

        int fd = _fd;
        _fd = -1;

        while ( ::close(fd) != 0 )
        {
            if( errno != EINTR )
                throw IOError( PT_ERROR_MSG("close failed") );
        }
    }
}*/


/*void IODeviceImpl::attach(EventLoop& loop)
{
    if( this->isOpen() )
    {
        _iohandle = loop.impl().enable(_device, this->fd());
    }
}*/


/*void IODeviceImpl::detach(EventLoop& loop)
{
    if(_iohandle)
    {
        loop.impl().disable(_iohandle);
        _iohandle = 0;
    }
}*/


/*void IODeviceImpl::cancel()
{
    EventLoop* loop = _device.parent();
    if( loop && _iohandle )
    {
        loop->impl().cancel(_iohandle);
    }
}*/


/*std::size_t IODeviceImpl::beginRead(char* buffer, std::size_t n, bool&)
{
    assert( _device.parent() );

    EventLoop* loop = _device.parent();
    if( loop && _iohandle)
    {
        loop->impl().beginRead( _iohandle );
    }

    return 0;
}*/


/*std::size_t IODeviceImpl::endRead(bool& eof)
{
    assert( _device.parent() );

    EventLoop* loop = _device.parent();
    if( loop && _iohandle )
    {
        loop->impl().endRead( _iohandle );
    }

    if (_errorPending)
    {
        _errorPending = false;
        throw IOError("read error", PT_SOURCEINFO);
    }

    return this->read( _device.rbuf(), _device.rbuflen(), eof );
}*/


/*std::size_t IODeviceImpl::beginWrite(const char* buffer, std::size_t n)
{
    ssize_t ret = ::write(_fd, (const void*)buffer, n);

    if (ret > 0)
        return static_cast<std::size_t>(ret);

    if (ret == 0 || errno == ECONNRESET || errno == EPIPE)
        throw System::IOError("lost connection to peer");

    EventLoop* loop = _device.parent();
    if( loop && _iohandle)
    {
        std::cerr << "IODeviceImpl::beginWrite on handle " << std::endl;
        loop->impl().beginWrite( _iohandle );
    }

    return 0;
}*/


/*std::size_t IODeviceImpl::endWrite()
{
    EventLoop* loop = _device.parent();
    if( loop && _iohandle )
    {
        loop->impl().endWrite( _iohandle );
    }

    if (_errorPending)
    {
        _errorPending = false;
        throw IOError("write error", PT_SOURCEINFO);
    }

    //if (_device.wavail() > 0)
    //{
    //    std::size_t n = _device.wavail();
    //    return n;
    //}

    return this->write( _device.wbuf(), _device.wbuflen() );
}*/


/*bool IODeviceImpl::run()
{
    std::cerr << "IODeviceImpl::run"<< std::endl;

    if( ! _iohandle)
        return false;

    int avail = 0;
    EventLoopImpl& impl = _device.parent()->impl();
    DestructionSentry sentry(_sentry);

    bool reading = _device.reading();
    if(reading)
    {
        if ( impl.isError(_iohandle) )
        {
            _errorPending = true;
    
            try
            {
                ++avail;
                _device.inputReady().send(_device);
    
                if( ! _sentry )
                    return avail;
            }
            catch (...)
            {
                _errorPending = false;
                throw;
            }
            _errorPending = false;
    
            return avail;
        }

        if( _device.ravail() > 0 || impl.isReadable(_iohandle) )
        {
            std::cerr << "IODeviceImpl::avail " << "READABLE" << std::endl;
            _device.inputReady().send(_device);
            ++avail;
        }
    }

    bool writing = _device.writing();
    if(writing)
    {
        if ( impl.isError(_iohandle) )
        {
            _errorPending = true;
    
            try
            {
                ++avail;
                _device.outputReady().send(_device);
    
                if( ! _sentry )
                    return avail;
            }
            catch (...)
            {
                _errorPending = false;
                throw;
            }
            _errorPending = false;
    
            return avail;
        }

        if( _device.wavail() > 0 || impl.isWritable(_iohandle) )
        {
            std::cerr << "IODeviceImpl::avail " << "WRITABLE" << std::endl;
            _device.outputReady().send(_device);
            ++avail;
        }
    }

    return avail > 0;
}*/


/*bool IODeviceImpl::run()
{
    std::cerr << "IODeviceImpl::avail"<< std::endl;

    if( ! _iohandle)
        return false;

    EventLoopImpl& impl = _device.parent()->impl();

    int avail = 0;
    DestructionSentry sentry(_sentry);

    if ( impl.isError(_iohandle) )
    {
        _errorPending = true;

        try
        {
            bool reading = _device.reading();
            bool writing = _device.writing();

            if (reading)
            {
                ++avail;
                _device.inputReady(_device);
            }

            if( ! _sentry )
                return avail;

            if (writing)
            {
                ++avail;
                _device.outputReady(_device);
            }

            if( ! _sentry )
                return avail;

            if (!reading && !writing)
            {
                avail = true;
                _device.close();
            }
        }
        catch (...)
        {
            _errorPending = false;
            throw;
        }
        _errorPending = false;

        return avail;
    }

    if( _device.wavail() > 0 || impl.isWritable(_iohandle) )
    {
        std::cerr << "IODeviceImpl::avail " << "WRITABLE" << std::endl;
        _device.outputReady(_device);
        ++avail;
    }

    if( ! sentry )
        return avail;

    if( _device.rbuf() && impl.isReadable(_iohandle) )
    {
        std::cerr << "IODeviceImpl::avail " << "READABLE" << std::endl;
        _device.inputReady(_device);
        ++avail;
    }

    return avail > 0;
}*/


/*bool IODeviceImpl::wait(std::size_t msecs)
{
    fd_set rfds;
    fd_set wfds;
    fd_set efds;
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&efds);
    this->initWait(rfds, wfds, efds);
    this->wait(msecs, &rfds, &wfds, &efds);
    return this->checkWait(rfds, wfds, efds);
}*/


/*void IODeviceImpl::initWait(fd_set& rfds, fd_set& wfds, fd_set& efds)
{
    if( this->fd() > FD_SETSIZE )
    {
        throw System::IOError( PT_ERROR_MSG("FD_SETSIZE too small for fd") );
    }

    if( this->fd() > 0 )
    {
        if( _device.rbuf() )
        {
            FD_SET(this->fd(), &rfds);
        }

        if( _device.wbuf() )
        {
            FD_SET(this->fd(), &wfds);
        }
    }
}*/


// TODO: move to enable
/*int IODeviceImpl::initSelect(fd_set& rfds, fd_set& wfds, fd_set& efds)
{
    _rfds = &rfds;
    _wfds = &wfds;
    _efds = &efds;
    this->initWait(rfds, wfds, efds);

    return this->fd();
}*/


// TODO: move to detach / disable
/*void IODeviceImpl::exitSelect()
{
    if( this->fd() > 0)
    {
        if(_rfds)
            FD_CLR(this->fd(), _rfds);
        if(_wfds)
            FD_CLR(this->fd(), _wfds);
        if(_efds)
            FD_CLR(this->fd(), _efds);
    }

    _rfds = 0;
    _wfds = 0;
    _efds = 0;
}*/


/*int IODeviceImpl::checkWait(fd_set& rfds, fd_set& wfds, fd_set& efds)
{
    int avail = 0;

    if( this->fd() < 0)
        return 0;

    DestructionSentry sentry(_sentry);

    if ( FD_ISSET(this->fd(), &efds) )
    {
        _errorPending = true;

        try
        {
            bool reading = _device.reading();
            bool writing = _device.writing();

            if (reading)
            {
                ++avail;
                _device.inputReady(_device);
            }

            if( ! _sentry )
                return avail;

            if (writing)
            {
                ++avail;
                _device.outputReady(_device);
            }

            if( ! _sentry )
                return avail;

            if (!reading && !writing)
            {
                avail = true;
                _device.close();
            }
        }
        catch (...)
        {
            _errorPending = false;
            throw;
        }
        _errorPending = false;

        return avail;
    }


    if( _device.wavail() > 0 || FD_ISSET(this->fd(), &wfds) )
    {
        _device.outputReady(_device);
        ++avail;
    }

    if( ! sentry )
        return avail;

    if( _device.rbuf() && FD_ISSET(this->fd(), &rfds) )
    {
        _device.inputReady(_device);
        ++avail;
    }

    return avail;
}*/

}//namespace System

}//namespace Pt
