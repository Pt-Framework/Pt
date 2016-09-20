/* Copyright (C) 2015 Marc Boris Duerner 
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan
  
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

#include "win32.h"
#include "PaintSurfaceImpl.h"
#include "PictureImpl.h"
#include <Pt/Hmi/Application.h>
#include <Pt/Gfx/Argb32Format.h>
#include <tchar.h>
#include "PixmapSurfaceImpl.h"

namespace {

DWORD getPenStyle(const Pt::Gfx::Pen& pen)
{
  using namespace Pt;

#ifdef _WIN32_WCE
    DWORD penStyle = 0;
#else
    DWORD penStyle = PS_GEOMETRIC;
#endif

    switch( pen.style() )
    {
        case Gfx::Pen::SolidStyle:
            penStyle |= PS_SOLID;
        break;
        case Gfx::Pen::DashStyle:
            penStyle |= PS_DASH;
        break;
    }

#ifndef _WIN32_WCE
    switch( pen.capStyle() )
    {
        case Gfx::Pen::RoundCap:
            penStyle |= PS_ENDCAP_ROUND;
        break;
        case Gfx::Pen::FlatCap:
            penStyle |= PS_ENDCAP_FLAT;
        break;
    }

    switch( pen.joinStyle() )
    {
        case Gfx::Pen::RoundJoin:
             penStyle |= PS_JOIN_ROUND;
        break;
        case Gfx::Pen::BevelJoin:
             penStyle |= PS_JOIN_BEVEL;
        break;
    }
#endif

    return penStyle;
}


HFONT getFont(const Pt::Gfx::Font& font)
{
    int fontWeight;
    
    switch( font.fontStyle() ) 
    {
        default:
        case Pt::Gfx::Font::NormalStyle:
        case Pt::Gfx::Font::ItalicStyle:
            fontWeight = FW_NORMAL;
            break;

        case Pt::Gfx::Font::BoldStyle:
        case Pt::Gfx::Font::BoldItalicStyle:
            fontWeight = FW_BOLD;
            break;
    }

    BYTE italic = font.fontStyle() == Pt::Gfx::Font::ItalicStyle || 
                  font.fontStyle() == Pt::Gfx::Font::BoldItalicStyle;

    LOGFONT lf;
    lf.lfHeight         = -((int)font.size());         // converted to device units
    lf.lfWidth          = 0;                           // default width of the font
    lf.lfEscapement     = font.angle();                // escapement angle
    lf.lfOrientation    = 0;                           // orientation
    lf.lfWeight         = fontWeight;                  // font weight
    lf.lfItalic         = italic;                      // italic
    lf.lfUnderline      = FALSE;                       // underline
    lf.lfStrikeOut      = FALSE;                       // strikeout
    lf.lfCharSet        = DEFAULT_CHARSET;             // use the default charset
    lf.lfOutPrecision   = OUT_DEFAULT_PRECIS;          // default output precision
    lf.lfClipPrecision  = CLIP_DEFAULT_PRECIS;         // default clipping behaviour
    lf.lfQuality        = DEFAULT_QUALITY;             // default quality
    lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE; // default pitch and family
   
    memcpy(lf.lfFaceName, font.name().c_str(),
           std::min<size_t>( LF_FACESIZE, font.name().size() + 1) );

    HFONT hf = CreateFontIndirect(&lf);
    return hf;
}


HBRUSH gradientBrush(HDC dc, int width, int height,
                     Pt::Gfx::Color gradientStart, 
                     Pt::Gfx::Color gradientStop, 
                     Pt::Gfx::Brush::FillStyle style)
{

    BITMAPINFO bi;
    ZeroMemory(&bi.bmiHeader, sizeof(BITMAPINFOHEADER));

    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
    bi.bmiHeader.biPlanes       = 1;         // always 1
    bi.bmiHeader.biBitCount     = 32;        // ARGB 32
    bi.bmiHeader.biCompression  = BI_RGB;    // uncompressed RGB
    bi.bmiHeader.biSizeImage    = 0;         // automatic
    bi.bmiHeader.biClrUsed      = 0;         // no color table
    bi.bmiHeader.biClrImportant = 0;         // no color table 
    
    if( style == Pt::Gfx::Brush::HorizontalGradient )
    {
        bi.bmiHeader.biWidth    = width;
        bi.bmiHeader.biHeight   = 1;
    }
    else // vertical
    {
        bi.bmiHeader.biWidth    = 1;
        bi.bmiHeader.biHeight   = height;

        std::swap(gradientStart, gradientStop);
    }

    int length = bi.bmiHeader.biWidth + bi.bmiHeader.biHeight - 1;

    VOID* imageBits = NULL;
    HBITMAP bitmap = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &imageBits, NULL, 0);

    Pt::uint8_t* pixel = reinterpret_cast<Pt::uint8_t*>(imageBits);
            
    for(int n = 0; n < length; ++n)
    {
        float f1 = (length - n) / float(length);
        float f2 = n / float(length);

        float r1 = gradientStart.red() * f1;
        float r2 = gradientStop.red() * f2;

        float g1 = gradientStart.green() * f1;
        float g2 = gradientStop.green() * f2;

        float b1 = gradientStart.blue() * f1;
        float b2 = gradientStop.blue() * f2;
                
        
        pixel[0] = (b1 + b2) / 257;
        pixel[1] = (g1 + g2) / 257;
        pixel[2] = (r1 + r2) / 257;                
        pixel[3] = 0;

        pixel += 4;
    }

    HBRUSH brush = CreatePatternBrush(bitmap);
    DeleteObject(bitmap);
    
    return brush;
}

} // namespace

namespace Pt {

namespace Hmi {

PixmapSurfaceImpl::PixmapSurfaceImpl()
: _deviceContext(0)
, _gradientBrush(false)
{
    _size = Gfx::SizeF(10,10);

    HDC screenDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
    _deviceContext = CreateCompatibleDC(screenDC);
    _bitmapHandle = CreateCompatibleBitmap(screenDC, (int)_size.width(), (int)_size.height());
    
    DeleteDC(screenDC);

    _oldPen   = (HPEN) GetCurrentObject(_deviceContext, OBJ_PEN);
    _oldBrush = (HBRUSH) GetCurrentObject(_deviceContext, OBJ_BRUSH);
    _oldFont  = (HFONT) GetCurrentObject(_deviceContext, OBJ_FONT);

    SelectObject(_deviceContext, _bitmapHandle);
    SetBkMode(_deviceContext, TRANSPARENT);
}


PixmapSurfaceImpl::~PixmapSurfaceImpl()
{
    HPEN pen = (HPEN)SelectObject(_deviceContext, _oldPen);
    DeleteObject(pen);

    HPEN brush = (HPEN)SelectObject(_deviceContext, _oldBrush);
    DeleteObject(brush);

    HPEN font = (HPEN)SelectObject(_deviceContext, _oldFont);
    DeleteObject(font);

    DeleteDC(_deviceContext);
    DeleteObject(_bitmapHandle);
}


const Gfx::ImageFormat& PixmapSurfaceImpl::format() const
{
  return Gfx::ImageFormat::argb32();
}


void PixmapSurfaceImpl::clear(const Gfx::Color& c)
{
}


void PixmapSurfaceImpl::resize(const Gfx::SizeF& size)
{
    if( _size == size )
        return;

    _size = size;
    Gfx::Size nsize = Application::instance().screen().fromUnit( _size );

    //Save the old settings
    COLORREF textColor = GetTextColor(_deviceContext);
    _oldPen   = (HPEN) GetCurrentObject(_deviceContext, OBJ_PEN);
    _oldBrush = (HBRUSH) GetCurrentObject(_deviceContext, OBJ_BRUSH);
    _oldFont  = (HFONT) GetCurrentObject(_deviceContext, OBJ_FONT);

    //Delete the context and bitmap
    DeleteDC(_deviceContext);
    DeleteObject(_bitmapHandle);

    //Create a new context and bitmap
    HDC screenDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
    _deviceContext = CreateCompatibleDC(screenDC);
    _bitmapHandle = CreateCompatibleBitmap(screenDC, nsize.width(), nsize.height());
    DeleteDC(screenDC);

    //Restore the old settings
    SelectObject(_deviceContext, _bitmapHandle);
    SelectObject(_deviceContext, _oldFont);
    SelectObject(_deviceContext, _oldBrush);
    SelectObject(_deviceContext, _oldPen);
    SetTextColor(_deviceContext, textColor);
    SetBkMode(_deviceContext, TRANSPARENT);
}


const Gfx::SizeF& PixmapSurfaceImpl::size() const
{
    return _size;
}


void PixmapSurfaceImpl::setClip(const Gfx::RectF& clipRect)
{
    if( clipRect.isNull() )
    {
        SelectClipRgn( _deviceContext, NULL );
        return;
    }

    Gfx::Rect rect = Application::instance().screen().fromUnit(clipRect);

    HRGN hrgn = CreateRectRgn( rect.x(), 
                               rect.y(), 
                               rect.bottomRight().x() + 1, 
                               rect.bottomRight().y() + 1 );

    SelectClipRgn(_deviceContext, hrgn);

    DeleteObject(hrgn);

    _clip = clipRect;
}


const Gfx::RectF& PixmapSurfaceImpl::clip()
{
  return _clip;
}


void PixmapSurfaceImpl::setPen(const Gfx::Pen& pen)
{
    DWORD penStyle = getPenStyle( pen );
    DWORD penColor = RGB( pen.color().red()  / 257, 
                          pen.color().green() / 257, 
                          pen.color().blue()  / 257 );

#ifdef _WIN32_WCE
    HPEN newPen = CreatePen(penStyle, pen.size(), penColor);
#else
    LOGBRUSH brush;
    brush.lbStyle = BS_SOLID ;
    brush.lbColor = penColor;

    HPEN newPen = ExtCreatePen(penStyle, pen.size(), &brush, 0, NULL);
#endif

    HPEN oldPen = (HPEN) SelectObject(_deviceContext, newPen);
    DeleteObject(oldPen);

    SetTextColor(_deviceContext, penColor);
}


void PixmapSurfaceImpl::setBrush(const Gfx::Brush& brush)
{
    _gradientBrush = false;

    HBRUSH brushHandle = NULL;
    DWORD brushColor = RGB(brush.color().red() / 257, 
                           brush.color().green() / 257, 
                           brush.color().blue() / 257);

    switch( brush.fillStyle() ) 
    {
        case Gfx::Brush::Solid: 
        {
            brushHandle = CreateSolidBrush(brushColor);
            break;
        }

        case Gfx::Brush::Texture: 
        {
            const Gfx::Image& texture = brush.texture();

            // use an empty brush for empty textures
            if(texture.width() == 0)
            {
                brushHandle = (HBRUSH) GetStockObject(NULL_BRUSH);
                break;
            }

            BITMAPINFO bi;
            ZeroMemory(&bi.bmiHeader, sizeof(BITMAPINFOHEADER));

            bi.bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);    
            bi.bmiHeader.biWidth        = texture.width();                // width
            bi.bmiHeader.biHeight       = -(ssize_t)texture.height();     // top-down image
            bi.bmiHeader.biPlanes       = 1;                              // always 1
            bi.bmiHeader.biBitCount     = texture.view().pixelStride()*8; // 32-bit
            bi.bmiHeader.biCompression  = BI_RGB;                         // uncompressed RGB
            bi.bmiHeader.biSizeImage    = 0;                              // automatic
            bi.bmiHeader.biClrUsed      = 0;                              // no color table
            bi.bmiHeader.biClrImportant = 0;                              // no color table

            VOID* imageBits;
            HBITMAP bitmap = CreateDIBSection(_deviceContext, &bi, 
                                              DIB_RGB_COLORS, &imageBits, NULL, 0);
            memcpy(imageBits, 
                    texture.data(), 
                    texture.width() * texture.height() * texture.view().pixelStride());

            brushHandle = CreatePatternBrush(bitmap);
            DeleteObject(bitmap);
            break;     
        }

        case Gfx::Brush::HorizontalGradient:
        case Gfx::Brush::VerticalGradient:
        {
            _gradientBrush = true;
            _gradientStyle = brush.fillStyle();
            _gradientStart = brush.color();
            _gradientStop = brush.gradientColor();

            // do not set a brush now, because the gradient brush pattern can
            // only be calculated later, when the fill area is known
            return;
        }

        default:
            return;
    }

    HGDIOBJ oldBrush = SelectObject(_deviceContext, brushHandle);
    DeleteObject(oldBrush);
}


void PixmapSurfaceImpl::setFont(const Gfx::Font& font)
{
    SetTextAlign(_deviceContext, TA_BASELINE | TA_LEFT | TA_NOUPDATECP);

    HFONT newFont = getFont(font);

    HGDIOBJ oldFont = SelectObject(_deviceContext, newFont);
    DeleteObject(oldFont);
}


Gfx::FontMetrics PixmapSurfaceImpl::fontMetrics(const Pt::String& text) const
{
    SIZE textSize;
    TEXTMETRIC tm;
    GetTextMetrics(_deviceContext, &tm);

    std::wstring wtext;
    text.toUtf16( std::back_inserter(wtext) );
    
    GetTextExtentPoint32W(_deviceContext, 
                          wtext.c_str(), wtext.size(), &textSize);
    
    Gfx::Size size(textSize.cx, textSize.cy);
    Gfx::SizeF sizeF = Application::instance().screen().toUnit(size);

    return Gfx::FontMetrics(tm.tmAscent, 
                            tm.tmDescent, 
                            (int)sizeF.width(), 
                            (int)sizeF.height());
}


void PixmapSurfaceImpl::drawLine(const Gfx::PointF& fromF, const Gfx::PointF& toF)
{
    Gfx::Point from = Application::instance().screen().fromUnit(fromF);
    Gfx::Point to = Application::instance().screen().fromUnit(toF);

    POINT points[2];
    points[0].x = from.x();
    points[0].y = from.y();
    points[1].x = to.x();
    points[1].y = to.y();

    Polyline(_deviceContext, points, 2);
}


void PixmapSurfaceImpl::drawText(const Gfx::PointF& toF, const Pt::String& text)
{
    Gfx::Point to = Application::instance().screen().fromUnit(toF);
  
    RECT rectangle;
    SetRect(&rectangle, to.x(), to.y(), to.x(), to.y());

    _text.clear();
    text.toUtf16( std::back_inserter(_text) );    
    
    int rezt = DrawTextW(_deviceContext, _text.c_str(), -1, 
                         &rectangle, DT_NOCLIP| DT_NOPREFIX );    
}


void PixmapSurfaceImpl::drawRect(const Gfx::RectF& rectF)
{
    Gfx::Rect rect = Application::instance().screen().fromUnit(rectF);

    if (rect.size().width() == 1 && rect.size().height() == 1) 
    {
        // windows does not paint rectangles with a size of 1x1
        // we draw a pixel (1|1) instead
        drawLine(rectF.topLeft(), rectF.topLeft());
        return;
    }

    HBRUSH originalBrush = (HBRUSH)SelectObject(_deviceContext, GetStockObject(NULL_BRUSH));
    
    Rectangle(_deviceContext, rect.left(), rect.top(), rect.right()+1, rect.bottom()+1);

    SelectObject(_deviceContext, originalBrush);
}


void PixmapSurfaceImpl::fillRect(const Gfx::RectF& rectF)
{
    Gfx::Rect rect = Application::instance().screen().fromUnit(rectF);
  
    RECT rectangle;
    rectangle.left  = rect.left();
    rectangle.top   = rect.top();
    rectangle.right  = rect.right() + 1;    
    rectangle.bottom = rect.bottom() + 1;

    if(_gradientBrush)
    {
        HBRUSH brush = gradientBrush(_deviceContext, rect.width(), rect.height(),
                                     _gradientStart, _gradientStop, _gradientStyle);

        POINT brushOrigin = {0};
        SetBrushOrgEx(_deviceContext, rect.x(), rect.y(), &brushOrigin);

        FillRect(_deviceContext, &rectangle, brush);

        SetBrushOrgEx(_deviceContext, brushOrigin.x, brushOrigin.y, NULL);
        DeleteObject(brush);
        return;
    }

    HBRUSH currentBrush = (HBRUSH) GetCurrentObject(_deviceContext, OBJ_BRUSH);

    FillRect(_deviceContext, &rectangle, currentBrush);
}


void PixmapSurfaceImpl::drawEllipse(const Gfx::PointF& topLeftF, const Gfx::SizeF& sizeF)
{
    Gfx::Point topLeft = Application::instance().screen().fromUnit(topLeftF);
    Gfx::Size size = Application::instance().screen().fromUnit(sizeF);

    HBRUSH originalBrush = (HBRUSH)SelectObject(_deviceContext, GetStockObject(NULL_BRUSH));

    Ellipse(_deviceContext, 
            topLeft.x(), topLeft.y(), 
            topLeft.x() + size.width(), 
            topLeft.y() + size.height() );

    SelectObject(_deviceContext, originalBrush);
}


void PixmapSurfaceImpl::fillEllipse(const Gfx::PointF& topLeftF, const Gfx::SizeF& sizeF)
{
    POINT brushOrigin = {0};
    Gfx::Point topLeft = Application::instance().screen().fromUnit(topLeftF);
    Gfx::Size size = Application::instance().screen().fromUnit(sizeF);

    if(_gradientBrush)
    {
        HBRUSH brush = gradientBrush(_deviceContext, size.width(), size.height(),
                                     _gradientStart, _gradientStop, _gradientStyle);

        HGDIOBJ oldBrush = SelectObject(_deviceContext, brush);
        DeleteObject(oldBrush);

        SetBrushOrgEx(_deviceContext, topLeft.x(), topLeft.y(), &brushOrigin);
    }

    HPEN originalPen = (HPEN) SelectObject(_deviceContext, GetStockObject(NULL_PEN));

    Ellipse(_deviceContext,
            topLeft.x(),
            topLeft.y(),
            topLeft.x() + size.width() + 1,
            topLeft.y() + size.height() + 1);

    SelectObject(_deviceContext, originalPen);

    if(_gradientBrush)
    {
        SetBrushOrgEx(_deviceContext, brushOrigin.x, brushOrigin.y, NULL);
    }
}


void PixmapSurfaceImpl::drawPolyline(const Gfx::PointF* points, const size_t pointCount)
{
    std::vector<POINT> winPoints(pointCount);

    for (size_t i = 0; i < pointCount; i++)
    {
        Gfx::Point p = Application::instance().screen().fromUnit(points[i]);
        winPoints[i].x = p.x();
        winPoints[i].y = p.y();
    }

    Polyline( _deviceContext, &winPoints[0], pointCount );
}


void PixmapSurfaceImpl::fillPolygon(const Gfx::PointF* points, const size_t pointCount)
{
    if( ! pointCount ) 
        return;

    POINT brushOrigin = {0};

    int left = std::numeric_limits<int>::max();
    int top = std::numeric_limits<int>::max();
    int right = 0;
    int bottom = 0;

    std::vector<POINT> winPoints(pointCount);

    for(size_t i = 0; i < pointCount; i++)
    {
        Gfx::Point p = Application::instance().screen().fromUnit(points[i]);
        winPoints[i].x = p.x();
        winPoints[i].y = p.y();

        if( p.y() < top)
            top = p.y();

        if( p.y() > bottom)
            bottom = p.y();

        if( p.x() < left)
            left = p.x();

        if( p.x() > right)
            right = p.x();
    }

    if(_gradientBrush)
    {
        HBRUSH brush = gradientBrush(_deviceContext, right - left, bottom - top,
                                     _gradientStart, _gradientStop, _gradientStyle);

        HGDIOBJ oldBrush = SelectObject(_deviceContext, brush);
        DeleteObject(oldBrush);

        SetBrushOrgEx(_deviceContext, left, top, NULL);
    }

    HPEN originalPen = (HPEN) SelectObject(_deviceContext, GetStockObject(NULL_PEN));
    Polygon(_deviceContext, &winPoints[0], pointCount);
    SelectObject(_deviceContext, originalPen);

    if(_gradientBrush)
    {
        SetBrushOrgEx(_deviceContext, brushOrigin.x, brushOrigin.y, NULL);
    }
}


void PixmapSurfaceImpl::drawSurface(const Gfx::PointF& toF, const PixmapSurface& surface)
{
    Gfx::Point to = Application::instance().screen().fromUnit(toF);
    Gfx::Size size = Application::instance().screen().fromUnit( surface.size() );

    BitBlt( _deviceContext, 
            to.x(), to.y(), size.width(), size.height(), 
            surface.pixmapImpl()->deviceContext(), 0, 0, SRCCOPY);
}


void PixmapSurfaceImpl::drawSurface(const Gfx::PointF& toF, 
                                    const PixmapSurface& pm, 
                                    const Gfx::RectF& pmRect)
{
    Gfx::Point to = Application::instance().screen().fromUnit(toF);
    Gfx::Size size = Application::instance().screen().fromUnit( pmRect.size() );
    Gfx::Point from = Application::instance().screen().fromUnit(pmRect.topLeft());

    BitBlt( _deviceContext, 
            to.x(), to.y(), size.width(), size.height(), 
            pm.pixmapImpl()->deviceContext(), from.x(), from.y(), SRCCOPY);
}


void PixmapSurfaceImpl::drawPicture(const Gfx::PointF& toF, const Picture& pic)
{
    const PictureImpl* picImpl = pic.impl();

    if( picImpl->empty() )
      return;

    if( picImpl->mask() )
    {
        bitBlit(toF, picImpl->width(), picImpl->height(), pic.impl()->mask(), SRCAND);
        bitBlit(toF, picImpl->width(), picImpl->height(), pic.impl()->bitmap(), SRCPAINT);
        return;
    }

    // NOTE: it is impossible to use SourceCopy since picture is
    //       already premultiplied...

    Gfx::Point to = Application::instance().screen().fromUnit(toF);
    HDC bitmapDC = CreateCompatibleDC(NULL);

    SelectObject( bitmapDC, picImpl->bitmap() );

    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = 0xFF; // only per pixel alpha
    bf.AlphaFormat = AC_SRC_ALPHA;    

    AlphaBlend(_deviceContext, to.x(), to.y(), picImpl->width(), picImpl->height(),
                bitmapDC, 0, 0, picImpl->width(), picImpl->height(), bf);

    DeleteDC(bitmapDC);
}


void PixmapSurfaceImpl::bitBlit( const Gfx::PointF& pos, size_t width, size_t height, HBITMAP bitmap, DWORD op )
{
    Gfx::Point to = Application::instance().screen().fromUnit(pos);

    HDC bitmapDC = CreateCompatibleDC(NULL);
    SelectObject(bitmapDC, bitmap);

    BitBlt(_deviceContext,  to.x(), to.y(), width, height, bitmapDC, 0, 0, op);

    DeleteDC(bitmapDC);
}


void PixmapSurfaceImpl::drawImage(const Gfx::PointF& to, const Gfx::Image& image, const Gfx::RectF& imgRect)
{
  //Todo:
    throw std::runtime_error("not implemented");
}



void PixmapSurfaceImpl::drawImage(const Gfx::PointF& toF, const Gfx::Image& image)
{
    Gfx::Point to = Application::instance().screen().fromUnit(toF);

    const size_t depth = image.view().pixelStride() * 8; 
    const Pt::uint8_t* data = image.data();

    HBITMAP bitmap = CreateBitmap(image.width(), image.height(), 1, depth, (VOID*)data);

    if (bitmap == NULL) 
    {
        BITMAPINFO bitmapInfo;
        ZeroMemory(&bitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));

        bitmapInfo.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER); 
        bitmapInfo.bmiHeader.biWidth       = image.width();   
        bitmapInfo.bmiHeader.biHeight      = -(ssize_t)image.height();  // top-down image
        bitmapInfo.bmiHeader.biPlanes      = 1;                         // always 1            
        bitmapInfo.bmiHeader.biBitCount    = depth;                     // 32-bit 
        bitmapInfo.bmiHeader.biCompression = BI_RGB;                    // uncompressed RGB
        bitmapInfo.bmiHeader.biSizeImage   = 0;                         // automatic
        bitmapInfo.bmiHeader.biClrUsed     = 0;                         // no color table
        bitmapInfo.bmiHeader.biClrImportant= 0;                         // no color table

        VOID* imageBits;
        bitmap = CreateDIBSection(_deviceContext, &bitmapInfo, 
                                  DIB_RGB_COLORS, &imageBits, NULL, 0);
        memcpy(imageBits, data, image.width() * image.height() * 4);
    }

    HDC bitmapDC = CreateCompatibleDC(NULL);
    SelectObject(bitmapDC, bitmap);

    BitBlt(_deviceContext, 
           to.x(), to.y(), image.width(), image.height(), 
           bitmapDC, 0, 0, SRCCOPY);

    DeleteDC(bitmapDC);
    DeleteObject(bitmap);
}


HDC PixmapSurfaceImpl::deviceContext() const
{
    return _deviceContext;
}

} // namespace

} // namespace
