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

// Based on: Bitmap/Bézier curves/Quadratic
//           https://rosettacode.org/wiki/Bitmap/B%C3%A9zier_curves/Quadratic#C
//           Last modified on February 17, 2017
static inline void generateQuadraticBezierPoints(std::vector<PointF>& dst, float x1, float y1, float x2, float y2, float x3, float y3, Pt::int32_t nSegs)
{
    // Check if the points actually specify a straight line
    const Pt::int32_t sx = x3 - x2;
    const Pt::int32_t sy = y3 - y2;
    const Pt::int32_t xx = x1 - x2;
    const Pt::int32_t yy = y1 - y2;

    // Curvature
    if( !(xx * sy - yy * sx) ) {
        if( dst.empty() || dst.back().x() != x1 || dst.back().y() != y1 ) dst.push_back( PointF(x1, y1) );
        if( dst.empty() || dst.back().x() != x3 || dst.back().y() != y3 ) dst.push_back( PointF(x3, y3) );
        return;
    }

    // Process as a quadratic bezier curve
    if(nSegs < 3) nSegs = 3;

    for(Pt::int32_t i = 0; i <= nSegs; ++i) {
        const float t = (float) i / (float) nSegs;
        const float a = (1.0f - t) * (1.0f - t);
        const float b =  2.0f * t  * (1.0f - t);
        const float c = t * t;
        const float x = a * x1 + b * x2 + c * x3;
        const float y = a * y1 + b * y2 + c * y3;
        if( dst.empty() || dst.back().x() != x || dst.back().y() != y ) dst.push_back( PointF(x, y) );
    }
}

static inline void generateEllipsePoints(std::vector<Point>& dst, Pt::int32_t radiusX, Pt::int32_t radiusY, Pt::int32_t centerX, Pt::int32_t centerY)
{
    // Calculate the ellipse's parameters
    const Pt::int32_t radiusM = std::max(radiusX, radiusY);
    const Pt::int32_t numSegs = (radiusM * 2 / 3 / 20) * 20;

    // Generate a polygon that approximates the ellipse
    for(Pt::int32_t i = 0; i <= numSegs; ++i) {
        const float angle = Gfx::Math::PiMul2 * i / numSegs;
        // Calculate the coordinate
        const Pt::int32_t x = round( centerX + radiusX * Gfx::Math::fastCos(angle) );
        const Pt::int32_t y = round( centerY - radiusY * Gfx::Math::fastSin(angle) ); // Sign inversion due to differences between cartesian and computer coordinate systems
        // Store the coordinate only if it is different with the previous one
        if( !dst.empty() && dst.back().x() == x && dst.back().y() == y ) continue;
        dst.push_back( Point(x, y) );
    }

    // Discard the last point if it has the same coordinate with the first one
    if(dst.back() == dst[0]) dst.pop_back();
}

static inline void generateArcPoints(std::vector<Point>& dst, Pt::int32_t radiusX, Pt::int32_t radiusY, Pt::int32_t centerX, Pt::int32_t centerY, float degBegin, float degEnd)
{
    // Calculate the ellipse's parameters
    const Pt::int32_t radiusM = std::max(radiusX, radiusY);
    const Pt::int32_t deltaDg = degEnd - degBegin;
    const Pt::int32_t numSegs = (radiusM * 2 * deltaDg / 180 / 3 / 20) * 20;
    const float       fdegInc = (deltaDg * Gfx::Math::PiDiv180) / numSegs;

    // Generate a polygon that approximates the arc
    float angle = degBegin * Gfx::Math::PiDiv180;

    for(Pt::int32_t i = 0; i <= numSegs; ++i) {
        // Calculate the coordinate
        const Pt::int32_t x = round( centerX + radiusX * Gfx::Math::fastCos(angle) );
        const Pt::int32_t y = round( centerY - radiusY * Gfx::Math::fastSin(angle) ); // Sign inversion due to differences between cartesian and computer coordinate systems
        // Update the angle
        angle += fdegInc;
        // Store the coordinate only if it is different with the previous one
        if( !dst.empty() && dst.back().x() == x && dst.back().y() == y ) continue;
        dst.push_back( Point(x, y) );
    }

    // Discard the last point if it has the same coordinate with the first one
    if(dst.back() == dst[0]) dst.pop_back();
}

static inline void generateLineButtCap(std::vector<PointF>& dst, float x, float y, float nx, float ny)
{
    dst.push_back( PointF(x + nx, y + ny) );
    dst.push_back( PointF(x - nx, y - ny) );
}

static inline void generateLineSquareCap(std::vector<PointF>& dst, float x, float y, float dx, float dy, float nx, float ny)
{
    dst.push_back( PointF(x - dx + nx, y - dy + ny) );
    dst.push_back( PointF(x - dx - nx, y - dy - ny) );
}

static inline void generateLineRoundCap(std::vector<PointF>& dst, float x, float y, float wh, float dx, float dy, float nx, float ny)
{
    generateQuadraticBezierPoints(
        dst,
        round(x + nx       ), round(y + ny       ),
        round(x - dx * 2.0f), round(y - dy * 2.0f),
        round(x - nx       ), round(y - ny       ),
        ceil(wh) - 1
    );
}

static inline void generateLineTriangularOutCap(std::vector<PointF>& dst, float x, float y, float dx, float dy, float nx, float ny)
{
    dst.push_back( PointF(x + nx, y + ny) );
    dst.push_back( PointF(x - dx, y - dy) );
    dst.push_back( PointF(x - nx, y - ny) );
}

static inline void generateLineTriangularInCap(std::vector<PointF>& dst, float x, float y, float dx, float dy, float nx, float ny)
{
    dst.push_back( PointF(x + nx, y + ny) );
    dst.push_back( PointF(x + dx, y + dy) );
    dst.push_back( PointF(x - nx, y - ny) );
}

