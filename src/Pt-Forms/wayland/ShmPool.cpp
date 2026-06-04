/* Copyright (C) 2015-2026 Marc Boris Duerner 
  
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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/

#include "ShmPool.h"

#include <Pt/System/IOError.h>

#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>

#ifndef MFD_CLOEXEC
#include <linux/memfd.h>
#endif
#include <sys/syscall.h>

namespace Pt {
namespace Forms {

static int createShmFd(std::size_t size)
{
    int fd = static_cast<int>( syscall(SYS_memfd_create, "pt-forms-shm", MFD_CLOEXEC) );
    if( fd < 0 )
        throw Pt::System::AccessFailed("memfd_create failed");

    if( ftruncate(fd, static_cast<off_t>(size)) < 0 )
    {
        ::close(fd);
        throw Pt::System::AccessFailed("ftruncate failed");
    }

    return fd;
}


ShmBuffer::ShmBuffer()
: _buffer(0)
, _data(0)
, _pool(0)
, _fd(-1)
, _width(0)
, _height(0)
, _stride(0)
, _busy(false)
{
}


ShmBuffer::~ShmBuffer()
{
    destroy();
}


void ShmBuffer::create(struct wl_shm* shm, int width, int height, int stride)
{
    destroy();

    _width = width;
    _height = height;
    _stride = stride;

    std::size_t poolSize = static_cast<std::size_t>(height) * stride;
    _fd = createShmFd(poolSize);

    _data = static_cast<Pt::uint8_t*>(
        mmap(0, poolSize, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0)
    );

    if( _data == MAP_FAILED )
    {
        _data = 0;
        ::close(_fd);
        _fd = -1;
        throw Pt::System::AccessFailed("mmap failed for shm buffer");
    }

    struct wl_shm_pool* pool = wl_shm_create_pool(shm, _fd, static_cast<int32_t>(poolSize));
    _buffer = wl_shm_pool_create_buffer(pool, 0, width, height, stride,
                                         WL_SHM_FORMAT_ARGB8888);
    wl_shm_pool_destroy(pool);

    static const struct wl_buffer_listener bufferListener = { onRelease };
    wl_buffer_add_listener(_buffer, &bufferListener, this);

    _busy = false;
}


void ShmBuffer::destroy()
{
    if( _buffer )
    {
        wl_buffer_destroy(_buffer);
        _buffer = 0;
    }

    if( _data )
    {
        std::size_t poolSize = static_cast<std::size_t>(_height) * _stride;
        munmap(_data, poolSize);
        _data = 0;
    }

    if( _fd >= 0 )
    {
        ::close(_fd);
        _fd = -1;
    }

    _width = 0;
    _height = 0;
    _stride = 0;
    _busy = false;
}


void ShmBuffer::onRelease(void* data, struct wl_buffer* /*buffer*/)
{
    ShmBuffer* self = static_cast<ShmBuffer*>(data);
    self->_busy = false;

    if( self->_pool )
        self->_pool->onBufferReleased();
}


ShmPool::ShmPool()
: _shm(0)
, _width(0)
, _height(0)
, _repaintOnRelease(false)
{
}


ShmPool::~ShmPool()
{
}


void ShmPool::init(struct wl_shm* shm)
{
    _shm = shm;
    _buffers[0].setPool(this);
    _buffers[1].setPool(this);
}


void ShmPool::resize(int width, int height)
{
    if( _width == width && _height == height )
        return;

    _width = width;
    _height = height;

    int stride = width * 4;

    if( ! _buffers[0].busy() )
        _buffers[0].create(_shm, width, height, stride);

    if( ! _buffers[1].busy() )
        _buffers[1].create(_shm, width, height, stride);
}


ShmBuffer* ShmPool::acquireBuffer()
{
    if( ! _buffers[0].busy() )
        return &_buffers[0];

    if( ! _buffers[1].busy() )
        return &_buffers[1];

    return 0;
}


void ShmPool::onBufferReleased()
{
    if( ! _repaintOnRelease )
        return;

    _repaintOnRelease = false;
    _released.send(*this);
}

} // namespace Forms
} // namespace Pt
