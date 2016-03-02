/*
 * Copyright (C) 2014 Laurentiu-Gheorghe Crisan
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "win32.h"
#include "PaintSurfaceImpl.h"
#include "PainterImpl.h"
#include <Pt/Hmi/Application.h>
#include <tchar.h>

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

} // namespace

namespace Pt {

namespace Hmi {

/////////////////////////////////////////////////////////////////////
// PaintSurfaceImpl
/////////////////////////////////////////////////////////////////////

#ifdef _WIN32_WCE

static int CALLBACK EnumFontsProc(LOGFONT *logFont, TEXTMETRIC *physFont, DWORD type, LPARAM param)
{
    WCHAR* faceName = logFont->lfFaceName;

    if (faceName[0] != '@') {  // Ignore fonts with @ as first character.
        ((std::list<std::string>*)param)->push_back(win32::toMultiByte(faceName));
    }

    return 1;
}

#else

static int CALLBACK EnumFontFamExProc(ENUMLOGFONTEX *logFont, NEWTEXTMETRICEX *physFont, DWORD type, LPARAM param)
{
    char* faceName = logFont->elfLogFont.lfFaceName;

    if (faceName[0] != '@') {  // Ignore fonts with @ as first character.
        ((std::list<std::string>*)param)->push_back(faceName);
    }

    return 1;
}

#endif


std::list<std::string> PaintSurfaceImpl::fontFamilyNames()
{
    std::list<std::string> fonts;
    HDC dc = GetDC(NULL);

#ifdef _WIN32_WCE
    EnumFonts(dc, 0, (FONTENUMPROC)&EnumFontsProc, (LPARAM)this);
#else
    LOGFONT lf;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfFaceName[0] = '\0';
    lf.lfPitchAndFamily = 0;

    EnumFontFamiliesEx(dc, &lf, (FONTENUMPROC)&EnumFontFamExProc, (LPARAM)(&fonts), 0);
#endif

    ReleaseDC(NULL, dc);

    fonts.unique();
    return fonts;
}


std::string PaintSurfaceImpl::defaultFont()
{
    HDC dc = GetDC(NULL);

    std::vector<TCHAR> buffer(32);
    GetTextFace(dc, buffer.size(), &buffer[0]);

    ReleaseDC(NULL, dc);

    return Pt::win32::toMultiByte(&buffer[0]);
}


Gfx::FontMetrics PaintSurfaceImpl::fontMetrics(const Gfx::Font& font, const Pt::String& text)
{   
    HDC dc = GetDC(NULL);

    SIZE textSize;
    TEXTMETRIC tm;
    GetTextMetrics(dc, &tm);

    std::wstring wtext;
    text.toUtf16( std::back_inserter(wtext) );
    
    GetTextExtentPoint32W(dc, wtext.c_str(), wtext.size(), &textSize);
    
    Gfx::Size size(textSize.cx, textSize.cy);
    Gfx::SizeF sizeF = Application::instance().mainScreen().toUnit(size);

    ReleaseDC(NULL, dc);

    return Gfx::FontMetrics(tm.tmAscent, 
                            tm.tmDescent, 
                            (int)sizeF.width(), 
                            (int)sizeF.height());
}

/////////////////////////////////////////////////////////////////////
// PaintRegionImpl
/////////////////////////////////////////////////////////////////////

PaintRegionImpl::PaintRegionImpl()
: _surface(0)
{
}


PaintRegionImpl::~PaintRegionImpl()
{
}


void PaintRegionImpl::set(PaintSurface& surface, const Gfx::RectF& area)
{
    _surface = &surface;
    _area = area;
}


const Gfx::SizeF& PaintRegionImpl::size() const
{
    return _area.size();
}


void PaintRegionImpl::setPen(const Gfx::Pen& pen)
{
    _surface->impl()->setPen(pen);
}


void PaintRegionImpl::setBrush(const Gfx::Brush& brush)
{
    _surface->impl()->setBrush(brush);
}


void PaintRegionImpl::setFont(const Gfx::Font& font)
{
    _surface->impl()->setFont(font);
}


Gfx::FontMetrics PaintRegionImpl::fontMetrics(const Pt::String& text) const
{
    return _surface->impl()->fontMetrics(text);
}


void PaintRegionImpl::drawLine(const Gfx::PointF& fromF, const Gfx::PointF& toF)
{
    _surface->impl()->drawLine(fromF + _area.topLeft(),
                               toF + _area.topLeft() );
}


void PaintRegionImpl::drawText(const Gfx::PointF& toF, const Pt::String& text)
{
    _surface->impl()->drawText(toF + _area.topLeft(), text);
}


void PaintRegionImpl::drawRect(const Gfx::RectF& r)
{
    Gfx::RectF rect(r);
    rect.setOrigin(r.topLeft() + _area.topLeft());

    _surface->impl()->drawRect(rect);
}


void PaintRegionImpl::fillRect(const Gfx::RectF& r)
{
    Gfx::RectF rect(r);
    rect.setOrigin(r.topLeft() + _area.topLeft());

    _surface->impl()->fillRect(rect);
}


void PaintRegionImpl::drawEllipse(const Gfx::PointF& topLeftF, const Gfx::SizeF& sizeF)
{
    _surface->impl()->drawEllipse(topLeftF + _area.topLeft(), sizeF);
}


void PaintRegionImpl::fillEllipse(const Gfx::PointF& topLeftF, const Gfx::SizeF& sizeF)
{
    _surface->impl()->fillEllipse(topLeftF + _area.topLeft(), sizeF);
}


void PaintRegionImpl::drawPolyline(const Gfx::PointF* points, size_t pointCount)
{
    std::vector<POINT> winPoints(pointCount);

    for (size_t i = 0; i < pointCount; i++)
    {
        Gfx::PointF pt = points[i] + _area.topLeft();
        Gfx::Point p = Application::instance().mainScreen().fromUnit(pt);
        winPoints[i].x = p.x();
        winPoints[i].y = p.y();
    }

    _surface->impl()->drawPolyline(&winPoints[0], pointCount);
}


void PaintRegionImpl::drawPolyline(POINT* points, size_t pointCount)
{
    _surface->impl()->drawPolyline(points, pointCount);
}


void PaintRegionImpl::fillPolygon(const Gfx::PointF* points, size_t pointCount)
{
    std::vector<POINT> winPoints(pointCount);

    for (size_t i = 0; i < pointCount; i++)
    {
        Gfx::PointF pt = points[i] + _area.topLeft();
        Gfx::Point p = Application::instance().mainScreen().fromUnit(pt);
        winPoints[i].x = p.x();
        winPoints[i].y = p.y();
    }

    _surface->impl()->fillPolygon(&winPoints[0], pointCount);
}


void PaintRegionImpl::fillPolygon(POINT* points, size_t pointCount)
{
    _surface->impl()->fillPolygon(points, pointCount);
}


void PaintRegionImpl::drawSurface(const Gfx::PointF& toF, const PixmapSurface& surface)
{
    _surface->impl()->drawSurface(toF + _area.topLeft(), surface);
}


void PaintRegionImpl::drawSurface(const Gfx::PointF& toF, 
                                  const PixmapSurface& pm,
                                  const Gfx::RectF& pmRect)
{
    _surface->impl()->drawSurface(toF + _area.topLeft(), pm, pmRect);
}


void PaintRegionImpl::drawImage(const Gfx::PointF& toF, const Gfx::Image& image)
{
    _surface->impl()->drawImage(toF + _area.topLeft(), image);
}

/////////////////////////////////////////////////////////////////////
// PixmapSurfaceImpl
/////////////////////////////////////////////////////////////////////

PixmapSurfaceImpl::PixmapSurfaceImpl()
: _deviceContext(0)
{
    _size = Gfx::SizeF(10,10);

    Gfx::Size nsize = Application::instance().mainScreen().fromUnit(_size);

    HDC screenDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
    _deviceContext = CreateCompatibleDC(screenDC);
    _bitmapHandle = CreateCompatibleBitmap(screenDC, nsize.width(), nsize.height());
    
    DeleteDC(screenDC);

    _oldPen   = (HPEN) GetCurrentObject(_deviceContext, OBJ_PEN);
    _oldBrush = (HBRUSH) GetCurrentObject(_deviceContext, OBJ_BRUSH);
    _oldFont  = (HFONT) GetCurrentObject(_deviceContext, OBJ_FONT);

    SelectObject(_deviceContext, _bitmapHandle);
    SetBkMode(_deviceContext, TRANSPARENT);
}


PixmapSurfaceImpl::~PixmapSurfaceImpl()
{
    HPEN oldPen = (HPEN)SelectObject(_deviceContext, _oldPen);
    DeleteObject(oldPen);

    HPEN oldBrush = (HPEN)SelectObject(_deviceContext, _oldBrush);
    DeleteObject(oldBrush);

    HPEN oldFont = (HPEN)SelectObject(_deviceContext, _oldFont);
    DeleteObject(oldFont);

    DeleteDC(_deviceContext);
    DeleteObject(_bitmapHandle);
}


void PixmapSurfaceImpl::clear()
{
}


void PixmapSurfaceImpl::resize(const Gfx::SizeF& size)
{
    if( _size == size )
        return;

    _size = size;
    Gfx::Size nsize = Application::instance().mainScreen().fromUnit( _size );

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


void PixmapSurfaceImpl::setPen(const Gfx::Pen& pen)
{
    DWORD penStyle = getPenStyle( pen );
    DWORD penColor = RGB(pen.color().red() * 255, 
                         pen.color().green() * 255, 
                         pen.color().blue() * 255);

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
    HBRUSH newBrushHandle;
    DWORD brushColor = RGB(brush.color().red() * 255, 
                           brush.color().green() * 255, 
                           brush.color().blue() * 255);

    switch( brush.fillStyle() ) 
    {
        case Gfx::Brush::SolidFill: 
        {
            newBrushHandle = CreateSolidBrush(brushColor);
            break;
        }

        case Gfx::Brush::TextureFill: 
        {
            const Gfx::Image& texture = brush.texture();

            // use an empty brush for empty textures
            if(texture.width() == 0)
            {
                newBrushHandle = (HBRUSH)GetStockObject(NULL_BRUSH);
                break;
            }

            BITMAPINFO bi;
            ZeroMemory(&bi.bmiHeader, sizeof(BITMAPINFOHEADER));

            bi.bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);    
            bi.bmiHeader.biWidth        = texture.width();                // width
            bi.bmiHeader.biHeight       = -(ssize_t)texture.height();     // top-down image
            bi.bmiHeader.biPlanes       = 1;                              // always 1
            bi.bmiHeader.biBitCount     = texture.format().pixelSize()*8; // 32-bit
            bi.bmiHeader.biCompression  = BI_RGB;                         // uncompressed RGB
            bi.bmiHeader.biSizeImage    = 0;                              // automatic
            bi.bmiHeader.biClrUsed      = 0;                              // no color table
            bi.bmiHeader.biClrImportant = 0;                              // no color table

            VOID* imageBits;
            HBITMAP bitmap = CreateDIBSection(_deviceContext, &bi, 
                                              DIB_RGB_COLORS, &imageBits, NULL, 0);
            memcpy(imageBits, 
                    texture.pixel(0,0), 
                    texture.width() * texture.height() * texture.format().pixelSize());

            newBrushHandle = CreatePatternBrush(bitmap);
            DeleteObject(bitmap);
            break;     
        }

        default:
            return;
    }

    HBRUSH oldBrushHandle = (HBRUSH)SelectObject(_deviceContext, newBrushHandle);
    DeleteObject(oldBrushHandle);
}


void PixmapSurfaceImpl::setFont(const Gfx::Font& font)
{
    SetTextAlign(_deviceContext, TA_BASELINE | TA_LEFT | TA_NOUPDATECP);

    int fontWeight;
    
    switch( font.fontStyle() ) 
    {
        default:
        case Gfx::Font::NormalStyle:
        case Gfx::Font::ItalicStyle:
            fontWeight = FW_NORMAL;
            break;

        case Gfx::Font::BoldStyle:
        case Gfx::Font::BoldItalicStyle:
            fontWeight = FW_BOLD;
            break;
    }

    BYTE italic = font.fontStyle() == Gfx::Font::ItalicStyle || 
                  font.fontStyle() == Gfx::Font::BoldItalicStyle;

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
   
    memset(lf.lfFaceName, 0, LF_FACESIZE * sizeof(TCHAR));
    
    memcpy(lf.lfFaceName, font.name().c_str(),
           std::min<size_t>( LF_FACESIZE - 1, font.name().size() + 1) );

    HFONT newFont = CreateFontIndirect(&lf);

    HFONT oldFont = (HFONT)SelectObject(_deviceContext, newFont);
    DeleteObject(oldFont);
        
    //SetTextColor(_deviceContext, RGB(_pen.color().red()*255, _pen.color().green()*255, _pen.color() .blue()*255));
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
    Gfx::SizeF sizeF = Application::instance().mainScreen().toUnit(size);

    return Gfx::FontMetrics(tm.tmAscent, 
                            tm.tmDescent, 
                            (int)sizeF.width(), 
                            (int)sizeF.height());
}


void PixmapSurfaceImpl::drawLine(const Gfx::PointF& fromF, const Gfx::PointF& toF)
{
    Gfx::Point from = Application::instance().mainScreen().fromUnit(fromF);
    Gfx::Point to = Application::instance().mainScreen().fromUnit(toF);

    POINT points[2];
    points[0].x = from.x();
    points[0].y = from.y();
    points[1].x = to.x();
    points[1].y = to.y();

    Polyline(_deviceContext, points, 2);
}


void PixmapSurfaceImpl::drawText(const Gfx::PointF& toF, const Pt::String& text)
{
    Gfx::Point to = Application::instance().mainScreen().fromUnit(toF);
  
    RECT rectangle;
    SetRect(&rectangle, to.x(), to.y(), to.x(), to.y());

    _text.clear();
    text.toUtf16( std::back_inserter(_text) );    
    
    int rezt = DrawTextW(_deviceContext, _text.c_str(), -1, 
                         &rectangle, DT_NOCLIP| DT_NOPREFIX );    
}


void PixmapSurfaceImpl::drawRect(const Gfx::RectF& rectF)
{
    Gfx::Rect rect = Application::instance().mainScreen().fromUnit(rectF);

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
    Gfx::Rect rect = Application::instance().mainScreen().fromUnit(rectF);
  
    RECT rectangle;
    rectangle.left  = rect.left();
    rectangle.top   = rect.top();
    rectangle.right  = rect.right() + 1;    
    rectangle.bottom = rect.bottom() + 1;

    HBRUSH currentBrush = (HBRUSH)GetCurrentObject(_deviceContext, OBJ_BRUSH);
    FillRect(_deviceContext, &rectangle, currentBrush);
}


void PixmapSurfaceImpl::drawEllipse(const Gfx::PointF& topLeftF, const Gfx::SizeF& sizeF)
{
    Gfx::Point topLeft = Application::instance().mainScreen().fromUnit(topLeftF);
    Gfx::Size size = Application::instance().mainScreen().fromUnit(sizeF);

    HBRUSH originalBrush = (HBRUSH)SelectObject(_deviceContext, GetStockObject(NULL_BRUSH));

    Ellipse(_deviceContext, 
            topLeft.x(), topLeft.y(), 
            topLeft.x() + size.width(), 
            topLeft.y() + size.height() );

    SelectObject(_deviceContext, originalBrush);
}


void PixmapSurfaceImpl::fillEllipse(const Gfx::PointF& topLeftF, const Gfx::SizeF& sizeF)
{
    Gfx::Point topLeft = Application::instance().mainScreen().fromUnit(topLeftF);
    Gfx::Size size = Application::instance().mainScreen().fromUnit(sizeF);

    HPEN originalPen = (HPEN)SelectObject(_deviceContext, GetStockObject(NULL_PEN));

    Ellipse(_deviceContext,
            topLeft.x(),
            topLeft.y(),
            topLeft.x() + size.width() + 1,
            topLeft.y() + size.height() + 1);

    SelectObject(_deviceContext, originalPen);
}


void PixmapSurfaceImpl::drawPolyline(const Gfx::PointF* points, const size_t pointCount)
{
    std::vector<POINT> winPoints(pointCount);

    for (size_t i = 0; i < pointCount; i++)
    {
        Gfx::Point p = Application::instance().mainScreen().fromUnit(points[i]);
        winPoints[i].x = p.x();
        winPoints[i].y = p.y();
    }

    drawPolyline(&winPoints[0], pointCount);
}


void PixmapSurfaceImpl::drawPolyline(POINT* points, size_t pointCount)
{
    Polyline( _deviceContext, points, pointCount );
}


void PixmapSurfaceImpl::fillPolygon(const Gfx::PointF* points, const size_t pointCount)
{
    std::vector<POINT> winPoints(pointCount);

    for (size_t i = 0; i < pointCount; i++)
    {
        Gfx::Point p = Application::instance().mainScreen().fromUnit(points[i]);
        winPoints[i].x = p.x();
        winPoints[i].y = p.y();
    }

    fillPolygon(&winPoints[0], pointCount);
}


void PixmapSurfaceImpl::fillPolygon(POINT* points, size_t pointCount)
{
    HPEN originalPen = (HPEN)SelectObject(_deviceContext, GetStockObject(NULL_PEN));
    Polygon(_deviceContext, points, pointCount);
    SelectObject(_deviceContext, originalPen);
}


void PixmapSurfaceImpl::drawSurface(const Gfx::PointF& toF, const PixmapSurface& surface)
{
    Gfx::Point to = Application::instance().mainScreen().fromUnit(toF);
    Gfx::Size size = Application::instance().mainScreen().fromUnit( surface.impl()->size() );

    BitBlt( _deviceContext, 
            to.x(), to.y(), size.width(), size.height(), 
            surface.pixmapImpl()->deviceContext(), 0, 0, SRCCOPY);
}


void PixmapSurfaceImpl::drawSurface(const Gfx::PointF& toF, 
                                    const PixmapSurface& pm, 
                                    const Gfx::RectF& pmRect)
{
    Gfx::Point to = Application::instance().mainScreen().fromUnit(toF);
    Gfx::Size size = Application::instance().mainScreen().fromUnit( pmRect.size() );
    Gfx::Point from = Application::instance().mainScreen().fromUnit(pmRect.topLeft());

    BitBlt( _deviceContext, 
            to.x(), to.y(), size.width(), size.height(), 
            pm.pixmapImpl()->deviceContext(), from.x(), from.y(), SRCCOPY);
}


void PixmapSurfaceImpl::drawImage(const Gfx::PointF& toF, const Gfx::Image& image)
{
    Gfx::Point to = Application::instance().mainScreen().fromUnit(toF);

    const size_t depth = image.format().pixelSize() * 8; 
    const Pt::uint8_t* data = image.pixel(0,0);

    HBITMAP bitmap = CreateBitmap(image.width(), image.height(), 1, depth, (VOID*)data);
    if (bitmap == NULL) 
    {
        BITMAPINFO bitmapInfo;
        ZeroMemory(&bitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));

        bitmapInfo.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER); 
        bitmapInfo.bmiHeader.biWidth       = image.width();   
        bitmapInfo.bmiHeader.biHeight      = -(ssize_t)image.height();  // top-down image
        bitmapInfo.bmiHeader.biPlanes      = 1;                         // always 1            
        bitmapInfo.bmiHeader.biBitCount    = 32;                        // 32-bit 
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
