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

#ifndef Pt_Forms_DrmBuffer_h
#define Pt_Forms_DrmBuffer_h

#include <Pt/Types.h>

namespace Pt {

namespace Forms {

class DrmBuffer
{
    public:
        DrmBuffer();

        ~DrmBuffer();

        void create(int drmFd, uint32_t width, uint32_t height);

        void destroy();

        uint32_t fbId() const;

        uint32_t handle() const;

        uint32_t pitch() const;

        uint32_t width() const;

        uint32_t height() const;

        Pt::uint8_t* map();

        const Pt::uint8_t* map() const;

    private:
        DrmBuffer(const DrmBuffer&);
        DrmBuffer& operator=(const DrmBuffer&);

    private:
        int           _drmFd;
        uint32_t      _handle;
        uint32_t      _fbId;
        uint32_t      _width;
        uint32_t      _height;
        uint32_t      _pitch;
        uint64_t      _size;
        Pt::uint8_t*  _map;
};

} // namespace

} // namespace

#endif
