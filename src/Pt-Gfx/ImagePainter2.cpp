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

#include "AffineMatrix2D.h"
#include "FreeType2.h"
#include "Rasterizer2.h"


namespace Pt {
namespace Gfx {



// ======================================================================================
// ===== Internal Helper Functions ======================================================
// ======================================================================================

static bool intersectLine(bool& inLine, PointF& intersect, const PointF& line1a, const PointF& line1b, const PointF& line2a, const PointF& line2b)
{
    // The first line
    const float x11   = line1a.x();
    const float y11   = line1a.y();
    const float x12   = line1b.x();
    const float y12   = line1b.y();
    const float minX1 = std::min(x11, x12);
    const float minY1 = std::min(y11, y12);
    const float maxX1 = std::max(x11, x12);
    const float maxY1 = std::max(y11, y12);
    const float a1    = y12 - y11;
    const float b1    = x11 - x12;
    const float c1    = -(x11 * y12 - x12 * y11);

    // The second line
    const float x21   = line1a.x();
    const float y21   = line1a.y();
    const float x22   = line1b.x();
    const float y22   = line1b.y();
    const float minX2 = std::min(x21, x22);
    const float minY2 = std::min(y21, y22);
    const float maxX2 = std::max(x21, x22);
    const float maxY2 = std::max(y21, y22);
    const float a2    = y22 - y21;
    const float b2    = x21 - x22;
    const float c2    = -(x21 * y22 - x22 * y21);

    // Check if there is intersection
    const float denom = a1 * b2 - a2 * b1;
    if(denom == 0.0f) return false;

    // Calculate the intersection point
    const float intX  = (b1 * c2 - b2 * c1) / denom;
    const float intY  = (a2 * c1 - a1 * c2) / denom;

    // Determine if the intersection point is inside the line
    inLine = (intX >= minX1 && intX <= maxX1 && intY >= minY1 && intY <= maxY1)
           | (intX >= minX2 && intX <= maxX2 && intY >= minY2 && intY <= maxY2);

    // Done
    return true;
}


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
: _affineMatrix2D( new AffineMatrix2D() )
, _rasterizer    ( new Rasterizer2(image) )
{ setAntiAliasingMode(); /* Call the setter to enable the default anti-aliasing mode */ }

ImagePainter2::~ImagePainter2()
{
    delete _affineMatrix2D;
    delete _rasterizer;
}

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
    // Rasterize one-pixel line
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

    if(_rasterizer->pen().style() == Pen::Solid)
        generateSolidLineSegment(pointsF, from.x(), from.y(), to.x(), to.y(), true, true);
    else
        generatePatternedLineSegment(pointsF, from.x(), from.y(), to.x(), to.y(), true, true);

    // Rasterize the polygon
    std::vector<Point> points;

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
    // Rasterize one-pixel polyline
    if(_rasterizer->pen().size() == 1) {
        // Copy the points
        std::vector<Point> points;
        convertPointTrunc(points, ps, pointCount);
        // Rasterize the polygon
        _rasterizer->strokeOnePixelPolygon(points.data(), pointCount, autoClose);
        return;
    }


    // Separate the polygons convert them and recombine them
    size_t startIndex = 0;

    std::vector<PointF> pointsF;
    pointsF.reserve(pointCount * 2);

    for(size_t i = 0; i <= pointCount; ++i) {
        // Search for the end and/or separator points
        if( i == pointCount || (ps[i].x() > MAXIMUM_COORD && ps[i].y() > MAXIMUM_COORD) ) {
            // Get the base pointer and the number of points for this polygon
            const PointF* basePtr = ps + startIndex;
                  size_t  curPCnt = i - startIndex;
            // Determine if this polygon is a closed polygon
            bool closedPolygon = autoClose;
            if(basePtr[0] == basePtr[curPCnt - 1]) {
                closedPolygon = true;
                --curPCnt;
            }
            // Thicken the polygon
            if(closedPolygon) {
                if(!thickenClosedPolygon(pointsF, basePtr, curPCnt)) return;
            }
            else {
                if(!thickenOpenPolygon(pointsF, basePtr, curPCnt)) return;
            }
            // Update the start index
            startIndex = i + 1;
        }
    }

    // Rasterize the polygon
    std::vector<Point> points;

