/* Copyright (C) 2017-2017 Aloysius Indrayanto
   Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan

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

#include <Pt/Gfx/ImagePainter2.h>

#include "FreeType2.h"
#include "Rasterizer2.h"


namespace Pt {
namespace Gfx {


// ======================================================================================
// ===== Static Public Member Functions =================================================
// ======================================================================================

void ImagePainter2::setFontDir(const Pt::System::Path& path)
{ FreeType2::instance().setFontDir(path); }

void ImagePainter2::setDefaultFont(const std::string& f)
{ FreeType2::instance().setDefaultFont(f); }

std::string ImagePainter2::defaultFont()
{ return FreeType2::instance().defaultFont(); }

std::vector<std::string> ImagePainter2::fontNames()
{ return FreeType2::instance().fontNames(); }

FontMetrics ImagePainter2::fontMetrics( const Font& font, const Pt::String& text )
{ return Rasterizer2::fontMetrics(font, text); }


// ======================================================================================
// ===== Public Member Functions ========================================================
// ======================================================================================

ImagePainter2::ImagePainter2(Image& image)
: _rasterizer(new Rasterizer2(image))
{ setAntiAliasingMode(); /* Call the setter to enable the default anti-aliasing mode */ }

ImagePainter2::~ImagePainter2()
{ delete _rasterizer; }

void ImagePainter2::setAntiAliasingMode(AntiAliasingMode mode)
{ _rasterizer->setAntiAliasingMode(mode); }

void ImagePainter2::setImage(Image& image)
{ _rasterizer->setImage(image); }

const ImageFormat& ImagePainter2::format() const
{ return _rasterizer->format(); }

void ImagePainter2::setCompositionMode(const CompositionMode& mode)
{ _rasterizer->setCompositionMode(mode); }

const CompositionMode& ImagePainter2::compositionMode() const
{ return _rasterizer->compositionMode(); }

void ImagePainter2::setClip( const RectF& clipIn )
{
    const Rect clip(
        Point( (Pt::int32_t) clipIn.x    (), (Pt::int32_t) clipIn.y     () ),
        Size ( (Pt::int32_t) clipIn.width(), (Pt::int32_t) clipIn.height() )
    );

    _rasterizer->setClip(clip);
    _clip = clipIn;
}

const Gfx::RectF& ImagePainter2::clip() const
{ return _clip; }

void ImagePainter2::setPen( const Pen& pen )
{ _rasterizer->setPen(pen) ; }

const Pen& ImagePainter2::pen() const
{ return _rasterizer->pen(); }

void ImagePainter2::setBrush(const Brush& brush)
{ _rasterizer->setBrush(brush); }

const Brush& ImagePainter2::brush() const
{ return _rasterizer->brush(); }

void ImagePainter2::setFont(const Font& font)
{ _rasterizer->setFont( font ); }

const Font& ImagePainter2::font() const
{ return _rasterizer->font(); }

FontMetrics ImagePainter2::fontMetrics(const String& text) const
{ return _rasterizer->fontMetrics( text ); }

void ImagePainter2::drawImage( const PointF& toIn, const Image& image )
{
    const Point to( (Pt::int32_t) toIn.x(), (Pt::int32_t) toIn.y() );

    _rasterizer->image(to, image);
}

void ImagePainter2::drawImage( const PointF& toIn, const Image& image, const RectF& imageRect )
{
    const Point to( (Pt::int32_t) toIn.x(), (Pt::int32_t) toIn.y() );
    const Rect  ir(
        Point( (Pt::int32_t) imageRect.    x(), (Pt::int32_t) imageRect.     y() ),
        Size ( (Pt::int32_t) imageRect.width(), (Pt::int32_t) imageRect.height() )
    );

    _rasterizer->image(to, image, ir);
}

void ImagePainter2::drawText( const PointF& toIn, const String& text )
{
    const Point to( (Pt::int32_t) toIn.x(), (Pt::int32_t) toIn.y() );

    _rasterizer->strokeText( to, text );
}

void ImagePainter2::drawLine( const PointF& from, const PointF& to )
{
    // Rasterize one pixel line
    if(_rasterizer->pen().size() == 1) {
        // Copy the points
        const Point a( (Pt::int32_t) from.x(), (Pt::int32_t) from.y() );
        const Point b( (Pt::int32_t) to  .x(), (Pt::int32_t) to  .y() );
        // Rasterize the line
        _rasterizer->strokeOnePixelLine(a, b, 0);
        return;
    }

    // Generate a polygon that represents the thick line
    std::vector<PointF> pointsF;

    generateLineSegment(pointsF, from.x(), from.y(), to.x(), to.y(), true, true);

    // Rasterize the polygon
    std::vector<Point> points(pointsF.size());

    convertPointRound(points, pointsF.data(), pointsF.size());
    _rasterizer->strokePolygonSeparate(points.data(), points.size());
}