static inline void calculateLineParams(float& wh, float& dx, float& dy, float& nx, float& ny, float x1, float y1, float x2, float y2, size_t w)
{
    // Line equation : 0 = aX + By + c
    // Normal        : n = ai + bj
    const float a = y2 - y1;
    const float b = x1 - x2;
  //const float c = -(x1 * y2 - x2 * y1);

    // Inverse line length
    const float il = 1.0f / Gfx::Math::fastSqrt(a * a + b * b);

    // Half line width
    wh = (float) w * 0.5f;

    // Direction vector
    dx = -b * il * wh;
    dy =  a * il * wh;

    // Normal vector vector
    nx =  a * il * wh;
    ny =  b * il * wh;
}

static inline bool intersectLine(bool& inLine, PointF& intersect, const PointF& line1a, const PointF& line1b, const PointF& line2a, const PointF& line2b)
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
    const float x21   = line2a.x();
    const float y21   = line2a.y();
    const float x22   = line2b.x();
    const float y22   = line2b.y();
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
    const float idenom = 1.0f / denom;
    const float ipX    = (b1 * c2 - b2 * c1) * idenom;
    const float ipY    = (a2 * c1 - a1 * c2) * idenom;

    intersect.set(ipX, ipY);

    // Determine if the intersection point is inside the line
    inLine = (ipX >= minX1 && ipX <= maxX1 && ipY >= minY1 && ipY <= maxY1)
           | (ipX >= minX2 && ipX <= maxX2 && ipY >= minY2 && ipY <= maxY2);

    // Done
    //lprintf("Line 1       : (%7.3f, %7.3f) - (%7.3f, %7.3f)\n", x11, y11, x12, y12);
    //lprintf("Line 2       : (%7.3f, %7.3f) - (%7.3f, %7.3f)\n", x21, y21, x22, y22);
    //lprintf("Intersection : (%7.3f, %7.3f) - %s \n", ipX, ipY, inLine ? "inline" : "outline");
    //lprintf("\n");
    return true;
}

static inline void combineLinePointsAndAddCaps(std::vector<Point>& dst, const std::vector<Point>& inner, const std::vector<Point>& outer, Pen::CapStyle begCap, Pen::CapStyle endCap, size_t penSize)
{
    // Calculate the end lines' parameters
    const Pt::int32_t ox2a = outer[outer.size() - 1].x();
    const Pt::int32_t oy2a = outer[outer.size() - 1].y();
    const Pt::int32_t ox2b = outer[outer.size() - 2].x();
    const Pt::int32_t oy2b = outer[outer.size() - 2].y();
    const Pt::int32_t ix2a = inner[inner.size() - 1].x();
    const Pt::int32_t iy2a = inner[inner.size() - 1].y();
    const Pt::int32_t ix2b = inner[inner.size() - 2].x();
    const Pt::int32_t iy2b = inner[inner.size() - 2].y();
    const float       x2a  = (float) (ox2a + ix2a) * 0.5f;
    const float       y2a  = (float) (oy2a + iy2a) * 0.5f;
    const float       x2b  = (float) (ox2b + ix2b) * 0.5f;
    const float       y2b  = (float) (oy2b + iy2b) * 0.5f;

    // Intersect the end lines
    float wh2, dx2, dy2, nx2, ny2;
    calculateLineParams(wh2, dx2, dy2, nx2, ny2, x2a, y2a, x2b, y2b, penSize);

    // Generate the end cap
    switch(endCap) {
        case Pen::SquareCap:
            dst.push_back( Point( ix2a - dx2, iy2a - dy2 ) );
            dst.push_back( Point( ox2a - dx2, oy2a - dy2 ) );
            break;

        case Pen::RoundCap: {
            std::vector<PointF> tmp;
            generateQuadraticBezierPoints(tmp, ix2a, iy2a, x2a - dx2 * 2.0f, y2a - dy2 * 2.0f, ox2a, oy2a, ceil(penSize * 0.5f) - 1);
            if(tmp.size() <= 2) break;
            for(size_t i = 1; i < tmp.size() - 1; ++i) {
                dst.push_back( Point( round(tmp[i].x()), round(tmp[i].y()) ) );
            }
            break;
        }

        case Pen::TriangularOutCap:
            dst.push_back( Point( x2a - dx2, y2a - dy2 ) );
            break;

        case Pen::TriangularInCap:
            dst.push_back( Point( x2a + dx2, y2a + dy2 ) );
            break;

        default:
            break;
    }

    // Store the "outside" points
    dst.insert(dst.end(), outer.rbegin(), outer.rend());

    // Calculate the begin lines' parameters
    const Pt::int32_t ox1a = outer[0].x();
    const Pt::int32_t oy1a = outer[0].y();
    const Pt::int32_t ox1b = outer[1].x();
    const Pt::int32_t oy1b = outer[1].y();
    const Pt::int32_t ix1a = inner[0].x();
    const Pt::int32_t iy1a = inner[0].y();
    const Pt::int32_t ix1b = inner[1].x();
    const Pt::int32_t iy1b = inner[1].y();
    const float       x1a  = (float) (ox1a + ix1a) * 0.5f;
    const float       y1a  = (float) (oy1a + iy1a) * 0.5f;
    const float       x1b  = (float) (ox1b + ix1b) * 0.5f;
    const float       y1b  = (float) (oy1b + iy1b) * 0.5f;

    // Intersect the begin lines
    float wh1, dx1, dy1, nx1, ny1;
    calculateLineParams(wh1, dx1, dy1, nx1, ny1, x1b, y1b, x1a, y1a, penSize);

    // Generate the begin cap
    switch(begCap) {
        case Pen::SquareCap:
            dst.push_back( Point( ox1a + dx1, oy1a + dy1 ) );
            dst.push_back( Point( ix1a + dx1, iy1a + dy1 ) );
            break;

        case Pen::RoundCap: {
            std::vector<PointF> tmp;
            generateQuadraticBezierPoints(tmp, ox1a, oy1a, x1a + dx1 * 2.0f, y1a + dy1 * 2.0f, ix1a, iy1a, ceil(penSize * 0.5f) - 1);
            if(tmp.size() <= 2) break;
            for(size_t i = 1; i < tmp.size() - 1; ++i) {
                dst.push_back( Point( round(tmp[i].x()), round(tmp[i].y()) ) );
            }
            break;
        }

        case Pen::TriangularOutCap:
            dst.push_back( Point( x1a + dx1, y1a + dy1 ) );
            break;

        case Pen::TriangularInCap:
            dst.push_back( Point( x1a - dx1, y1a - dy1 ) );
            break;

        default:
            break;
    }

    // Store the "inside" points
    dst.insert(dst.end(), inner. begin(), inner. end());
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

    _rasterizer->blitImage(to, image);
}

