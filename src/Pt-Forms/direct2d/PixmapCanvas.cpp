/* Copyright (C) 2015-2026 Marc Boris Duerner

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
#include "PixmapCanvas.h"
#include "PixmapImpl.h"
#include "D2DDevice.h"
#include "ApplicationImpl.h"

#include <Pt/Forms/Application.h>
#include "DWriteFontProvider.h"

#include <Pt/Forms/Pixmap.h>
#include <Pt/Gfx/Image.h>
#include <Pt/Gfx/FontMetrics.h>
#include <Pt/Gfx/TextMetrics.h>
#include <Pt/Gfx/Rgb32.h>
#include <Pt/Gfx/Path.h>
#include <Pt/Gfx/Transform.h>
#include <Pt/String.h>

#include <cmath>
#include <cassert>

namespace {

D2D1_COLOR_F toD2DColor(const Pt::Gfx::Color& c)
{
    return D2D1::ColorF(c.red() / 255.0f,
                        c.green() / 255.0f,
                        c.blue() / 255.0f,
                        c.alpha() / 255.0f);
}


D2D1_POINT_2F toD2DPoint(const Pt::Gfx::PointF& p)
{
    return D2D1::Point2F(static_cast<FLOAT>(p.x()),
                         static_cast<FLOAT>(p.y()));
}


D2D1_RECT_F toD2DRect(const Pt::Gfx::RectF& r)
{
    return D2D1::RectF(static_cast<FLOAT>(r.left()),
                       static_cast<FLOAT>(r.top()),
                       static_cast<FLOAT>(r.right()),
                       static_cast<FLOAT>(r.bottom()));
}

} // anonymous namespace

namespace Pt {

namespace Forms {

PixmapCanvas::PixmapCanvas()
: Gfx::Canvas()
, _pixmap(0)
, _deviceContext(0)
, _penBrush(0)
, _strokeStyle(0)
, _penWidth(1.0f)
, _fillBrush(0)
, _textureBitmap(0)
, _textFormat(0)
, _pathGeom(0)
, _clipSet(false)
, _hasClip(false)
, _painting(false)
{
    Application::instance().impl()->d2d().d2dDevice()->CreateDeviceContext(
        D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &_deviceContext);
}


PixmapCanvas::~PixmapCanvas()
{
    if(_pathGeom)
        _pathGeom->Release();

    if(_textFormat)
        _textFormat->Release();

    if(_strokeStyle)
        _strokeStyle->Release();

    if(_textureBitmap)
        _textureBitmap->Release();

    if(_fillBrush)
        _fillBrush->Release();

    if(_penBrush)
        _penBrush->Release();

    if(_deviceContext)
        _deviceContext->Release();
}


void PixmapCanvas::setPixmap(PixmapImpl& pixmap)
{
    _pixmap = &pixmap;
}


void PixmapCanvas::drawPixmap(const Gfx::PointF& toF,
                              const Pixmap& pm,
                              const Gfx::RectF* rect)
{
    if( ! _deviceContext || ! _painting)
        return;

    applyState();

    const PixmapImpl* srcImpl = pm.impl();
    if( ! srcImpl)
        return;

    ID2D1Bitmap1* srcBitmap = srcImpl->bitmap();
    if( ! srcBitmap)
        return;

    LONG srcW = srcImpl->width();
    LONG srcH = srcImpl->height();
    if(srcW == 0 || srcH == 0)
        return;

    // Compute destination rect in logical coordinates
    const Gfx::Scaling& dstScaling = scaling();

    D2D1_RECT_F destRect;
    D2D1_RECT_F srcRect;
    D2D1_RECT_F* srcRectPtr = 0;

    if(rect)
    {
        const Gfx::Scaling& srcScaling = srcImpl->scaling();
        Gfx::RectF physRect = srcScaling.toPhysical(*rect);
        srcRect = D2D1::RectF( static_cast<FLOAT>(physRect.x()),
                               static_cast<FLOAT>(physRect.y()),
                               static_cast<FLOAT>(physRect.x() + physRect.width()),
                               static_cast<FLOAT>(physRect.y() + physRect.height()) );
        srcRectPtr = &srcRect;
        destRect = D2D1::RectF( static_cast<FLOAT>(toF.x()),
                                static_cast<FLOAT>(toF.y()),
                                static_cast<FLOAT>(toF.x() + rect->width()),
                                static_cast<FLOAT>(toF.y() + rect->height()) );
    }
    else
    {
        double logW = dstScaling.toLogical(srcImpl->size().width());
        double logH = dstScaling.toLogical(srcImpl->size().height());

        destRect = D2D1::RectF( static_cast<FLOAT>(toF.x()),
                                static_cast<FLOAT>(toF.y()),
                                static_cast<FLOAT>(toF.x() + logW),
                                static_cast<FLOAT>(toF.y() + logH) );
    }

    _deviceContext->DrawBitmap(srcBitmap, destRect, 1.0f,
                               D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
                               srcRectPtr);
}


void PixmapCanvas::onBeginPaint(const Gfx::Paint& /*paint*/)
{
    if( ! _pixmap || ! _deviceContext)
        return;

    ID2D1Bitmap1* target = _pixmap->bitmap();
    if( ! target)
        return;

    _deviceContext->SetTarget(target);
    _deviceContext->BeginDraw();
    _deviceContext->SetTransform(D2D1::Matrix3x2F::Identity());
    _painting = true;
}


