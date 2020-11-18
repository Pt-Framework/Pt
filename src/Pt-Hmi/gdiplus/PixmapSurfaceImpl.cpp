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
#include "PixmapSurfaceImpl.h"

#include <Pt/Hmi/Application.h>
#include <Pt/Hmi/PixmapSurface.h>
#include <Pt/Gfx/Argb32Format.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/Algorithm.h>

namespace {

std::string getWin32DefaultFont()
{
    HDC dc = GetDC(NULL);

    std::vector<TCHAR> buffer(32);
    GetTextFace(dc, buffer.size(), &buffer[0]);

    ReleaseDC(NULL, dc);

    return Pt::win32::toMultiByte(&buffer[0]);
}

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

#endif

} // namespace

namespace Pt {

namespace Hmi {

PixmapSurfaceImpl::PixmapSurfaceImpl()
: _size(0, 0)
, _paintData(0)
, _painter(0)
, _dc(0)
, _bitmap(0)
, _graphics(0)
, _compositionMode(Gfx::CompositionMode::SourceCopy)
{
    Gfx::SizeF size = Gfx::SizeF(10, 10);

    HDC screenDC = GetDC(NULL);
    _dc = CreateCompatibleDC(screenDC);
    _bitmap = CreateCompatibleBitmap(screenDC, lround(size.width()), 
                                               lround(size.height()));
    ReleaseDC(NULL, screenDC);

    SelectObject(_dc, _bitmap);
    SetBkMode(_dc, TRANSPARENT);

    SetGraphicsMode(_dc, GM_ADVANCED);

    _graphics = new Gdiplus::Graphics(_dc);
    _graphics->SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHalf);
    _graphics->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
}


PixmapSurfaceImpl::~PixmapSurfaceImpl()
{
    delete _graphics;
    
    DeleteDC(_dc);
    DeleteObject(_bitmap);
}


void PixmapSurfaceImpl::clear(const Gfx::Color& c)
{
}


void PixmapSurfaceImpl::resize(const Gfx::SizeF& size)
{
    if( _size == size )
        return;

    _size = size;

    DeleteObject(_bitmap);
    DeleteDC(_dc);

    HDC screenDC = GetDC(NULL);
    HBITMAP bitmap = CreateCompatibleBitmap(screenDC, lround( _size.width() ), 
                                                      lround( _size.height() ) );

    _dc = CreateCompatibleDC(screenDC);
    ReleaseDC(NULL, screenDC);

    SelectObject(_dc, bitmap);
    _bitmap = bitmap;

    delete _graphics;
    _graphics = new Gdiplus::Graphics(_dc);
    _graphics->SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHalf);
    _graphics->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
}


const Gfx::SizeF& PixmapSurfaceImpl::size() const
{
    return _size;
}


void PixmapSurfaceImpl::begin(Gfx::Painter& painter)
{
    _painter = &painter;

    Gfx::PaintData* pd = painter.paintData();
    _paintData = dynamic_cast<PaintData*>(pd);

    if (_paintData == 0)
    {
        delete pd;

        _paintData = new PaintData();
        painter.setPaintData(_paintData);
    }
}


void PixmapSurfaceImpl::finish()
{
    _paintData = 0;
    _painter = 0;
}


const Gfx::ImageFormat& PixmapSurfaceImpl::format() const
{
    return Gfx::ImageFormat::argb32();
}


void PixmapSurfaceImpl::setClip(const Gfx::RectF& clipRect)
{
    if( clipRect.isNull() )
        _graphics->ResetClip();
    else
        _graphics->SetClip(PaintData::toGdi(clipRect));
}


void PixmapSurfaceImpl::resetClip()
{
    _graphics->ResetClip();
}


void PixmapSurfaceImpl::setCompositionMode(const Gfx::CompositionMode& mode)
{
  _compositionMode = mode;
}


void PixmapSurfaceImpl::setPen(const Gfx::Pen& pen)
{
  _paintData->setPen(pen);
}


void PixmapSurfaceImpl::setBrush(const Gfx::Brush& brush)
{
  _paintData->setBrush(brush);
}


void PixmapSurfaceImpl::setFont(const Gfx::Font& font)
{
  _paintData->setFont(font);
}


