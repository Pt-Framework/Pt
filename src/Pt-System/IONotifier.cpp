/*
 * Copyright (C) 2006-2018 Marc Boris Duerner
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

#include "IONotifierImpl.h"
#include <Pt/System/IONotifier.h>
#include <Pt/System/EventLoop.h>

namespace Pt {

namespace System {

IONotifier::IONotifier()
: _impl(0)
, _loop(0)
, _isWaiting(false)
{
  _impl = new IONotifierImpl(*this);
}


IONotifier::IONotifier(int fd)
: _impl(0)
, _loop(0)
, _isWaiting(false)
{
    _impl = new IONotifierImpl(*this);
    _impl->setFd(fd);
}


IONotifier::IONotifier(void* handle)
    : _impl(0)
    , _loop(0)
    , _isWaiting(false)
{
    _impl = new IONotifierImpl(*this);
    _impl->setHandle(handle);
}


IONotifier::~IONotifier()
{
    reset();
    delete _impl;
}


void IONotifier::reset()
{
    cancel();
    _impl->reset();
}


void IONotifier::setFd(int fd)
{
    cancel();
    _impl->setFd(fd);
}


void IONotifier::setHandle(void* h)
{
    cancel();
    _impl->setHandle(h);
}


void IONotifier::beginWait(int flags)
{
    EventLoop* loop = this->loop();
    if( ! loop )
        throw std::logic_error("I/O notifier not active");
  
    _impl->beginWait(*loop, flags);
    _isWaiting = true;
}


int IONotifier::endWait()
{
    EventLoop* loop = this->loop();
    if( ! loop )
        throw std::logic_error("I/O notifier not active");

    _isWaiting = false;
    return _impl->endWait(*loop);
}


void IONotifier::onAttach(EventLoop& loop)
{ 
    _loop = &loop;
}


void IONotifier::onDetach(EventLoop& loop)
{ 
    _loop = 0; 
}


void IONotifier::onCancel()
{
    EventLoop* loop = this->loop();
    if(loop)
    {
        _impl->cancel(*loop);
        _isWaiting = false;
    }
}


bool IONotifier::onRun()
{
    EventLoop* loop = this->loop();
    if( ! loop )
        return false;

    if(_isWaiting)
    {
        bool isReady = _impl->runWait(*loop);
        if(isReady)
        {
            _eventReady.send();
            return true;
        }
    }

    return false;
}

} // namespace System

} // namespace Pt