void PixmapCanvas::onFinishPaint()
{
    if( ! _deviceContext || ! _painting)
        return;

    if(_hasClip)
    {
        _deviceContext->PopAxisAlignedClip();
        _hasClip = false;
    }

    HRESULT hr = _deviceContext->EndDraw();
    _painting = false;

    _deviceContext->SetTarget(nullptr);

    if(hr == D2DERR_RECREATE_TARGET)
    {
        // Device lost: recreate context and brushes
        _deviceContext->Release();
        _deviceContext = 0;

        if(_penBrush)
        {
            _penBrush->Release();
            _penBrush = 0;
        }

        if(_fillBrush)
        {
            _fillBrush->Release();
            _fillBrush = 0;
        }

        if(_textureBitmap)
        {
            _textureBitmap->Release();
            _textureBitmap = 0;
        }

        Application::instance().impl()->d2d().d2dDevice()->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &_deviceContext);

        // Recreate brushes immediately
        if(_deviceContext)
        {
            D2D1_COLOR_F penColor = toD2DColor( _pen.color() );
            _deviceContext->CreateSolidColorBrush(penColor, &_penBrush);

            createBrush();
        }
    }
}


void PixmapCanvas::onSetTransform(const Gfx::Transform& /*tx*/)
{
}


void PixmapCanvas::onApplyTransform()
{
    if( ! _deviceContext)
        return;

    const Gfx::Transform& tx = transform();

    D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F(
        static_cast<FLOAT>(tx.m11()), static_cast<FLOAT>(tx.m12()),
        static_cast<FLOAT>(tx.m21()), static_cast<FLOAT>(tx.m22()),
        static_cast<FLOAT>(tx.dx()),  static_cast<FLOAT>(tx.dy()));

    _deviceContext->SetTransform(matrix);
}


void PixmapCanvas::onSetCompositionMode(const Gfx::CompositionMode& mode)
{
    _compositionMode = mode;
}


void PixmapCanvas::onApplyCompositionMode()
{
    if( ! _deviceContext)
        return;

    if(_compositionMode == Gfx::CompositionMode::SourceCopy)
        _deviceContext->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_COPY);
    else
        _deviceContext->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);
}


void PixmapCanvas::onSetPen(const Gfx::Pen& pen)
{
    _pen = pen;

    _penWidth = static_cast<FLOAT>( _pen.size() );
    if(_penWidth < 1.0f)
        _penWidth = 1.0f;

    // Update or create pen brush
    D2D1_COLOR_F color = toD2DColor( _pen.color() );

    if(_penBrush)
    {
        _penBrush->SetColor(color);
    }
    else if(_deviceContext)
    {
        _deviceContext->CreateSolidColorBrush(color, &_penBrush);
    }

    // Recreate stroke style
    if(_strokeStyle)
    {
        _strokeStyle->Release();
        _strokeStyle = 0;
    }

    D2D1_CAP_STYLE capStyle = D2D1_CAP_STYLE_FLAT;
    switch( _pen.capStyle() )
    {
        case Gfx::Pen::FlatCap:
            capStyle = D2D1_CAP_STYLE_FLAT;
            break;
        case Gfx::Pen::RoundCap:
            capStyle = D2D1_CAP_STYLE_ROUND;
            break;
        case Gfx::Pen::SquareCap:
            capStyle = D2D1_CAP_STYLE_SQUARE;
            break;
    }

    D2D1_LINE_JOIN joinStyle = D2D1_LINE_JOIN_MITER;
    switch( _pen.joinStyle() )
    {
        case Gfx::Pen::NoJoin:
        case Gfx::Pen::MiterJoin:
            joinStyle = D2D1_LINE_JOIN_MITER;
            break;
        case Gfx::Pen::RoundJoin:
            joinStyle = D2D1_LINE_JOIN_ROUND;
            break;
        case Gfx::Pen::BevelJoin:
            joinStyle = D2D1_LINE_JOIN_BEVEL;
            break;
    }

    D2D1_DASH_STYLE dashStyle = D2D1_DASH_STYLE_SOLID;
    _dashes.clear();

    switch( _pen.style() )
    {
        case Gfx::Pen::Solid:
            dashStyle = D2D1_DASH_STYLE_SOLID;
            break;
        case Gfx::Pen::Dash:
            dashStyle = D2D1_DASH_STYLE_DASH;
            break;
        case Gfx::Pen::Dot:
            dashStyle = D2D1_DASH_STYLE_DOT;
            break;
        case Gfx::Pen::DashPattern:
        {
            dashStyle = D2D1_DASH_STYLE_CUSTOM;
            const std::vector<Pt::uint8_t>& pattern = _pen.dashPattern();
            _dashes.resize( pattern.size() );
            for(size_t i = 0; i < pattern.size(); ++i)
                _dashes[i] = static_cast<FLOAT>(pattern[i]);
            break;
        }
    }

    D2D1_STROKE_STYLE_PROPERTIES strokeProps = D2D1::StrokeStyleProperties(
        capStyle, capStyle, capStyle,
        joinStyle, 10.0f, dashStyle, 0.0f);

    Application::instance().impl()->d2d().d2dFactory()->CreateStrokeStyle(
        strokeProps,
        _dashes.empty() ? nullptr : _dashes.data(),
        static_cast<UINT32>(_dashes.size()),
        &_strokeStyle);
}


