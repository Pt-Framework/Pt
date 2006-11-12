/***************************************************************************
 *   Copyright (C) 2006 Marc Boris Dürner                                  *
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

#include "Pt/Types.h"
#include "Pt/Gui/Pixmap.h"
#include "Pt/Gfx/Rect.h"
#include "Pt/Gfx/FontMetrics.h"
#include "Pt/Gfx/XRgb8888Color.h"
#include "win32.h"

#include <iostream>
#include <algorithm>

#include <windows.h>


namespace Pt {

namespace Gui {


PainterImpl::PainterImpl(Drawable& drawable)
: _drawable(drawable),
  _pen(Gfx::Pen(1)),
  _brush(Gfx::Brush(Gfx::ARgbColor(0, 0, 0))),
  _font(Gfx::Font(determinePlatformDefaultFontName()))
{
}


PainterImpl::~PainterImpl()
{
}


void PainterImpl::begin()
{
	_drawable.beginPaint();
}


void PainterImpl::end()
{
	_drawable.endPaint();
}


void PainterImpl::setPen(const Gfx::Pen& pen)
{
	if (pen == _pen) {
		return;
	}

	_pen = pen;
	updatePen();
}


void PainterImpl::updatePen()
{
	if (!_drawable.isPainting()) {
		return;
	}

	Gfx::XRgb8888Color penCol = _pen.color();

#ifdef _WIN32_WCE
	HPEN newPen = CreatePen(PS_SOLID, _pen.size(), RGB(penCol.red(), penCol.green(), penCol.blue()));
#else
	LOGBRUSH brush;
	brush.lbStyle = BS_SOLID;
	brush.lbColor = RGB(penCol.red(), penCol.green(), penCol.blue());

	HPEN newPen = ExtCreatePen(PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_FLAT | PS_JOIN_BEVEL, _pen.size(), &brush, 0, NULL);
#endif

	HPEN oldPen = (HPEN)SelectObject(_drawable.deviceContext(), newPen);

	DeleteObject(oldPen);


	// Set the text color to the pen color.
	SetTextColor(_drawable.deviceContext(), RGB(penCol.red(), penCol.green(), penCol.blue()));
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
	if (!_drawable.isPainting()) {
		return;
	}

	HBRUSH newBrushHandle;

	switch (_brush.fillStyle()) {

		case Gfx::Brush::SolidFill: {
			Gfx::XRgb8888Color col = _brush.color();
			newBrushHandle = CreateSolidBrush(RGB(col.red(), col.green(), col.blue()));
			break;
		}

		case Gfx::Brush::TextureFill: {
			const Gfx::ARgbImage& texture = _brush.texture();

			// Convert our generic format to a 32 bit image format which Windows can understand.
			Gfx::XRgb8888Image rgb32Image(_brush.texture().width(), _brush.texture().height());
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
			HBITMAP bitmap = CreateDIBSection(_drawable.deviceContext(), &bitmapInfo, DIB_RGB_COLORS, &imageBits, NULL, 0);

			// Copy image data from the texture to the Windows bitmap.
			memcpy(imageBits, rgb32Image.data(), texture.width() * texture.height() * 4);

			// Create the actual brush from this bitmap.
			newBrushHandle = CreatePatternBrush(bitmap);

			// Free the bitmap again.
			DeleteObject(bitmap);

			break;
		}

		default:
			// TODO Throw runtime exception in case we do not check for every possible value of Brush::FillStyle?
			return;
	}

	HBRUSH oldBrushHandle = (HBRUSH)SelectObject(_drawable.deviceContext(), newBrushHandle);

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
	if (!_drawable.isPainting()) {
		return;
	}

	// General font settings, independent of given font settings.
	SetTextAlign(_drawable.deviceContext(), TA_BASELINE | TA_LEFT | TA_NOUPDATECP);

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

	HFONT oldFont = (HFONT)SelectObject(_drawable.deviceContext(), newFont);

	DeleteObject(oldFont);
}



std::string PainterImpl::determinePlatformDefaultFontName()
{
	HFONT defaultFont = (HFONT)GetStockObject(SYSTEM_FONT);
	SelectObject(_drawable.deviceContext(), defaultFont);

	std::vector<TCHAR> buffer(32);
	GetTextFace(_drawable.deviceContext(), buffer.size(), &buffer[0]);

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
	GetTextMetrics(_drawable.deviceContext(), &metrics);

	return Gfx::FontMetrics(metrics.tmAscent, metrics.tmDescent, 0, metrics.tmHeight);
}


Gfx::FontMetrics PainterImpl::fontMetrics(std::string text) const
{
	ensureActivePainter();

	// Basic font metrics without the text-specific width.
	TEXTMETRIC basicMetrics;
	GetTextMetrics(_drawable.deviceContext(), &basicMetrics);

	SIZE textSize;
	GetTextExtentPoint32(_drawable.deviceContext(), win32::fromMultiByte(text).c_str(), text.length(), &textSize);

	return Gfx::FontMetrics(basicMetrics.tmAscent, basicMetrics.tmDescent, textSize.cx, textSize.cy);
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
			EnumFonts(_drawable.deviceContext(), 0, (FONTENUMPROC)&EnumFontsProc, (LPARAM)this);
		#else
			LOGFONT logFont;
			logFont.lfCharSet = DEFAULT_CHARSET;
			logFont.lfFaceName[0] = '\0';
			logFont.lfPitchAndFamily = 0;

			EnumFontFamiliesEx(_drawable.deviceContext(), &logFont,	(FONTENUMPROC)&EnumFontFamExProc, (LPARAM)this,	0);
		#endif

		_fontNamesList.unique();
	}

	return _fontNamesList;
}


int PainterImpl::depth() const
{
	ensureActivePainter();

	return GetDeviceCaps(_drawable.deviceContext(), BITSPIXEL);
}


void PainterImpl::drawPixel(const Gfx::Point& to)
{
	ensureActivePainter();

	Gfx::XRgb8888Color col = _pen.color();
	SetPixel(
		_drawable.deviceContext(),
		to.x(),
		to.y(),
		RGB(col.red(), col.green(), col.blue())
	);
}


void PainterImpl::drawLine(const Gfx::Point& from, const Gfx::Point& to)
{
	ensureActivePainter();

	if (_pen.size() == 0) {
		return; // Draw nothing if the pen size is 0.
	}

	POINT points[2];
	points[0].x = from.x();
	points[0].y = from.y();
	points[1].x = to.x();
	points[1].y = to.y();

	Polyline(_drawable.deviceContext(), points, 2);
}


void PainterImpl::drawText(const Gfx::Point& to, const std::string& text)
{
	ensureActivePainter();

	RECT rectangle;
	SetRect(&rectangle, to.x(), to.y(), to.x(), to.y());
	DrawText(_drawable.deviceContext(), win32::fromMultiByte(text).c_str(), -1, &rectangle, DT_NOCLIP);
}


void PainterImpl::fillRect(const Gfx::Rect& rect)
{
	ensureActivePainter();

	RECT rectangle;
	SetRect(&rectangle, rect.x(), rect.y(), rect.x() + rect.width(), rect.y() + rect.height());

	HBRUSH currentBrush = (HBRUSH)GetCurrentObject(_drawable.deviceContext(), OBJ_BRUSH);
	FillRect(_drawable.deviceContext(), &rectangle, currentBrush);
}


void PainterImpl::drawRect(const Gfx::Rect& rect)
{
	ensureActivePainter();

	if (rect.size().width() == 1 && rect.size().height() == 1) {
		// Windows does not paint outline rectangles with a size of 1,1. For compatibility
		// to other windowing systems we draw a pixel (1|1) instead.
		drawPixel(rect.topLeft());
		return;
	}

	// Temporarily select the empty brush to only draw the outline.
	HBRUSH originalBrush = (HBRUSH)SelectObject(_drawable.deviceContext(), GetStockObject(NULL_BRUSH));
	Rectangle(_drawable.deviceContext(), rect.x(), rect.y(), rect.x() + rect.width(), rect.y() + rect.height());

	// Select the original brush again.
	SelectObject(_drawable.deviceContext(), originalBrush);
}


void PainterImpl::drawEllipse(const Gfx::Point& topLeft, const Gfx::Size& size)
{
	ensureActivePainter();

	// Temporarily select the empty brush to only draw the outline.
	HBRUSH originalBrush = (HBRUSH)SelectObject(_drawable.deviceContext(), GetStockObject(NULL_BRUSH));

	Ellipse(_drawable.deviceContext(),
	        topLeft.x(),
	        topLeft.y(),
	        topLeft.x() + size.width(),
	        topLeft.y() + size.height()
	);

	// Select the original brush again.
	SelectObject(_drawable.deviceContext(), originalBrush);
}


void PainterImpl::fillEllipse(const Gfx::Point& topLeft, const Gfx::Size& size)
{
	ensureActivePainter();

	// Temporarily select the empty pen to only draw the filling.
	HPEN originalPen = (HPEN)SelectObject(_drawable.deviceContext(), GetStockObject(NULL_PEN));

	Ellipse(_drawable.deviceContext(),
	        topLeft.x(),
	        topLeft.y(),
	        topLeft.x() + size.width() + 1,
	        topLeft.y() + size.height() + 1
	);

	// Select the original pen again.
	SelectObject(_drawable.deviceContext(), originalPen);
}


void PainterImpl::drawPolyline(const Gfx::Point* points, const size_t pointCount) const
{
	ensureActivePainter();

	if (_pen.size() == 0) {
		return; // Draw nothing if the pen size is 0.
	}

	std::vector<POINT> winPoints(pointCount);

	for (size_t i = 0; i < pointCount; i++) {
		winPoints[i].x = points[i].x();
		winPoints[i].y = points[i].y();
	}

    Polyline(_drawable.deviceContext(), &(winPoints[0]), pointCount);
}


void PainterImpl::fillPolygon(const Pt::Gfx::Point* points, const size_t pointCount) const
{
	ensureActivePainter();

	// Temporarily select the empty pen to only draw the filling.
	HPEN originalPen = (HPEN)SelectObject(_drawable.deviceContext(), GetStockObject(NULL_PEN));


	std::vector<POINT> winPoints(pointCount);

	for (size_t i = 0; i < pointCount; i++) {
		winPoints[i].x = points[i].x();
		winPoints[i].y = points[i].y();
	}

    Polygon(_drawable.deviceContext(), &(winPoints[0]), pointCount);


   	// Select the original pen again.
	SelectObject(_drawable.deviceContext(), originalPen);
}


void PainterImpl::drawPixmap(const Gfx::Point& to, Pixmap& pixmap, const Gfx::Rect& pixmapRect)
{
	ensureActivePainter();

	pixmap.impl().beginPaint();

	// Copy contents from the source bitmap to the destination (=new) bitmap.
	BitBlt(
		_drawable.deviceContext(),
		to.x(),	to.y(),
		pixmapRect.width(), pixmapRect.height(),
		pixmap.impl().deviceContext(),
		pixmapRect.x(), pixmapRect.y(),
		SRCCOPY
	);

	pixmap.impl().endPaint();
}

void PainterImpl::drawPixmap(const Gfx::Point& to, Pixmap& pixmap)
{
	ensureActivePainter();

	pixmap.impl().beginPaint();

	// Copy contents from the source bitmap to the destination (=new) bitmap.
	BitBlt(
		_drawable.deviceContext(),
		to.x(),	to.y(),
		pixmap.size().width(), pixmap.size().height(),
		pixmap.impl().deviceContext(),
		0, 0,
		SRCCOPY
	);

	pixmap.impl().endPaint();
}


void PainterImpl::drawImage(const Gfx::Point& to, const Gfx::ARgbImage& image)
{
	ensureActivePainter();

	this->drawImage( to.x(), to.y(), image.begin(), image.end(), image.width(), image.height() );
}


void PainterImpl::drawImage(const Gfx::Point& to, const Gfx::ARgbImage& image, const Gfx::Rect& imageRect)
{
	ensureActivePainter();

	Gfx::ARgbSubImage subImage(const_cast<Gfx::ARgbImage&>( image ), imageRect);
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
	HBITMAP bitmap = CreateDIBSection(_drawable.deviceContext(), &bitmapInfo, DIB_RGB_COLORS, &imageBits, NULL, 0);

	memcpy(imageBits, data, width * height * 4);

	HDC bitmapDeviceContext = CreateCompatibleDC(NULL);
	SelectObject(bitmapDeviceContext, bitmap);

	BitBlt(_drawable.deviceContext(), x, y, width, height, bitmapDeviceContext, 0, 0, SRCCOPY);

	DeleteDC(bitmapDeviceContext);
	DeleteObject(bitmap);
}


void PainterImpl::drawCompatibleImage(size_t x, size_t y, size_t depth, const char* data, size_t width, size_t height)
{
	HBITMAP bitmap = CreateBitmap(width, height, 1, depth, (VOID*)data);

	if (bitmap == NULL) {
		drawIndependentImage(x, y, data, width, height);
		return;
	}

	HDC bitmapDeviceContext = CreateCompatibleDC(NULL);
	SelectObject(bitmapDeviceContext, bitmap);

	BitBlt(_drawable.deviceContext(), x, y, width, height, bitmapDeviceContext, 0, 0, SRCCOPY);

	DeleteDC(bitmapDeviceContext);
	DeleteObject(bitmap);
}


} // namespace Gui

} // namespace Pt