Gfx::FontMetrics PixmapSurfaceImpl::fontMetrics(const Pt::String& text) const
{
    std::wstring wtext;
    text.toUtf16( std::back_inserter(wtext) );

    const Gdiplus::StringFormat* format = Gdiplus::StringFormat::GenericTypographic();
    const Gdiplus::Font& font = _paintData->font();
    const Gdiplus::FontFamily& family = _paintData->fontFamily();

    Gdiplus::REAL height = font.GetHeight( _graphics->GetDpiY() );

    UINT16 ascentUnits = family.GetCellAscent( font.GetStyle() );
    UINT16 descentUnits = family.GetCellDescent( font.GetStyle() );
    UINT16 heightUnits = family.GetLineSpacing( font.GetStyle() );

    Gdiplus::REAL pixelsPerUnit = height / heightUnits;
    Gdiplus::REAL ascentF = ascentUnits * pixelsPerUnit;
    Gdiplus::REAL descentF = descentUnits * pixelsPerUnit;
    Gdiplus::REAL heightF = ascentF + descentF;

    Gdiplus::RectF textRect;
    _graphics->MeasureString(wtext.c_str(), wtext.size(), &font,
                             Gdiplus::PointF(0, 0), format, &textRect);

    const int dpix = GetDeviceCaps(_dc, LOGPIXELSX);
    const double scaling = 96.0 / dpix;

    return Gfx::FontMetrics(ascentF * scaling, descentF * scaling, 
                            textRect.Width * scaling, heightF * scaling);
}


void PixmapSurfaceImpl::drawText(const Gfx::PointF& to, 
                                 const Pt::String& text, 
                                 const Gfx::Transform& trans)
{
    _text.clear();
    text.toUtf16(std::back_inserter(_text));

    Gfx::Transform tt = trans;    

    const int dpix = GetDeviceCaps(_dc, LOGPIXELSX);
    const double scaling = 96.0 / dpix;

    tt.scale(scaling, scaling);
    tt.translate( to.x(), to.y());

    const Gdiplus::Font& font = _paintData->font();
    const Gdiplus::FontFamily& family = _paintData->fontFamily();
    
    Gdiplus::REAL height = font.GetHeight(_graphics->GetDpiY() );

    UINT16 ascentUnits = family.GetCellAscent( font.GetStyle() );
    UINT16 descentUnits = family.GetCellDescent( font.GetStyle() );
    UINT16 heightUnits = family.GetLineSpacing( font.GetStyle() );
    Gdiplus::REAL pixelsPerUnit = height / heightUnits;

    Gdiplus::REAL ascent = ascentUnits * pixelsPerUnit;
    Gdiplus::REAL descent = descentUnits * pixelsPerUnit;
    Gdiplus::REAL spacing = height - ascent - descent;
    Gdiplus::REAL offsetY = ascent + 1;
    
    Gdiplus::PointF origin( 0, -offsetY );

    const Gdiplus::StringFormat* format = Gdiplus::StringFormat::GenericTypographic();

    Gdiplus::Matrix oldMatrix;
    _graphics->GetTransform(&oldMatrix);

    Gdiplus::Matrix matrix( static_cast<Gdiplus::REAL>( tt.m11() ), 
                            static_cast<Gdiplus::REAL>( tt.m12() ),
                            static_cast<Gdiplus::REAL>( tt.m21() ), 
                            static_cast<Gdiplus::REAL>( tt.m22() ),
                            static_cast<Gdiplus::REAL>( tt.dx() ), 
                            static_cast<Gdiplus::REAL>( tt.dy() ) );

    _graphics->SetTransform(&matrix);

    const Gfx::Color& color = _painter->pen().color();
    BYTE alpha = color.alpha() / 257;
    BYTE red   = color.red()   / 257;
    BYTE green = color.green() / 257; 
    BYTE blue  = color.blue()  / 257;

    Gdiplus::SolidBrush brush( Gdiplus::Color(alpha, red, green, blue) );

    _graphics->DrawString( _text.c_str(), _text.size(), &font,
                         origin, format, &brush);

    _graphics->SetTransform(&oldMatrix);
}


void PixmapSurfaceImpl::drawLine(const Gfx::PointF& from, const Gfx::PointF& to)
{
    const Gdiplus::Pen& pen = _paintData->pen();
    _graphics->DrawLine(&pen, PaintData::toGdi(from), PaintData::toGdi(to));
}


void PixmapSurfaceImpl::drawRect(const Gfx::RectF& rect)
{
    const Gdiplus::Pen& pen = _paintData->pen();
    _graphics->DrawRectangle(&pen, PaintData::toGdi(rect));
}


void PixmapSurfaceImpl::fillRect(const Gfx::RectF& rect)
{
    const Gdiplus::Brush& brush = _paintData->brush();
    _graphics->FillRectangle(&brush, PaintData::toGdi(rect));
}