void ImagePainter2::drawImage( const PointF& toIn, const Image& image, const RectF& imageRect )
{
    const Point to( (Pt::int32_t) toIn.x(), (Pt::int32_t) toIn.y() );
    const Rect  ir(
        Point( (Pt::int32_t) imageRect.    x(), (Pt::int32_t) imageRect.     y() ),
        Size ( (Pt::int32_t) imageRect.width(), (Pt::int32_t) imageRect.height() )
    );

    _rasterizer->blitImage(to, image, ir);
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
    // Rasterize one-pixel rectangle
    if(_rasterizer->pen().size() == 1) {
        // Copy the points
        const Point tl( (Pt::int32_t) rect.topLeft    ().x(), (Pt::int32_t) rect.topLeft    ().y() );
        const Point br( (Pt::int32_t) rect.bottomRight().x(), (Pt::int32_t) rect.bottomRight().y() );
        // Rasterize the rectangle
        _rasterizer->strokeOnePixelRect(tl, br);
        return;
    }

    // Generate a polygon that represents the rectangle
    const PointF pointsF[4] = {
        rect.bottomLeft(), rect.bottomRight(), rect.topRight(), rect.topLeft()
    };
    drawPolyline(pointsF, 4, true);
}

void ImagePainter2::fillRect( const RectF& rect )
{
    const Point tl( (Pt::int32_t) rect.topLeft    ().x(), (Pt::int32_t) rect.topLeft    ().y() );
    const Point br( (Pt::int32_t) rect.bottomRight().x(), (Pt::int32_t) rect.bottomRight().y() );

    _rasterizer->fillRect(tl, br);
}

void ImagePainter2::drawRoundRect( const RectF& rect, float radius )
{
    // Extract the coordinates
    const float x1 = rect.topLeft    ().x();
    const float y1 = rect.topLeft    ().y();
    const float x2 = rect.bottomRight().x();
    const float y2 = rect.bottomRight().y();

    // Generate a quadratic polybezier that represents the rounded-rectangle
    const PointF pbz[] = { // CCW
        // Bottom left
        PointF(x1         , y2 - radius),
        PointF(x1         , y2         ),
        PointF(x1 + radius, y2         ),
        // Bottom middle
        PointF((x1 + x2) * 0.5f, y2),
        // Bottom right
        PointF(x2 - radius, y2         ),
        PointF(x2         , y2         ),
        PointF(x2         , y2 - radius),
        // Center right
        PointF(x2, (y1 + y2) * 0.5f),
        // Top right
        PointF(x2         , y1 + radius),
        PointF(x2         , y1         ),
        PointF(x2 - radius, y1         ),
        // Top middle
        PointF((x1 + x2) * 0.5f, y1),
        // Top left
        PointF(x1 + radius, y1         ),
        PointF(x1         , y1         ),
        PointF(x1         , y1 + radius),
        // Center left
        PointF(x1, (y1 + y2) * 0.5f)
    };

    // Save the original pen and create a new pen with miter join
    const Pen orgPen = _rasterizer->pen();

    Pen newPen = orgPen;
    newPen.setJoinStyle(Pen::MiterJoin);

    // Draw the quadratic polybezier
    _rasterizer->setPen(newPen);
    drawQuadraticPolybezier(pbz, sizeof(pbz) / sizeof(pbz[0]), true);
    _rasterizer->setPen(orgPen);
}

