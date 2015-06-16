/*
 * Copyright (C) 2006 Marc Boris Duerner
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
#include "PainterImpl.h"
#include "PaintSurfaceImpl.h"
#include "Pt/Types.h"
#include <Pt/System/Clock.h>
#include "Pt/Ui/Rect.h"
#include "Pt/Ui/Region.h"
#include "Pt/Ui/FontMetrics.h"
#include "Pt/Ui/Color.h"
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/PaintSurface.h>
#include <iostream>
#include <algorithm>

namespace Pt {
namespace Hmi {

PainterImpl::PainterImpl(PaintSurfaceImpl* surface)
: _surface(surface)
, _pen(Ui::Pen(1))
, _brush(Ui::Brush(Ui::Color(0, 0, 0)))
, _font(Ui::Font(determinePlatformDefaultFontName()))
, _renderMode(Ui::RenderMode::NoAlpha)
{
}


PainterImpl::~PainterImpl()
{
}


void PainterImpl::drawText( const Ui::PointF& to, const Pt::String& text, const Ui::Color* outline )
{
	//Todo: impl
    
    drawText(to, text);
}

void PainterImpl::setRenderMode( Ui::RenderMode::Type mode )
{
	_renderMode = mode;
}

void PainterImpl::setPen(const Ui::Pen& pen)
{
    _pen = pen;
    updatePen();
}


DWORD PainterImpl::toGdiPenStyle( const Ui::Pen& pen )
{
#ifdef _WIN32_WCE
    DWORD penStyle = 0;
#else
    DWORD penStyle = PS_GEOMETRIC;
#endif

    switch( pen.style() )
    {
        case Ui::Pen::SolidStyle:
            penStyle |= PS_SOLID;
        break;
        case Ui::Pen::DashStyle:
            penStyle |= PS_DASH;
        break;
    }

#ifndef _WIN32_WCE
    switch( pen.capStyle() )
    {
        case Ui::Pen::RoundCap:
            penStyle |= PS_ENDCAP_ROUND;
        break;
        case Ui::Pen::FlatCap:
            penStyle |= PS_ENDCAP_FLAT;
        break;
    }

    switch( pen.joinStyle() )
    {
        case Ui::Pen::RoundJoin:
             penStyle |= PS_JOIN_ROUND;
        break;
        case Ui::Pen::BevelJoin:
             penStyle |= PS_JOIN_BEVEL;
        break;
    }
#endif

    return penStyle;
}


void PainterImpl::updatePen()
{
    DWORD penStyle = toGdiPenStyle( _pen );

#ifdef _WIN32_WCE
    HPEN newPen = CreatePen( penStyle, _pen.size(), RGB(penCol.red(), penCol.green(), penCol.blue()) );
#else
    LOGBRUSH brush;
    brush.lbStyle = BS_SOLID ;
    brush.lbColor = RGB(_pen.color().red()* 255, _pen.color().green()* 255, _pen.color().blue()* 255);

    HPEN newPen = ExtCreatePen( penStyle , _pen.size(), &brush, 0, NULL );
#endif

    HPEN oldPen = (HPEN)SelectObject(_surface->deviceContext(), newPen);

    DeleteObject(oldPen);

    // Set the Text color to the pen color.
    SetTextColor(_surface->deviceContext(), RGB(_pen.color().red()*255, _pen.color().green()*255, _pen.color().blue()*255));
}


const Ui::Pen& PainterImpl::pen() const
{
    return _pen;
}


void PainterImpl::setBrush(const Ui::Brush& brush)
{
    _brush = brush;
    updateBrush();
}


void PainterImpl::updateBrush()
{
    HBRUSH newBrushHandle;

    switch (_brush.fillStyle()) {

        case Ui::Brush::SolidFill: 
				{
            newBrushHandle = CreateSolidBrush(RGB(_brush.color().red()*255, _brush.color().green()*255, _brush.color().blue()*255));
            break;
        }

        case Ui::Brush::TextureFill: 
				{
            const Ui::Image& texture = _brush.texture();

            if (texture.width() != 0)
            {
                // Fill the info for a device-independent bitmap to hold the texture data in the Windows system.
                BITMAPINFO bitmapInfo;
                ZeroMemory(&bitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));

                bitmapInfo.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);    // Size of this struct.
                bitmapInfo.bmiHeader.biWidth       = texture.width();             // Bitmap width.
                bitmapInfo.bmiHeader.biHeight      = -(ssize_t)texture.height();  // Bitmap height. Top-down image.
                bitmapInfo.bmiHeader.biPlanes      = 1;                           // Always 1.
                bitmapInfo.bmiHeader.biBitCount    = texture.format().pixelSize()*8;  // We internally use a 32-bit bitmap.
                bitmapInfo.bmiHeader.biCompression = BI_RGB;                      // Uncompressed (top-down) RGB bitmap.
                bitmapInfo.bmiHeader.biSizeImage   = 0;                           // 0 = automatic for BI_RGB-images.
                bitmapInfo.bmiHeader.biClrUsed     = 0;                           // 0 = No color table.
                bitmapInfo.bmiHeader.biClrImportant= 0;                           // 0 = No color table.

                // Create the device-independent bitmap that will be filled with the texture
                // and used as brush.
                VOID* imageBits;
                HBITMAP bitmap = CreateDIBSection(_surface->deviceContext(), &bitmapInfo, DIB_RGB_COLORS, &imageBits, NULL, 0);

                // Copy image data from the texture to the Windows bitmap.
                memcpy(imageBits, texture.pixel(0,0), texture.width() * texture.height() * texture.format().pixelSize());

                // Create the actual brush from this bitmap.
                newBrushHandle = CreatePatternBrush(bitmap);

                // Free the bitmap again.
                DeleteObject(bitmap);
            }
            else // texture.empty() == true
            {
                // Use the empty brush for empty textures.
                newBrushHandle = (HBRUSH)GetStockObject(NULL_BRUSH);
            }            
        }
		break;

        default:
            // TODO Throw runtime exception in case we do not check for every possible value of Brush::FillStyle?
	    return;
    }

    HBRUSH oldBrushHandle = (HBRUSH)SelectObject(_surface->deviceContext(), newBrushHandle);

    DeleteObject(oldBrushHandle);
}


const Ui::Brush& PainterImpl::brush() const
{
    return _brush;
}

void PainterImpl::setFont(const Ui::Font& font)
{
    if (font == _font) 
        return;

    _font = font;
    updateFont();
}


void PainterImpl::updateFont()
{
    SetTextAlign(_surface->deviceContext(), TA_BASELINE | TA_LEFT | TA_NOUPDATECP);

    int fontWeight;
    
	switch (_font.fontStyle()) 
	{
        case Ui::Font::NormalStyle:
        case Ui::Font::ItalicStyle:
            fontWeight = FW_NORMAL;
        break;

        case Ui::Font::BoldStyle:
        case Ui::Font::BoldItalicStyle:
            fontWeight = FW_BOLD;
        break;
    }

    BYTE italic = (_font.fontStyle() == Ui::Font::ItalicStyle || _font.fontStyle() == Ui::Font::BoldItalicStyle);

    LOGFONT fontDescription;
    fontDescription.lfHeight         = -((int)_font.size()); // negative value -> Value is converted to device units.
    fontDescription.lfWidth          = 0;                    // width - Default width of the font.
    fontDescription.lfEscapement     = _font.angle();        // escapement angle
    fontDescription.lfOrientation    = 0;                    // orientation
    fontDescription.lfWeight         = fontWeight;           // font weight
    fontDescription.lfItalic         = italic;               // italic
    fontDescription.lfUnderline      = FALSE;                // underline
    fontDescription.lfStrikeOut      = FALSE;                // strikeout
    fontDescription.lfCharSet        = DEFAULT_CHARSET;      // charset - use the default charset
    fontDescription.lfOutPrecision   = OUT_DEFAULT_PRECIS;   // output precision - default output precision
    fontDescription.lfClipPrecision  = CLIP_DEFAULT_PRECIS;  // clipping behaviour - default clipping behaviour
    fontDescription.lfQuality        = DEFAULT_QUALITY;      // quality - default quality
    fontDescription.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE; // font pitch and family - default
    memset(fontDescription.lfFaceName, 0, LF_FACESIZE * sizeof(TCHAR));
    memcpy(fontDescription.lfFaceName,
           _font.name().c_str(),
           std::min<size_t>( LF_FACESIZE - 1, _font.name().size() + 1)
          );

    HFONT newFont = CreateFontIndirect(&fontDescription);

    HFONT oldFont = (HFONT)SelectObject(_surface->deviceContext(), newFont);

    DeleteObject(oldFont);
    	
    SetTextColor(_surface->deviceContext(), RGB(_pen.color().red()*255, _pen.color().green()*255, _pen.color() .blue()*255));
}

std::string PainterImpl::determinePlatformDefaultFontName()
{
    HFONT defaultFont = (HFONT)GetStockObject(SYSTEM_FONT);
    SelectObject(_surface->deviceContext(), defaultFont);

    std::vector<TCHAR> buffer(32);
    GetTextFace(_surface->deviceContext(), buffer.size(), &buffer[0]);

    return win32::toMultiByte(&buffer[0]);
}


const Ui::Font& PainterImpl::font() const
{
    return _font;
}


Ui::FontMetrics PainterImpl::fontMetrics() const
{
    TEXTMETRIC metrics;
    GetTextMetrics(_surface->deviceContext(), &metrics);

    return Ui::FontMetrics(metrics.tmAscent, metrics.tmDescent, 0, metrics.tmHeight);
}


Ui::FontMetrics PainterImpl::fontMetrics(Pt::String text) const
{
    SIZE textSize;
    TEXTMETRIC basicMetrics;
    GetTextMetrics(_surface->deviceContext(), &basicMetrics);

    _text.clear();
	text.toUtf16( std::back_inserter(_text) );
    GetTextExtentPoint32W(_surface->deviceContext(), _text.c_str(), _text.length(), &textSize);
	Ui::Size size(textSize.cx, textSize.cy);
	Ui::SizeF sizeF = Application::instance().toUnit(size);

    return Ui::FontMetrics(basicMetrics.tmAscent, basicMetrics.tmDescent, (int)sizeF.width(), (int)sizeF.height());
}


void PainterImpl::addFontName(const std::string& fontName)
{
    _fontNamesList.push_back(fontName);
}


#ifdef _WIN32_WCE

    static int CALLBACK EnumFontsProc(LOGFONT *logFont, TEXTMETRIC *physFont, DWORD type, LPARAM param)
    {
        WCHAR* faceName = logFont->lfFaceName;

        if (faceName[0] != '@') {  // Ignore fonts with @ as first character.
            ((PainterImpl*)param)->addFontName(win32::toMultiByte(faceName));
        }

        return 1;
    }

#else

    static int CALLBACK EnumFontFamExProc(ENUMLOGFONTEX *logFont, NEWTEXTMETRICEX *physFont, DWORD type, LPARAM param)
    {
        char* faceName = logFont->elfLogFont.lfFaceName;

        if (faceName[0] != '@') {  // Ignore fonts with @ as first character.
            ((PainterImpl*)param)->addFontName(faceName);
        }

        return 1;
    }

#endif


const std::list<std::string>& PainterImpl::fontFamilyNames()
{
    if (_fontNamesList.empty()) {

        #ifdef _WIN32_WCE
            EnumFonts(_surface->deviceContext(), 0, (FONTENUMPROC)&EnumFontsProc, (LPARAM)this);
        #else
            LOGFONT logFont;
            logFont.lfCharSet = DEFAULT_CHARSET;
            logFont.lfFaceName[0] = '\0';
            logFont.lfPitchAndFamily = 0;

            EnumFontFamiliesEx(_surface->deviceContext(), &logFont,    (FONTENUMPROC)&EnumFontFamExProc, (LPARAM)this,    0);
        #endif

        _fontNamesList.unique();
    }

    return _fontNamesList;
}


int PainterImpl::depth() const
{
    return GetDeviceCaps(_surface->deviceContext(), BITSPIXEL);
}


void PainterImpl::drawPixel(const Ui::PointF& toF)
{
  Ui::Point to = Application::instance().fromUnit(fromOrigin(toF));    
    
	SetPixel( _surface->deviceContext(), to.x(), to.y(), RGB(_pen.color().red() *255, _pen.color() .green()*255, _pen.color().blue() * 255) );
}


void PainterImpl::drawLine(const Ui::PointF& fromF, const  Ui::PointF& toF)
{
	if (_pen.size() == 0) 
		return; 

	Ui::Point from = Application::instance().fromUnit(fromOrigin(fromF));
	Ui::Point to = Application::instance().fromUnit(fromOrigin(toF));

  POINT points[2];
  points[0].x = from.x();
  points[0].y = from.y();
  points[1].x = to.x();
  points[1].y = to.y();

  Polyline(_surface->deviceContext(), points, 2);
}

void PainterImpl::drawText(const Ui::PointF& toF, const Pt::String& text)
{
	Ui::Point to = Application::instance().fromUnit(fromOrigin(toF));
  
  RECT rectangle;
  SetRect(&rectangle, to.x(), to.y(), to.x(), to.y());

  _text.clear();
	text.toUtf16( std::back_inserter(_text) );	
	
  int rezt = DrawTextW(_surface->deviceContext(), _text.c_str(), -1, &rectangle, DT_NOCLIP| DT_NOPREFIX );	
}


void PainterImpl::fillRect(const Ui::RectF& rectF)
{
	Ui::Rect rect = Application::instance().fromUnit(fromOrigin(rectF));

  RECT rectangle;
  const Ui::Point topLeft     = rect.topLeft();
  const Ui::Point bottomRight = rect.bottomRight();
  SetRect(&rectangle, topLeft.x(), topLeft.y(), bottomRight.x(), bottomRight.y());

  HBRUSH currentBrush = (HBRUSH)GetCurrentObject(_surface->deviceContext(), OBJ_BRUSH);
  FillRect(_surface->deviceContext(), &rectangle, currentBrush);
}

void PainterImpl::drawRect(const Ui::RectF& rectF)
{
	Ui::Rect rect = Application::instance().fromUnit(fromOrigin(rectF));

    if (rect.size().width() == 1 && rect.size().height() == 1) 
	{
        // Windows does not paint outline rectangles with a size of 1,1. For compatibility
        // to other windowing systems we draw a pixel (1|1) instead.
        drawPixel(rectF.topLeft());
        return;
    }

    HBRUSH originalBrush = (HBRUSH)SelectObject(_surface->deviceContext(), GetStockObject(NULL_BRUSH));

    const Ui::Point topLeft     = rect.topLeft();
    const Ui::Point bottomRight = rect.bottomRight();
    Rectangle(_surface->deviceContext(), topLeft.x(), topLeft.y(), bottomRight.x(), bottomRight.y());

    SelectObject(_surface->deviceContext(), originalBrush);
}


void PainterImpl::drawEllipse(const Ui::PointF& topLeftF, const Ui::SizeF& sizeF)
{
	Ui::Point topLeft = Application::instance().fromUnit(fromOrigin(topLeftF));
	Ui::Size size = Application::instance().fromUnit(sizeF);

  HBRUSH originalBrush = (HBRUSH)SelectObject(_surface->deviceContext(), GetStockObject(NULL_BRUSH));

  Ellipse(_surface->deviceContext(), topLeft.x(), topLeft.y(), topLeft.x() + size.width(), topLeft.y() + size.height() );

  SelectObject(_surface->deviceContext(), originalBrush);
}


void PainterImpl::fillEllipse(const Ui::PointF& topLeftF, const Ui::SizeF& sizeF)
{
	Ui::Point topLeft = Application::instance().fromUnit(fromOrigin(topLeftF));
	Ui::Size size = Application::instance().fromUnit(sizeF);

    // Temporarily select the empty pen to only draw the filling.
    HPEN originalPen = (HPEN)SelectObject(_surface->deviceContext(), GetStockObject(NULL_PEN));

    Ellipse(_surface->deviceContext(),
            topLeft.x(),
            topLeft.y(),
            topLeft.x() + size.width() + 1,
            topLeft.y() + size.height() + 1
    );

    // Select the original pen again.
    SelectObject(_surface->deviceContext(), originalPen);
}


void PainterImpl::drawPolyline(const Ui::PointF* points, const size_t pointCount)
{
    if (_pen.size() == 0)
       return;

    std::vector<POINT> winPoints(pointCount);

    for (size_t i = 0; i < pointCount; i++)
    {
		    Ui::Point p = Application::instance().fromUnit(fromOrigin(points[i]));
        winPoints[i].x = p.x();
        winPoints[i].y = p.y();
    }

    Polyline( _surface->deviceContext(), &(winPoints[0]), pointCount );
}


void PainterImpl::fillPolygon(const Ui::PointF* points, const size_t pointCount)
{
    HPEN originalPen = (HPEN)SelectObject(_surface->deviceContext(), GetStockObject(NULL_PEN));

    std::vector<POINT> winPoints(pointCount);

    for (size_t i = 0; i < pointCount; i++)
    {
		    Ui::Point p = Application::instance().fromUnit(fromOrigin(points[i]));
        winPoints[i].x = p.x();
        winPoints[i].y = p.y();
    }

    Polygon(_surface->deviceContext(), &(winPoints[0]), pointCount);

    SelectObject(_surface->deviceContext(), originalPen);
}


void PainterImpl::drawSurface(const Ui::PointF& toF, PaintSurface& surface, const  Ui::Region& pixmapRegion)
{

	Ui::Point to = Application::instance().fromUnit(fromOrigin(toF));

    // Copy contents from the source bitmap to the destination (=new) bitmap.
    BitBlt( _surface->deviceContext(), to.x(), to.y(), pixmapRegion.width(), pixmapRegion.height(),
            surface.impl()->deviceContext(), pixmapRegion.x(), pixmapRegion.y(), SRCCOPY );

}

void PainterImpl::drawSurface(const Ui::PointF& toF, PaintSurface& surface)
{

	Ui::Point to = Application::instance().fromUnit(fromOrigin(toF));
	Ui::Size size = Application::instance().fromUnit(surface.size());

    BitBlt( _surface->deviceContext(), to.x(), to.y(), size.width(), size.height(), surface.impl()->deviceContext(),  
	        0, 0, SRCCOPY);
}

void PainterImpl::drawImage(const Ui::PointF& toF, const Ui::Image& image)
{
	Ui::Point to = Application::instance().fromUnit(fromOrigin(toF));

	const size_t depth = image.format().pixelSize() * 8; 

	drawCompatibleImage(to.x(), to.y(), depth,  (const char*) image.pixel(0,0), image.width(), image.height() );
}

void PainterImpl::drawImage(const Ui::PointF& toF, const Ui::Image& image, const  Ui::Region& imageRegion)
{
	Ui::Point to = Application::instance().fromUnit(fromOrigin(toF));

	//Todo: impl region
	const size_t depth = image.format().pixelSize() * 8; 

	drawCompatibleImage(to.x(), to.y(), depth,  (const char*) image.pixel(0,0), image.width(), image.height() );
}


void PainterImpl::drawIndependentImage(size_t x, size_t y, const char* data, size_t width, size_t height)
{
    BITMAPINFO bitmapInfo;
    ZeroMemory(&bitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));

    bitmapInfo.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER); // Size of this struct.
    bitmapInfo.bmiHeader.biWidth       = width;             // Bitmap width.
    bitmapInfo.bmiHeader.biHeight      = -(ssize_t)height;  // Bitmap height. Negative value = top-down image.
    bitmapInfo.bmiHeader.biPlanes      = 1;                 // Always 1.			
    bitmapInfo.bmiHeader.biBitCount    = 32;                // We internally use a 32-bit bitmap.
    bitmapInfo.bmiHeader.biCompression = BI_RGB;            // Uncompressed (top-down) RGB bitmap.
    bitmapInfo.bmiHeader.biSizeImage   = 0;                 // 0 = automatic for BI_RGB-images.
    bitmapInfo.bmiHeader.biClrUsed     = 0;                 // 0 = No color table.
    bitmapInfo.bmiHeader.biClrImportant= 0;                 // 0 = No color table.

    VOID* imageBits;
    HBITMAP bitmap = CreateDIBSection(_surface->deviceContext(), &bitmapInfo, DIB_RGB_COLORS, &imageBits, NULL, 0);

    memcpy(imageBits, data, width * height * 4);

    HDC bitmapDeviceConText = CreateCompatibleDC(NULL);
    SelectObject(bitmapDeviceConText, bitmap);

    BitBlt(_surface->deviceContext(), x, y, width, height, bitmapDeviceConText, 0, 0, SRCCOPY);

    DeleteDC(bitmapDeviceConText);
    DeleteObject(bitmap);
}


void PainterImpl::drawCompatibleImage(size_t x, size_t y, size_t depth, const char* data, size_t width, size_t height)
{
    HBITMAP bitmap = CreateBitmap(width, height, 1, depth, (VOID*)data);

    if (bitmap == NULL) 
	  {
        drawIndependentImage(x, y, data, width, height);
        return;
    }

    HDC bitmapDeviceConText = CreateCompatibleDC(NULL );
    SelectObject(bitmapDeviceConText, bitmap);

    BitBlt(_surface->deviceContext(), x, y, width, height, bitmapDeviceConText, 0, 0, SRCCOPY);

    DeleteDC(bitmapDeviceConText);
    DeleteObject(bitmap);
}

Ui::PointF PainterImpl::fromOrigin(const Ui::PointF& p)
{
  return Ui::PointF(_surface->originPos().x() + p.x(), _surface->originPos().y() + p.y());
}

Ui::RectF PainterImpl::fromOrigin(const Ui::RectF& p)
{
  return Ui::RectF( Ui::PointF(_surface->originPos().x() + p.left(), _surface->originPos().y() + p.top()), p.size() );
}

void PainterImpl::setSurface(PaintSurface& surface)
{
	_surface = surface.impl();
	updateBrush();
	updateFont();
	updatePen();	
}

}}