void PixmapSurfaceImpl::drawEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    const Gdiplus::Pen& pen = _paintData->pen();
    _graphics->DrawEllipse(&pen, PaintData::toGdi(Gfx::PointF(topLeft.x(), topLeft.y()), 
                                                    Gfx::SizeF(size.width(), size.height())));
}


void PixmapSurfaceImpl::fillEllipse(const Gfx::PointF& topLeft, const Gfx::SizeF& size)
{
    const Gdiplus::Brush& brush = _paintData->brush();
    _graphics->FillEllipse(&brush, PaintData::toGdi(Gfx::PointF(topLeft.x(), topLeft.y()), 
                                                      Gfx::SizeF(size.width(), size.height())));
}


void PixmapSurfaceImpl::drawPolyline(const Gfx::PointF* ps, const size_t n)
{
    if (!n)
        return;

    std::vector<Gdiplus::PointF> points(n);

    for(unsigned i = 0; i < n; i++)
        points[i] = PaintData::toGdi(ps[i]);

    const Gdiplus::Pen& pen = _paintData->pen();
    _graphics->DrawLines(&pen, &points[0], n);
}


void PixmapSurfaceImpl::fillPolygon(const Gfx::PointF* ps, const size_t n)
{
    if(n == 0)
        return;

    std::vector<Gdiplus::PointF> points(n);

    for (unsigned i = 0; i < n; i++)
        points[i] = PaintData::toGdi(ps[i]);

    const Gdiplus::Brush& brush = _paintData->brush();
    _graphics->FillPolygon(&brush, &points[0], n);
}



void PixmapSurfaceImpl::drawPath(const Gfx::Path& path, float smoothness)
{
    Gdiplus::GraphicsPath gdiPath;
    
    //Todo::

    /*
    const Gdiplus::Pen& pen = _painter->impl()->pen();
    _graphics->DrawPath(&pen, &gdiPath);
    */
}


void PixmapSurfaceImpl::drawSurface(const Gfx::PointF& to, 
                                    const PixmapSurface& surface)
{
    const Gfx::Size size = Gfx::round( surface.size() );

    switch (_compositionMode)
    {
        case Gfx::CompositionMode::SourceCopy:
        {
            BitBlt(_dc, lround(to.x()), lround(to.y()), size.width(), size.height(),
                   surface.pixmapImpl()->deviceContext(), 0, 0, SRCCOPY);
        }
        break;

        case  Gfx::CompositionMode::SourceOver:
        {
            BLENDFUNCTION bf;
            bf.BlendOp = AC_SRC_OVER;
            bf.BlendFlags = 0;
            bf.SourceConstantAlpha = 0xFF; // only per pixel alpha
            bf.AlphaFormat = AC_SRC_ALPHA;

            AlphaBlend(_dc, to.x(), to.y(), size.width(), size.height(),
                       surface.pixmapImpl()->deviceContext(), 
                       0, 0, size.width(), size.height(), bf);
        }
        break;
    }
}


void PixmapSurfaceImpl::drawSurface(const Gfx::PointF& to, 
                                    const PixmapSurface& pm, 
                                    const Gfx::RectF& pmRect)
{
    const Gfx::Size size = Gfx::round(pmRect.size());
    const Gfx::Point from = Gfx::round(pmRect.topLeft());

    switch (_compositionMode)
    {
        case Gfx::CompositionMode::SourceCopy:
        {
            BitBlt(_dc, lround(to.x()), lround(to.y()), size.width(), size.height(),
                   pm.pixmapImpl()->deviceContext(), from.x(), from.y(), SRCCOPY);
        }
        break;

        case  Gfx::CompositionMode::SourceOver:
        {
            BLENDFUNCTION bf;
            bf.BlendOp = AC_SRC_OVER;
            bf.BlendFlags = 0;
            bf.SourceConstantAlpha = 0xFF; // only per pixel alpha
            bf.AlphaFormat = AC_SRC_ALPHA;

            AlphaBlend(_dc, to.x(), to.y(), size.width(), size.height(),
                       pm.pixmapImpl()->deviceContext(), 
                       from.x(), from.y(), size.width(), size.height(), bf);
        }
        break;
    }
}


