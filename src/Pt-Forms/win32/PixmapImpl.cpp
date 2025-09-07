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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#include "win32.h"
#include "PixmapImpl.h"
#include "PaintContext.h"

#include <Pt/Gfx/Painter.h>
#include <Pt/Gfx/Image.h>

namespace {

#ifdef PT_FORMS_WIN32_RASTER

#else // PT_FORMS_WIN32_RASTER

#ifdef _WIN32_WCE

static int CALLBACK EnumFontsProc(LOGFONT* logFont, TEXTMETRIC* physFont, DWORD type, LPARAM param)
{
    WCHAR* faceName = logFont->lfFaceName;

    // Ignore fonts with @ as first character.
    if (faceName[0] != '@')
    {
        std::string name = win32::toMultiByte(faceName);
        reinterpret_cast<std::vector<std::string>*>(param)->push_back(name);
    }

    return 1;
}

#else

static int CALLBACK EnumFontFamExProc(ENUMLOGFONTEX* logFont, NEWTEXTMETRICEX* physFont, DWORD type, LPARAM param)
{
    char* faceName = logFont->elfLogFont.lfFaceName;

    // Ignore fonts with @ as first character.
    if (faceName[0] != '@')
    {
        reinterpret_cast<std::vector<std::string>*>(param)->push_back(faceName);
    }

    return 1;
}

#endif // _WIN32_WCE

#endif // PT_FORMS_WIN32_RASTER

void toPreMulAlpha(const Pt::Gfx::Image& image, 
                   std::vector<Pt::uint8_t>& bitmapData)
{
    size_t _width = image.width();
    size_t _height = image.height();

    for (std::size_t y = 0; y < image.height(); ++y)
    {
        for (std::size_t x = 0; x < image.width(); ++x)
        {
            Pt::Gfx::ConstPixel pixel(image.view(), x, y);
            Pt::Gfx::Color color = pixel.getColor();

            const Pt::uint8_t r = color.red() / 257;
            const Pt::uint8_t g = color.green() / 257;
            const Pt::uint8_t b = color.blue() / 257;
            const Pt::uint8_t a = color.alpha() / 257;

            bitmapData.push_back((Pt::uint8_t) (a * b / 255));
            bitmapData.push_back((Pt::uint8_t) (a * g / 255));
            bitmapData.push_back((Pt::uint8_t) (a * r / 255));
            bitmapData.push_back((Pt::uint8_t) (a));
        }
    }
}

} // nasmespace

namespace Pt {

namespace Forms {

#ifdef PT_FORMS_WIN32_RASTER

#else // PT_FORMS_WIN32_RASTER

PixmapImpl::PixmapImpl()
: _physicalSize(0, 0)
, _width(0)
, _height(0)
, _dc(0)
, _paintContext(0)
{
    Gfx::SizeF size = Gfx::SizeF(10, 10);

    HDC screenDC = GetDC(NULL);
    _dc = CreateCompatibleDC(screenDC);
    _bitmap = CreateCompatibleBitmap(screenDC, lround(size.width()), 
                                               lround(size.height()));
    ReleaseDC(NULL, screenDC);

    _oldPen    = (HPEN) GetCurrentObject(_dc, OBJ_PEN);
    _oldBrush  = (HBRUSH) GetCurrentObject(_dc, OBJ_BRUSH);
    _oldFont   = (HFONT) GetCurrentObject(_dc, OBJ_FONT);
    _oldBitmap = (HBITMAP) GetCurrentObject(_dc, OBJ_BITMAP);
    
    SelectObject(_dc, _bitmap);
    SetBkMode(_dc, TRANSPARENT);

    SetGraphicsMode(_dc, GM_ADVANCED);
}


PixmapImpl::~PixmapImpl()
{
    SelectObject(_dc, _oldBitmap);

    DeleteDC(_dc);
    DeleteObject(_bitmap);
}


void PixmapImpl::set(const Gfx::Image& image)
{
    size_t width = image.width();
    size_t height = image.height();

    Gfx::SizeF size(width, height);
    resize(size);

    std::vector<Pt::uint8_t> bitmapData;
    toPreMulAlpha(image, bitmapData);

    const Pt::uint8_t* data = bitmapData.empty() ? 0 : &bitmapData[0];

    const size_t depth = 32;

    HBITMAP bitmap = CreateBitmap(image.width(), image.height(), 
                                  1, depth, (VOID*)data);

    if (bitmap == NULL)
    {
        BITMAPINFO bitmapInfo;
        ZeroMemory(&bitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));

        bitmapInfo.bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);
        bitmapInfo.bmiHeader.biWidth        = width;
        bitmapInfo.bmiHeader.biHeight       = -(ssize_t)height; // top-down image
        bitmapInfo.bmiHeader.biPlanes       = 1;                        // always 1            
        bitmapInfo.bmiHeader.biBitCount     = static_cast<WORD>(depth); // bits per pixel
        bitmapInfo.bmiHeader.biCompression  = BI_RGB;                   // uncompressed RGB
        bitmapInfo.bmiHeader.biSizeImage    = 0;                        // automatic
        bitmapInfo.bmiHeader.biClrUsed      = 0;                        // no color table
        bitmapInfo.bmiHeader.biClrImportant = 0;                        // no color table

        VOID* imageBits = 0;
        bitmap = CreateDIBSection(_dc, &bitmapInfo, DIB_RGB_COLORS, 
                                  &imageBits, NULL, 0);
        memcpy(imageBits, data, image.width() * image.height() * 4);
    }