void ImagePainter2::fillRoundRect( const RectF& rect, float radius )
{
    // Extract the coordinates
    const float x1 = rect.topLeft    ().x();
    const float y1 = rect.topLeft    ().y();
    const float x2 = rect.bottomRight().x();
    const float y2 = rect.bottomRight().y();

    // Generate a polygon that represents the rounded-rectangle
    const Pt::int32_t   nSegs = ceil(_rasterizer->pen().size() * 0.5f) - 1;
    std::vector<PointF> pointsF; // CCW

    // --- Bottom left ---
    generateQuadraticBezierPoints(
        pointsF,
        x1         , y2 - radius,
        x1         , y2         ,
        x1 + radius, y2         ,
        nSegs
    );
    // --- Bottom middle ---
    pointsF.push_back( PointF((x1 + x2) * 0.5f, y2) );
    // --- Bottom left ---
    generateQuadraticBezierPoints(
        pointsF,
        x2 - radius, y2         ,
        x2,          y2         ,
        x2,          y2 - radius,
        nSegs
    );
    // --- Center right ---
    pointsF.push_back( PointF(x2, (y1 + y2) * 0.5f) );
    // --- Top right ---
    generateQuadraticBezierPoints(
        pointsF,
        x2,          y1 + radius,
        x2,          y1         ,
        x2 - radius, y1         ,
        nSegs
    );
    // --- Top middle ---
    pointsF.push_back( PointF((x1 + x2) * 0.5f, y1) );
    // --- Top left ---
    generateQuadraticBezierPoints(
        pointsF,
        x1 + radius, y1         ,
        x1,          y1         ,
        x1,          y1 + radius,
        nSegs
    );
    // --- Center left ---
    pointsF.push_back( PointF(x1, (y1 + y2) * 0.5f) );

    // Draw the polygon
    fillPolygon(pointsF.data(), pointsF.size());
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

    // Rasterize thick polyline
    drawThickPolyline_impl(ps, pointCount, autoClose, 0);
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

    // Rasterize one-pixel polybezier
    if(_rasterizer->pen().size() == 1) {
        // Prepare the buffer
        std::vector<Point> points;
        points.reserve(autoClose ? (pointCount + 1) : pointCount);
        // Copy the points
        convertPointTrunc(points, ps, pointCount);
        if(autoClose) points.push_back( Point( (Pt::int32_t) ps[0].x(), (Pt::int32_t) ps[0].y() ) );
        // Rasterize the bezier
        _rasterizer->strokeOnePixelQuadraticPolybezier(points.data(), points.size());
        return;
    }

    // Check if there is no actual point
    if(!pointCount) return;

    // Generate a polygon that approximates the polybezier
    const size_t         adjPC = autoClose ? pointCount : (pointCount - 1);
    std::vector<PointF>  pointsF, pointsFTmp;
    std::vector<int32_t> segmentIndexMarker;

    for(size_t i = 0; i < adjPC; i += 2) {
        // Calculate the coordinates and length
        const bool  lp   = ( autoClose && i == (adjPC - 2) );
        const float x1   = ps[           i      ].x();
        const float y1   = ps[           i      ].y();
        const float x2   = ps[           i + 1  ].x();
        const float y2   = ps[           i + 1  ].y();
        const float x3   = ps[ lp ? 0 : (i + 2) ].x();
        const float y3   = ps[ lp ? 0 : (i + 2) ].y();
        const float dx32 = x3 - x2;
        const float dy32 = y3 - y2;
        const float dx21 = x2 - x1;
        const float dy21 = y2 - y1;
        const float l32  = Gfx::Math::fastSqrt(dx32 * dx32 + dy32 * dy32);
        const float l21  = Gfx::Math::fastSqrt(dx21 * dx21 + dy21 * dy21);
        const float l31  = l32 + l21;
        // Generate points for one quadratic bezier curve
        pointsFTmp.clear();
        generateQuadraticBezierPoints(pointsFTmp, x1, y1, x2, y2, x3, y3, ceil(l31 / 16.0f) + 1);
        // Concat the points
        for(size_t j = 0; j < pointsFTmp.size(); ++j) {
            // Skip similar points
            if(!pointsF.empty()) {
                const float dx = ::fabs( pointsF.back().x() - pointsFTmp[j].x() );
                const float dy = ::fabs( pointsF.back().y() - pointsFTmp[j].y() );
                if(dx < 0.5f && dy < 0.5f) continue;
            }
            // Store the points
            pointsF.push_back(pointsFTmp[j]);
        }
        // Put the segment index marker
        segmentIndexMarker.push_back(pointsF.size() - 1);
    }
    segmentIndexMarker.push_back(-1);

    // Rasterize the polygon
    drawThickPolyline_impl(pointsF.data(), pointsF.size(), false, segmentIndexMarker.data());
}