void PixmapCanvas::onApplyPen()
{
    if( ! _deviceContext)
        return;

    // Lazy-init after RT recreation
    if( ! _penBrush)
    {
        D2D1_COLOR_F color = toD2DColor( _pen.color() );
        _deviceContext->CreateSolidColorBrush(color, &_penBrush);
    }
}


void PixmapCanvas::onSetBrush(const Gfx::Brush& brush)
{
    _brush = brush;

    if(_fillBrush)
    {
        _fillBrush->Release();
        _fillBrush = 0;
    }

    if( ! _deviceContext)
        return;

    createBrush();
}


void PixmapCanvas::onApplyBrush()
{
    if( ! _deviceContext)
        return;

    // Lazy-init after device context recreation
    if( ! _fillBrush)
        createBrush();
}


void PixmapCanvas::createBrush()
{
    if( ! _deviceContext)
        return;

    if(_textureBitmap)
    {
        _textureBitmap->Release();
        _textureBitmap = 0;
    }

    if( _brush.isTexture() )
    {
        createTextureBrush();
        return;
    }

    if( _brush.isGradient() )
    {
        createGradientBrush();
        return;
    }

    createSolidBrush();
}


void PixmapCanvas::createSolidBrush()
{
    ID2D1SolidColorBrush* solidBrush = 0;
    _deviceContext->CreateSolidColorBrush(toD2DColor(_brush.color()), &solidBrush);
    _fillBrush = solidBrush;
}


void PixmapCanvas::createTextureBrush()
{
    const Gfx::Image& texture = _brush.texture();

    if(texture.width() == 0 || texture.height() == 0)
    {
        createSolidBrush();
        return;
    }

    const Pt::uint8_t* data = texture.data();
    Gfx::Rgb32Image rgb32Texture;

    if(texture.format() != Gfx::ImageFormat::rgb32() || texture.padding() != 0)
    {
        rgb32Texture.reset(texture.width(), texture.height());
        Gfx::copyView(texture, rgb32Texture);
        data = rgb32Texture.data();
    }

    D2D1_SIZE_U bmpSize = D2D1::SizeU(
        static_cast<UINT32>(texture.width()),
        static_cast<UINT32>(texture.height()));
    D2D1_BITMAP_PROPERTIES bmpProps = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED));

    HRESULT hr = _deviceContext->CreateBitmap(
        bmpSize, data,
        static_cast<UINT32>(texture.width() * 4),
        bmpProps, &_textureBitmap);

    if(FAILED(hr) || ! _textureBitmap)
        return;

    D2D1_BITMAP_BRUSH_PROPERTIES brushProps;
    brushProps.extendModeX = D2D1_EXTEND_MODE_WRAP;
    brushProps.extendModeY = D2D1_EXTEND_MODE_WRAP;
    brushProps.interpolationMode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;

    ID2D1BitmapBrush* bitmapBrush = 0;
    hr = _deviceContext->CreateBitmapBrush(
        _textureBitmap, brushProps, &bitmapBrush);

    _fillBrush = bitmapBrush;
}


