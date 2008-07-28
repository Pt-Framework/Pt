/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Duerner                                 *
 *   Copyright (C) 2008 Peter Barth                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "PixmapImpl.h"
#include <iostream>

#include "ApplicationImpl.h"
#include "SymbAppUi.h"

// symbian APIs
#include <fbs.h>
#include <eikenv.h>

namespace Pt {

namespace Gui {

PixmapImpl::PixmapImpl(size_t width, size_t height)
: _size( std::max(width, size_t(1)), std::max(height, size_t(1)) )
, _painter(*this) 
, _defaultFont(0)
, _bitmap(0)
, _bitmapGc(0) 
, _bitmapDevice(0)
, _lastError(0)
{
    Environment::instance().registerResource(this);
    construct();
}


PixmapImpl::PixmapImpl(const PixmapImpl& pimpl)
: _size( pimpl.size() )
, _painter(*this)
, _defaultFont(0)
, _bitmap(0)
, _bitmapGc(0)
, _bitmapDevice(0)
{
    Environment::instance().registerResource(this);
    construct();
    //TODO: Copy over contents from existing bitmap?
}


PixmapImpl::~PixmapImpl()
{
    destruct();
    Environment::instance().unregisterResource(this);
}


Painter PixmapImpl::painter()
{
    return Painter(&_painter);
}

PainterImpl::ContextInfo PixmapImpl::beginDraw()
{
    PainterImpl::ContextInfo contextInfo;
    
    if (!_bitmapGc)
        construct();

    if (_bitmapGc)
    {
        contextInfo._gc = _bitmapGc;
        contextInfo._device = _bitmapDevice;        
        contextInfo._nativeFont = _defaultFont;        
        _bitmapGc->UseFont(_defaultFont);
        contextInfo._coeEnv = CEikonEnv::Static();
    }

    return contextInfo;
}

void PixmapImpl::endDraw()
{
}

void PixmapImpl::construct()
{
    // TODO: handle bit depth if bitmap is supporting other bit depths
    // no reason to create yet another bitmap
    if (_bitmap && _bitmap->SizeInPixels() == TSize(_size.width(), _size.height()))
        return;
    
    destruct();

    // get default font from UI
    CSymbAppUi& ui = Pt::Gui::Environment::instance().symbAppUi();

    _defaultFont = &ui.Font();

    // try to create bitmap instance
    TRAP(_lastError, _bitmap = new(ELeave) CFbsBitmap());
    if (_lastError != KErrNone)
        throw std::bad_alloc();            

    // TODO: For now bitmap is always of type EColor16M
    _lastError = _bitmap->Create(TSize(_size.width(), _size.height()), EColor16M);
    if (_lastError != KErrNone)
    {
        delete _bitmap;
        _bitmap = 0;
        throw std::runtime_error("Bitmap creation failed" + PT_SOURCEINFO);
    }

    TRAP(_lastError, _bitmapDevice = CFbsBitmapDevice::NewL(_bitmap));
    if (_lastError != KErrNone)
    {
        delete _bitmap;
        _bitmap = 0;
        throw std::runtime_error("Bitmap device creation failed" + PT_SOURCEINFO);            
    }

    _bitmap->SetSizeInTwips(CEikonEnv::Static()->ScreenDevice());
        
    // for debugging purpose I'm leaving this in, sometimes
    // it's interesting to check the aspect ration between twips and pixels
    //TSize sizeInTwips = _bitmap->SizeInTwips();
    //TSize sizeInPixels = _bitmap->SizeInPixels();            
    //float xs = (float)sizeInTwips.iWidth / (float)sizeInPixels.iWidth;
    //float ys = (float)sizeInTwips.iHeight / (float)sizeInPixels.iHeight;

    TRAP(_lastError, _bitmapGc = CFbsBitGc::NewL());
    if (_lastError != KErrNone)        
    {
        delete _bitmapDevice;
        _bitmapDevice = 0;
        delete _bitmap;
        _bitmap = 0;
        throw std::runtime_error("Bitmap context creation failed" + PT_SOURCEINFO);                        
    }
    
    _bitmapGc->Activate(_bitmapDevice);
}

void PixmapImpl::destruct()
{
    if (_bitmap)
    {
        delete _bitmap;
        _bitmap = 0;
    }

    if (_bitmapDevice)
    {
        delete _bitmapDevice;
        _bitmapDevice = 0;
    }

    if (_bitmapGc)
    {
        delete _bitmapGc;
        _bitmapGc = 0;
    }
    
    _defaultFont = 0;
    
    _painter.destructResources();
}

} // namespace Gui

} // namespace Pt
