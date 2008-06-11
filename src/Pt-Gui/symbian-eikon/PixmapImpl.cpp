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
#include "ApplicationImpl.h"
#include "SymbApp.h"
#include "SymbDoc.h"
#include "SymbAppUi.h"

#include <iostream>

// symbian APIs
#include <fbs.h>
#include <eikenv.h>

namespace Pt {

namespace Gui {

PixmapImpl::PixmapImpl(size_t width, size_t height)
: _size( std::max(width, size_t(1)), std::max(height, size_t(1)) )
, _painter(*this) 
, _bitmap(0)
, _bitmapGc(0) 
, _bitmapDevice(0)
, _lastError(0)
{
    ResourceRegistry::instance().registerPixmap(this);
    construct();
}


PixmapImpl::PixmapImpl(const PixmapImpl& pimpl)
: _size( pimpl.size() )
, _painter(*this)
, _bitmap(0)
, _bitmapGc(0)
, _bitmapDevice(0)
{
    ResourceRegistry::instance().registerPixmap(this);
    construct();
}


PixmapImpl::~PixmapImpl()
{
    ResourceRegistry::instance().unregisterPixmap(this);
    destruct();
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
        
        assert(Pt::Gui::ApplicationImpl::_self);        
        assert(Pt::Gui::ApplicationImpl::_self && Pt::Gui::ApplicationImpl::_self->_symbApp->HasInitialized());
        const CFont* font = Pt::Gui::ApplicationImpl::_self->_symbApp->Document().AppUi().Font();
        contextInfo._nativeFont = font;
        
        _bitmapGc->UseFont(font);
        contextInfo._coeEnv = CEikonEnv::Static();
    }

    return contextInfo;
}

void PixmapImpl::endDraw()
{
}

void PixmapImpl::construct()
{
    if (_bitmap && _bitmap->SizeInPixels() == TSize(_size.width(), _size.height()))
        return;
    
    if (Pt::Gui::ApplicationImpl::_self && 
        Pt::Gui::ApplicationImpl::_self->_symbApp->HasInitialized())
    {
        destruct();

        // TODO: Handle leave
        _bitmap = new(ELeave) CFbsBitmap();    
        // TODO: check error code and handle bit depth
        // handle errors    
        _lastError = _bitmap->Create(TSize(_size.width(), _size.height()), EColor16M);
        if (_lastError != KErrNone)
        {
            delete _bitmap;
            _bitmap = 0;
            // TODO: Throw error
            std::cout << "Bitmap creation failed";
        }

        TRAP(_lastError, _bitmapDevice = CFbsBitmapDevice::NewL(_bitmap));
        if (_lastError == KErrNone)
        {
            _bitmap->SetSizeInTwips(CEikonEnv::Static()->ScreenDevice());
            
            //TSize sizeInTwips = _bitmap->SizeInTwips();
            //TSize sizeInPixels = _bitmap->SizeInPixels();            
            //float xs = (float)sizeInTwips.iWidth / (float)sizeInPixels.iWidth;
            //float ys = (float)sizeInTwips.iHeight / (float)sizeInPixels.iHeight;
            
            // TODO: Handle leave
            _bitmapGc = CFbsBitGc::NewL();
            _bitmapGc->Activate(_bitmapDevice);
        }
    }
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
    
    _painter.destructResources();
}

} // namespace Gui

} // namespace Pt