    HDC bitmapDC = CreateCompatibleDC(NULL);
    SelectObject(bitmapDC, bitmap);

    BitBlt(_dc, 0, 0, image.width(), image.height(), bitmapDC, 0, 0, SRCCOPY);

    DeleteDC(bitmapDC);
    DeleteObject(bitmap);
}


Gfx::Image PixmapImpl::toImage() const
{
    BITMAPINFO bitmapInfo;
    ZeroMemory(&bitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));

    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biWidth = _width;
    bitmapInfo.bmiHeader.biHeight = -(ssize_t)_height;  // top-down image
    bitmapInfo.bmiHeader.biPlanes = 1;                         // always 1
    bitmapInfo.bmiHeader.biBitCount = 32;                      // bits per pixel
    bitmapInfo.bmiHeader.biCompression = BI_RGB;               // uncompressed RGB
    bitmapInfo.bmiHeader.biSizeImage = 0;                      // automatic
    bitmapInfo.bmiHeader.biClrUsed = 0;                        // no color table
    bitmapInfo.bmiHeader.biClrImportant = 0;                   // no color table

    Pt::Gfx::Image image(Pt::Gfx::ImageFormat::argb32(), _width, _height);
    Pt::uint8_t* data = image.data();

    int ret = GetDIBits(_dc, _bitmap, 0, _height, data, 
                        &bitmapInfo, DIB_RGB_COLORS);

    return image;
}


void PixmapImpl::clear(const Gfx::Color& c)
{
}


const Gfx::SizeF& PixmapImpl::onGetSize() const
{
    return physicalSize();
}


const Gfx::SizeF& PixmapImpl::physicalSize() const
{
    return _physicalSize;
}


const Gfx::SizeF& PixmapImpl::logicalSize() const
{
    return _logicalSize;
}


void PixmapImpl::resize(const Gfx::SizeF& size)
{
    LONG width = lround( size.width() );
    LONG height = lround( size.height() );

    if( _width == width && _height == height )
        return;
    
    HDC screenDC = GetDC(NULL);
    HBITMAP bitmap = CreateCompatibleBitmap(screenDC, width, height);
    ReleaseDC(NULL, screenDC);

    SelectObject(_dc, bitmap);
    DeleteObject(_bitmap);
    _bitmap = bitmap;
    _width = width;
    _height = height;

    _physicalSize.set(width, height);

    _logicalSize = scaling().toLogical(_physicalSize);
}


void PixmapImpl::setScaleFactor(double scaleFactor)
{
    _scaling.setScaleFactor(scaleFactor);
    _logicalSize = _scaling.toLogical(_physicalSize);
}


void PixmapImpl::draw(Gfx::PaintSurface& surface,
                      const Gfx::Paint& paint,
                      const Gfx::PointF& to,
                      const Gfx::RectF* rect) const
{
    Gfx::Painter painter(surface);
    painter.setCompositionMode( paint.compositionMode() );
    
    Gfx::Image pixmapImage = toImage();
    if(rect)
    {
        Gfx::RectF imageRect = scaling().toPhysical(*rect);
        painter.drawImage(to, pixmapImage, imageRect);
    }
    else
    {
        painter.drawImage(to, pixmapImage);
    }
}


HDC PixmapImpl::deviceContext() const
{
    return _dc;
}


const Gfx::ImageFormat& PixmapImpl::onGetFormat() const
{
    return Gfx::ImageFormat::argb32();
}


const Gfx::Scaling& PixmapImpl::onGetScaling() const
{
    return _scaling;
}


Gfx::PaintContext* PixmapImpl::onCreateContext(Gfx::PaintContext* context)
{
    PaintContext* paintContext = dynamic_cast<PaintContext*>(context);
    if( ! paintContext ) 
        paintContext  = new PaintContext();
    
    paintContext->setPixmap(*this);

    _paintContext = paintContext;
    return _paintContext;
}


void PixmapImpl::onReleaseContext()
{
    // NOTE: this might be called from the attached context base class destructor

    SelectObject(_dc, _oldPen);
    SelectObject(_dc, _oldBrush);
    SelectObject(_dc, _oldFont);
    SelectClipRgn(_dc, NULL);
    AbortPath(_dc);

    _paintContext = 0;
}


const std::string& PixmapImpl::defaultFont()
{
    return getDefaultFont();
}


void PixmapImpl::setDefaultFont(const std::string& f)
{
    getDefaultFont() = f;
}


std::vector<std::string> PixmapImpl::fontNames()
{
    std::vector<std::string> fonts;
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

    fonts.erase(std::unique(fonts.begin(), fonts.end()), fonts.end());
    return fonts;
}


void PixmapImpl::setFontDir(const System::Path& path)
{
}


std::string& PixmapImpl::getDefaultFont()
{
    static std::string _defaultFont; // = getSystemFont();
    return _defaultFont;
}


std::string PixmapImpl::getSystemFont()
{
    HDC dc = GetDC(NULL);

    // TODO: returns a font named "System", which is useless... 

    std::vector<TCHAR> buffer(32);
    GetTextFace(dc, buffer.size(), &buffer[0]);

    ReleaseDC(NULL, dc);

    return Pt::win32::toMultiByte(&buffer[0]);
}

#endif // PT_FORMS_WIN32_RASTER

} // namespace

} // namespace
