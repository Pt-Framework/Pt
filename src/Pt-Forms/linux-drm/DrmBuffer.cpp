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

#include "DrmBuffer.h"

#include <stdexcept>
#include <cstring>

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <sys/mman.h>
#include <drm_fourcc.h>

namespace Pt {

namespace Forms {

DrmBuffer::DrmBuffer()
: _drmFd(-1)
, _handle(0)
, _fbId(0)
, _width(0)
, _height(0)
, _pitch(0)
, _size(0)
, _map(0)
{
}


DrmBuffer::~DrmBuffer()
{
    destroy();
}


void DrmBuffer::create(int drmFd, uint32_t width, uint32_t height)
{
    _drmFd = drmFd;
    _width = width;
    _height = height;

    // create dumb buffer
    struct drm_mode_create_dumb createReq = {};
    createReq.width = width;
    createReq.height = height;
    createReq.bpp = 32;

    if( drmIoctl(_drmFd, DRM_IOCTL_MODE_CREATE_DUMB, &createReq) < 0 )
        throw std::runtime_error("DRM_IOCTL_MODE_CREATE_DUMB failed");

    _handle = createReq.handle;
    _pitch = createReq.pitch;
    _size = createReq.size;

    // add framebuffer object
    uint32_t handles[4] = { _handle, 0, 0, 0 };
    uint32_t pitches[4] = { _pitch, 0, 0, 0 };
    uint32_t offsets[4] = { 0, 0, 0, 0 };

    if( drmModeAddFB2(_drmFd, _width, _height, DRM_FORMAT_XRGB8888,
                      handles, pitches, offsets, &_fbId, 0) < 0 )
    {
        destroy();
        throw std::runtime_error("drmModeAddFB2 failed");
    }

    // memory-map the buffer
    struct drm_mode_map_dumb mapReq = {};
    mapReq.handle = _handle;

    if( drmIoctl(_drmFd, DRM_IOCTL_MODE_MAP_DUMB, &mapReq) < 0 )
    {
        destroy();
        throw std::runtime_error("DRM_IOCTL_MODE_MAP_DUMB failed");
    }

    _map = static_cast<Pt::uint8_t*>(
        mmap(0, _size, PROT_READ | PROT_WRITE, MAP_SHARED, _drmFd, mapReq.offset) );

    if( _map == MAP_FAILED )
    {
        _map = 0;
        destroy();
        throw std::runtime_error("mmap of DRM dumb buffer failed");
    }

    std::memset(_map, 0, _size);
}


void DrmBuffer::destroy()
{
    if( _map )
    {
        munmap(_map, _size);
        _map = 0;
    }

    if( _fbId && _drmFd >= 0 )
    {
        drmModeRmFB(_drmFd, _fbId);
        _fbId = 0;
    }

    if( _handle && _drmFd >= 0 )
    {
        struct drm_mode_destroy_dumb destroyReq = {};
        destroyReq.handle = _handle;
        drmIoctl(_drmFd, DRM_IOCTL_MODE_DESTROY_DUMB, &destroyReq);
        _handle = 0;
    }

    _drmFd = -1;
}


uint32_t DrmBuffer::fbId() const
{
    return _fbId;
}


uint32_t DrmBuffer::handle() const
{
    return _handle;
}


uint32_t DrmBuffer::pitch() const
{
    return _pitch;
}


uint32_t DrmBuffer::width() const
{
    return _width;
}


uint32_t DrmBuffer::height() const
{
    return _height;
}


Pt::uint8_t* DrmBuffer::map()
{
    return _map;
}


const Pt::uint8_t* DrmBuffer::map() const
{
    return _map;
}

} // namespace

} // namespace