void PixmapSurfaceImpl::toPreMulAlpha(const Pt::Gfx::Image& image, 
                                      std::vector<Pt::uint8_t>& bitmapData)
{
    size_t _width = image.width();
    size_t _height = image.height();

    for (std::size_t y = 0; y < image.height(); ++y)
    {
        for (std::size_t x = 0; x < image.width(); ++x)
        {
            Gfx::ConstPixel pixel(image.view(), x, y);
            Gfx::Color color = image.format().getColor(pixel);

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


void PixmapSurfaceImpl::bitBlit( const Gfx::Point& to, size_t width, size_t height, HBITMAP bitmap, DWORD op )
{
    HDC bitmapDC = CreateCompatibleDC(NULL);
    SelectObject(bitmapDC, bitmap);

    BitBlt(_dc,  to.x(), to.y(), width, height, bitmapDC, 0, 0, op);

    DeleteDC(bitmapDC);
}


void PixmapSurfaceImpl::drawImage(const Gfx::PointF& to, 
                                  const Gfx::Image& image, 
                                  const Gfx::RectF& imgRect)
{
    // TODO
    throw std::runtime_error("not implemented");
}


void PixmapSurfaceImpl::drawImage(const Gfx::PointF& toF, const Gfx::Image& image)
{
    Gfx::Point to = Gfx::round(toF);

    switch (_compositionMode)
    {
        case Gfx::CompositionMode::SourceCopy:
        {
            const Pt::uint8_t* data = image.data();
            size_t depth = image.view().pixelStride() * 8; 

            HBITMAP bitmap = CreateBitmap(image.width(), image.height(), 1, 
                                          depth, (VOID*)data);
            if (bitmap == NULL) 
            {
                BITMAPINFO bitmapInfo;
                ZeroMemory(&bitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));

                bitmapInfo.bmiHeader.biSize         = sizeof(BITMAPINFOHEADER); 
                bitmapInfo.bmiHeader.biWidth        = image.width();   
                bitmapInfo.bmiHeader.biHeight       = -(ssize_t)image.height(); // top-down image
                bitmapInfo.bmiHeader.biPlanes       = 1;                        // always 1
                bitmapInfo.bmiHeader.biBitCount     = static_cast<WORD>(depth); // bits per pixel
                bitmapInfo.bmiHeader.biCompression  = BI_RGB;                   // uncompressed RGB
                bitmapInfo.bmiHeader.biSizeImage    = 0;                        // automatic
                bitmapInfo.bmiHeader.biClrUsed      = 0;                        // no color table
                bitmapInfo.bmiHeader.biClrImportant = 0;                        // no color table

                VOID* imageBits = 0;
                bitmap = CreateDIBSection(_dc, &bitmapInfo, 
                                          DIB_RGB_COLORS, &imageBits, NULL, 0);
                memcpy(imageBits, data, image.width() * image.height() * 4);
            }

            HDC bitmapDC = CreateCompatibleDC(NULL);
            SelectObject(bitmapDC, bitmap);

            BitBlt(_dc, to.x(), to.y(), image.width(), image.height(), 
                   bitmapDC, 0, 0, SRCCOPY);

            DeleteDC(bitmapDC);
            DeleteObject(bitmap);
            break;
        }

        case Gfx::CompositionMode::SourceOver:
        {
            std::vector<Pt::uint8_t> bitmapData;
            toPreMulAlpha(image, bitmapData);

            const Pt::uint8_t* data = bitmapData.empty() ? 0 : &bitmapData[0];
            size_t depth = 32;

            HBITMAP bitmap = CreateBitmap(image.width(), image.height(), 1, 
                                          depth, (VOID*)data);
            if (bitmap == NULL)
            {
                BITMAPINFO bitmapInfo;
                ZeroMemory(&bitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));

                bitmapInfo.bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);
                bitmapInfo.bmiHeader.biWidth        = image.width();
                bitmapInfo.bmiHeader.biHeight       = -(ssize_t)image.height(); // top-down image
                bitmapInfo.bmiHeader.biPlanes       = 1;                        // always 1            
                bitmapInfo.bmiHeader.biBitCount     = static_cast<WORD>(depth); // bits per pixel
                bitmapInfo.bmiHeader.biCompression  = BI_RGB;                   // uncompressed RGB
                bitmapInfo.bmiHeader.biSizeImage    = 0;                        // automatic
                bitmapInfo.bmiHeader.biClrUsed      = 0;                        // no color table
                bitmapInfo.bmiHeader.biClrImportant = 0;                        // no color table

                VOID* imageBits = 0;
                bitmap = CreateDIBSection(_dc, &bitmapInfo,
                                          DIB_RGB_COLORS, &imageBits, NULL, 0);
                memcpy(imageBits, data, image.width() * image.height() * 4);
            }

            HDC bitmapDC = CreateCompatibleDC(NULL);

            SelectObject(bitmapDC, bitmap);

            BLENDFUNCTION bf;
            bf.BlendOp = AC_SRC_OVER;
            bf.BlendFlags = 0;
            bf.SourceConstantAlpha = 0xFF; // only per pixel alpha
            bf.AlphaFormat = AC_SRC_ALPHA;

            AlphaBlend(_dc, to.x(), to.y(), _size.width(), _size.height(), 
                       bitmapDC, 0, 0, _size.width(), _size.height(), bf);

            DeleteObject(bitmap);
            DeleteDC(bitmapDC);
            break;
        }
    }
}


