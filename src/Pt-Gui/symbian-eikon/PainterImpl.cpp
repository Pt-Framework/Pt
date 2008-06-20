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
#include "PainterImpl.h"
#include "Drawable.h"
#include <iostream>
#include "SymbianTools.h"

#include "PixmapImpl.h"

#include "Pt/Gui/Pixmap.h"
#include "Pt/Gfx/FontMetrics.h"
#include "Pt/Text/Utf16Codec.h"
#include "Pt/Text/AsciiCodec.h"
#include "Pt/Text/TextStream.h"

// symbian APIs
#include <gdi.h>
#include <eikenv.h>

namespace Pt {

namespace Gui {

enum 
{ 
    KMaxStringSize = 1024
};

const Gfx::Pen PainterImpl::_defaultPen;
const Gfx::Brush PainterImpl::_defaultBrush;
const Gfx::Font PainterImpl::_defaultFont("sans-serif");

PainterImpl::PainterImpl()
: _pen(_defaultPen)
, _brush(_defaultBrush)
, _font(_defaultFont)
, _oldPen(_defaultPen)
, _oldBrush(_defaultBrush)
, _oldFont(_defaultFont)
, _oldPenRef(0)
, _oldBrushRef(0)
, _oldFontRef(0)
, _gc(0)
, _device(0)
, _nativeFont(0)
, _fontOwner(false)
, _offset(0, 0)
, _clipRect(0, 0, 0, 0)
, _brushBitmap(0)
, _drawBitmap(0)
{
}

PainterImpl::~PainterImpl()
{
    if (_brushBitmap)
        delete _brushBitmap;
    if (_drawBitmap)
        delete _drawBitmap;
}

void PainterImpl::freeFont()
{
    // only destroy the font if we created it
    if (_fontOwner && _nativeFont)
    {
        assert(_device);
        _device->ReleaseFont(const_cast<CFont*>(_nativeFont));
        _nativeFont = 0;
        _fontOwner = false;
    }    
}

void PainterImpl::destructResources()
{
    // context should not be active
    // otherwise we're deleting resources the context holds references to
    assert(!_gc);
    
    if (_brushBitmap)
    {
        delete _brushBitmap;
        _brushBitmap = 0;
    }
    
    if (_drawBitmap)
    {
        delete _drawBitmap;
        _drawBitmap = 0;
    }

    // brush/pen is shared using a smart pointer
    // we assign it to a global const object to release the current
    // reference
    
    _pen = _oldPen = _defaultPen;
    _brush = _oldBrush = _defaultBrush;
    _font = _oldFont = _defaultFont;
    
    freeFont();
}

void PainterImpl::begin()
{
    activateBrush();
    activatePen();
    activateFont();
}

void PainterImpl::end()
{
    if (_gc)
    {
        _gc->DiscardBrushPattern();       
        _gc->DiscardFont();
    }
    
    freeFont();

    _gc = 0;
    _device = 0;
    _nativeFont = 0;
    _fontOwner = false;
    _offset = TPoint(0, 0);
    _clipRect = TRect(0, 0, 0, 0);
    
    _oldBrushRef = 0;
    _oldPenRef = 0;
    _oldFontRef = 0;
}

void PainterImpl::applyContextInfo(const ContextInfo& contextInfo)
{
    // we are getting a new default font, make sure the old one is gone
    freeFont();
    
    _gc = contextInfo._gc;
    _device = contextInfo._device;
    _nativeFont = contextInfo._nativeFont;
    _coeEnv = contextInfo._coeEnv;
    _offset = contextInfo._offset;
    _clipRect = contextInfo._clipRect;
}

void PainterImpl::cleanUp()
{    
    end();
}

void PainterImpl::setPen(const Gfx::Pen& pen)
{
    _pen = pen;
}

const Gfx::Pen& PainterImpl::pen() const
{
    return _pen;
}

void PainterImpl::setBrush(const Gfx::Brush& brush)
{
    _brush = brush;
}


const Gfx::Brush& PainterImpl::brush() const
{
    return _brush;
}

void PainterImpl::setFont(const Gfx::Font& font)
{
    _font = font;
}

const Gfx::Font& PainterImpl::font() const
{
    return _font;
}

Gfx::FontMetrics PainterImpl::fontMetrics() const
{
    // fontMetrics is const but ensureActiveContext can't be const
    // still we know what we are doing, try to activate context,
    // otherwise we can't activate and the font and thus can't 
    // return font metrics.
    if (!const_cast<PainterImpl*>(this)->ensureActiveContext())
        return Gfx::FontMetrics(0, 0, 0, 0); 
    
    const_cast<PainterImpl*>(this)->activateFont();
    
    return Gfx::FontMetrics(_nativeFont->AscentInPixels(), 
            _nativeFont->DescentInPixels(), 
            _nativeFont->MaxCharWidthInPixels(), 
            _nativeFont->HeightInPixels());
}

Gfx::FontMetrics PainterImpl::fontMetrics(const Pt::String& text) const
{
    // see comment above
    if (!const_cast<PainterImpl*>(this)->ensureActiveContext())
        return Gfx::FontMetrics(0, 0, 0, 0); 

    const_cast<PainterImpl*>(this)->activateFont();

    std::string narrowString = text.narrow();
    // TODO: Find dynamic size solution and correctly handle Unicode strings
    if (narrowString.length() > KMaxStringSize)
        throw std::runtime_error("Input string too long" + PT_SOURCEINFO);        
        
    TPtrC8 temp(reinterpret_cast<const TUint8*>(narrowString.c_str()));
    TBuf<KMaxStringSize> desc;
    desc.Copy(temp);

    return Gfx::FontMetrics(_nativeFont->AscentInPixels(), 
            _nativeFont->DescentInPixels(), 
            _nativeFont->TextWidthInPixels(desc), 
            _nativeFont->HeightInPixels());
}

const std::list<std::string>& PainterImpl::fontFamilyNames()
{ 
    static std::list<std::string> _fontList;
 
    if (_fontList.empty() && ensureActiveContext())
    {
        assert(_coeEnv);
        
        TInt numTypefaces = _coeEnv->ScreenDevice()->NumTypefaces();
        TTypefaceSupport myTypefaceSupportNow;
        for (TInt i = 0; i < numTypefaces; i++)
        {
            _coeEnv->ScreenDevice()->TypefaceSupport(myTypefaceSupportNow, i);
            TTypeface& typeface = myTypefaceSupportNow.iTypeface;
            
            // construct string vector
            // TODO: what if name contains wide characters?
            std::vector<char> strVec;            
            for (int j = 0; j < typeface.iName.Length(); ++j)
                strVec.push_back((char)typeface.iName[j]);

            // terminate string
            strVec.push_back((char)0);
            // convert string vector to std::string
            std::string str((const char*)&strVec[0]);
            
            _fontList.push_back(str);
        }
    }
    return _fontList;
}


void PainterImpl::drawPixel(const Math::Point& to)
{
    if (!ensureActiveContext())
        return;

    activatePen();
    
    _gc->Plot(SymbianTools::makeTPoint(to) + _offset);
}

void PainterImpl::drawLine(const Math::Point& from, const Math::Point& to)
{
    if (!ensureActiveContext())
        return;
    
    activatePen();

    _gc->DrawLine(SymbianTools::makeTPoint(from) + _offset, 
            SymbianTools::makeTPoint(to) + _offset);
}


void PainterImpl::drawRect(const Gfx::Rect& rect)
{
    if (!ensureActiveContext())
        return;

    activatePen();    
    
    // symbian can't draw outline rect, do it ourselves
    // rect must be normalized btw.
    TPoint p1(rect.x(), rect.y());
    TPoint p2(rect.x()+rect.width(), rect.y());
    TPoint p3(rect.x()+rect.width(), rect.y()+rect.height());
    TPoint p4(rect.x(), rect.y()+rect.height());

    p1+=_offset;
    p2+=_offset;
    p3+=_offset;
    p4+=_offset;
    
    _gc->DrawLine(p1,p2);
    _gc->DrawLine(p2,p3);
    _gc->DrawLine(p3,p4);
    _gc->DrawLine(p4,p1);
}

void PainterImpl::drawText(const Math::Point& to, const Pt::String& text)
{
    if (!ensureActiveContext())
        return;

    activatePen();
    activateFont();
    
    std::string narrowString = text.narrow();
    if (narrowString.length() > KMaxStringSize)
        throw std::runtime_error("Input string too long" + PT_SOURCEINFO);        

    const TUint8* str = reinterpret_cast<const TUint8*>(narrowString.c_str());
    TPtrC8 temp(str);
    // TODO: Find dynamic size solution
    TBuf<KMaxStringSize> desc;
    desc.Copy(temp);

    // make sure font is enabled
    // otherwise panics will occur
    _gc->DrawText(desc, SymbianTools::makeTPoint(to) + _offset);
}

/**
 * @brief Helper function to convert Pt points into a TPoint vector
 */
static inline void translatePoints(const Math::Point* points, const size_t pointCount, 
        std::vector<TPoint>& points_, const TPoint& offset)
{
    for (size_t i = 0; i < pointCount; i++)
    {
        TPoint point(SymbianTools::makeTPoint(points[i]));
        point+=offset;
        points_.push_back(point);
    }    
}

void PainterImpl::drawPolyline(const Math::Point* points, const size_t pointCount)
{
    if (!ensureActiveContext())
        return;

    activatePen();
    
    std::vector<TPoint> points_;
    translatePoints(points, pointCount, points_, _offset);
    
    _gc->DrawPolyLine(&points_[0], pointCount);
}


void PainterImpl::drawEllipse(const Math::Point& topLeft, const Math::Size& size)
{
    if (!ensureActiveContext())
        return;

    activatePen();

    TRect rect(SymbianTools::makeTRect(topLeft, size));
    rect.Move(_offset.iX, _offset.iY);
    
    // ellipse is filled by default on symbian, arc will do the job just fine
    _gc->DrawArc(rect,
            SymbianTools::makeTPoint(topLeft) + _offset,
            SymbianTools::makeTPoint(topLeft) + _offset);
}

void PainterImpl::fillRect(const Gfx::Rect& rect)
{
    if (!ensureActiveContext())
        return;

    activatePen();
    activateBrush();

    // rect has got outline with pen color
    _gc->SetPenStyle(CGraphicsContext::ENullPen);

    TRect rect_(SymbianTools::makeTRect(rect));
    rect_.Move(_offset.iX, _offset.iY);

    _gc->DrawRect(rect_);
    // force restore pen (we did change it)
    updatePen();
}

void PainterImpl::fillEllipse(const Math::Point& topLeft, const Math::Size& size)
{
    if (!ensureActiveContext())
        return;

    activatePen();
    activateBrush();

    // ellipse has got outline with pen color
    // just remove the pen and use only inner part with brush color
    _gc->SetPenStyle(CGraphicsContext::ENullPen);
    TRect rect = SymbianTools::makeTRect(topLeft, size);
    // adjust size a little. Necessary?
    rect.Move(-1, -1);
    rect.Grow(1, 1);
    rect.Move(_offset);
    _gc->DrawEllipse(rect);    
    // force restore pen (we did change it)
    updatePen();
}

void PainterImpl::fillPolygon(const Math::Point* points, const size_t pointCount)
{
    if (!ensureActiveContext())
        return;

    activatePen();
    activateBrush();

    std::vector<TPoint> points_;
    translatePoints(points, pointCount, points_, _offset);
    
    _gc->DrawPolygon(&points_[0], pointCount);    
}

void PainterImpl::drawPixmap(const Math::Point& to, Pixmap& pm)
{
    if (!ensureActiveContext())
        return;
    
    const CFbsBitmap* bitmap = pm.impl().getNativeBitmap();

    if (!bitmap)
        return;
    
    // TODO: DrawBitmap is rather slow, find faster solution (e.g. BitBlt)
    _gc->DrawBitmap(SymbianTools::makeTPoint(to) + _offset, bitmap);
}


void PainterImpl::drawPixmap(const Math::Point& to, Pixmap& pm, const Gfx::Region& pmRegion)
{
    if (!ensureActiveContext())
        return;

    const CFbsBitmap* bitmap = pm.impl().getNativeBitmap();

    if (!bitmap)
        return;
    
    TRect rect(SymbianTools::makeTRect(to, pmRegion.size()));
    rect.Move(_offset);
    TRect pmRect(SymbianTools::makeTRect(pmRegion));
    // TODO: DrawBitmap is rather slow, find faster solution (e.g. BitBlit)
    _gc->DrawBitmap(rect, bitmap, pmRect);
}

void PainterImpl::drawImage(const Pt::Math::Point& to, const Gfx::ARgbImage& image)
{
    this->drawImage( to.x(), to.y(), image.begin(), image.end(), image.width(), image.height() );
}

void PainterImpl::drawImage(const Pt::Math::Point& to, const Gfx::ARgb8888Image& image)
{
    this->drawImage( to.x(), to.y(), image.begin(), image.end(), image.width(), image.height() );
}

void PainterImpl::drawImage(const Pt::Math::Point& to, const Gfx::ARgbImage& image, const  Pt::Gfx::Region& imageRegion)
{
    Gfx::ARgbSubImage subImage(const_cast<Gfx::ARgbImage&>( image ), imageRegion);
    this->drawImage( to.x(), to.y(), subImage.begin(), subImage.end(), subImage.width(), subImage.height() );
}

void PainterImpl::drawImage(const Pt::Math::Point& to, const Gfx::ARgb8888Image& image, const  Pt::Gfx::Region& imageRegion)
{
    Gfx::ARgb8888SubImage subImage(const_cast<Gfx::ARgb8888Image&>( image ), imageRegion);
    this->drawImage( to.x(), to.y(), subImage.begin(), subImage.end(), subImage.width(), subImage.height() );
}

void PainterImpl::drawCompatibleImage(size_t x, size_t y, const char* data, size_t width, size_t height)
{
    if (!ensureActiveContext())
        return;
    
    // TODO: find faster solution
    // 1. Hold CFbsBitmap* cache pool
    // 2. Use BitBlt instead of DrawImage
    // 3. deal with different bit depths
    if (_drawBitmap && TSize(width, height) != _drawBitmap->SizeInPixels())
        _drawBitmap->Resize(TSize(width, height));
    else if (!_drawBitmap)
    {
        TRAPD(errorCode, _drawBitmap = new(ELeave) CFbsBitmap());
        if (errorCode != KErrNone)
            throw std::bad_alloc();        
            
        errorCode = _drawBitmap->Create(TSize(width, height), EColor16M);        
        if (errorCode != KErrNone)
            std::runtime_error("Bitmap creation failed." + PT_SOURCEINFO);
    }
    
    CFbsBitmap* bitmap = _drawBitmap;
    
    // convert bitmap data
    // symbian bitmap is 24 bits = 3 bytes
    // Pt bitmap is 32 bits = 4 bytes
    // TODO: Endianess might cause problems, 
    // but since ARM and x86 are both little endian
    // I don't see any trouble
    const char* src = data;
    bitmap->LockHeap();
    char* dst = reinterpret_cast<char*>(bitmap->DataAddress());
    
    for (unsigned int i = 0; i < height; ++i)
    {
        for (unsigned int j = 0; j < width; ++j)
        {
            *dst = *src;
            *(dst+1) = *(src+1);
            *(dst+2) = *(src+2);
            src+=4;
            dst+=3;
        }
    }
    bitmap->UnlockHeap();
    
    // we need to set the twips size using the display device
    // otherwise nothing will be drawn at all
    bitmap->SetSizeInTwips(_device); 

    _gc->DrawBitmap(TPoint(x,y) + _offset, bitmap);
}

void PainterImpl::updatePen()
{
    if (!ensureActiveContext())
        return;
    
    _oldPenRef = 0;
    
    activatePen();
}

void PainterImpl::activatePen()
{
    if (_oldPenRef && *_oldPenRef == _pen)
            return;
    
    if (!_gc)
        return;

    Gfx::Rgb888Color col;
    assign(col, _pen.color());    
    _gc->SetPenColor(TRgb(col.red(), col.green(), col.blue()));

    _gc->SetPenSize(TSize(_pen.size(), _pen.size()));
    
    switch (_pen.style()) 
    {
         case Gfx::Pen::SolidStyle:
             _gc->SetPenStyle(CGraphicsContext::ESolidPen);
             break;

         case Gfx::Pen::DashStyle:
             _gc->SetPenStyle(CGraphicsContext::EDashedPen);
             break;
         
         case Gfx::Pen::DoubleDash:
             // TODO: Actually doesn't exist on symbian, just use something else
             _gc->SetPenStyle(CGraphicsContext::EDotDashPen);
             break;
         
         default:
             return;
     }        

    _oldPenRef = &_oldPen;
    *_oldPenRef = _pen;
}

void PainterImpl::updateBrush()
{
    if (!ensureActiveContext())
        return;

    _oldBrushRef = 0;

    activateBrush();
}

void PainterImpl::activateBrush()
{
    if (_oldBrushRef && *_oldBrushRef == _brush)
        return;

    if (!_gc)
        return;

    switch (_brush.fillStyle()) 
    {
         case Gfx::Brush::SolidFill: 
         {
             _gc->DiscardBrushPattern();
             _gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
             Gfx::Rgb888Color col;
             assign(col, _brush.color());
             _gc->SetBrushColor(TRgb(col.red(), col.green(), col.blue()));             
             break;
         }

         case Gfx::Brush::TextureFill: 
         {
             const Gfx::ARgbImage& texture = _brush.texture();

             if (!texture.empty())
             {                 
                 // Convert our generic format to a 32 bit image format which Symbian can understand.
                 Gfx::Rgb888Image brushImage(_brush.texture().width(), _brush.texture().height());
                 assign(_brush.texture().begin(), _brush.texture().end(), brushImage.begin());

                 if (_brushBitmap)
                 {
                     delete _brushBitmap;
                     _brushBitmap = 0;
                 }
                 
                 TRAPD(errorCode, _brushBitmap = new(ELeave) CFbsBitmap());
                 if (errorCode != KErrNone)
                     throw std::bad_alloc();        
                 errorCode = _brushBitmap->Create(TSize(texture.width(), texture.height()), EColor16M);
                 if (errorCode != KErrNone)
                     std::runtime_error("Bitmap creation failed." + PT_SOURCEINFO);
                 
                 // convert bitmap data
                 // symbian bitmap is 24 bits = 3 bytes
                 // Pt bitmap is 32 bits = 4 bytes
                 // TODO: Endianess might cause problems, 
                 // but since ARM and x86 are both little endian
                 // I don't see any trouble
                 const char* src = reinterpret_cast<char*>(brushImage.data());
                 _brushBitmap->LockHeap();
                 char* dst = reinterpret_cast<char*>(_brushBitmap->DataAddress());
                 
                 const int height = texture.height();
                 const int width = texture.width();
                 for (int y = 0; y < height; ++y)
                 {
                     for (int x = 0; x < height; ++x)
                     {
                         *dst = *src;
                         *(dst+1) = *(src+1);
                         *(dst+2) = *(src+2);
                         src+=4;
                         dst+=3;
                     }
                 }
                 _brushBitmap->UnlockHeap();
                 
                 _gc->UseBrushPattern(_brushBitmap);
                 _gc->SetBrushStyle(CGraphicsContext::EPatternedBrush);
             }
             else // texture.empty() == true
             {
                 _gc->SetBrushStyle(CGraphicsContext::ENullBrush);
             }

             break;
         }

         default:
             return;
     }        

    _oldBrushRef = &_oldBrush;
    *_oldBrushRef = _brush;
}

void PainterImpl::updateFont()
{
    _oldFontRef = 0;

    activateFont();
}

void PainterImpl::activateFont()
{
    if (_oldFontRef && *_oldFontRef == _font)
        return;

    if (!_gc)
        return;
    
    if (!_coeEnv)
        return;
    
    std::string narrowString = _font.name();
    if (narrowString.length() > KMaxStringSize)
        std::runtime_error("Font name too long." + PT_SOURCEINFO);
        
    const TUint8* str = reinterpret_cast<const TUint8*>(narrowString.c_str());
    TPtrC8 temp(str);
    // TODO: Find dynamic size solution
    TBuf<KMaxStringSize> desc;
    desc.Copy(temp);

    TFontSpec spec(desc, _device->VerticalPixelsToTwips(_font.size()));
    CFont* newFont = 0;
    if (_device->GetNearestFontToDesignHeightInTwips(newFont, spec) == KErrNone && 
        newFont)
    {
        freeFont();
        _nativeFont = newFont;        
        _fontOwner = true;
    }
    
    assert(_nativeFont);
    // old font is automatically discarded
    _gc->UseFont(_nativeFont);

    _oldFontRef = &_oldFont;
    *_oldFontRef = _font;
}

bool PainterImpl::ensureActiveContext()
{
    if (!_gc)
    {
        begin();
        if (!_gc)
            return false;
    }
    return true;
}

ConcretePainter::ConcretePainter(Drawable& drawable)
: _drawable(drawable)
, _active(false)
{
}


ConcretePainter::~ConcretePainter()
{
    if (_active)
        end(); 
}


void ConcretePainter::begin()
{
    if (!_active)
    {
        ContextInfo contextInfo = _drawable.beginDraw();
        
        applyContextInfo(contextInfo);
        
        if (_gc)
            _active = true;
    }
    
    PainterImpl::begin();
}


void ConcretePainter::end()
{
    PainterImpl::end();
    
    if (_active)
    {
        _drawable.endDraw();        
        _active = false;
    }
}

} // namespace Gui

} // namespace Pt


