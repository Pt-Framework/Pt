/* Copyright (C) 2017-2020 Aloysius Indrayanto
   Copyright (C) 2017-2017 Marc Boris Duerner

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

#include "Rasterizer2.h" // FIXED constants

#include "Polygonizer.h"


//#define DEBUG_INTERSECT_LINE


namespace Pt {

namespace Gfx {


// Predefined patterns
static const Pt::uint8_t dashPatternDot_ [] = { 1, 1 };
static const Pt::uint8_t dashPatternDash_[] = { 3, 1 };

std::vector<Pt::uint8_t> Polygonizer::dashPatternDot  = std::vector<Pt::uint8_t>( dashPatternDot_,  dashPatternDot_  + sizeof(dashPatternDot_ ) );
std::vector<Pt::uint8_t> Polygonizer::dashPatternDash = std::vector<Pt::uint8_t>( dashPatternDash_, dashPatternDash_ + sizeof(dashPatternDash_) );

// Rounding factors used for cleaning-up polygons
const double Polygonizer::VecResScaleUp = 64.0;
const double Polygonizer::VecResScaleDn = 1.0 / 64.0;


Polygonizer::Polygonizer()
{
}


void Polygonizer::setPattern(const Pen::Style& style, const Pen::CapStyle& cap,
                             const std::vector<Pt::uint8_t>& userDashPattern, std::size_t penSize)
{
    // Select the pattern
    const std::vector<Pt::uint8_t>* selDashPattern;

    if(!userDashPattern.empty()) {
        selDashPattern = &userDashPattern;
    }
    else {
        switch(style)
        {
            default        :
            case Pen::Dot  : selDashPattern = &dashPatternDot;  break;
            case Pen::Dash : selDashPattern = &dashPatternDash; break;
        }
    }

    // Generate the pattern
    dashPatternBuffer.clear();
    dashPatternBuffer.resize(selDashPattern->size());

    for(unsigned i = 0; i < selDashPattern->size(); ++i)
        dashPatternBuffer[i] = selDashPattern->operator[](i) * penSize;
}


// Based on: Bitmap/B�zier curves/Quadratic
//           https://rosettacode.org/wiki/Bitmap/B%C3%A9zier_curves/Quadratic#C
//           Last modified on February 17, 2017
void Polygonizer::renderQuadraticBezierPoints(std::vector<PointF>& dst,
                                              float x1, float y1,
                                              float x2, float y2,
                                              float x3, float y3,
                                              Pt::int32_t nSegs)
{
    // Check if the points actually specify a straight line
    const float sx = x3 - x2;
    const float sy = y3 - y2;
    const float xx = x1 - x2;
    const float yy = y1 - y2;

    if( !(xx * sy - yy * sx) )
    {
        // Curvature
        if( dst.empty() || dst.back().x() != x1 || dst.back().y() != y1 )
            dst.push_back( PointF(x1, y1) );

        if( dst.empty() || dst.back().x() != x3 || dst.back().y() != y3 )
            dst.push_back( PointF(x3, y3) );

        return;
    }

    // Ensure that the number of segments are not too few
    if(nSegs < 0) {
        nSegs = -nSegs;
        if(nSegs < 2) nSegs = 2;
    }
    else {
        if(nSegs < 4) nSegs = 4;
    }

    // Calculate the inverse multiplication factor
    const float nSegs1i = 1.0f / (nSegs - 1);

    for(Pt::int32_t i = 0; i < nSegs; ++i)
    {
        // Calculate the coordinates
        const float t  = i * nSegs1i;
        const float it = 1.0f - t;
        const float a  = it * it;
        const float b  = 2.0f * t  * it;
        const float c  = t * t;
        const float x  = a * x1 + b * x2 + c * x3;
        const float y  = a * y1 + b * y2 + c * y3;

        // Check if the coordinate is the same with the previous one
        if( !dst.empty() && ( dst.back().x() == x && dst.back().y() == y ) )
            continue;

        // Store the coordinate
        dst.push_back( PointF(x, y) );
    }
}


void Polygonizer::renderLineButtCap(std::vector<PointF>& dst, float x, float y, float wh, float dx, float dy, float nx, float ny)
{
#if 0
    // Hack for small-width lines?
    if(wh <= 1.0f) {
        renderLineSquareCap(dst, x, y, dx, dy, nx, ny);
        return;
    }
#endif

    dst.push_back( PointF(x + nx, y + ny) );
    dst.push_back( PointF(x - nx, y - ny) );
}


void Polygonizer::renderLineSquareCap(std::vector<PointF>& dst, float x, float y, float dx, float dy, float nx, float ny)
{
    dst.push_back( PointF(x - dx + nx, y - dy + ny) );
    dst.push_back( PointF(x - dx - nx, y - dy - ny) );
}


void Polygonizer::renderLineRoundCap(std::vector<PointF>& dst, float x, float y, float wh, float dx, float dy, float nx, float ny)
{
#if 1
    // Hack for small-width lines?
    if(wh <= 1.5f) {
        dst.push_back( PointF(x + nx,        y + ny       ) );
        dst.push_back( PointF(x - dx * 1.5f, y - dy * 1.5f) );
        dst.push_back( PointF(x - nx,        y - ny       ) );
        return;
    }
#endif

#if 0
    // This one seems produce worse result
    renderQuadraticBezierPoints(
        dst,
        roundf((x + nx     ) * 10.0f) * 0.1f, roundf((y + ny     ) * 10.0f) * 0.1f,
        roundf((x + nx - dx) * 10.0f) * 0.1f, roundf((y + ny - dy) * 10.0f) * 0.1f,
        roundf((x      - dx) * 10.0f) * 0.1f, roundf((y      - dy) * 10.0f) * 0.1f,
        (Pt::int32_t) ceil(wh) - 1
    );
    renderQuadraticBezierPoints(
        dst,
        roundf((x      - dx) * 10.0f) * 0.1f, roundf((y      - dy) * 10.0f) * 0.1f,
        roundf((x - nx - dx) * 10.0f) * 0.1f, roundf((y - ny - dy) * 10.0f) * 0.1f,
        roundf((x - nx     ) * 10.0f) * 0.1f, roundf((y - ny     ) * 10.0f) * 0.1f,
        (Pt::int32_t) ceil(wh) - 1
    );
#else
    // This one seems produce better result
    renderQuadraticBezierPoints(
        dst,
        roundf((x + nx       ) * 10.0f) * 0.1f, roundf((y + ny       ) * 10.0f) * 0.1f,
        roundf((x - dx * 2.0f) * 10.0f) * 0.1f, roundf((y - dy * 2.0f) * 10.0f) * 0.1f,
        roundf((x - nx       ) * 10.0f) * 0.1f, roundf((y - ny       ) * 10.0f) * 0.1f,
        (Pt::int32_t) ceil(wh) - 1
    );
#endif
}


void Polygonizer::calculateLineParams(float& wh, float& dx, float& dy,
                                      float& nx, float& ny, float x1, float y1,
                                      float x2, float y2, size_t w)
{
    // Line equation : 0 = aX + By + c
    // Normal        : n = ai + bj
    const float a = y2 - y1;
    const float b = x1 - x2;
  //const float c = -(x1 * y2 - x2 * y1);

    // Line length
    // NOTE: Gfx::Math::fastInvSqrt() will produce artifacts!
    const float ll = ::sqrtf(a * a + b * b);

    // Inverse line length
    const float il = 1.0f / ll;

    // Half-line width
    wh = (float) w * 0.5f;

    // Adjust the half-line width
    wh = floor(wh);
    if( !(w & 1) && wh >= 0.5f ) { // For lines with even widths only
        wh -= 0.5f;
    }

    // Calculate the Direction vector
    dx = -b * il * wh;
    dy =  a * il * wh;

    // Calculate the normal vector
    nx =  a * il * wh;
    ny =  b * il * wh;
}


bool Polygonizer::intersectLine(bool& inLine, PointF& intersect,
                                const PointF& line1a, const PointF& line1b,
                                const PointF& line2a, const PointF& line2b, size_t penSize)
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

    // Check if the line is parallel
    const float denom = a1 * b2 - a2 * b1;

#ifdef DEBUG_INTERSECT_LINE
    fprintf(stderr, "Line 1       : (%7.3f, %7.3f) - (%7.3f, %7.3f)\n", x11, y11, x12, y12);
    fprintf(stderr, "Line 2       : (%7.3f, %7.3f) - (%7.3f, %7.3f)\n", x21, y21, x22, y22);
    fprintf(stderr, "a1b2 a2b1 dn : (%7.3f, %7.3f) - (%7.3f, %7.3f) : %7.3f\n", a1, b2, a2, b1, denom);
#endif

    if(denom == 0.0f) {
        // Check for special cases
        if(y11 == y12 && y11 == y21 && y11 == y22 && x12 == x21) {
            intersect.set(x12, y11);
            inLine = true;
#ifdef DEBUG_INTERSECT_LINE
            fprintf(stderr, "Intersect #Y : (%7.3f, %7.3f) - %s\n", intersect.x(), intersect.y(), inLine ? "inline" : "outline");
#endif
            return true;
        }
        if(x11 == x12 && x11 == x21 && x11 == x22 && y12 == y21) {
            intersect.set(x11, y12);
            inLine = true;
#ifdef DEBUG_INTERSECT_LINE
            fprintf(stderr, "Intersect #X : (%7.3f, %7.3f) - %s\n", intersect.x(), intersect.y(), inLine ? "inline" : "outline");
#endif
            return true;
        }
        // No intersection
#ifdef DEBUG_INTERSECT_LINE
        fprintf(stderr, "Intersect NONE \n");
#endif
        return false;
    }

    // Calculate the intersection point
    const float idenom = 1.0f / denom;
          float ipX    = (b1 * c2 - b2 * c1) * idenom;
          float ipY    = (a2 * c1 - a1 * c2) * idenom;

    // Check and fix the coordinate of the intersection point
    // (for very steep lines, the coordinate of the intersection point can be incorrectly calculated)
    const size_t pzf = FIXED_POINT_TO_INT(penSize * FIXED_POINT_CONSTANT_SQRT2);
         if(ipX < minX1 - pzf && ipX < minX2 - pzf) ipX = (minX1 + minX2) * 0.5f;
    else if(ipX > maxX1 + pzf && ipX > maxX2 + pzf) ipX = (maxX1 + maxX2) * 0.5f;
         if(ipY < minY1 - pzf && ipY < minY2 - pzf) ipY = (minY1 + minY2) * 0.5f;
    else if(ipY > maxY1 + pzf && ipY > maxY2 + pzf) ipY = (maxY1 + maxY2) * 0.5f;

    // Store the intersection point
    intersect.set(ipX, ipY);

    // Determine if the intersection point is inside the line
    inLine = (ipX >= minX1 && ipX <= maxX1 && ipY >= minY1 && ipY <= maxY1)
           | (ipX >= minX2 && ipX <= maxX2 && ipY >= minY2 && ipY <= maxY2);

#ifdef DEBUG_INTERSECT_LINE
    fprintf(stderr, "Intersect #G : (%7.3f, %7.3f) - %s\n", intersect.x(), intersect.y(), inLine ? "inline" : "outline");
#endif

    // Done
    return true;
}



} // namespace

} // namespace
