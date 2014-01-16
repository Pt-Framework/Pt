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
#include "Pt/Gui/Pixmap.h"
#include "Pt/Gfx/Rect.h"
#include "Pt/Gfx/Region.h"
#include "Pt/Gfx/FontMetrics.h"
#include "Pt/Gfx/Rgb888Color.h"
#include <Pt/Hmi/PaintSurface.h>
#include <Pt/Hmi/Application.h>
#include <iostream>
#include <algorithm>

namespace Pt {
namespace Hmi {

PainterImpl::PainterImpl(PaintSurfaceImpl* surface)
: _surface(surface)
, _pen(Gfx::Pen(1))
, _brush(Gfx::Brush(Gfx::ARgbColor(0, 0, 0)))
, _font(Gfx::Font(determinePlatformDefaultFontName()))
{
}


PainterImpl::~PainterImpl()
{
}


void PainterImpl::ensureActivePainter() const
{
    if ( !_surface->isPainting() ) {
        throw std::logic_error("Painter is not currently active. Use painter() to activate painter." + PT_SOURCEINFO);
    }
}

void PainterImpl::drawText( const Gfx::PointF& to, const Pt::String& text, const Gfx::ARgbColor* outline )
{
    drawText(to, text);
}

void PainterImpl::setPen(const Gfx::Pen& pen)
{
    if (pen == _pen) {
        return;
    }

    _pen = pen;
    updatePen();
}


DWORD PainterImpl::toGdiPenStyle( const Pt::Gfx::Pen& pen )
{
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
        case Pt::Gfx::Pen::RoundCap:
            penStyle |= PS_ENDCAP_ROUND;
        break;
        case Pt::Gfx::Pen::FlatCap:
            penStyle |= PS_ENDCAP_FLAT;
        break;
    }

    switch( pen.joinStyle() )
    {
        case Pt::Gfx::Pen::RoundJoin:
             penStyle |= PS_JOIN_ROUND;
        break;
        case Pt::Gfx::Pen::BevelJoin:
             penStyle |= PS_JOIN_BEVEL;
        break;
    }
#endif

    return penStyle;
}


void PainterImpl::updatePen()
{
    if (!_surface->isPainting()) {
        return;
    }

    Gfx::Rgb888Color penCol;
    assign( penCol, _pen.color() );

    DWORD penStyle = toGdiPenStyle( _pen );

#ifdef _WIN32_WCE
    HPEN newPen = CreatePen( penStyle, _pen.size(), RGB(penCol.red(), penCol.green(), penCol.blue()) );
#else
    LOGBRUSH brush;
    brush.lbStyle = BS_SOLID ;
    brush.lbColor = RGB(penCol.red(), penCol.green(), penCol.blue());

    HPEN newPen = ExtCreatePen( penStyle , _pen.size(), &brush, 0, NULL );
#endif

    HPEN oldPen = (HPEN)SelectObject(_surface->deviceContext(), newPen);

    DeleteObject(oldPen);

    // Set the Text color to the pen color.
    SetTextColor(_surface->deviceContext(), RGB(penCol.red(), penCol.green(), penCol.blue()));
}


const Gfx::Pen& PainterImpl::pen() const
{
    return _pen;
}


void PainterImpl::setBrush(const Gfx::Brush& brush)
{
    _brush = brush;
    updateBrush();
}


void PainterImpl::updateBrush()
{
    if (!_surface->isPainting()) {
        return;
    }

    HBRUSH newBrushHandle;

    switch (_brush.fillStyle()) {

        case Gfx::Brush::SolidFill: {
            Gfx::Rgb888Color col;
            assign( col, _brush.color() );
            newBrushHandle = CreateSolidBrush(RGB(col.red(), col.green(), col.blue()));
            break;
        }

        case Gfx::Brush::TextureFill: {
            const Gfx::ARgbImage& texture = _brush.texture();

            if (!texture.empty())
            {
                // Convert our generic format to a 32 bit image format which Windows can understand.
                Gfx::Rgb888Image rgb32Image(_brush.texture().width(), _brush.texture().height());
                assign(_brush.texture().begin(), _brush.texture().end(), rgb32Image.begin());

                // Fill the info for a device-independent bitmap to hold the texture data in the Windows system.
                BITMAPINFO bitmapInfo;
                ZeroMemory(&bitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));

                bitmapInfo.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);    // Size of this struct.
                bitmapInfo.bmiHeader.biWidth       = texture.width();             // Bitmap width.
                bitmapInfo.bmiHeader.biHeight      = -(ssize_t)texture.height();  // Bitmap height. Top-down image.
                bitmapInfo.bmiHeader.biPlanes      = 1;                           // Always 1.
                bitmapInfo.bmiHeader.biBitCount    = 32;                          // We internally use a 32-bit bitmap.
                bitmapInfo.bmiHeader.biCompression = BI_RGB;                      // Uncompressed (top-down) RGB bitmap.
                bitmapInfo.bmiHeader.biSizeImage   = 0;                           // 0 = automatic for BI_RGB-images.
                bitmapInfo.bmiHeader.biClrUsed     = 0;                           // 0 = No color table.
                bitmapInfo.bmiHeader.biClrImportant= 0;                           // 0 = No color table.

                // Create the device-independent bitmap that will be filled with the texture
                // and used as brush.
                VOID* imageBits;
                HBITMAP bitmap = CreateDIBSection(_surface->deviceContext(), &bitmapInfo, DIB_RGB_COLORS, &imageBits, NULL, 0);

                // Copy image data from the texture to the Windows bitmap.
                memcpy(imageBits, rgb32Image.data(), texture.width() * texture.height() * 4);

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

            break;
        }

        default:
            // TODO Throw runtime exception in case we do not check for every possible value of Brush::FillStyle?
            return;
    }

