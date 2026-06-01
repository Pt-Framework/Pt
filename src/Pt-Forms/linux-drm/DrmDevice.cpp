/* Copyright (C) 2026 Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#include "DrmDevice.h"
#include "DrmDisplay.h"

#include <Pt/System/Application.h>
#include <Pt/System/EventLoop.h>

#include <xf86drmMode.h>

#include <stdexcept>
#include <iostream>

#include <fcntl.h>
#include <unistd.h>
#include <xf86drm.h>

namespace Pt {

namespace Forms {

DrmDevice::DrmDevice()
: _fd(-1)
, _ioh(*this)
, _loop(0)
{
    std::string device = Pt::System::Application::getEnvVar("PT_DRM_DEVICE");
    if( device.empty() )
        device = "/dev/dri/card0";

    _fd = ::open(device.c_str(), O_RDWR | O_CLOEXEC);
    if( _fd < 0 )
        throw std::runtime_error("failed to open DRM device: " + device);

    if( drmSetClientCap(_fd, DRM_CLIENT_CAP_ATOMIC, 1) < 0 )
    {
        ::close(_fd);
        _fd = -1;
        throw std::runtime_error("DRM atomic modesetting not supported");
    }

    if( drmSetClientCap(_fd, DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1) < 0 )
    {
        ::close(_fd);
        _fd = -1;
        throw std::runtime_error("DRM universal planes not supported");
    }

    _ioh.fd = _fd;

    std::clog << "DRM device: " << device << ", fd: " << _fd << std::endl;
}


DrmDevice::~DrmDevice()
{
    shutdown();
}


void DrmDevice::shutdown()
{
    if( _loop )
    {
        Pt::System::Selector& selector = _loop->selector();
        selector.endRead(&_ioh);
        _loop = 0;
    }

    if( _fd >= 0 )
    {
        ::close(_fd);
        _fd = -1;
        _ioh.fd = -1;
    }
}


void DrmDevice::begin()
{
    if( ! _loop )
        throw std::logic_error("DRM device not active");

    Pt::System::Selector& selector = _loop->selector();
    selector.beginRead(&_ioh);
}


void DrmDevice::onAttach(System::EventLoop& loop)
{
    _loop = &loop;
}


void DrmDevice::onDetach(System::EventLoop& loop)
{
    _loop = 0;
}


bool DrmDevice::onRun()
{
    Pt::System::Selector& selector = _loop->selector();
    bool isAvail = selector.isReadable(&_ioh);
    if( ! isAvail )
        return false;

    selector.endRead(&_ioh);
    selector.beginRead(&_ioh);

    drmEventContext evCtx = {};
    evCtx.version = 3;
    evCtx.page_flip_handler2 = [](int /*fd*/, unsigned int /*seq*/,
                                  unsigned int /*tvSec*/, unsigned int /*tvUsec*/,
                                  unsigned int /*crtcId*/, void* userData)
    {
        DrmDisplay* display = static_cast<DrmDisplay*>(userData);
        display->onFlipComplete();
    };

    drmHandleEvent(_fd, &evCtx);
    _flipComplete.send();

    return true;
}


void DrmDevice::onCancel()
{
    if( _loop )
    {
        Pt::System::Selector& selector = _loop->selector();
        selector.endRead(&_ioh);
    }
}

} // namespace

} // namespace