void ImagePainter2::drawEllipse( const PointF& topLeft, const SizeF& size )
{
    // Rasterize one-pixel ellipse
    if(_rasterizer->pen().size() == 1) {
        // Copy the points
        const Point tl( (Pt::int32_t) topLeft.x    (), (Pt::int32_t) topLeft.y     () );
        const Size  sz( (Pt::int32_t) size   .width(), (Pt::int32_t) size   .height() );
        // Rasterize the ellipse
        _rasterizer->strokeOnePixelEllipseArc(tl, sz, 0, 0, ArcMode::Open);
        return;
    }

    // Save the original pen and create a new pen with miter join
    const Pen orgPen = _rasterizer->pen();

    Pen newPen = orgPen;
    newPen.setJoinStyle(Pen::MiterJoin);

    // Solid
    if(_rasterizer->pen().style() == Pen::Solid) {
        // Calculate the ellipse's parameters
        const size_t      penSize  = _rasterizer->pen().size();
        const Pt::int32_t radiusXo = ( size.width () + penSize ) / 2;
        const Pt::int32_t radiusYo = ( size.height() + penSize ) / 2;
        const Pt::int32_t radiusXi = ( size.width () - penSize ) / 2;
        const Pt::int32_t radiusYi = ( size.height() - penSize ) / 2;
        const Pt::int32_t centerX  = topLeft.x() + size.width () / 2;
        const Pt::int32_t centerY  = topLeft.y() + size.height() / 2;
        // Generate a polygon that approximates the ellipse
        std::vector<Point> points;
        generateEllipsePoints(points, radiusXo, radiusYo, centerX, centerY);
        points.push_back(Painter::PolygonSeparatorPoint);
        generateEllipsePoints(points, radiusXi, radiusYi, centerX, centerY);
        // Rasterize the polygon
        _rasterizer->setPen(newPen);
        _rasterizer->strokePolygon(points.data(), points.size());
        _rasterizer->setPen(orgPen);
    }

    // Patterned
    else {
        // ### TODO ###
    }
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
    // Rasterize one-pixel arc
    if(_rasterizer->pen().size() == 1) {
        // Copy the points
        const Point tl( (Pt::int32_t) topLeft.x    (), (Pt::int32_t) topLeft.y     () );
        const Size  sz( (Pt::int32_t) size   .width(), (Pt::int32_t) size   .height() );
        // Rasterize the arc
        _rasterizer->strokeOnePixelEllipseArc(tl, sz, degBegin, degEnd, arcMode);
        return;
    }

    // Save the original pen and create a new pen with miter join
    const Pen orgPen = _rasterizer->pen();

    Pen newPen = orgPen;
    newPen.setJoinStyle(Pen::MiterJoin);

    // Ensure that the begin angle is within the acceptable range
    while(degBegin < -360.0f) degBegin += 360.0f;
    while(degBegin >  360.0f) degBegin -= 360.0f;

    // Ensure that the end angle is within the acceptable range
    while(degEnd < -360.0f) degEnd += 360.0f;
    while(degEnd >  360.0f) degEnd -= 360.0f;

    // Calculate the coordinate shift
    const size_t penSize  = _rasterizer->pen().size();
    const size_t penSize2 = penSize / 2;
    const float  degMid   = (degBegin + degEnd) / 2.0f * Gfx::Math::PiDiv180;
    const float  shiftX   = Gfx::Math::fastCos(degMid);
    const float  shiftY   = Gfx::Math::fastSin(degMid);
    const float  shiftXps = shiftX * penSize2;
    const float  shiftYps = shiftY * penSize2;

    // Calculate the angle adjustment factor
    const float aafa = size.width () / 2.0f;
    const float aafb = size.height() / 2.0f;
    const float aafc = Gfx::Math::PiMul2 * Gfx::Math::fastSqrt( (aafa * aafa + aafb * aafb) / 2.0f );
    const float aafd = 360.0f * penSize2 / aafc;

    // Calculate the adjusted angle
    const float odegBegin = (degBegin < 0) ? (degBegin - aafd) : (degBegin + aafd);
    const float odegEnd   = (degEnd   < 0) ? (degEnd   - aafd) : (degEnd   + aafd);
    const float idegBegin = (degBegin < 0) ? (degBegin + aafd) : (degBegin - aafd);
    const float idegEnd   = (degEnd   < 0) ? (degEnd   + aafd) : (degEnd   - aafd);

    // Solid
    if(_rasterizer->pen().style() == Pen::Solid) {
        // Calculate the arc's parameters
        const Pt::int32_t radiusXo   = ( size.width () + penSize ) / 2;
        const Pt::int32_t radiusYo   = ( size.height() + penSize ) / 2;
        const Pt::int32_t radiusXi   = ( size.width () - penSize ) / 2;
        const Pt::int32_t radiusYi   = ( size.height() - penSize ) / 2;
        const Pt::int32_t centerX    = topLeft.x() + size.width () / 2;
        const Pt::int32_t centerY    = topLeft.y() + size.height() / 2;
        const Pt::int32_t centerXsub = round(centerX - shiftXps);
        const Pt::int32_t centerYsub = round(centerY - shiftYps);
        const Pt::int32_t centerXadd = round(centerX + shiftXps);
        const Pt::int32_t centerYadd = round(centerY + shiftYps);
        // The arc's points
        std::vector<Point> points;
        // Generate a polygon that approximates the arc
        if(arcMode == ArcMode::Chord) {
            // The arc's "outside" lines
            generateArcPoints(points, radiusXo, radiusYo, centerX, centerY, degBegin, degEnd);
            // The arc's "inside" lines
            points.push_back(Painter::PolygonSeparatorPoint);
            generateArcPoints(points, radiusXi, radiusYi, centerX + shiftX, centerY + shiftY, degBegin, degEnd);
        }
        else if(arcMode == ArcMode::Pie) {
            // The arc's "outside" lines
            generateArcPoints(points, radiusXo, radiusYo, centerX, centerY, odegBegin, odegEnd);
            points.push_back(Point(centerXsub, centerYsub));
            // The arc's "inside" lines
            points.push_back(Painter::PolygonSeparatorPoint);
            generateArcPoints(points, radiusXi, radiusYi, centerX, centerY, idegBegin, idegEnd);
            points.push_back(Point(centerXadd, centerYadd));
        }
        else { // ArcMode::Open
            // The arc's "inside" and "outside" lines
            std::vector<Point> inner, outer;
            generateArcPoints(inner, radiusXi, radiusYi, centerX, centerY, degBegin, degEnd);
            generateArcPoints(outer, radiusXo, radiusYo, centerX, centerY, degBegin, degEnd);
            // Combine the arc's lines and add caps
            combineLinePointsAndAddCaps(points, inner, outer, _rasterizer->pen().capStyle(), _rasterizer->pen().capStyle(), penSize);
        }
        // Rasterize the polygon
        _rasterizer->setPen(newPen);
        _rasterizer->strokePolygon(points.data(), points.size());
        _rasterizer->setPen(orgPen);
    }

    // Patterned
    else {
        // ### TODO ###
    }
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

// --- Drawing Function ---

void ImagePainter2::drawThickPolyline_impl(const PointF* ps, const size_t pointCount, bool autoClose, const int32_t* segmentIndexMarker)
{
    // Check if there is no actual point
    if(!pointCount) return;

    // Prepare the buffer
    std::vector<PointF> pointsF;
    pointsF.reserve( pointCount * _rasterizer->pen().size() );

    // Separate the polygons convert them and recombine them
    size_t startIndex = 0;

    for(size_t i = 0; i <= pointCount; ++i) {
        // Search for the end and/or separator points
        if( i == pointCount || (ps[i].x() > MAXIMUM_COORD && ps[i].y() > MAXIMUM_COORD) ) {
            // Get the base pointer and the number of points for this polygon
            const PointF* basePtr = ps + startIndex;
                  size_t  curPCnt = i - startIndex;
            // Update the start index
            startIndex = i + 1;
            // Determine if this polygon is a closed polygon
            bool closedPolygon = autoClose;
            if(basePtr[0] == basePtr[curPCnt - 1]) {
                closedPolygon = true;
                --curPCnt;
            }
            // Thicken the polygon
            if(closedPolygon) {
                if(!thickenSolidClosedPolygon(pointsF, basePtr, curPCnt, segmentIndexMarker)) return;
            }
            else {
                if(!thickenSolidOpenPolygon(pointsF, basePtr, curPCnt, segmentIndexMarker)) return;
            }
            // Convert the points
            std::vector<Point> points;
            convertPointRound(points, pointsF.data(), pointsF.size());
            // Rasterize the polygon
            if(closedPolygon) _rasterizer->strokePolygon        (points.data(), points.size());
            else              _rasterizer->strokePolygonSeparate(points.data(), points.size());
        }
    }
}

// --- Solid Line Thickener ---

void ImagePainter2::generateSolidLineSegment(std::vector<PointF>& dst, float x1, float y1, float x2, float y2, bool openingCap, bool closingCap)
{
    // Calculate the line's parameters
    float wh, dx, dy, nx, ny;

    calculateLineParams(wh, dx, dy, nx, ny, x1, y1, x2, y2,  _rasterizer->pen().size());

    // Generate points (CCW)
    // --- Begin point ---
    if(openingCap) {
        switch(_rasterizer->pen().capStyle()) {
            case Pen::SquareCap        : generateLineSquareCap       (dst, x1, y1,     dx, dy, nx, ny); break;
            case Pen::RoundCap         : generateLineRoundCap        (dst, x1, y1, wh, dx, dy, nx, ny); break;
            case Pen::TriangularOutCap : generateLineTriangularOutCap(dst, x1, y1,     dx, dy, nx, ny); break;
            case Pen::TriangularInCap  : generateLineTriangularInCap (dst, x1, y1,     dx, dy, nx, ny); break;
            default                    : openingCap = false;
        }
    }
    if(!openingCap) generateLineButtCap(dst, x1, y1, nx, ny);
    // --- End point ---
    if(closingCap) {
        switch(_rasterizer->pen().capStyle()) {
            case Pen::SquareCap        : generateLineSquareCap       (dst, x2, y2,     -dx, -dy, -nx, -ny); break;
            case Pen::RoundCap         : generateLineRoundCap        (dst, x2, y2, wh, -dx, -dy, -nx, -ny); break;
            case Pen::TriangularOutCap : generateLineTriangularOutCap(dst, x2, y2,     -dx, -dy, -nx, -ny); break;
            case Pen::TriangularInCap  : generateLineTriangularInCap (dst, x2, y2,     -dx, -dy, -nx, -ny); break;
            default                    : closingCap = false;
        }
    }
    if(!closingCap) generateLineButtCap(dst, x2, y2, -nx, -ny);
}

bool ImagePainter2::thickenSolidOpenPolygon(std::vector<PointF>& pointsF, const PointF* basePtr, size_t curPCnt, const int32_t* segmentIndexMarker)
{
    // Prepare the buffers
    std::vector<PointF> pointsFPolygon;
    std::vector<PointF> pointsFInner;
    std::vector<PointF> pointsFSegment;

    const size_t pz2 = (_rasterizer->pen().size() > 2) ? (_rasterizer->pen().size() / 2) : 1;
    const size_t pz4 = (_rasterizer->pen().size() > 4) ? (_rasterizer->pen().size() / 4) : 1;

    pointsFPolygon.reserve(curPCnt * pz2);
    pointsFInner  .reserve(curPCnt * pz4);
    pointsFSegment.reserve(          pz2);

    // Get the number of point
    const size_t curPC1 = curPCnt - 1;
    const size_t curPC2 = curPCnt - 2;

    // Walk through the polygon's lines
    for(size_t i = 0; i < curPC1; ++i) {
        // Get the coordinates
        const PointF& from = *basePtr++;
        const PointF& to   = *basePtr;
        // Check if the "to" point belongs to the same segment
        bool inSameSegment = !!segmentIndexMarker;
        if(inSameSegment && *segmentIndexMarker < (Pt::int32_t) (i + 1)) {
            inSameSegment = false;
            ++segmentIndexMarker;
        }
        // Solid line
        if(_rasterizer->pen().style() == Pen::Solid) {
            pointsFSegment.clear();
            generateSolidLineSegment(pointsFSegment, from.x(), from.y(), to.x(), to.y(), i == 0, i == curPC2);
            if(!combineLineSegmentForSolidOpenPolygon(
                pointsFPolygon, pointsFInner, pointsFSegment, from, inSameSegment
            )) return false;
        }
        // Patterned line
        else {
            // ### TODO ###
            return false;
        }
    }

    // Process the "inside" lines' points
    if(true) {
        // Solid line
        if(_rasterizer->pen().style() == Pen::Solid) {
            // Store the "inside" lines' points to the main polygon buffer in reverse
            pointsFPolygon.insert(pointsFPolygon.end(), pointsFInner.rbegin(), pointsFInner.rend());
        }
        // Patterned line
        else {
            // ### TODO ###
            return false;
        }
    }

    // Combine the polygon data
    if(!pointsF.empty()) pointsF.push_back(Painter::PolygonSeparatorPointF);
    pointsF.insert(pointsF.end(), pointsFPolygon.begin(), pointsFPolygon.end());

    // Done
    return true;
}

bool ImagePainter2::thickenSolidClosedPolygon(std::vector<PointF>& pointsF, const PointF* basePtr, size_t curPCnt, const int32_t* segmentIndexMarker)
{
    // Prepare the buffers
    std::vector<PointF> pointsFOuter;
    std::vector<PointF> pointsFInner;
    std::vector<PointF> pointsFSegment;

    const size_t pz2 = (_rasterizer->pen().size() > 2) ? (_rasterizer->pen().size() / 2) : 1;
    const size_t pz4 = (_rasterizer->pen().size() > 4) ? (_rasterizer->pen().size() / 4) : 1;

    pointsFOuter  .reserve(curPCnt * pz2);
    pointsFInner  .reserve(curPCnt * pz4);
    pointsFSegment.reserve(          pz2);

    // Get the number of point
    const size_t curPC1 = curPCnt - 1;

    // Save the original base pointer
    const PointF* ptrZero = basePtr;

    // Walk through the polygon's lines
    for(size_t i = 0; i <= curPC1; ++i) {
        // Get the coordinates
        const PointF& from = *basePtr++;
        const PointF& to   = (i == curPC1) ? *ptrZero : *basePtr;
        // Check if the "to" point belongs to the same segment
        bool inSameSegment = !!segmentIndexMarker && (i != curPC1);
        if(inSameSegment && *segmentIndexMarker < (Pt::int32_t) (i + 1)) {
            inSameSegment = false;
            ++segmentIndexMarker;
        }
        // Solid line
        if(_rasterizer->pen().style() == Pen::Solid) {
            pointsFSegment.clear();
            generateSolidLineSegment(pointsFSegment, from.x(), from.y(), to.x(), to.y(), false, false);
            if(!combineLineSegmentForSolidClosedPolygon(
                pointsFOuter, pointsFInner, pointsFSegment, from, i == 1, false, inSameSegment
            )) return false;
        }
        // Patterned line
        else {
            // ### TODO ###
            return false;
        }
    }

    // Reprocess the first and second segments to generate the last join
    if(true) {
        // Get the coordinates
        const PointF& from = *ptrZero++;
        const PointF& to   = *ptrZero;
        // Solid line
        if(_rasterizer->pen().style() == Pen::Solid) {
            pointsFSegment.clear();
            generateSolidLineSegment(pointsFSegment, from.x(), from.y(), to.x(), to.y(), false, false);
            if(!combineLineSegmentForSolidClosedPolygon(
                pointsFOuter, pointsFInner, pointsFSegment, from, false, true, false
            )) return false;
        }
        // Patterned line
        else {
            // ### TODO ###
            return false;
        }
    }

    // Combine the polygon data
    if(pointsFOuter.empty() || pointsFInner.empty()) return false;

    if(!pointsF.empty()) pointsF.push_back(Painter::PolygonSeparatorPointF);
    pointsF.insert(pointsF.end(), pointsFOuter.begin(), pointsFOuter.end());

    pointsF.push_back(Painter::PolygonSeparatorPointF);
    pointsF.insert(pointsF.end(), pointsFInner.begin(), pointsFInner.end());

    // Done
    return true;
}

bool ImagePainter2::combineLineSegmentForSolidOpenPolygon(std::vector<PointF>& polygon, std::vector<PointF>& inner, const std::vector<PointF>& segment, const PointF& origMeetingPoint, bool inSameSegment)
{
    // If the main polygon buffer is still empty, simply copy the points
    if(polygon.empty()) {
        polygon.insert(polygon.end(), segment.begin(), segment.end());
        return true;
    }

    // Get the width of the pen
    const size_t penSize = _rasterizer->pen().size();

    // Copy point #(N-1) and #(N-2) from the main polygon buffer
    const size_t N1           = polygon.size() - 1;
    const PointF polyPointN   = polygon[N1    ];
    const PointF polyPointNm1 = polygon[N1 - 1];
    const PointF polyPointNm2 = polygon[N1 - 2];

    // Remove point #(N-1) and #N from the main polygon buffer
    polygon.pop_back();
    polygon.pop_back();

    // Get the "outside" lines
    const PointF& oline1a = polyPointNm2;
    const PointF& oline1b = polyPointNm1;
    const PointF& oline2a = segment[1];
    const PointF& oline2b = segment[2];

    // Intersect the "outside" lines
    bool   inLine;
    PointF intersect;
    if(!intersectLine(inLine, intersect, oline1a, oline1b, oline2a, oline2b)) return false;

    /*
    const PointF& ochk1 = oline1b - intersect;
    const PointF& ochk2 = oline2a - intersect;

    inLine |= ( fabs(ochk1.x()) <= 0.8f && fabs(ochk1.y()) <= 0.8f ) || // For preventing artifacts
              ( fabs(ochk2.x()) <= 0.8f && fabs(ochk2.y()) <= 0.8f );
    */

    // Store the "outside" line's points to the main polygon buffer
    const Pen::JoinStyle js1 = (inSameSegment || inLine) ? Pen::MiterJoin : _rasterizer->pen().joinStyle();
    switch(js1) {
        // No join
        case Pen::NoJoin:
            polygon.push_back(oline1b);
            polygon.push_back(origMeetingPoint);
            polygon.push_back(oline2a);
            break;
        // Bevel join
        case Pen::BevelJoin:
            polygon.push_back(oline1b);
            polygon.push_back(oline2a);
            break;
        // Miter join
        case Pen::MiterJoin:
            polygon.push_back(intersect);
            break;
        // Round join
        case Pen::RoundJoin:
            generateQuadraticBezierPoints(
                polygon,
                round(oline1b  .x()), round(oline1b  .y()),
                round(intersect.x()), round(intersect.y()),
                round(oline2a  .x()), round(oline2a  .y()),
                ceil(penSize * 0.5f) - 1
            );
            break;
        // Invalid join type
        default:
            return false;
    }

    // Store points #2 to #N from the segment to the main polygon buffer
    const size_t N2 = segment.size() - 1;
    for(size_t i = 2; i <= N2; ++i) polygon.push_back(segment[i]);

    // Get the "inside" lines
    const PointF& iline1a = inner.empty() ? polygon[0 ] : inner.back();
    const PointF& iline1b =                 polyPointN;
    const PointF& iline2a =                 segment[0 ];
    const PointF& iline2b =                 segment[N2];

    // Intersect the "inside" lines
    if(!intersectLine(inLine, intersect, iline1a, iline1b, iline2a, iline2b)) return false;

    /*
    const PointF& ichk1 = iline1b - intersect;
    const PointF& ichk2 = iline2a - intersect;

    inLine |= ( fabs(ichk1.x()) <= 0.8f && fabs(ichk1.y()) <= 0.8f ) || // For preventing artifacts
              ( fabs(ichk2.x()) <= 0.8f && fabs(ichk2.y()) <= 0.8f );
    */

    // Store the "inside" line's points to the auxiliary polygon buffer
    const Pen::JoinStyle js2 = (inSameSegment || inLine) ? Pen::MiterJoin : _rasterizer->pen().joinStyle();
    switch(js2) {
        // No join
        case Pen::NoJoin:
            inner.push_back(iline1b);
            inner.push_back(origMeetingPoint);
            inner.push_back(iline2a);
            break;
        // Bevel join
        case Pen::BevelJoin:
            inner.push_back(iline1b);
            inner.push_back(iline2a);
            break;
        // Miter join
        case Pen::MiterJoin:
            inner.push_back(intersect);
            break;
        // Round join
        case Pen::RoundJoin:
            generateQuadraticBezierPoints(
                inner,
                round(iline1b  .x()), round(iline1b  .y()),
                round(intersect.x()), round(intersect.y()),
                round(iline2a  .x()), round(iline2a  .y()),
                ceil(penSize * 0.5f) - 1
            );
            break;
        // Invalid join type
        default:
            return false;
    }

    // Done
    return true;
}

bool ImagePainter2::combineLineSegmentForSolidClosedPolygon(std::vector<PointF>& outer, std::vector<PointF>& inner, const std::vector<PointF>& segment, const PointF& origMeetingPoint, bool isFirst, bool isLast, bool inSameSegment)
{
    // If the main polygon buffer is still empty, simply copy the points
    if(outer.empty()) {
        outer.push_back(segment[1]);
        outer.push_back(segment[2]);
        inner.push_back(segment[0]);
        inner.push_back(segment[3]);
        return true;
    }

    // Get the width of the pen
    const size_t penSize = _rasterizer->pen().size();

    // Get the "outside" lines
    const PointF& oline1a = outer[outer.size() - 2];
    const PointF& oline1b = outer[outer.size() - 1];
    const PointF& oline2a = segment[1];
    const PointF& oline2b = segment[2];

    // Intersect the "outside" lines
    bool   inLine;
    PointF intersect;
    if(!intersectLine(inLine, intersect, oline1a, oline1b, oline2a, oline2b)) return false;

    /*
    const PointF& ochk1 = oline1b - intersect;
    const PointF& ochk2 = oline2a - intersect;

    inLine |= ( fabs(ochk1.x()) <= 0.8f && fabs(ochk1.y()) <= 0.8f ) || // For preventing artifacts
              ( fabs(ochk2.x()) <= 0.8f && fabs(ochk2.y()) <= 0.8f );
    */

    // Store the "outside" line's points
    const Pen::JoinStyle js1 = (inSameSegment || inLine) ? Pen::MiterJoin : _rasterizer->pen().joinStyle();
    outer.pop_back();
    if(isFirst) outer.pop_back();
    switch(js1) {
        // No join
        case Pen::NoJoin:
            outer.push_back(oline1b);
            outer.push_back(origMeetingPoint);
            outer.push_back(oline2a);
            break;
        // Bevel join
        case Pen::BevelJoin:
            outer.push_back(oline1b);
            outer.push_back(oline2a);
            break;
        // Miter join
        case Pen::MiterJoin:
            outer.push_back(intersect);
            break;
        // Round join
        case Pen::RoundJoin:
            generateQuadraticBezierPoints(
                outer,
                round(oline1b  .x()), round(oline1b  .y()),
                round(intersect.x()), round(intersect.y()),
                round(oline2a  .x()), round(oline2a  .y()),
                ceil(penSize * 0.5f) - 1
            );
            break;
        // Invalid join type
        default:
            return false;
    }
    if(!isLast) outer.push_back(oline2b);

    // Get the "inside" lines
    const PointF& iline1a = inner[inner.size() - 2];
    const PointF& iline1b = inner[inner.size() - 1];
    const PointF& iline2a = segment[0];
    const PointF& iline2b = segment[3];

    // Intersect the "inside" lines
    if(!intersectLine(inLine, intersect, iline1a, iline1b, iline2a, iline2b)) return false;

    /*
    const PointF& ichk1 = iline1b - intersect;
    const PointF& ichk2 = iline2a - intersect;

    inLine |= ( fabs(ichk1.x()) <= 0.8f && fabs(ichk1.y()) <= 0.8f ) || // For preventing artifacts
              ( fabs(ichk2.x()) <= 0.8f && fabs(ichk2.y()) <= 0.8f );
    */

    // Store the "inside" line's points
    const Pen::JoinStyle js2 = (inSameSegment || inLine) ? Pen::MiterJoin : _rasterizer->pen().joinStyle();
    inner.pop_back();
    if(isFirst) inner.pop_back();
    switch(js2) {
        // No join
        case Pen::NoJoin:
            inner.push_back(iline1b);
            inner.push_back(origMeetingPoint);
            inner.push_back(iline2a);
            break;
        // Bevel join
        case Pen::BevelJoin:
            inner.push_back(iline1b);
            inner.push_back(iline2a);
            break;
        // Miter join
        case Pen::MiterJoin:
            inner.push_back(intersect);
            break;
        // Round join
        case Pen::RoundJoin:
            generateQuadraticBezierPoints(
                inner,
                round(iline1b  .x()), round(iline1b  .y()),
                round(intersect.x()), round(intersect.y()),
                round(iline2a  .x()), round(iline2a  .y()),
                ceil(penSize * 0.5f) - 1
            );
            break;
        // Invalid join type
        default:
            return false;
    }
    if(!isLast) inner.push_back(iline2b);

    // Done
    return true;
}

// --- Patterned Line Thickener ---

void ImagePainter2::generatePatternedLineSegment(std::vector<PointF>& dst, float x1, float y1, float x2, float y2, bool openingCap, bool closingCap)
{

    // Calculate the line's parameters
    float wh, dx, dy, nx, ny;

    calculateLineParams(wh, dx, dy, nx, ny, x1, y1, x2, y2,  _rasterizer->pen().size());



/*
    const Pt::uint64_t* pBuf = _rasterizer->patternBufferMP64();
    //PATTERN_BUFFER_COUNTER_MAXMP

    // Generate points (CCW)
    // --- Begin point ---
    if(openingCap) {
        switch(_rasterizer->pen().capStyle()) {
            case Pen::SquareCap        : generateLineSquareCap       (dst, x1, y1,     dx, dy, nx, ny); break;
            case Pen::RoundCap         : generateLineRoundCap        (dst, x1, y1, wh, dx, dy, nx, ny); break;
            case Pen::TriangularOutCap : generateLineTriangularOutCap(dst, x1, y1,     dx, dy, nx, ny); break;
            case Pen::TriangularInCap  : generateLineTriangularInCap (dst, x1, y1,     dx, dy, nx, ny); break;
            default                    : openingCap = false;
        }
    }
    if(!openingCap) generateLineButtCap(dst, x1, y1, nx, ny);
    // --- End point ---
    if(closingCap) {
        switch(_rasterizer->pen().capStyle()) {
            case Pen::SquareCap        : generateLineSquareCap       (dst, x2, y2,     -dx, -dy, -nx, -ny); break;
            case Pen::RoundCap         : generateLineRoundCap        (dst, x2, y2, wh, -dx, -dy, -nx, -ny); break;
            case Pen::TriangularOutCap : generateLineTriangularOutCap(dst, x2, y2,     -dx, -dy, -nx, -ny); break;
            case Pen::TriangularInCap  : generateLineTriangularInCap (dst, x2, y2,     -dx, -dy, -nx, -ny); break;
            default                    : closingCap = false;
        }
    }
    if(!closingCap) generateLineButtCap(dst, x2, y2, -nx, -ny);
*/


    // ### TODO ###
}



} // namespace
} // namespace