Gfx::Image PixmapSurfaceImpl::toImage(const Gfx::ImageFormat& iformat) const
{
    Pt::Gfx::Image dest(iformat, round(_size));
    Pt::uint8_t* srcBuffer;

    const size_t depth = 32;
    BITMAPINFO bitmapInfo;
    ZeroMemory(&bitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));

    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biWidth = _size.width();
    bitmapInfo.bmiHeader.biHeight = -(ssize_t)_size.height(); // top-down image
    bitmapInfo.bmiHeader.biPlanes = 1;                        // always 1
    bitmapInfo.bmiHeader.biBitCount = static_cast<WORD>(depth); // bits per pixel
    bitmapInfo.bmiHeader.biCompression = BI_RGB;                   // uncompressed RGB
    bitmapInfo.bmiHeader.biSizeImage = 0;                        // automatic
    bitmapInfo.bmiHeader.biClrUsed = 0;                        // no color table
    bitmapInfo.bmiHeader.biClrImportant = 0;                        // no color table

    int ret =  GetDIBits(_dc, _bitmap, 0, _size.height(), srcBuffer, &bitmapInfo, DIB_RGB_COLORS);


    Pt::Gfx::Image source(format(), srcBuffer, round(_size));

    Pt::Gfx::copy(source.begin(), source.end(), dest.begin());
    return dest;
}


void PixmapSurfaceImpl::set(const Gfx::Image& image)
{
    resize(Gfx::SizeF(image.size().width(), image.size().height()));

    size_t _width = image.width();
    size_t _height = image.height();

    std::vector<Pt::uint8_t> bitmapData;

    toPreMulAlpha(image, bitmapData);

    const Pt::uint8_t* data = bitmapData.empty() ? 0 : &bitmapData[0];

    const size_t depth = 32;

    HBITMAP bitmap = CreateBitmap(image.width(), image.height(), 1, depth, (VOID*)data);

    if (bitmap == NULL)
    {
        BITMAPINFO bitmapInfo;
        ZeroMemory(&bitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));

        bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bitmapInfo.bmiHeader.biWidth = image.width();
        bitmapInfo.bmiHeader.biHeight = -(ssize_t)image.height(); // top-down image
        bitmapInfo.bmiHeader.biPlanes = 1;                        // always 1            
        bitmapInfo.bmiHeader.biBitCount = static_cast<WORD>(depth); // bits per pixel
        bitmapInfo.bmiHeader.biCompression = BI_RGB;                   // uncompressed RGB
        bitmapInfo.bmiHeader.biSizeImage = 0;                        // automatic
        bitmapInfo.bmiHeader.biClrUsed = 0;                        // no color table
        bitmapInfo.bmiHeader.biClrImportant = 0;                        // no color table

        VOID* imageBits = 0;
        bitmap = CreateDIBSection(_dc, &bitmapInfo, DIB_RGB_COLORS, &imageBits, NULL, 0);
        memcpy(imageBits, data, image.width() * image.height() * 4);
    }

    HDC bitmapDC = CreateCompatibleDC(NULL);
    SelectObject(bitmapDC, bitmap);

    BitBlt(_dc, 0, 0, image.width(), image.height(), bitmapDC, 0, 0, SRCCOPY);

    DeleteDC(bitmapDC);
    DeleteObject(bitmap);
}


std::string PixmapSurfaceImpl::defaultFont()
{
    return getDefaultFont();
}


void PixmapSurfaceImpl::setDefaultFont(const std::string& f)
{
    getDefaultFont() = f;
}


std::string& PixmapSurfaceImpl::getDefaultFont()
{
    static std::string _defaultFont = getWin32DefaultFont();
    return _defaultFont;
}


std::vector<std::string> PixmapSurfaceImpl::fontNames()
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


void PixmapSurfaceImpl::setFontDir(const System::Path& path)
{
}


Gfx::FontMetrics PixmapSurfaceImpl::fontMetrics(const Gfx::Font& font, const Pt::String& text)
{
    return PaintData::fontMetrics(font, text);
}


HDC PixmapSurfaceImpl::deviceContext() const
{
    return _dc;
}

} // namespace

} // namespace