    convertPointRound(points, pointsF.data(), pointsF.size());
    _rasterizer->strokePolygonSeparate(points.data(), points.size());
}

void ImagePainter2::fillPolygon( const PointF* ps, const size_t pointCount )
{
    // Copy the points
    std::vector<Point> points;

    convertPointTrunc(points, ps, pointCount);

    // Rasterize the polygon
    _rasterizer->fillPolygon(points.data(), pointCount);
}

void ImagePainter2::drawQuadraticPolybezier(const PointF* ps, const size_t pointCount, bool autoClose)
{
    // Check the number of points
    if(  autoClose && (pointCount < 4 ||  (pointCount & 1)) ) return; // The number of points must be >= 4 and even
    if( !autoClose && (pointCount < 3 || !(pointCount & 1)) ) return; // The number of points must be >= 3 and odd

    // Copy the points
    std::vector<Point> points;
    if(autoClose) points.reserve(pointCount + 1);

    convertPointTrunc(points, ps, pointCount);

    if(autoClose) points.push_back( Point( (Pt::int32_t) ps[0].x(), (Pt::int32_t) ps[0].y() ) );

    // Rasterize the bezier
    if(_rasterizer->pen().size() == 1) {
        _rasterizer->strokeOnePixelQuadraticPolybezier(points.data(), points.size());
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

void ImagePainter2::generateSolidLineSegment(std::vector<PointF>& dst, float x1, float y1, float x2, float y2, bool openingCap, bool closingCap)
{
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
            case Pen::SquareCap        : generateLineSquareCap       (dst, x1, y1, wh, dx, dy, nx, ny); break;
            case Pen::RoundCap         : generateLineRoundCap        (dst, x1, y1, wh, dx, dy, nx, ny); break;
            case Pen::TriangularOutCap : generateLineTriangularOutCap(dst, x1, y1, wh, dx, dy, nx, ny); break;
            case Pen::TriangularInCap  : generateLineTriangularInCap (dst, x1, y1, wh, dx, dy, nx, ny); break;
            default                    : openingCap = false;
        }
    }
    if(!openingCap) generateLineButtCap(dst, x1, y1, wh, dx, dy, nx, ny);
    // --- End point ---
    if(closingCap) {
        switch(_rasterizer->pen().capStyle()) {
            case Pen::SquareCap        : generateLineSquareCap       (dst, x2, y2, wh, -dx, -dy, -nx, -ny); break;
            case Pen::RoundCap         : generateLineRoundCap        (dst, x2, y2, wh, -dx, -dy, -nx, -ny); break;
            case Pen::TriangularOutCap : generateLineTriangularOutCap(dst, x2, y2, wh, -dx, -dy, -nx, -ny); break;
            case Pen::TriangularInCap  : generateLineTriangularInCap (dst, x2, y2, wh, -dx, -dy, -nx, -ny); break;
            default                    : closingCap = false;
        }
    }
    if(!closingCap) generateLineButtCap(dst, x2, y2, wh, -dx, -dy, -nx, -ny);
}

void ImagePainter2::generatePatternedLineSegment(std::vector<PointF>& dst, float x1, float y1, float x2, float y2, bool openingCap, bool closingCap)
{
    // ### TODO ###
}

void ImagePainter2::generateLineButtCap(std::vector<PointF>& dst, float x, float y, float wh, float px, float py, float nx, float ny)
{
    /*
    static bool f = true;
    if(f) {
        f = false;
        _rasterizer->strokeText(Point(x + nx, y + ny), "0");
        _rasterizer->strokeText(Point(x - nx, y - ny), "1");
    }
    else {
        _rasterizer->strokeText(Point(x + nx, y + ny), "2");
        _rasterizer->strokeText(Point(x - nx, y - ny), "3");
    }
    return;
    */

    (void) wh;
    (void) px;
    (void) py;

    dst.push_back( PointF(x + nx, y + ny) );
    dst.push_back( PointF(x - nx, y - ny) );
}

void ImagePainter2::generateLineSquareCap(std::vector<PointF>& dst, float x, float y, float wh, float px, float py, float nx, float ny)
{
    (void) wh;

    dst.push_back( PointF(x - px + nx, y - py + ny) );
    dst.push_back( PointF(x - px - nx, y - py - ny) );
}

// Based on: Bitmap/Bézier curves/Quadratic
//           https://rosettacode.org/wiki/Bitmap/B%C3%A9zier_curves/Quadratic#C
//           Last modified on February 17, 2017
void ImagePainter2::generateLineRoundCap(std::vector<PointF>& dst, float x, float y, float wh, float px, float py, float nx, float ny)
{
    // Determine the coordinates
    const float x1 = round(x + nx);
    const float y1 = round(y + ny);
    const float x2 = round(x - px * 2.0f);
    const float y2 = round(y - py * 2.0f);
    const float x3 = round(x - nx);
    const float y3 = round(y - ny);

    // Generate the points
    Pt::int32_t nSeg = round(wh) - 1;
    if(nSeg < 3) nSeg = 3;

    for(Pt::int32_t i = 0; i <= nSeg; ++i) {
        const float t = (float) i / (float) nSeg;
        const float a = (1.0f - t) * (1.0f - t);
        const float b =  2.0f * t  * (1.0f - t);
        const float c = t * t;
        const float x = a * x1 + b * x2 + c * x3;
        const float y = a * y1 + b * y2 + c * y3;
        if( dst.empty() || (dst.back().x() != x && dst.back().y() != y) ) dst.push_back( PointF(x, y) );
    }
}

void ImagePainter2::generateLineTriangularOutCap(std::vector<PointF>& dst, float x, float y, float wh, float px, float py, float nx, float ny)
{
    /*
    static bool f = true;
    if(f) {
        f = false;
        _rasterizer->strokeText(Point(x + nx, y + ny), "0");
        _rasterizer->strokeText(Point(x - px, y - py), "1");
        _rasterizer->strokeText(Point(x - nx, y - ny), "2");
    }
    else {
        _rasterizer->strokeText(Point(x + nx, y + ny), "3");
        _rasterizer->strokeText(Point(x - px, y - py), "4");
        _rasterizer->strokeText(Point(x - nx, y - ny), "5");
    }
    return;
    */

    (void) wh;

    dst.push_back( PointF(x + nx, y + ny) );
    dst.push_back( PointF(x - px, y - py) );
    dst.push_back( PointF(x - nx, y - ny) );
}

void ImagePainter2::generateLineTriangularInCap(std::vector<PointF>& dst, float x, float y, float wh, float px, float py, float nx, float ny)
{
    (void) wh;

    dst.push_back( PointF(x + nx, y + ny) );
    dst.push_back( PointF(x + px, y + py) );
    dst.push_back( PointF(x - nx, y - ny) );
}

bool ImagePainter2::thickenOpenPolygon(std::vector<PointF>& pointsF, const PointF* basePtr, size_t curPCnt)
{
    std::vector<PointF> pointsFPolygon;
    std::vector<PointF> pointsFOuter;
    std::vector<PointF> pointsFSegment;

    const size_t curPC1 = curPCnt - 1;
    const size_t curPC2 = curPCnt - 2;

    // Walk thorugh the polygon's lines
    for(size_t i = 0; i < curPC1; ++i) {
        const PointF& from = *basePtr++;
        const PointF& to   = *basePtr;
        if(_rasterizer->pen().style() == Pen::Solid) {
            pointsFSegment.clear();
            generateSolidLineSegment(pointsFSegment, from.x(), from.y(), to.x(), to.y(), i == 0, i == curPC2);
            if(!combineLineSegmentForOpenPolygon(pointsFPolygon, pointsFOuter, pointsFSegment, i == 1, i == curPC2)) return false;
        }
        else {
            // ### TODO ###
            return false;
        }
    }

    finalizeLineSegmentForOpenPolygon(pointsFPolygon, pointsFOuter);

    // Combine the polygon data
    if(!pointsF.empty()) pointsF.push_back(Painter::PolygonSeparatorPointF);
    pointsF.insert(pointsF.end(), pointsFPolygon.begin(), pointsFPolygon.end());

    // Done
    return true;
}

bool ImagePainter2::combineLineSegmentForOpenPolygon(std::vector<PointF>& polygon, std::vector<PointF>& outer, const std::vector<PointF>& segment, bool isBeg, bool isEnd)
{
    // If the target polygon is still empty, simply copy the points
    if(polygon.empty()) {
        polygon.insert(polygon.end(), segment.begin(), segment.end());
        return true;
    }

    // Get the width of the pen
    const size_t penWidth = _rasterizer->pen().size();

    // Combine the segments
    std::vector<PointF> proc;
    proc.reserve(polygon.size() + segment.size() + penWidth + 3);

    // Store points #0 to #(N-2) from the current polygon
    const size_t N = polygon.size() - 1;
    for(size_t i = 0; i <= N - 2; ++i) proc.push_back(polygon[i]);

    // Get the "outside" lines
    const PointF* line1a = &polygon[1];
    const PointF* line1b = &polygon[2];
    const PointF* line2a = &segment[1];
    const PointF* line2b = &segment[2];
    if(isBeg) {
        line1a = &polygon[N - 2];
        line1b = &polygon[N - 1];
    }

    // Intersect the "outside" lines
    bool   inLine;
    PointF intersect;
    if(!intersectLine(inLine, intersect, *line1a, *line1b, *line2a, *line2b)) return false;

    // Store the "outside" line's points

    // Done
    return true;
}

void ImagePainter2::finalizeLineSegmentForOpenPolygon(std::vector<PointF>& polygon, const std::vector<PointF>& outer)
{
    // ### TODO ###
}

bool ImagePainter2::thickenClosedPolygon(std::vector<PointF>& pointsF, const PointF* basePtr, size_t curPCnt)
{
    // ### TODO ###
    return false;
}

bool ImagePainter2::combineLineSegmentForClosedPolygon(std::vector<PointF>& outer, std::vector<PointF>& inner, const std::vector<PointF>& segment)
{
    // ### TODO ###
    return false;
}

void ImagePainter2::finalizeLineSegmentForClosedPolygon(std::vector<PointF>& outer, std::vector<PointF>& inner, const std::vector<PointF>& segment)
{
    // ### TODO ###
}


} // namespace
} // namespace
