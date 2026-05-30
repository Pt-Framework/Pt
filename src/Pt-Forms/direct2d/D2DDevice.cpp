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

#include "D2DDevice.h"

#include <d3d11.h>
#include <dxgi1_2.h>

#include <cassert>

namespace Pt {

namespace Forms {

D2DDevice::D2DDevice()
: _d2dFactory(nullptr)
, _d2dDevice(nullptr)
, _dwriteFactory(nullptr)
, _dxgiFactory(nullptr)
, _dxgiDevice(nullptr)
{
    // Create D2D1 factory (single-threaded)
    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                   __uuidof(ID2D1Factory1),
                                   reinterpret_cast<void**>(&_d2dFactory));
    assert(SUCCEEDED(hr));

    // Create D3D11 device (hardware with WARP fallback)
    ID3D11Device* d3dDevice = nullptr;
    D3D_FEATURE_LEVEL featureLevel;
    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_1
    };

    hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE,
                           0, creationFlags,
                           featureLevels, ARRAYSIZE(featureLevels),
                           D3D11_SDK_VERSION,
                           &d3dDevice, &featureLevel, nullptr);

    if(FAILED(hr))
    {
        // Fallback to WARP software renderer
        hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP,
                               0, creationFlags,
                               featureLevels, ARRAYSIZE(featureLevels),
                               D3D11_SDK_VERSION,
                               &d3dDevice, &featureLevel, nullptr);
    }
    assert(SUCCEEDED(hr));

    // Get DXGI device
    hr = d3dDevice->QueryInterface(__uuidof(IDXGIDevice),
                                   reinterpret_cast<void**>(&_dxgiDevice));
    assert(SUCCEEDED(hr));

    // Get DXGI factory for swap chain creation
    IDXGIAdapter* dxgiAdapter = nullptr;
    hr = _dxgiDevice->GetAdapter(&dxgiAdapter);
    assert(SUCCEEDED(hr));

    hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2),
                                reinterpret_cast<void**>(&_dxgiFactory));
    assert(SUCCEEDED(hr));
    dxgiAdapter->Release();

    // Create D2D device
    hr = _d2dFactory->CreateDevice(_dxgiDevice, &_d2dDevice);
    assert(SUCCEEDED(hr));

    d3dDevice->Release();

    // Create DWrite factory
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
                             __uuidof(IDWriteFactory),
                             reinterpret_cast<IUnknown**>(&_dwriteFactory));
    assert(SUCCEEDED(hr));
}


D2DDevice::~D2DDevice()
{
    if(_dwriteFactory)
    {
        _dwriteFactory->Release();
        _dwriteFactory = nullptr;
    }

    if(_dxgiFactory)
    {
        _dxgiFactory->Release();
        _dxgiFactory = nullptr;
    }

    if(_dxgiDevice)
    {
        _dxgiDevice->Release();
        _dxgiDevice = nullptr;
    }

    if(_d2dDevice)
    {
        _d2dDevice->Release();
        _d2dDevice = nullptr;
    }

    if(_d2dFactory)
    {
        _d2dFactory->Release();
        _d2dFactory = nullptr;
    }
}


ID2D1Factory1* D2DDevice::d2dFactory()
{
    return _d2dFactory;
}


ID2D1Device* D2DDevice::d2dDevice()
{
    return _d2dDevice;
}


IDWriteFactory* D2DDevice::dwriteFactory()
{
    return _dwriteFactory;
}


IDXGIFactory2* D2DDevice::dxgiFactory()
{
    return _dxgiFactory;
}


IDXGIDevice* D2DDevice::dxgiDevice()
{
    return _dxgiDevice;
}

} // namespace

} // namespace
