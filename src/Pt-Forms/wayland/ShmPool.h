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

#ifndef Pt_Forms_ShmPool_h
#define Pt_Forms_ShmPool_h

#include <Pt/Forms/Api.h>
#include <Pt/Signal.h>
#include <Pt/Types.h>

#include <wayland-client.h>

#include <cstddef>

namespace Pt {
namespace Forms {

class ShmPool;


class ShmBuffer
{
    public:
        ShmBuffer();
        ~ShmBuffer();

        void create(struct wl_shm* shm, int width, int height, int stride);
        void destroy();

        struct wl_buffer* buffer() const { return _buffer; }
        Pt::uint8_t* data() const { return _data; }
        int width() const { return _width; }
        int height() const { return _height; }
        int stride() const { return _stride; }
        std::size_t size() const { return static_cast<std::size_t>(_height) * _stride; }
        bool busy() const { return _busy; }
        void setBusy(bool b) { _busy = b; }

        void setPool(ShmPool* pool) { _pool = pool; }

    private:
        static void onRelease(void* data, struct wl_buffer* buffer);

    private:
        struct wl_buffer* _buffer;
        Pt::uint8_t*     _data;
        ShmPool*         _pool;
        int              _fd;
        int              _width;
        int              _height;
        int              _stride;
        bool             _busy;
};


class ShmPool
{
    public:
        ShmPool();
        ~ShmPool();

        void init(struct wl_shm* shm);
        void resize(int width, int height);

        ShmBuffer* acquireBuffer();
        struct wl_shm* shm() const { return _shm; }
        int width() const { return _width; }
        int height() const { return _height; }

        void setRepaintOnRelease(bool b) { _repaintOnRelease = b; }
        Signal<ShmPool&>& released() { return _released; }

    private:
        friend class ShmBuffer;
        void onBufferReleased();

    private:
        Signal<ShmPool&> _released;
        struct wl_shm*   _shm;
        ShmBuffer        _buffers[2];
        int              _width;
        int              _height;
        bool             _repaintOnRelease;
};

} // namespace Forms
} // namespace Pt

#endif