    HBRUSH oldBrushHandle = (HBRUSH)SelectObject(_surface->deviceContext(), newBrushHandle);

    DeleteObject(oldBrushHandle);
}


const Gfx::Brush& PainterImpl::brush() const
{
    return _brush;
}

void PainterImpl::setFont(const Gfx::Font& font)
{
    if (font == _font) {
        return;
    }

    _font = font;
    updateFont();
}


void PainterImpl::updateFont()
{
    if (!_surface->isPainting()) {
        return;
    }

    // General font settings, independent of given font settings.
    SetTextAlign(_surface->deviceContext(), TA_BASELINE | TA_LEFT | TA_NOUPDATECP);

    int fontWeight;
    switch (_font.fontStyle()) {
        case Gfx::Font::NormalStyle:
        case Gfx::Font::ItalicStyle:
            fontWeight = FW_NORMAL;
            break;

        case Gfx::Font::BoldStyle:
        case Gfx::Font::BoldItalicStyle:
            fontWeight = FW_BOLD;
            break;
    }

    BYTE italic = (_font.fontStyle() == Gfx::Font::ItalicStyle || _font.fontStyle() == Gfx::Font::BoldItalicStyle);

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
}



std::string PainterImpl::determinePlatformDefaultFontName()
{
    HFONT defaultFont = (HFONT)GetStockObject(SYSTEM_FONT);
    SelectObject(_surface->deviceContext(), defaultFont);

    std::vector<TCHAR> buffer(32);
    GetTextFace(_surface->deviceContext(), buffer.size(), &buffer[0]);

    return win32::toMultiByte(&buffer[0]);
}


const Gfx::Font& PainterImpl::font() const
{
    return _font;
}


Gfx::FontMetrics PainterImpl::fontMetrics() const
{
    ensureActivePainter();

    TEXTMETRIC metrics;
    GetTextMetrics(_surface->deviceContext(), &metrics);

    return Gfx::FontMetrics(metrics.tmAscent, metrics.tmDescent, 0, metrics.tmHeight);
}


Gfx::FontMetrics PainterImpl::fontMetrics(Pt::String text) const
{
    ensureActivePainter();

    // Basic font metrics without the Text-specific width.
    SIZE textSize;
    TEXTMETRIC basicMetrics;
    GetTextMetrics(_surface->deviceContext(), &basicMetrics);

    // Convert the 32-bit string into 16 bit (using UTF-16).
    //_textStream.clear();
    //_stringStream.str("");
    //_textStream << text;
    //_textStream.flush();

    // Calculate the width and height for the text.
    //std::string utf16Text = _stringStream.str();

    // Every UTF16 character uses 2 bytes, so divide by 2 to get the length of the encoded text.
    //size_t utf16Length = utf16Text.length() / 2;
    //GetTextExtentPoint32W(_surface->deviceContext(), (wchar_t*)utf16Text.c_str(), utf16Length, &textSize);

    _text.clear();
	text.toUtf16( std::back_inserter(_text) );
    GetTextExtentPoint32W(_surface->deviceContext(), _text.c_str(), _text.length(), &textSize);
	Pt::Gfx::Size size(textSize.cx, textSize.cy);
	Pt::Gfx::SizeF sizeF = Application::instance().toUnit(size);

    return Gfx::FontMetrics(basicMetrics.tmAscent, basicMetrics.tmDescent, (int)sizeF.width(), (int)sizeF.height());
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
    ensureActivePainter();

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
    ensureActivePainter();

    return GetDeviceCaps(_surface->deviceContext(), BITSPIXEL);
}