void PixmapCanvas::createGradientBrush()
{
    const Gfx::ColorStops& stops = _brush.gradientStops();

    std::vector<D2D1_GRADIENT_STOP> d2dStops;
    d2dStops.reserve(stops.size());

    for(std::size_t i = 0; i < stops.size(); ++i)
    {
        D2D1_GRADIENT_STOP gs;
        gs.position = stops[i].position();
        gs.color = toD2DColor( stops[i].color() );
        d2dStops.push_back(gs);
    }

    // Fallback: if no stops, use brush color
    if(d2dStops.empty())
    {
        D2D1_GRADIENT_STOP gs;
        gs.position = 0.0f;
        gs.color = toD2DColor( _brush.color() );
        d2dStops.push_back(gs);
        gs.position = 1.0f;
        d2dStops.push_back(gs);
    }

    ID2D1GradientStopCollection* stopCollection = 0;
    HRESULT hr = _deviceContext->CreateGradientStopCollection(
        d2dStops.data(),
        static_cast<UINT32>(d2dStops.size()),
        D2D1_GAMMA_2_2,
        D2D1_EXTEND_MODE_CLAMP,
        &stopCollection);

    if(FAILED(hr) || ! stopCollection)
        return;

    Gfx::Brush::GradientStyle style = _brush.gradient();

    if(style == Gfx::Brush::Linear ||
       style == Gfx::Brush::Horizontal ||
       style == Gfx::Brush::Vertical)
    {
        D2D1_POINT_2F startPt;
        D2D1_POINT_2F endPt;

        if(style == Gfx::Brush::Horizontal)
        {
            startPt = D2D1::Point2F(0.0f, 0.0f);
            endPt = D2D1::Point2F(1.0f, 0.0f);
        }
        else if(style == Gfx::Brush::Vertical)
        {
            startPt = D2D1::Point2F(0.0f, 0.0f);
            endPt = D2D1::Point2F(0.0f, 1.0f);
        }
        else
        {
            const Gfx::PointF& begin = _brush.gradientBegin();
            const Gfx::PointF& end = _brush.gradientEnd();
            startPt = D2D1::Point2F(static_cast<FLOAT>(begin.x()),
                                    static_cast<FLOAT>(begin.y()));
            endPt = D2D1::Point2F(static_cast<FLOAT>(end.x()),
                                  static_cast<FLOAT>(end.y()));
        }

        D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES props;
        props.startPoint = startPt;
        props.endPoint = endPt;

        ID2D1LinearGradientBrush* linearBrush = 0;
        hr = _deviceContext->CreateLinearGradientBrush(
            props, stopCollection, &linearBrush);

        _fillBrush = linearBrush;
    }
    else if(style == Gfx::Brush::Radial)
    {
        const Gfx::PointF& center = _brush.gradientEnd();
        const Gfx::PointF& origin = _brush.gradientBegin();

        FLOAT radiusX = static_cast<FLOAT>(_brush.gradientEndRadius());
        FLOAT radiusY = radiusX;

        // Offset from center to focus point
        FLOAT offsetX = static_cast<FLOAT>(origin.x() - center.x());
        FLOAT offsetY = static_cast<FLOAT>(origin.y() - center.y());

        D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES props;
        props.center = D2D1::Point2F(static_cast<FLOAT>(center.x()),
                                     static_cast<FLOAT>(center.y()));
        props.gradientOriginOffset = D2D1::Point2F(offsetX, offsetY);
        props.radiusX = radiusX;
        props.radiusY = radiusY;

        ID2D1RadialGradientBrush* radialBrush = 0;
        hr = _deviceContext->CreateRadialGradientBrush(
            props, stopCollection, &radialBrush);

        _fillBrush = radialBrush;
    }

    stopCollection->Release();
}


bool PixmapCanvas::isRelativeGradient() const
{
    return _brush.isGradient() && _brush.positionMode() == Gfx::Brush::Relative;
}


void PixmapCanvas::prepareGradientBrush(const D2D1_RECT_F& bounds)
{
    FLOAT width  = bounds.right  - bounds.left;
    FLOAT height = bounds.bottom - bounds.top;

    D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F(
        width,  0.0f,
        0.0f,   height,
        bounds.left, bounds.top);

    _fillBrush->SetTransform(matrix);
}


void PixmapCanvas::onSetFont(const Gfx::Font& font)
{
    _font = font;

    if(_textFormat && font == _cachedFont)
        return;

    if(_textFormat)
    {
        _textFormat->Release();
        _textFormat = 0;
    }

    _cachedFont = font;

    std::string family = font.family();
    if( family.empty() )
        family = DWriteFontProvider::instance().defaultFont();

    // Convert family to wide string
    std::wstring wFamily(family.begin(), family.end());

    DWRITE_FONT_WEIGHT weight = static_cast<DWRITE_FONT_WEIGHT>(
        static_cast<int>(font.weight()));
    
    DWRITE_FONT_STYLE style = DWRITE_FONT_STYLE_NORMAL;
    if(font.slant() == Gfx::Font::Slant::Italic)
        style = DWRITE_FONT_STYLE_ITALIC;
    else if(font.slant() == Gfx::Font::Slant::Oblique)
        style = DWRITE_FONT_STYLE_OBLIQUE;

    // DWrite fontSize is in DIPs (device-independent pixels), convert from points.
    FLOAT fontSize = static_cast<FLOAT>(font.size()) * 96.0f / 72.0f;
    if(fontSize <= 0.0f)
        fontSize = 12.0f * 96.0f / 72.0f;

    HRESULT hr = Application::instance().impl()->d2d().dwriteFactory()->CreateTextFormat(
        wFamily.c_str(),
        nullptr,
        weight,
        style,
        DWRITE_FONT_STRETCH_NORMAL,
        fontSize,
        L"",
        &_textFormat);

    if(FAILED(hr))
        return;

    // Get font metrics
    IDWriteFontCollection* collection = nullptr;
    _textFormat->GetFontCollection(&collection);

    if(collection)
    {
        UINT32 index = 0;
        BOOL exists = FALSE;
        collection->FindFamilyName(wFamily.c_str(), &index, &exists);

        if(exists)
        {
            IDWriteFontFamily* fontFamily = nullptr;
            collection->GetFontFamily(index, &fontFamily);

            if(fontFamily)
            {
                IDWriteFont* dwFont = nullptr;
                fontFamily->GetFirstMatchingFont(weight, DWRITE_FONT_STRETCH_NORMAL,
                                                 style, &dwFont);

                if(dwFont)
                {
                    DWRITE_FONT_METRICS metrics;
                    dwFont->GetMetrics(&metrics);

                    FLOAT designToEm = fontSize / metrics.designUnitsPerEm;

                    _fontMetrics.setAscent(
                        static_cast<double>(metrics.ascent * designToEm));
                    _fontMetrics.setDescent(
                        static_cast<double>(metrics.descent * designToEm));
                    _fontMetrics.setLeading(
                        static_cast<double>(metrics.lineGap * designToEm));
                    _fontMetrics.setCapHeight(
                        static_cast<double>(metrics.capHeight * designToEm));
                    _fontMetrics.setXHeight(
                        static_cast<double>(metrics.xHeight * designToEm));
                    _fontMetrics.setUnderlinePos(
                        static_cast<double>(-metrics.underlinePosition * designToEm));
                    _fontMetrics.setUnderlineThickness(
                        static_cast<double>(metrics.underlineThickness * designToEm));
                    _fontMetrics.setStrikeoutPos(
                        static_cast<double>(metrics.strikethroughPosition * designToEm));
                    _fontMetrics.setStrikeoutThickness(
                        static_cast<double>(metrics.strikethroughThickness * designToEm));

                    dwFont->Release();
                }

                fontFamily->Release();
            }
        }

        collection->Release();
    }
}


