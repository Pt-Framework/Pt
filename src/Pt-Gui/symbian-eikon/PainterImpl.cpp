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
#include "PixmapImpl.h"
#include "Pt/Gui/Pixmap.h"
#include "Pt/Gfx/FontMetrics.h"
#include "Pt/Text/Utf16Codec.h"
#include "Pt/Text/AsciiCodec.h"
#include "Pt/Text/TextStream.h"
#include "SymbianTools.h"

#include <iostream>

// symbian APIs
#include <gdi.h>
#include <eikenv.h>

namespace Pt {

namespace Gui {

PainterImpl::PainterImpl()
: _font("sans-serif")
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
    if (_fontOwner && _nativeFont)
    {
        //assert(_coeEnv);
        //_coeEnv->ReleaseScreenFont(const_cast<CFont*>(_nativeFont));
        assert(_device);
        _device->ReleaseFont(const_cast<CFont*>(_nativeFont));
        _nativeFont = 0;
        _fontOwner = false;
    }    
}

void PainterImpl::destructResources()
{
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
    
    _oldBrushRef = 0;
    _oldPenRef = 0;
    _oldFontRef = 0;
}

void PainterImpl::cleanUp()
{    
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
    if (!const_cast<PainterImpl*>(this)->ensureActiveContext())
        return Gfx::FontMetrics(0, 0, 0, 0); 

    const_cast<PainterImpl*>(this)->activateFont();

    std::string narrowString = text.narrow();
    TPtrC8 temp(reinterpret_cast<const TUint8*>(narrowString.c_str()));
    // TODO: Find dynamic size solution
    TBuf<1024> desc;
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
            std::vector<char> strVec;            
            for (int j = 0; j < typeface.iName.Length(); ++j)
            {
                strVec.push_back((char)typeface.iName[j]);
            }
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
    const TUint8* str = reinterpret_cast<const TUint8*>(narrowString.c_str());
    TPtrC8 temp(str);
    // TODO: Find dynamic size solution
    TBuf<1024> desc;
    desc.Copy(temp);

    // make sure font is enabled
    _gc->DrawText(desc, SymbianTools::makeTPoint(to) + _offset);
}

static void translatePoints(const Math::Point* points, const size_t pointCount, 
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
    for (size_t i = 0; i < pointCount; i++)
    {
        TPoint point(SymbianTools::makeTPoint(points[i]));
        point+=_offset;
        points_.push_back(point);
    }   
    
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
    _gc->SetPenStyle(CGraphicsContext::ENullPen);
    TRect rect = SymbianTools::makeTRect(topLeft, size);
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
    for (size_t i = 0; i < pointCount; i++)
    {
        TPoint point(SymbianTools::makeTPoint(points[i]));
        point+=_offset;
        points_.push_back(point);
    }   
    
    _gc->DrawPolygon(&points_[0], pointCount);    
}

void PainterImpl::drawPixmap(const Math::Point& to, Pixmap& pm)
{
    if (!ensureActiveContext())
        return;
    
    CFbsBitmap* bitmap = pm.impl().getNativeBitmap();

    if (!bitmap)
        return;
    
    _gc->DrawBitmap(SymbianTools::makeTPoint(to) + _offset, bitmap);
}


void PainterImpl::drawPixmap(const Math::Point& to, Pixmap& pm, const Gfx::Region& pmRegion)
{
    if (!ensureActiveContext())
        return;

    CFbsBitmap* bitmap = pm.impl().getNativeBitmap();

    if (!bitmap)
        return;
    
    TRect rect(SymbianTools::makeTRect(to, pmRegion.size()));
    rect.Move(_offset);
    TRect pmRect(SymbianTools::makeTRect(pmRegion));
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
    if (_drawBitmap && TSize(width, height) != _drawBitmap->SizeInPixels())
        _drawBitmap->Resize(TSize(width, height));
    else if (!_drawBitmap)
    {
        _drawBitmap = new CFbsBitmap();
        _drawBitmap->Create(TSize(width, height), EColor16M);        
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
                 
                 // TODO: handle leave
                 _brushBitmap = new CFbsBitmap();
                 _brushBitmap->Create(TSize(texture.width(), texture.height()), EColor16M);
                 
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
    const TUint8* str = reinterpret_cast<const TUint8*>(narrowString.c_str());
    TPtrC8 temp(str);
    // TODO: Find dynamic size solution
    TBuf<1024> desc;
    desc.Copy(temp);

    TFontSpec spec(desc, _device->VerticalPixelsToTwips(_font.size()));
    // TODO: Handle leave
    //_nativeFont = _coeEnv->CreateScreenFontL(spec);    
    CFont* newFont = 0;
    if (_device->GetNearestFontToDesignHeightInTwips(newFont, spec) == KErrNone && newFont)
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

} // namespace Gui

} // namespace Pt