void PainterImpl::drawPixel(const Pt::Gfx::PointF& toF)
{
    ensureActivePainter();
	Pt::Gfx::Point to = Application::instance().fromUnit(toF);

    Gfx::Rgb888Color col;
    assign( col, _pen.color() );
    SetPixel( _surface->deviceContext(),
              to.x(),
              to.y(),
              RGB(col.red(), col.green(), col.blue())
    );
}


void PainterImpl::drawLine(const Pt::Gfx::PointF& fromF, const  Pt::Gfx::PointF& toF)
{
    ensureActivePainter();

	Pt::Gfx::Point from = Application::instance().fromUnit(fromF);
	Pt::Gfx::Point to = Application::instance().fromUnit(toF);

    if (_pen.size() == 0) {
        return; // Draw nothing if the pen size is 0.
    }

    POINT points[2];
    points[0].x = from.x();
    points[0].y = from.y();
    points[1].x = to.x();
    points[1].y = to.y();

    Polyline(_surface->deviceContext(), points, 2);
}


void PainterImpl::drawText(const Pt::Gfx::PointF& toF, const Pt::String& text)
{
    ensureActivePainter();

	Pt::Gfx::Point to = Application::instance().fromUnit(toF);

    //_textStream.clear();
    //_stringStream.str("");
    //_textStream << text << Char(0); // Append extra \0 for proper line termination.
    //_textStream.flush();

    RECT rectangle;
    SetRect(&rectangle, to.x(), to.y(), to.x(), to.y());

    // Every UTF16 character uses 2 bytes, so divide by 2 to get the length of the encoded text.
    //DrawTextW(_surface->deviceContext(), (wchar_t*)_stringStream.str().c_str(), -1, &rectangle, DT_NOCLIP);

    _text.clear();
	text.toUtf16( std::back_inserter(_text) );	
	
    DrawTextW(_surface->deviceContext(), _text.c_str(), -1, &rectangle, DT_NOCLIP);
	
}


void PainterImpl::fillRect(const Pt::Gfx::RectF& rectF)
{
    ensureActivePainter();

	Pt::Gfx::Rect rect = Application::instance().fromUnit(rectF);

    RECT rectangle;
    const Pt::Gfx::Point topLeft     = rect.topLeft();
    const Pt::Gfx::Point bottomRight = rect.bottomRight();
    SetRect(&rectangle, topLeft.x(), topLeft.y(), bottomRight.x(), bottomRight.y());

    HBRUSH currentBrush = (HBRUSH)GetCurrentObject(_surface->deviceContext(), OBJ_BRUSH);
    FillRect(_surface->deviceContext(), &rectangle, currentBrush);
}


void PainterImpl::drawRect(const Pt::Gfx::RectF& rectF)
{
    ensureActivePainter();

	Pt::Gfx::Rect rect = Application::instance().fromUnit(rectF);

    if (rect.size().width() == 1 && rect.size().height() == 1) {
        // Windows does not paint outline rectangles with a size of 1,1. For compatibility
        // to other windowing systems we draw a pixel (1|1) instead.
        drawPixel(rectF.topLeft());
        return;
    }

    // Temporarily select the empty brush to only draw the outline.
    HBRUSH originalBrush = (HBRUSH)SelectObject(_surface->deviceContext(), GetStockObject(NULL_BRUSH));

    const Pt::Gfx::Point topLeft     = rect.topLeft();
    const Pt::Gfx::Point bottomRight = rect.bottomRight();
    Rectangle(_surface->deviceContext(), topLeft.x(), topLeft.y(), bottomRight.x(), bottomRight.y());

    // Select the original brush again.
    SelectObject(_surface->deviceContext(), originalBrush);
}


void PainterImpl::drawEllipse(const Pt::Gfx::PointF& topLeftF, const Pt::Gfx::SizeF& sizeF)
{
    ensureActivePainter();

	Pt::Gfx::Point topLeft = Application::instance().fromUnit(topLeftF);
	Pt::Gfx::Size size = Application::instance().fromUnit(sizeF);

    // Temporarily select the empty brush to only draw the outline.
    HBRUSH originalBrush = (HBRUSH)SelectObject(_surface->deviceContext(), GetStockObject(NULL_BRUSH));

    Ellipse(_surface->deviceContext(),
            topLeft.x(),
            topLeft.y(),
            topLeft.x() + size.width(),
            topLeft.y() + size.height()
    );

    // Select the original brush again.
    SelectObject(_surface->deviceContext(), originalBrush);
}