void PixmapCanvas::onApplyFont()
{
    // Font/textFormat already created in onSetFont.
    // Nothing additional needed for D2D.
}


void PixmapCanvas::onSetClip(const Gfx::RectF* clip)
{
    if(clip)
    {
        _clip = *clip;
        _clipSet = true;
    }
    else
    {
        _clipSet = false;
    }
}


void PixmapCanvas::onApplyClip()
{
    if( ! _deviceContext)
        return;

    if(_hasClip)
    {
        _deviceContext->PopAxisAlignedClip();
        _hasClip = false;
    }

    if(_clipSet)
    {
        D2D1_RECT_F d2dClip = toD2DRect(_clip);
        _deviceContext->PushAxisAlignedClip(d2dClip, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        _hasClip = true;
    }
}


void PixmapCanvas::onDrawLine(const Gfx::PointF& from,
                              const Gfx::PointF& to)
{
    if( ! _deviceContext || ! _penBrush)
        return;

    _deviceContext->DrawLine(toD2DPoint(from), toD2DPoint(to),
                             _penBrush, _penWidth, _strokeStyle);
}


void PixmapCanvas::onDrawPolyline(const Gfx::PointF* pts, const size_t n)
{
    if( ! _deviceContext || ! _penBrush || n < 2)
        return;

    for(size_t i = 0; i < n - 1; ++i)
    {
        _deviceContext->DrawLine(toD2DPoint(pts[i]), toD2DPoint(pts[i + 1]),
                                 _penBrush, _penWidth, _strokeStyle);
    }
}


void PixmapCanvas::onFillPolygon(const Gfx::PointF* pts, const size_t n)
{
    if( ! _deviceContext || ! _fillBrush || n < 3)
        return;

    ID2D1PathGeometry* geom = 0;
    Application::instance().impl()->d2d().d2dFactory()->CreatePathGeometry(&geom);
    if( ! geom)
        return;

    ID2D1GeometrySink* sink = 0;
    geom->Open(&sink);
    if(sink)
    {
        sink->BeginFigure(toD2DPoint(pts[0]), D2D1_FIGURE_BEGIN_FILLED);

        for(size_t i = 1; i < n; ++i)
            sink->AddLine(toD2DPoint(pts[i]));

        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        sink->Close();
        sink->Release();

        if(isRelativeGradient())
        {
            FLOAT minX = static_cast<FLOAT>(pts[0].x());
            FLOAT minY = static_cast<FLOAT>(pts[0].y());
            FLOAT maxX = minX;
            FLOAT maxY = minY;
            for(size_t i = 1; i < n; ++i)
            {
                FLOAT px = static_cast<FLOAT>(pts[i].x());
                FLOAT py = static_cast<FLOAT>(pts[i].y());
                if(px < minX) minX = px;
                if(py < minY) minY = py;
                if(px > maxX) maxX = px;
                if(py > maxY) maxY = py;
            }
            prepareGradientBrush(D2D1::RectF(minX, minY, maxX, maxY));
        }

        _deviceContext->FillGeometry(geom, _fillBrush);
    }

    geom->Release();
}


void PixmapCanvas::onDrawRect(const Gfx::RectF& rectangle)
{
    if( ! _deviceContext || ! _penBrush)
        return;

    D2D1_RECT_F rect = toD2DRect(rectangle);
    _deviceContext->DrawRectangle(rect, _penBrush, _penWidth, _strokeStyle);
}


void PixmapCanvas::onFillRect(const Gfx::RectF& rectangle)
{
    if( ! _deviceContext || ! _fillBrush)
        return;

    D2D1_RECT_F rect = toD2DRect(rectangle);
    if(isRelativeGradient())
        prepareGradientBrush(rect);
    _deviceContext->FillRectangle(rect, _fillBrush);
}


void PixmapCanvas::onDrawEllipse(const Gfx::PointF& topLeft,
                                 const Gfx::SizeF& size)
{
    if( ! _deviceContext || ! _penBrush)
        return;

    D2D1_ELLIPSE ellipse = D2D1::Ellipse(
        D2D1::Point2F(static_cast<FLOAT>(topLeft.x() + size.width() / 2.0),
                      static_cast<FLOAT>(topLeft.y() + size.height() / 2.0)),
        static_cast<FLOAT>(size.width() / 2.0),
        static_cast<FLOAT>(size.height() / 2.0));

    _deviceContext->DrawEllipse(ellipse, _penBrush, _penWidth, _strokeStyle);
}


void PixmapCanvas::onFillEllipse(const Gfx::PointF& topLeft,
                                 const Gfx::SizeF& size)
{
    if( ! _deviceContext || ! _fillBrush)
        return;

    D2D1_ELLIPSE ellipse = D2D1::Ellipse(
        D2D1::Point2F(static_cast<FLOAT>(topLeft.x() + size.width() / 2.0),
                      static_cast<FLOAT>(topLeft.y() + size.height() / 2.0)),
        static_cast<FLOAT>(size.width() / 2.0),
        static_cast<FLOAT>(size.height() / 2.0));

    if(isRelativeGradient())
    {
        D2D1_RECT_F bounds = D2D1::RectF(
            static_cast<FLOAT>(topLeft.x()),
            static_cast<FLOAT>(topLeft.y()),
            static_cast<FLOAT>(topLeft.x() + size.width()),
            static_cast<FLOAT>(topLeft.y() + size.height()));
        prepareGradientBrush(bounds);
    }

    _deviceContext->FillEllipse(ellipse, _fillBrush);
}


Gfx::TextMetrics PixmapCanvas::onGetTextMetrics(const Pt::String& text) const
{
    if( ! _textFormat)
        return Gfx::TextMetrics();

    // Convert to wide string
    std::wstring wtext;
    wtext.reserve( text.size() );
    for(size_t i = 0; i < text.size(); ++i)
        wtext.push_back( static_cast<wchar_t>(text[i].value()) );

    IDWriteTextLayout* layout = 0;
    HRESULT hr = Application::instance().impl()->d2d().dwriteFactory()->CreateTextLayout(
        wtext.c_str(),
        static_cast<UINT32>(wtext.size()),
        _textFormat,
        100000.0f, 100000.0f,
        &layout);

    if(FAILED(hr) || ! layout)
        return Gfx::TextMetrics();

    DWRITE_TEXT_METRICS metrics;
    layout->GetMetrics(&metrics);

    DWRITE_LINE_METRICS lineMetrics;
    UINT32 lineCount = 0;
    layout->GetLineMetrics(&lineMetrics, 1, &lineCount);

    layout->Release();

    Gfx::TextMetrics tm;
    tm.setAdvance( static_cast<double>(metrics.widthIncludingTrailingWhitespace) );
    tm.setBoundingWidth( static_cast<double>(metrics.width) );
    tm.setBoundingHeight( static_cast<double>(metrics.height) );

    if(lineCount > 0)
        tm.setBearingY( static_cast<double>(lineMetrics.baseline) );

    return tm;
}


const Gfx::FontMetrics& PixmapCanvas::onGetFontMetrics() const
{
    return _fontMetrics;
}


void PixmapCanvas::onDrawText(const Gfx::PointF& to,
                              const Pt::String& text,
                              const Gfx::Transform* tform)
{
    if( ! _deviceContext || ! _penBrush || ! _textFormat)
        return;

    std::wstring wtext;
    wtext.reserve( text.size() );
    for(size_t i = 0; i < text.size(); ++i)
        wtext.push_back( static_cast<wchar_t>(text[i].value()) );

    // GDI uses TA_BASELINE, so 'to.y' is the baseline position.
    // DirectWrite DrawText places the text cell TOP at the layout rect origin,
    // with the baseline at top + ascent. Offset by -ascent to align baselines.
    FLOAT baselineOffset = static_cast<FLOAT>(_fontMetrics.ascent());

    if(tform)
    {
        // Additional text transform: translate to position, apply extra transform,
        // then draw at origin. Combine with the active D2D transform.
        D2D1_MATRIX_3X2_F oldTransform;
        _deviceContext->GetTransform(&oldTransform);

        Gfx::Transform tf;
        tf.translate(to.x(), to.y());
        tf *= *tform;

        D2D1_MATRIX_3X2_F textMatrix = D2D1::Matrix3x2F(
            static_cast<FLOAT>(tf.m11()), static_cast<FLOAT>(tf.m12()),
            static_cast<FLOAT>(tf.m21()), static_cast<FLOAT>(tf.m22()),
            static_cast<FLOAT>(tf.dx()),  static_cast<FLOAT>(tf.dy()));

        // Concatenate: textMatrix * oldTransform
        D2D1_MATRIX_3X2_F combined;
        combined._11 = textMatrix._11 * oldTransform._11 + textMatrix._12 * oldTransform._21;
        combined._12 = textMatrix._11 * oldTransform._12 + textMatrix._12 * oldTransform._22;
        combined._21 = textMatrix._21 * oldTransform._11 + textMatrix._22 * oldTransform._21;
        combined._22 = textMatrix._21 * oldTransform._12 + textMatrix._22 * oldTransform._22;
        combined._31 = textMatrix._31 * oldTransform._11 + textMatrix._32 * oldTransform._21 + oldTransform._31;
        combined._32 = textMatrix._31 * oldTransform._12 + textMatrix._32 * oldTransform._22 + oldTransform._32;

        _deviceContext->SetTransform(combined);

        D2D1_RECT_F layoutRect = D2D1::RectF(0.0f, -baselineOffset, 100000.0f, 100000.0f);

        _deviceContext->DrawText(wtext.c_str(),
                      static_cast<UINT32>(wtext.size()),
                      _textFormat,
                      layoutRect,
                      _penBrush);

        _deviceContext->SetTransform(oldTransform);
    }
    else
    {
        // Keep the active canvas transform so the current clip stays aligned.
        D2D1_RECT_F layoutRect = D2D1::RectF(static_cast<FLOAT>(to.x()),
                                             static_cast<FLOAT>(to.y()) - baselineOffset,
                                             static_cast<FLOAT>(to.x()) + 100000.0f,
                                             static_cast<FLOAT>(to.y()) + 100000.0f);

        _deviceContext->DrawText(wtext.c_str(),
                      static_cast<UINT32>(wtext.size()),
                      _textFormat,
                      layoutRect,
                      _penBrush);
    }
}


void PixmapCanvas::onDrawImage(const Gfx::PointF& toF,
                               const Gfx::Image& image,
                               const Gfx::RectF* rect)
{
    if( ! _deviceContext)
        return;

    if(image.width() == 0 || image.height() == 0)
        return;

    // Convert image to BGRA for D2D
    const Pt::uint8_t* data = image.data();
    Gfx::Rgb32Image rgb32Image;

    if(image.format() != Gfx::ImageFormat::rgb32() || image.padding() != 0)
    {
        rgb32Image.reset(image.width(), image.height());
        Gfx::copyView(image, rgb32Image);
        data = rgb32Image.data();
    }

    D2D1_SIZE_U bmpSize = D2D1::SizeU(
        static_cast<UINT32>(image.width()),
        static_cast<UINT32>(image.height()));
    D2D1_BITMAP_PROPERTIES bmpProps = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED));

    ID2D1Bitmap* d2dBitmap = 0;
    HRESULT hr = _deviceContext->CreateBitmap(bmpSize, data,
                                   static_cast<UINT32>(image.width() * 4),
                                   bmpProps, &d2dBitmap);
    if(FAILED(hr))
        return;

    // Draw at logical coordinates; the active D2D transform handles mapping.
    const Gfx::Scaling& dstScaling = scaling();

    D2D1_RECT_F destRect;
    D2D1_RECT_F srcRect;
    D2D1_RECT_F* srcRectPtr = 0;

    if(rect)
    {
        srcRect = toD2DRect(*rect);
        srcRectPtr = &srcRect;
        destRect = D2D1::RectF(
            static_cast<FLOAT>(toF.x()),
            static_cast<FLOAT>(toF.y()),
            static_cast<FLOAT>(toF.x() + rect->width()),
            static_cast<FLOAT>(toF.y() + rect->height()));
    }
    else
    {
        double logW = dstScaling.toLogical(static_cast<double>(image.width()));
        double logH = dstScaling.toLogical(static_cast<double>(image.height()));

        destRect = D2D1::RectF(
            static_cast<FLOAT>(toF.x()),
            static_cast<FLOAT>(toF.y()),
            static_cast<FLOAT>(toF.x() + logW),
            static_cast<FLOAT>(toF.y() + logH));
    }

    _deviceContext->DrawBitmap(d2dBitmap, destRect, 1.0f,
                    D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
                    srcRectPtr);

    d2dBitmap->Release();
}