void ImagePainter2::drawRect( const RectF& rect )
{
    // Copy the points
    const Point tl( (Pt::int32_t) rect.topLeft    ().x(), (Pt::int32_t) rect.topLeft    ().y() );
    const Point br( (Pt::int32_t) rect.bottomRight().x(), (Pt::int32_t) rect.bottomRight().y() );

    // Rasterize the rectangle
    if(_rasterizer->pen().size() == 1) {
        _rasterizer->strokeOnePixelRect(tl, br);
        return;
    }

    // TODO: Implement rectangle with thick lines and joins using polygon here!
}

void ImagePainter2::fillRect( const RectF& rect )
{
    const Point tl( (Pt::int32_t) rect.topLeft    ().x(), (Pt::int32_t) rect.topLeft    ().y() );
    const Point br( (Pt::int32_t) rect.bottomRight().x(), (Pt::int32_t) rect.bottomRight().y() );

    _rasterizer->fillRect(tl, br);
}

void ImagePainter2::drawPolyline( const PointF* ps, const size_t pointCount, bool autoClose )
{
    // Copy the points
    std::vector<Point> points(pointCount);

    convertPointTrunc(points, ps, pointCount);

    // Rasterize the polygon
    if(_rasterizer->pen().size() == 1) {
        _rasterizer->strokeOnePixelPolygon(points.data(), pointCount, autoClose);
        return;
    }

    // TODO: Implement polyline with thick lines, joins, and caps here!
}

void ImagePainter2::fillPolygon( const PointF* ps, const size_t pointCount )
{
    // Copy the points
    std::vector<Point> points(pointCount);

    convertPointTrunc(points, ps, pointCount);

    // Rasterize the polygon
    _rasterizer->fillPolygon(points.data(), pointCount);
}

void ImagePainter2::drawPolybezier(const PointF* ps, const size_t pointCount, bool autoClose)
{
    // Check the number of points
    if(  autoClose && (pointCount < 4 ||  (pointCount & 1)) ) return; // The number of points must be >= 4 and even
    if( !autoClose && (pointCount < 3 || !(pointCount & 1)) ) return; // The number of points must be >= 3 and odd

    // Copy the points
    std::vector<Point> points(autoClose ? (pointCount + 1) : pointCount);

    convertPointTrunc(points, ps, pointCount);

    if(autoClose) points[pointCount].set( (Pt::int32_t) ps[0].x(), (Pt::int32_t) ps[0].y() );

    // Rasterize the bezier
    if(_rasterizer->pen().size() == 1) {
        _rasterizer->strokeOnePixelPolybezier(points.data(), points.size());
        return;
    }

    // TODO: Implement polybezier with thick lines using polygon here!
}

void ImagePainter2::drawEllipse( const PointF& topLeft, const SizeF& size )
{
    // Copy the points
    const Point tl( (Pt::int32_t) topLeft.x    (), (Pt::int32_t) topLeft.y     () );
    const Size  sz( (Pt::int32_t) size   .width(), (Pt::int32_t) size   .height() );

    // Rasterize the ellipse
    if(_rasterizer->pen().size() == 1) {
        _rasterizer->strokeOnePixelEllipseArc(tl, sz, 0, 0, ArcMode::Open);
        return;
    }

    // TODO: Implement ellipse with thick lines using polygon here!
}

void ImagePainter2::fillEllipse( const PointF& topLeft, const SizeF& size )
{
    // Copy the points
    const Point tl( (Pt::int32_t) topLeft.x    (), (Pt::int32_t) topLeft.y     () );
    const Size  sz( (Pt::int32_t) size   .width(), (Pt::int32_t) size   .height() );

    // Rasterize the ellipse
    _rasterizer->fillEllipse(tl, sz);
}

void ImagePainter2::drawArc( const PointF& topLeft, const SizeF& size, float degBegin, float degEnd, const ArcMode& arcMode )
{
    // Copy the points
    const Point tl( (Pt::int32_t) topLeft.x    (), (Pt::int32_t) topLeft.y     () );
    const Size  sz( (Pt::int32_t) size   .width(), (Pt::int32_t) size   .height() );

    // Rasterize the arc
    if(_rasterizer->pen().size() == 1) {
        _rasterizer->strokeOnePixelEllipseArc(tl, sz, degBegin, degEnd, arcMode);
        return;
    }

    // TODO: Implement arc with thick lines using polygon here!
}