void PainterImpl::fillEllipse(const Pt::Gfx::PointF& topLeftF, const Pt::Gfx::SizeF& sizeF)
{
    ensureActivePainter();

	Pt::Gfx::Point topLeft = Application::instance().fromUnit(topLeftF);
	Pt::Gfx::Size size = Application::instance().fromUnit(sizeF);

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


void PainterImpl::drawPolyline(const Pt::Gfx::PointF* points, const size_t pointCount)
{
    if (_pen.size() == 0)
       return;

    ensureActivePainter();

    std::vector<POINT> winPoints(pointCount);

    for (size_t i = 0; i < pointCount; i++)
    {
		Pt::Gfx::Point p = Application::instance().fromUnit(points[i]);
        winPoints[i].x = p.x();
        winPoints[i].y = p.y();
    }

    Polyline( _surface->deviceContext(), &(winPoints[0]), pointCount );
/*
    ensureActivePainter();

    if (_pen.size() == 0) {
        return;
    }

    HDC hdc = _surface->deviceContext();

    MoveToEx(hdc, points[0].x(), points[0].y(), 0);

    for( size_t i = 1; i <  pointCount; i++) {
        LineTo(hdc, points[i].x(), points[i].y());
    }*/
}


void PainterImpl::fillPolygon(const Pt::Gfx::PointF* points, const size_t pointCount)
{
    ensureActivePainter();

    // Temporarily select the empty pen to only draw the filling.
    HPEN originalPen = (HPEN)SelectObject(_surface->deviceContext(), GetStockObject(NULL_PEN));


    std::vector<POINT> winPoints(pointCount);

     for (size_t i = 0; i < pointCount; i++)
    {
		Pt::Gfx::Point p = Application::instance().fromUnit(points[i]);
        winPoints[i].x = p.x();
        winPoints[i].y = p.y();
    }

    Polygon(_surface->deviceContext(), &(winPoints[0]), pointCount);


       // Select the original pen again.
    SelectObject(_surface->deviceContext(), originalPen);
}


void PainterImpl::drawSurface(const Pt::Gfx::PointF& toF, PaintSurface& surface, const  Pt::Gfx::Region& pixmapRegion)
{
    ensureActivePainter();

	Pt::Gfx::Point to = Application::instance().fromUnit(toF);

    // Copy contents from the source bitmap to the destination (=new) bitmap.
    BitBlt(
        _surface->deviceContext(),
        to.x(),    to.y(),
        pixmapRegion.width(), pixmapRegion.height(),
        surface.impl()->deviceContext(),
        pixmapRegion.x(), pixmapRegion.y(),
        SRCCOPY
    );

}

void PainterImpl::drawSurface(const Pt::Gfx::PointF& toF, PaintSurface& surface)
{
    ensureActivePainter();
	Pt::Gfx::Point to = Application::instance().fromUnit(toF);

    // Copy contents from the source bitmap to the destination (=new) bitmap.
    BitBlt(
        _surface->deviceContext(),
        to.x(),    to.y(),
        surface.size().width(), surface.size().height(),
        surface.impl()->deviceContext(),
        0, 0,
        SRCCOPY
    );
}

void PainterImpl::drawImage(const Pt::Gfx::PointF& toF, const Gfx::ARgbImage& image)
{
    ensureActivePainter();

	Pt::Gfx::Point to = Application::instance().fromUnit(toF);

    this->drawImage( to.x(), to.y(), image.begin(), image.end(), image.width(), image.height() );
}


void PainterImpl::drawImage(const Pt::Gfx::PointF& toF, const Gfx::ARgbImage& image, const  Pt::Gfx::Region& imageRegion)
{
    ensureActivePainter();
		Pt::Gfx::Point to = Application::instance().fromUnit(toF);

    Gfx::ARgbSubImage subImage(const_cast<Gfx::ARgbImage&>( image ), imageRegion);
    this->drawImage( to.x(), to.y(), subImage.begin(), subImage.end(), subImage.width(), subImage.height() );
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

    if (bitmap == NULL) {
        drawIndependentImage(x, y, data, width, height);
        return;
    }

    HDC bitmapDeviceConText = CreateCompatibleDC(NULL);
    SelectObject(bitmapDeviceConText, bitmap);

    BitBlt(_surface->deviceContext(), x, y, width, height, bitmapDeviceConText, 0, 0, SRCCOPY);

    DeleteDC(bitmapDeviceConText);
    DeleteObject(bitmap);
}

}}