ID2D1PathGeometry* PixmapCanvas::createPathGeometry(const Gfx::Path& path)
{
    ID2D1PathGeometry* geom = 0;
    Application::instance().impl()->d2d().d2dFactory()->CreatePathGeometry(&geom);
    if( ! geom)
        return 0;

    ID2D1GeometrySink* sink = 0;
    geom->Open(&sink);
    if( ! sink)
    {
        geom->Release();
        return 0;
    }

    bool figureOpen = false;

    for(Gfx::PathIterator it = path.begin(); it != path.end(); ++it)
    {
        switch( it->type() )
        {
            default:
                break;

            case Gfx::Path::MoveTo:
            {
                if(figureOpen)
                    sink->EndFigure(D2D1_FIGURE_END_OPEN);

                const Gfx::PointF& to = it->point(0);
                sink->BeginFigure(
                    D2D1::Point2F(static_cast<FLOAT>(to.x()),
                                  static_cast<FLOAT>(to.y())),
                    D2D1_FIGURE_BEGIN_FILLED);
                figureOpen = true;
                break;
            }

            case Gfx::Path::LineTo:
            {
                if( ! figureOpen)
                {
                    sink->BeginFigure(D2D1::Point2F(0, 0),
                                      D2D1_FIGURE_BEGIN_FILLED);
                    figureOpen = true;
                }

                const Gfx::PointF& to = it->point(0);
                sink->AddLine(D2D1::Point2F(
                    static_cast<FLOAT>(to.x()),
                    static_cast<FLOAT>(to.y())));
                break;
            }

            case Gfx::Path::QuadTo:
            {
                if( ! figureOpen)
                {
                    sink->BeginFigure(D2D1::Point2F(0, 0),
                                      D2D1_FIGURE_BEGIN_FILLED);
                    figureOpen = true;
                }

                const Gfx::PointF& cp = it->point(0);
                const Gfx::PointF& to = it->point(1);

                D2D1_QUADRATIC_BEZIER_SEGMENT quad;
                quad.point1 = D2D1::Point2F(
                    static_cast<FLOAT>(cp.x()),
                    static_cast<FLOAT>(cp.y()));
                quad.point2 = D2D1::Point2F(
                    static_cast<FLOAT>(to.x()),
                    static_cast<FLOAT>(to.y()));

                sink->AddQuadraticBezier(quad);
                break;
            }

            case Gfx::Path::CubicTo:
            {
                if( ! figureOpen)
                {
                    sink->BeginFigure(D2D1::Point2F(0, 0),
                                      D2D1_FIGURE_BEGIN_FILLED);
                    figureOpen = true;
                }

                const Gfx::PointF& c1 = it->point(0);
                const Gfx::PointF& c2 = it->point(1);
                const Gfx::PointF& to = it->point(2);

                D2D1_BEZIER_SEGMENT bezier;
                bezier.point1 = D2D1::Point2F(
                    static_cast<FLOAT>(c1.x()),
                    static_cast<FLOAT>(c1.y()));
                bezier.point2 = D2D1::Point2F(
                    static_cast<FLOAT>(c2.x()),
                    static_cast<FLOAT>(c2.y()));
                bezier.point3 = D2D1::Point2F(
                    static_cast<FLOAT>(to.x()),
                    static_cast<FLOAT>(to.y()));

                sink->AddBezier(bezier);
                break;
            }

            case Gfx::Path::Close:
            {
                if(figureOpen)
                {
                    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
                    figureOpen = false;
                }
                break;
            }
        }
    }

    if(figureOpen)
        sink->EndFigure(D2D1_FIGURE_END_OPEN);

    sink->Close();
    sink->Release();

    return geom;
}