void ImagePainter2::fillArc( const PointF& topLeft, const SizeF& size, float degBegin, float degEnd, const ArcMode& arcMode )
{
    // Copy the points
    const Point tl( (Pt::int32_t) topLeft.x    (), (Pt::int32_t) topLeft.y     () );
    const Size  sz( (Pt::int32_t) size   .width(), (Pt::int32_t) size   .height() );

    // Rasterize the arc
    _rasterizer->fillArc(tl, sz, degBegin, degEnd, arcMode);
}


// ======================================================================================
// ===== Private Member Functions ======================================================
// ======================================================================================

void ImagePainter2::generateLineSegment(std::vector<PointF>& dst, float x1, float y1, float x2, float y2, bool openingCap, bool closingCap)
{
    // Swap the points as needed; this also make sure that the line always faces top
    if(x1 > x2) {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    // Line equation : 0 = aX + By + c
    // Normal        : n = ai + bj
    const float w  = _rasterizer->pen().size();
    const float wh = w * 0.5f;
    const float a  = y2 - y1;
    const float b  = x1 - x2;
  //const float c  = -(x1 * y2 - x2 * y1);
    const float il = 1.0f / Gfx::Math::fastSqrt(a * a + b * b);
    const float dx = -b * il * wh;
    const float dy =  a * il * wh;
    const float nx =  a * il * wh;
    const float ny =  b * il * wh;

    // Generate points (CCW)
    // --- Begin point ---
    if(openingCap) {
        switch(_rasterizer->pen().capStyle()) {
            case Pen::SquareCap        : generateLineSquareCap       (dst, x1, y1, dx, dy, nx, ny); break;
            case Pen::RoundCap         : generateLineRoundCap        (dst, x1, y1, dx, dy, nx, ny); break;
            case Pen::TriangularOutCap : generateLineTriangularOutCap(dst, x1, y1, dx, dy, nx, ny); break;
            case Pen::TriangularInCap  : generateLineTriangularInCap (dst, x1, y1, dx, dy, nx, ny); break;
            default                    : openingCap = false;
        }
    }
    if(!openingCap) generateLineButtCap(dst, x1, y1, dx, dy, nx, ny);
    // --- End point ---
    if(closingCap) {
        switch(_rasterizer->pen().capStyle()) {
            case Pen::SquareCap        : generateLineSquareCap       (dst, x2, y2, -dx, -dy, -nx, -ny); break;
            case Pen::RoundCap         : generateLineRoundCap        (dst, x2, y2, -dx, -dy, -nx, -ny); break;
            case Pen::TriangularOutCap : generateLineTriangularOutCap(dst, x2, y2, -dx, -dy, -nx, -ny); break;
            case Pen::TriangularInCap  : generateLineTriangularInCap (dst, x2, y2, -dx, -dy, -nx, -ny); break;
            default                    : closingCap = false;
        }
    }
    if(!closingCap) generateLineButtCap(dst, x2, y2, -dx, -dy, -nx, -ny);
}


void ImagePainter2::generateLineButtCap(std::vector<PointF>& dst, float x, float y, float/*dx*/, float/*dy*/, float nx, float ny)
{
    dst.push_back( PointF(x + nx, y + ny) );
    dst.push_back( PointF(x - nx, y - ny) );
}


void ImagePainter2::generateLineSquareCap(std::vector<PointF>& dst, float x, float y, float dx, float dy, float nx, float ny)
{
    dst.push_back( PointF(x - dx + nx, y - dy + ny) );
    dst.push_back( PointF(x - dx - nx, y - dy - ny) );
}

void ImagePainter2::generateLineRoundCap(std::vector<PointF>& dst, float x, float y, float dx, float dy, float nx, float ny)
{
}

void ImagePainter2::generateLineTriangularOutCap(std::vector<PointF>& dst, float x, float y, float dx, float dy, float nx, float ny)
{
    dst.push_back( PointF(x + nx, y + ny) );
    dst.push_back( PointF(x - dx, y - dy) );
    dst.push_back( PointF(x - nx, y - ny) );
}

void ImagePainter2::generateLineTriangularInCap(std::vector<PointF>& dst, float x, float y, float dx, float dy, float nx, float ny)
{
    dst.push_back( PointF(x + nx, y + ny) );
    dst.push_back( PointF(x + dx, y + dy) );
    dst.push_back( PointF(x - nx, y - ny) );
}

void ImagePainter2::joinLineSegment(std::vector<PointF>& dst, const std::vector<PointF>& src)
{
}


} // namespace
} // namespace
