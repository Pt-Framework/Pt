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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#ifndef Pt_Forms_D2DDevice_h
#define Pt_Forms_D2DDevice_h

#include <d2d1.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <dxgi1_2.h>

namespace Pt {

namespace Forms {

class D2DDevice
{
    public:
        D2DDevice();

        ~D2DDevice();

        ID2D1Factory1* d2dFactory();

        ID2D1Device* d2dDevice();

        IDWriteFactory* dwriteFactory();

        IDXGIFactory2* dxgiFactory();

        IDXGIDevice* dxgiDevice();

    private:
        D2DDevice(const D2DDevice&);
        D2DDevice& operator=(const D2DDevice&);

    private:
        ID2D1Factory1*  _d2dFactory;
        ID2D1Device*    _d2dDevice;
        IDWriteFactory* _dwriteFactory;
        IDXGIFactory2*  _dxgiFactory;
        IDXGIDevice*    _dxgiDevice;
};

} // namespace

} // namespace

#endif