void PixmapCanvas::onSetPath(const Gfx::Path& path)
{
    if(_pathGeom)
    {
        _pathGeom->Release();
        _pathGeom = 0;
    }

    _ptPath = path;
    _pathGeom = createPathGeometry(path);
}


void PixmapCanvas::onDrawPath()
{
    if( ! _deviceContext || ! _penBrush || ! _pathGeom)
        return;

    _deviceContext->DrawGeometry(_pathGeom, _penBrush, _penWidth, _strokeStyle);
}


void PixmapCanvas::onFillPath()
{
    if( ! _deviceContext || ! _fillBrush || ! _pathGeom)
        return;

    if(isRelativeGradient())
    {
        D2D1_RECT_F bounds;
        if(SUCCEEDED(_pathGeom->GetBounds(nullptr, &bounds)))
            prepareGradientBrush(bounds);
    }

    _deviceContext->FillGeometry(_pathGeom, _fillBrush);
}


void PixmapCanvas::onDrawPath(const Gfx::Path& path)
{
    if( ! _deviceContext || ! _penBrush)
        return;

    ID2D1PathGeometry* geom = createPathGeometry(path);
    if( ! geom)
        return;

    _deviceContext->DrawGeometry(geom, _penBrush, _penWidth, _strokeStyle);
    geom->Release();
}


void PixmapCanvas::onFillPath(const Gfx::Path& path)
{
    if( ! _deviceContext || ! _fillBrush)
        return;

    ID2D1PathGeometry* geom = createPathGeometry(path);
    if( ! geom)
        return;

    if(isRelativeGradient())
    {
        D2D1_RECT_F bounds;
        if(SUCCEEDED(geom->GetBounds(nullptr, &bounds)))
            prepareGradientBrush(bounds);
    }

    _deviceContext->FillGeometry(geom, _fillBrush);
    geom->Release();
}

} // namespace

} // namespace
